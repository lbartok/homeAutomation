#include <Arduino.h>
#include <ArduinoJson.h>
#include <Controllino.h>
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
// Function to check if the requested pin is controlling Blinds
int checkBlindsPin(int pin)
{
    for (int pI = 0; pI < BLINDS_TOTAL; pI++)
    {
        if (pin == BLINDS[pI] || pin == (BLINDS[pI] + 1))
        {
            return pI;
        }
    }

    // needs to be more than BLINDS_TOTAL
    return (65535 / 2);
}

// Function to return Shutter based on position in the blindsArray array
Shutters *resolveShutter(int index)
{
    return blindsArray[index];
}

// TODO: check if works as written...
// Function to return blind's controllPin number by supplying Shutter refference
int resolveShutterPin(Shutters *s)
{
    for (int s0 = 0; s0 < BLINDS_TOTAL; s0++)
    {
        if (s == resolveShutter(s0))
        {
            // this callback was called from the blindsArray[s0]
            return BLINDS[s0];
        }
    }
}

// Helper function to construct JSON for STATE reporting
// TODO: define function to produce STATE JSON
String getStateJSON(int pin)
{
    // function to take PIN number and create JSON object

    // construct JSON object - state
    const size_t stateCapacity = JSON_ARRAY_SIZE(0) + JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument state(stateCapacity);

    // add pin number
    state["pin"] = (String)pin;

    // check if state request is for a blind
    int blindNum = checkBlindsPin(pin);
    if (blindNum < BLINDS_TOTAL)
    {
        // resolve correct blind to work with
        Shutters *blindOutput = resolveShutter(blindNum);

        // set the output state based on whether it is moving or not
        String blindOutputState = "";
        if ((*blindOutput).isIdle())
        {
            blindOutputState = (String)(*blindOutput).getCurrentLevel();
        }
        else
        {
            blindOutputState = "moving";
        }

        // write the state to JSON
        state["status"] = blindOutputState;
    }
    else
    {
        state["status"] = ((digitalRead(pin)) ? "on" : "off");
    }

    // define variable to return state for publish/print purposes
    String state2pub;
    serializeJson(state, state2pub);

    return state2pub;
}

// Main function handling operation of each shutter
void shuttersOperationHandler(Shutters *s, ShuttersOperation operation)
{
    // TODO: Check if this works this way...
    controllPin = resolveShutterPin(s);
    directionPin = controllPin + 1;
    /* old way of having the function here...
    for (int s0 = 0; s0 < BLINDS_TOTAL; s0++) {
        if (s == resolveShutter(s0)) {
            // this callback was called from the blindsArray[s0]
            controllPin = BLINDS[s0];
            directionPin = controllPin + 1;
            break;
        }
    }*/

    switch (operation)
    {
    case ShuttersOperation::UP:
        Serial.println("Shutters going up.");
        // Code for the shutters to go up
        digitalWrite(directionPin, LOW);
        digitalWrite(controllPin, HIGH);
        break;
    case ShuttersOperation::DOWN:
        Serial.println("Shutters going down.");
        // Code for the shutters to go down
        digitalWrite(directionPin, HIGH);
        digitalWrite(controllPin, HIGH);
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
    Serial.print("Shutters at ");
    Serial.print(level);
    Serial.println("%.");

    // TODO: Test if it works as expected - provide status when the shutter stops.
    if ((*shutters).isIdle())
    {
        String pin_state = getStateJSON(resolveShutterPin(shutters));
        // publish state
        client.publish("STATE", (char *)pin_state.c_str());
    } /* zbytocne, lebo predosle by malo zachytit tento stav ak nie, tak skusit toto
    else if (level == 100 || level == 0)
    {
        String pin_state = getStateJSON(resolveShutterPin(shutters));
        // publish state
        client.publish("STATE", (char *)pin_state.c_str());
    }*/
    // ... resubscribe
    client.subscribe(controllino);
}
/* New shutter code end */

// main toggle function for lights (and others)
void toggle(int pin)
{
    digitalWrite(pin, !digitalRead(pin));

    // get state for publish
    String pin_state = getStateJSON(pin);
    // publish state
    client.publish("STATE", (char *)pin_state.c_str());
    // ... resubscribe
    client.subscribe(controllino);
}

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length)
{

    const size_t capacity = JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument root(capacity);
    deserializeJson(root, payload);

    const char *action = root["action"];
    if (strcmp(action, "toggle") == 0)
    {
        for (unsigned int i = 0; i < root["output"].size(); i++)
        {
            const int button = root["output"][i];
            if (button != 0)
            {
                toggle(button);
                // to be removed when all is successful
                Serial.print(button);
                Serial.print("--");
                Serial.println(digitalRead(button));
                // end of "to be removed"
            }
        }
    }

    if (strcmp(action, "rolety") == 0)
    {
        // output should be all the blinds that needs to be moved [0,1,2,3,4,5]
        for (unsigned int y = 0; y < root["output"].size(); y++)
        {
            // set the blind to work with
            int blind = root["output"][y];
            Shutters *shutB = resolveShutter(blind);

            // prcnt should be between 0 (all the way up) and 100 (all the way down)
            uint8_t percentage = uint8_t(root["prcnt"]);
            // safeguard for overweighting prcnt value
            if (percentage > 100)
            {
                percentage = 100;
            }
            else if (percentage < 0)
            {
                percentage = 0;
            }

            // check whether the blind is running when button has been pressed again
            if ((*shutB).isIdle() != true && (percentage == 0 || percentage == 100))
            {
                (*shutB).stop();
                // TODO: Need to test if STATE is published here, otherwise need to add
                // to be removed when all working
                Serial.println("Sensed button pressed again hence stopping blind.");
                // end (to be removed)
            }
            else
            {
                // move the blind to the newly requested level
                (*shutB).setLevel(percentage);
            }
        }
    }

    // state handling
    if (strcmp(action, "state") == 0)
    {
        unsigned int output_size = root["output"].size();
        JsonArrayConst output_store = root["output"];

        for (unsigned int y = 0; y < output_size; y++)
        {
            // ! TODO: figure out how to process these, as second is sent as 0;
            const char *output_y = output_store[y];
            // publish state
            client.publish("STATE", output_y);
            // ... resubscribe
            client.subscribe(controllino);
            // when the request for state is for all devices connected
            if (strcmp(output_store[y], "all") == 0)
            {
                // output of all the digital outputs connected to controllino
                for (unsigned int st = 0; st < TOTAL_OUTPUT_DEF_ARRAY; st++)
                {
                    // get state of output/PIN
                    int output = OUTPUT_DEF_ARRAY[st];
                    String pin_state = getStateJSON(output);
                    // publish state
                    client.publish("STATE", (char *)pin_state.c_str());
                    // ... resubscribe
                    client.subscribe(controllino);
                }

                // output of all the blinds
                for (unsigned int st = 0; st < BLINDS_TOTAL; st++)
                {
                    // define blind_pin to get state for
                    int blind_pin = BLINDS[st];
                    String blind_state = getStateJSON(blind_pin);
                    // publish state
                    client.publish("STATE", (char *)blind_state.c_str());
                    // ... resubscribe
                    client.subscribe(controllino);
                }

                // when the request for state is for all blinds only
            }
            else if (strcmp(output_store[y], "blinds") == 0)
            {
                for (unsigned int st = 0; st < BLINDS_TOTAL; st++)
                {
                    // define blind_pin to get state for
                    int blind_pin = BLINDS[st];
                    String blind_state = getStateJSON(blind_pin);
                    // publish state
                    client.publish("STATE", (char *)blind_state.c_str());
                    // ... resubscribe
                    client.subscribe(controllino);
                }

                // when the request is for controllino only
            }
            else if (strcmp(output_store[y], "controllino") == 0 || strcmp(output_store[y], "whois") == 0)
            {
                // publish state
                client.publish("STATE", controllino);
                // ... resubscribe
                client.subscribe(controllino);

                // when the request for state is for few particular digital output pin(s)
            }
            else
            {
                Serial.println("Going through single pin state.");
                //const char * output = root["output"][y];
                // get state of output/PIN
                String pin_state = getStateJSON((int)output_y);
                // publish state
                client.publish("STATE", (char *)pin_state.c_str());
                // ... resubscribe
                client.subscribe(controllino);
            }
        }
    }

    if (strcmp(action, "info") == 0)
    {
        Serial.println("ACM1 reconnected...'info' command received.");
    }
}

boolean reconnect()
{
    // Serial.println("Attempting deviceACM1 - MQTT connection ...");
    if (client.connect("deviceACM1"))
    {
        // Once connected, publish an announcement...
        client.publish("ACM1", "{\"action\":\"info\"}");
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
        if (PUSH_BUTTONS_DEF[p].is(btn))
        {
            client.publish(PUSH_BUTTONS_ACT[0][p], PUSH_BUTTONS_ACT[1][p]);
            client.subscribe(controllino);

            Serial.println("PushButton from array pressed.");
        }
    }
}

void setup()
{
    // begin serial so we can see which buttons are being pressed through the serial monitor
    Serial.begin(9600);

    /* New shutters code */
    delay(100);
#ifdef ESP8266
    EEPROM.begin(512);
#endif
    /* New shutters code end */

    Serial.println("Init");

    // create MQTT
    client.setServer(server, 1883);
    client.setCallback(callback);

    client.subscribe(controllino);

    // Setup ethernet
    Ethernet.begin(mac, ip);
    delay(200);
    lastReconnectAttempt = 0;

    /* New shutters code */

    // Initialize shutters
    for (int s1 = 0; s1 < BLINDS_TOTAL; s1++)
    {
        // set the shutter to work with
        Shutters *shut1 = resolveShutter(s1);

        // get the last stored state of the shutter
        char storedShuttersState[(*shut1).getStateLength()];
        readInEeprom(storedShuttersState, (*shut1).getStateLength());

        // Initialize the shutter
        (*shut1)
            .setOperationHandler(shuttersOperationHandler)
            .setWriteStateHandler(shuttersWriteStateHandler)
            .restoreState(storedShuttersState)
            .setCourseTime(upCourseTime, downCourseTime)
            .onLevelReached(onShuttersLevelReached)
            .begin()
            .setLevel(100); // Go to 100% (all-the-way down)
    }
    /* New shutters code end */

    // Configure the button as you'd like - not necessary if you're happy with the defaults
    for (int p1 = 0; p1 < PUSH_BUTTONS_TOTAL; p1++)
    {
        PUSH_BUTTONS_DEF[p1].configureButton(configurePushButton);
    }

    // When the button is first pressed, call the function onButtonPressed (further down the page)
    for (int p2 = 0; p2 < PUSH_BUTTONS_TOTAL; p2++)
    {
        PUSH_BUTTONS_DEF[p2].onPress(checkPressedButton);
    }

    // initialize pinMode for blinds
    for (int pM = 0; pM < BLINDS_TOTAL; pM++)
    {
        pinMode(BLINDS[pM], OUTPUT);
        pinMode(BLINDS[pM] + 1, OUTPUT);
    }
}

void loop()
{
    // rolety / blinds - New shutters code
    for (int s2 = 0; s2 < BLINDS_TOTAL; s2++)
    {
        Shutters *shut2 = resolveShutter(s2);
        (*shut2).loop();
    }

    // Digital buttons
    for (int p3 = 0; p3 < PUSH_BUTTONS_TOTAL; p3++)
    {
        PUSH_BUTTONS_DEF[p3].update();
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
                client.publish(ANALOG_BUTTONS_ACT[0][aB2][aB], ANALOG_BUTTONS_ACT[1][aB2][aB]);
                client.subscribe(controllino);
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
