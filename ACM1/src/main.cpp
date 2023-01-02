#include <Arduino.h>
#include <ArduinoJson.h>
#include <Controllino.h>
#include <avr/wdt.h>
// For analog button detection
#include "AnalogMultiButton.h"
// For MQTT communication
#include <Ethernet.h>
#include <PubSubClient.h>
// For pushing digital buttons
#include <Button.h>
#include <ButtonEventCallback.h>
#include <PushButton.h>
#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce-Arduino-Wiring
// New shutter code
#include <Shutters.h>
#include <EEPROM.h>

// settings for individual home
#include <SettingsRez.h>

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;

/* New shutter code */
// Function to return Shutter based on position in the blindsArray array
Shutters *resolveShutter(String recTopic)
{
    for (unsigned int i = 0; i < BLINDS_TOTAL; i++)
    {
        if (recTopic.compareTo(blinds[i].entity) == 0)
        {
            return blinds[i].blind;
        }
    }
    return nullptr;
}

// Function to return blind's controllPin number by supplying Shutter refference
int resolveShutterPin(Shutters *s)
{
    for (int s0 = 0; s0 < BLINDS_TOTAL; s0++)
    {
        if (s == blinds[s0].blind)
        {
            // this callback was called from the blindsArray[s0]
            return blinds[s0].pin;
        }
    }

    return -1;
}

// Function to return blind's entity identifier by supplying Shutter refference
String resolveShutterEntity(Shutters *s)
{
    for (int s0 = 0; s0 < BLINDS_TOTAL; s0++)
    {
        if (s == blinds[s0].blind)
        {
            // this callback was called from the blindsArray[s0]
            return blinds[s0].entity;
        }
    }

    return "";
}

// Main function handling operation of each shutter
void shuttersOperationHandler(Shutters *s, ShuttersOperation operation)
{
    controllPin = resolveShutterPin(s);
    directionPin = controllPin + 1;
    // construct state topic to report blind state
    String stateTpc = "ACM1/";
    stateTpc += resolveShutterEntity(s);
    stateTpc += "/state";

    switch (operation)
    {
    case ShuttersOperation::UP:
        Serial.println("Shutters going up.");
        // Code for the shutters to go up
        digitalWrite(directionPin, LOW);
        digitalWrite(controllPin, HIGH);
        // State handling for MQTT
        client.publish(stateTpc.c_str(), "opening", retain);
        client.subscribe(controllino);
        break;
    case ShuttersOperation::DOWN:
        Serial.println("Shutters going down.");
        // Code for the shutters to go down
        digitalWrite(directionPin, HIGH);
        digitalWrite(controllPin, HIGH);
        // State handling for MQTT
        client.publish(stateTpc.c_str(), "closing", retain);
        client.subscribe(controllino);
        break;
    case ShuttersOperation::HALT:
        Serial.println("Shutters halting.");
        // Code for the shutters to halt
        digitalWrite(directionPin, LOW);
        digitalWrite(controllPin, LOW);
        break;
    }
}

// Function to read shutters level from EEPROM
void readInEeprom(char *dest, byte length)
{
    for (byte i = 0; i < length; i++)
    {
        dest[i] = EEPROM.read(eepromOffset + i);
    }
}

// Function to store shutters level in EEPROM
void shuttersWriteStateHandler(Shutters *shutters, const char *state, byte length)
{
    for (byte i = 0; i < length; i++)
    {
        EEPROM.write(eepromOffset + i, state[i]);
#ifdef ESP8266
        EEPROM.commit();
#endif
    }
}

// Shutters level reached code to announce each whole percent
void onShuttersLevelReached(Shutters *shutters, byte level)
{
    if ((*shutters).isIdle())
    {
        // construct state topic to report blind state
        String stateTopic = "ACM1/";
        stateTopic += resolveShutterEntity(shutters);
        stateTopic += "/state";
        // construct position topic to report blind state
        String posTopic = "ACM1/";
        posTopic += resolveShutterEntity(shutters);
        posTopic += "/pos";

        // publish state and position
        client.publish(stateTopic.c_str(), String(level < 100 ? "open" : "closed").c_str(), retain);
        client.publish(posTopic.c_str(), String(level).c_str(), retain);
        // ... resubscribe
        client.subscribe(controllino);
    }
}
/* New shutter code end */

// main toggle function for lights (and others)
int toggle(int pin)
{
    digitalWrite(pin, !digitalRead(pin));
    return digitalRead(pin);
}

// Helper function to find which output to choose for the topic received
int returnPin(String recEntity)
{
    for (unsigned int i = 0; i < OUTPUTS_TOTAL; i++)
    {
        if (recEntity.compareTo(c_outputs[i].entity) == 0)
        {
            return c_outputs[i].pin;
        };
    };

    return -1;
}

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length)
{
    // Convert the payload and topic (ACM0/light/kitchen/island) to workable strings
    String message, topicStr, stateTopicTemp;

    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    // Construct state topic for publishing state back to hassio
    // Preserve topic as a String for future checks
    topicStr.concat(topic);
    // Prepare temp topic to convert later to const char * for publishing
    stateTopicTemp.concat(topicStr.substring(0, topicStr.lastIndexOf("/")));
    stateTopicTemp.concat("/state");
    const char *stateTopic = stateTopicTemp.c_str();

    // Check the type to know what to do (cmd = hassio | toggle = switch)
    if (topicStr.lastIndexOf("cmd") >= 0)
    {
        // Check the type to know what to do (light/outlet/blind)
        if (topicStr.indexOf("light") >= 0 || topicStr.indexOf("outlet") >= 0 || topicStr.indexOf("lock") >= 0)
        {
            // Get the pin by the entity
            int foundPin = returnPin(topicStr.substring(5, topicStr.lastIndexOf("/")));

            // Check if request is to turn on and currently is off
            if ((message.equalsIgnoreCase("on") && digitalRead(foundPin) == LOW) || (message.equalsIgnoreCase("off") && digitalRead(foundPin) == HIGH))
            {
                // Switch the state
                toggle(foundPin);
            }

            // When it is already in desired state, just publish back the state
            client.publish(stateTopic, digitalRead(foundPin) == HIGH ? "on" : "off", retain);
            // ... and resubscribe
            client.subscribe(controllino);
        }
        else if (topicStr.indexOf("blind") >= 0)
        {
            // set the blind to work with
            Shutters *shutB = resolveShutter(topicStr.substring(5, topicStr.lastIndexOf("/")));

            // need to convert message received: "open" => 0 (all the way up), "close" => 100 (all the way down)
            // "stop" => ehm stop... (switches do not have STOP button, hence the special handling)

            // check whether the blind is running when button has been pressed again
            if (message.equalsIgnoreCase("stop") || !(*shutB).isIdle())
            {
                (*shutB).stop();
            }
            else if (message.equalsIgnoreCase("open"))
            {
                (*shutB).setLevel(0);
            }
            else if (message.equalsIgnoreCase("close"))
            {
                (*shutB).setLevel(100);
            }
            else
            {
                Serial.println("I have no idea what you want to do with the blind!");
            }
        }
    }
    // Check the type to know what to do (cmd = hassio | toggle = switch)
    else if (topicStr.lastIndexOf("toggle") >= 0)
    {
        // Check the type to know what to do (light/outlet/blind)
        if (topicStr.indexOf("light") >= 0 || topicStr.indexOf("outlet") >= 0 || topicStr.indexOf("lock") >= 0)
        {
            // Get the pin by the entity
            int foundPin = returnPin(topicStr.substring(5, topicStr.lastIndexOf("/")));

            // Toggle the pin and publish the state to the state topic
            client.publish(stateTopic, toggle(foundPin) == HIGH ? "on" : "off", retain);
            // ... and resubscribe
            client.subscribe(controllino);
        }
    }
    // when HassIO will set position through slider
    else if (topicStr.indexOf("set") >= 0)
    {
        // set the blind to work with
        Shutters *shutB = resolveShutter(topicStr.substring(5, topicStr.lastIndexOf("/")));

        // prcnt should be between 0 (all the way up) and 100 (all the way down)
        uint8_t percentage = message.toInt();
        // safeguard for overweighting prcnt value
        if (percentage > 100)
        {
            percentage = 100;
        }
        else if (percentage < 0)
        {
            percentage = 0;
        }

        // move the blind to the newly requested level
        (*shutB).setLevel(percentage);
    }

    if (topicStr.indexOf("info") >= 0)
    {
        Serial.println("ACM1 reconnected...'info' command received.");
        client.publish("ACM1/reconnected", "info accomplished");
        // ... and resubscribe
        client.subscribe(controllino);
    }
}

boolean reconnect()
{
    // Serial.println("Attempting deviceACM1 - MQTT connection ...");
    if (client.connect("ACM1", user, password))
    {
        // Once connected, publish an announcement...
        client.publish("ACM1/info", "reconnected");
        // ... and resubscribe
        client.subscribe(controllino);
    }
    return client.connected();
}

// Use this function to configure the internal Bounce object to suit you. See the documentation at: https://github.com/thomasfredericks/Bounce2/wiki
// This function can be left out if the defaults are acceptable - just don't call configureButton
void configurePushButton(Bounce &bouncedButton)
{
    // Set the debounce interval to 15ms - default is 10ms
    bouncedButton.interval(15);
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void checkPressedButton(Button &btn)
{
    for (int p = 0; p < PUSH_BUTTONS_TOTAL; p++)
    {
        if (p_button[p].definition.is(btn))
        {
            for (int tt = 0; tt < p_button[p].total_topics; tt++)
            {
                client.publish(p_button[p].topics[tt], p_button[p].payload);
                // ... and resubscribe
                client.subscribe(controllino);
            }
        }
    }
}

void setup()
{
    // begin serial so we can see which buttons are being pressed through the serial monitor
    Serial.begin(9600);

    /* Setup WatchDog timer */
    wdt_enable(WDTO_4S);

    /* New shutters code */
    delay(100);
#ifdef ESP8266
    EEPROM.begin(512);
#endif
    /* New shutters code end */

    Serial.println("Init");

    // create MQTT
    client.setServer(server, port);
    client.setCallback(callback);

    client.subscribe(controllino);

    // Setup ethernet
    Ethernet.begin(mac, ip);
    delay(200);
    lastReconnectAttempt = 0;

    // Initialize shutters
    for (int s1 = 0; s1 < BLINDS_TOTAL; s1++)
    {
        // get the last stored state of the shutter
        char storedShuttersState[(*blinds[s1].blind).getStateLength()];
        readInEeprom(storedShuttersState, (*blinds[s1].blind).getStateLength());

        // Initialize the shutter
        (*blinds[s1].blind)
            .setOperationHandler(shuttersOperationHandler)
            .setWriteStateHandler(shuttersWriteStateHandler)
            .restoreState(storedShuttersState)
            .setCourseTime(upCourseTime, downCourseTime)
            .onLevelReached(onShuttersLevelReached)
            .begin()
            .setLevel(100); // Go to 100% (all-the-way down)
    }

    // Configure the button as you'd like - not necessary if you're happy with the defaults
    for (int p1 = 0; p1 < PUSH_BUTTONS_TOTAL; p1++)
    {
        p_button[p1].definition.configureButton(configurePushButton);
    }

    // When the button is first pressed, call the function checkPressedPushButton (above)
    for (int p2 = 0; p2 < PUSH_BUTTONS_TOTAL; p2++)
    {
        p_button[p2].definition.onPress(checkPressedButton);
    }

    // initialize pinMode for blinds
    for (int pM = 0; pM < BLINDS_TOTAL; pM++)
    {
        pinMode(blinds[pM].pin, OUTPUT);
        pinMode(blinds[pM].pin + 1, OUTPUT);
    }

    // initialitze pinMode for all Outputs
    for (int pMo = 0; pMo < OUTPUTS_TOTAL; pMo++)
    {
        pinMode(c_outputs[pMo].pin, OUTPUT);
    }
}

void loop()
{
    // rolety / blinds - New shutters code
    for (int s2 = 0; s2 < BLINDS_TOTAL; s2++)
    {
        (*blinds[s2].blind).loop();
    }

    // Digital buttons
    for (int p3 = 0; p3 < PUSH_BUTTONS_TOTAL; p3++)
    {
        p_button[p3].definition.update();
    }

    // Analog buttons
    buttonsA6.update();
    buttonsA7.update();

    // Check if analog button is pressed
    for (int aB2 = 0; aB2 < ANALOG_BUTTONS_TOTAL; aB2++)
    {
        AnalogMultiButton AMBobject = *ANALOG_BUTTONS_DEF[aB2];
        // cycle through all possible buttons
        for (int aB = 0; aB < BUTTONS_TOTAL; aB++)
        {
            if (AMBobject.onPress(aB))
            {
                for (int tt = 0; tt < am_button[aB2].buttons[aB].total_topics; tt++)
                {
                    client.publish(am_button[aB2].buttons[aB].topics[tt], am_button[aB2].buttons[aB].payload);
                    // ... and resubscribe
                    client.subscribe(controllino);
                }
            }
        }
    }

    // MQTT connect & reconnect
    if (!client.connected())
    {
        long now = millis();
        if (now - lastReconnectAttempt > 5000)
        {
            lastReconnectAttempt = now;
            // Attempt to reconnect
            if (reconnect())
            {
                lastReconnectAttempt = 0;
            }
        }
    }
    else
    {
        // Client connected
        client.loop();
    }

    /* reset WatchDog timer*/
    wdt_reset();
}

// duration reports back how long it has been since the button was originally pressed.
// repeatCount tells us how many times this function has been called by this button.
void onButtonHeld(Button &btn, uint16_t duration, uint16_t repeatCount)
{
    Serial.print("button has been held for ");
    Serial.print(duration);
    Serial.print(" ms; this event has been fired ");
    Serial.print(repeatCount);
    Serial.println(" times");
}

// duration reports back the total time that the button was held down
void onButtonReleased(Button &btn, uint16_t duration)
{
    Serial.print("button released after ");
    Serial.print(duration);
    Serial.println(" ms");
}

/***/
