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
void shuttersOperationHandler(Shutters* s, ShuttersOperation operation) {
    for (int s0 = 0; s0 < BLINDS_TOTAL; s0++) {
        Shutters* shut0 = blindsArray[s0];

        if (s == shut0) {
            // this callback was called from the shutters[s0]
            controllPin = BLINDS[s0];
            directionPin = controllPin + 1;
        }
    }

    switch (operation) {
        case ShuttersOperation::UP:
        Serial.println("Shutters going up.");
        // TODO: Implement the code for the shutters to go up
        digitalWrite(directionPin, LOW);
        digitalWrite(controllPin, HIGH);
        break;
        case ShuttersOperation::DOWN:
        Serial.println("Shutters going down.");
        // TODO: Implement the code for the shutters to go down
        digitalWrite(directionPin, HIGH);
        digitalWrite(controllPin, HIGH);
        break;
        case ShuttersOperation::HALT:
        Serial.println("Shutters halting.");
        // TODO: Implement the code for the shutters to halt
        digitalWrite(directionPin, LOW);
        digitalWrite(controllPin, LOW);
        break;
    }
}

void readInEeprom(char* dest, byte length) {
    for (byte i = 0; i < length; i++) {
        dest[i] = EEPROM.read(eepromOffset + i);
    }
}

void shuttersWriteStateHandler(Shutters* shutters, const char* state, byte length) {
    for (byte i = 0; i < length; i++) {
        EEPROM.write(eepromOffset + i, state[i]);
        #ifdef ESP8266
        EEPROM.commit();
        #endif
    }
}

void onShuttersLevelReached(Shutters* shutters, byte level) {
    for (int s3 = 0; s3 < BLINDS_TOTAL; s3++) {
        Shutters* shut3 = blindsArray[s3];

        if (shutters == shut3) {
            Serial.print(s3);
            Serial.print("> Shutter at ");
            Serial.print(level);
            Serial.println("%");
        }
    }
}


/* New shutter code end */

// main toggle function for lights (and others)
void toggle(int pin)
{
    digitalWrite(pin, !digitalRead(pin));
}

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length) {

    const size_t capacity = JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument root(capacity);
    deserializeJson(root, payload);

    const char* action = root["action"];
    if (strcmp(action, "toggle") == 0)
    {
        for (unsigned int i = 0; i < root["output"].size(); i++)
        {
            const int button = root["output"][i];
            if (button != 0)
            {
                toggle(button);
                Serial.print(button);
                Serial.print("--");
                Serial.println(digitalRead(button));
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
            Shutters* shutB = blindsArray[blind];
            
            // prcnt should be between 0 (all the way up) and 100 (all the way down)
            int percentage = root["prcnt"];
            // safeguard for overweighting prcnt value
            if (percentage > 100) {
                percentage = 100;
            } else if (percentage < 0) {
                percentage = 0;
            }

            (*shutB).setLevel(percentage);
        }
    }

    if (strcmp(action, "info") == 0)
    {
        Serial.println("ACM1 reconnected...'info' command received.");
    }
}

boolean reconnect() {
    // Serial.println("Attempting deviceACM1 - MQTT connection ...");
    if (client.connect("deviceACM1"))
    {
        // Once connected, publish an announcement...
        client.publish("ACM1", "{\"action\":\"info\"}");
        // ... and resubscribe
        client.subscribe("ACM1");
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
            client.subscribe("ACM1");

            Serial.println("PushButton from array pressed.");
        }
    }
}



void setup()
{
    // begin serial so we can see which buttons are being pressed through the serial monitor
    Serial.begin(9600);
    Serial.println("Init");

    // create MQTT
    client.setServer(server, 1883);
    client.setCallback(callback);

    client.subscribe("ACM1");

    // Setup ethernet 
    Ethernet.begin(mac, ip);
    delay(200);
    lastReconnectAttempt = 0;

    /* New shutters code */
    #ifdef ESP8266
    EEPROM.begin(512);
    #endif

    // initialize shutters
    for (int s1 = 0; s1 < BLINDS_TOTAL; s1++) {
        Shutters* shut1 = blindsArray[s1];

        char storedShuttersState[(*shut1).getStateLength()];
        readInEeprom(storedShuttersState, (*shut1).getStateLength());

        (*shut1)
            .setOperationHandler(shuttersOperationHandler)
            .setWriteStateHandler(shuttersWriteStateHandler)
            .restoreState(storedShuttersState)
            .setCourseTime(upCourseTime, downCourseTime)
            .onLevelReached(onShuttersLevelReached)
            .begin();
        (*shut1).setLevel(0); // Go to 0% (hopefully all-the-way up)
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
    for (int pM = 0; pM < BLINDS_TOTAL; pM++) {
        pinMode(BLINDS[pM], OUTPUT);
        pinMode(BLINDS[pM] + 1, OUTPUT);
    }
}


void loop()
{
    // rolety / blinds
    /* New shutters code */
    for (int s2 = 0; s2 < BLINDS_TOTAL; s2++) {
        Shutters* shut2 = blindsArray[s2];
        (*shut2).loop();
    }
    /* New shutters code end */

    // Digital
    for (int p3 = 0; p3 < PUSH_BUTTONS_TOTAL; p3++)
    {
        PUSH_BUTTONS_DEF[p3].update();
    }
    
    // Analog
    buttonsA6.update();
    buttonsA7.update();
    
    // Check if analog button is pressed
    for (int aB2 = 0; aB2 < ANALOG_BUTTONS_TOTAL; aB2++)
    {
        AnalogMultiButton AMBobject = *ANALOG_BUTTONS_DEF[aB2];
        // cycle through all possible buttons
        for (int aB = 0; aB < BUTTONS_TOTAL; aB++) {
            if (AMBobject.onPress(aB)) {
                client.publish(ANALOG_BUTTONS_ACT[0][aB2][aB], ANALOG_BUTTONS_ACT[1][aB2][aB]);
                client.subscribe("ACM1");
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
