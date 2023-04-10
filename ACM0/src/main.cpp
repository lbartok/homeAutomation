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

// settings for individual home
#include <SettingsRez.h>

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;

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
    stateTopicTemp.concat(topic);
    // Modify the received topic for reporting state
    if (topicStr.indexOf("cmd") >= 0)
    {
        stateTopicTemp.replace("cmd", "state");
    }
    else if (topicStr.indexOf("toggle") >= 0)
    {
        stateTopicTemp.replace("toggle", "state");
    }
    else
    {
        stateTopicTemp = "Topic is unknown command!";
    }
    const char *stateTopic = stateTopicTemp.c_str();

    // Check the type to know what to do (cmd = hassio | toggle = switch)
    if (topicStr.lastIndexOf("cmd") >= 0)
    {
        // Get the pin by the entity
        int foundPin = returnPin(topicStr.substring(5, topicStr.lastIndexOf("/")));

        // Check if request is to turn on and currently is off
        if (message.compareTo("on") == 0 && digitalRead(foundPin) == LOW)
        {
            // Switch the state and publish
            toggle(foundPin);
            client.publish(stateTopic, "on", retain);
            // ... and resubscribe
            client.subscribe(controllino);
        }
        // Check if request it to turn off and currently is on
        else if (message.compareTo("off") == 0 && digitalRead(foundPin) == HIGH)
        {
            // Switch the state and publish
            toggle(foundPin);
            client.publish(stateTopic, "off", retain);
            // ... and resubscribe
            client.subscribe(controllino);
        }
        else
        {
            // When it is already in desired state, just publish back the state
            client.publish(stateTopic, digitalRead(foundPin) == HIGH ? "on" : "off", retain);
            // ... and resubscribe
            client.subscribe(controllino);
        }
    }

    // Check the type to know what to do (cmd = hassio | toggle = switch)
    if (topicStr.lastIndexOf("toggle") >= 0)
    {
        // Check the type to know what to do (light/outlet/blind)
        if (topicStr.indexOf("blind") < 0)
        {
            // Get the pin by the entity
            int foundPin = returnPin(topicStr.substring(5, topicStr.lastIndexOf("/")));

            // Toggle the pin value and Publish the state to the state topic
            client.publish(stateTopic, toggle(foundPin) == HIGH ? "on" : "off", retain);
            // ... and resubscribe
            client.subscribe(controllino);
        };
    }

    if (topicStr.indexOf("info") >= 0)
    {
        Serial.println("ACM0 reconnected...'info' command received.");
        client.publish("ACM0/reconnected", "info accomplished");
        // ... and resubscribe
        client.subscribe(controllino);
    }
}

boolean reconnect()
{
    if (client.connect("ACM0", user, password))
    {
        // Once connected, publish an announcement...
        client.publish("ACM0/info", "reconnected");
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
void checkPressedPushButton(Button &btn)
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
    Serial.println("Init");

    /* Setup WatchDog timer */
    wdt_enable(WDTO_4S);

    // create MQTT
    client.setServer(server, port);
    client.setCallback(callback);

    client.subscribe(controllino);

    // Setup ethernet
    Ethernet.begin(mac, ip);
    delay(200);
    lastReconnectAttempt = 0;

    // Configure the button as you'd like - not necessary if you're happy with the defaults
    for (int p1 = 0; p1 < PUSH_BUTTONS_TOTAL; p1++)
    {
        p_button[p1].definition.configureButton(configurePushButton);
    }

    // When the button is first pressed, call the function checkPressedPushButton (above)
    for (int p2 = 0; p2 < PUSH_BUTTONS_TOTAL; p2++)
    {
        p_button[p2].definition.onPress(checkPressedPushButton);
    }

    // initialitze pinMode for all Outputs
    for (int pMo = 0; pMo < OUTPUTS_TOTAL; pMo++)
    {
        pinMode(c_outputs[pMo].pin, OUTPUT);
    }
}

void loop()
{
    // Digital
    for (int p3 = 0; p3 < PUSH_BUTTONS_TOTAL; p3++)
    {
        p_button[p3].definition.update();
    }

    // Analog
    buttonsA0.update();
    buttonsA1.update();
    buttonsA2.update();
    buttonsA4.update();
    buttonsA13.update();
    buttonsA14.update();
    buttonsA15.update();

    // Check if button is pressed
    for (int aBarray = 0; aBarray < ANALOG_BUTTONS_TOTAL; aBarray++)
    {
        // dereference analog multi button reference
        AnalogMultiButton *p0 = am_button[aBarray].definition; // ANALOG_BUTTONS_DEF[aBarray];
        AnalogMultiButton deRefObject = *p0;
        // cycle through all possible buttons
        for (int aB = 0; aB < BUTTONS_TOTAL; aB++)
        {
            if (deRefObject.onPress(aB))
            {
                for (int tt = 0; tt < am_button[aBarray].buttons[aB].total_topics; tt++)
                {
                    client.publish(am_button[aBarray].buttons[aB].topics[tt], am_button[aBarray].buttons[aB].payload);
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

    /* reset Watchdog timer */
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
