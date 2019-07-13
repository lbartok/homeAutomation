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
// timer functionality
#include <neotimer.h>
// settings for individual home
#include <SettingsRez.h>




EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;

/* Moved to SettingsRez.h */
// Blinds variables
const char* direction = "up";
long percent = -1;
long workingTimePercent;
bool tiltBlind = 0;
// set MAX_TIME variable to max time needed to get blinds all the way
long MAX_TIME = 65535; // max time used for calculating working time
long workingTime = MAX_TIME;
/* end */

/* Moved to SettingsRez.h */
// set how many blinds there are on the house and initialize Neotimer and symbol array
const int BLINDS_TOTAL = 6;
Neotimer roletyTimer[BLINDS_TOTAL];
/* end */

int symbol[BLINDS_TOTAL];
// blinds timer storage where 0 is all the way UP and 100 all the way DOWN
long blindsTimerStorage[2][BLINDS_TOTAL];

/* Moved to SettingsRez.h */
// set blinds' controll pin array
const int BLINDS[BLINDS_TOTAL] = {
    CONTROLLINO_R2, CONTROLLINO_R4, CONTROLLINO_R6, CONTROLLINO_R8,
    CONTROLLINO_R10, CONTROLLINO_R12
};
/* end */

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
        // check for existence of 'percnt' parameter in JSON
        if(root.containsKey("percnt")){
            percent = root["percnt"];
            Serial.print("PercntExist and it is translated to: ");
            Serial.print(percent);
        } else {
            // legacy check - for push buttons to start/stop blinds
            // check for existence of 'time' parameter in JSON
            if(root.containsKey("time")){
                workingTime = root["time"];
                Serial.print("TimeExist and it is: ");
                Serial.println(workingTime);
            } else {
                workingTime = MAX_TIME;
                Serial.print("TimeNotExist and it is: ");
                Serial.println(workingTime);
            }
            // re-instantiate percent to -1 for the program to know it is not used
            percent = -1;
        }

        // direction would either be "up" or "down"
        // for manual button purposes
        if (root.containsKey("direction")) {
            direction = root["direction"];
        }
        // initialize PinStatus values
        bool controllPinStatus = LOW;
        bool directionPinStatus = LOW;

        // output should be all the blinds that needs to be moved [0,1,2,3,4,5]
        for (unsigned int y = 0; y < root["output"].size(); y++)
        {
            int blind = root["output"][y];
            // set controll and direction pin - this can be modified if we expand the array
            unsigned int controllPin = BLINDS[blind];
            unsigned int directionPin = controllPin + 1;

            // check if the blind is off
            if (digitalRead(controllPin) == LOW)
            {
                // set the timer for the current blind and start it
                if (percent >= 0) {
                    workingTimePercent = percent - blindsTimerStorage[0][blind];
                    if (workingTimePercent > 0) {
                        // blinds need to go down
                        direction = "down";
                        symbol[blind] = 1;
                        // tilt blind so they stay open and visible through
                        if (percent < 100) {
                            tiltBlind = 1;
                        } else {
                            tiltBlind = 0;
                        }
                    } else if (workingTimePercent < 0) {
                        // blinds need to go up
                        direction = "up";
                        symbol[blind] = -1;
                    } else {
                        // prevention of error during division
                        workingTimePercent = 1;
                        symbol[blind] = 0;
                    }
                    workingTime = symbol[blind] * (workingTimePercent / 100) * MAX_TIME;
                    blindsTimerStorage[0][blind] = percent;
                    blindsTimerStorage[1][blind] += (workingTime * symbol[blind]);
                }
                // set the timer for the current blind and start it
                roletyTimer[blind] = Neotimer(workingTime);
                roletyTimer[blind].start();

                // check desired direction and set the PIN correctly
                if (strcmp(direction, "up") == 0) 
                {
                    directionPinStatus = LOW;
                    if (symbol[blind] == 0) {
                        symbol[blind] = -1;
                    }
                } else {
                    directionPinStatus = HIGH;
                    if (symbol[blind] == 0) {
                        symbol[blind] = 1;
                    }
                }
                // set the controll PIN status to HIGH
                controllPinStatus = HIGH;
            } else {
                // clear the PINs and turn the blind off
                directionPinStatus = LOW;
                controllPinStatus = LOW;
                long timerRunTime = roletyTimer[blind].stop();
                Serial.print("Timer stopped with: ");
                Serial.print(timerRunTime);
                Serial.println("ms.");
                blindsTimerStorage[1][blind] = blindsTimerStorage[1][blind] - (blindsTimerStorage[1][blind] - (symbol[blind] * timerRunTime)); // amount of milis blinds ran
                blindsTimerStorage[0][blind] = (blindsTimerStorage[1][blind] / MAX_TIME) * 100;
                roletyTimer[blind].reset();
                symbol[blind] = 0;
                // for serial troubleshooting purposes only
                Serial.print("Blind #");
                Serial.print(blind);
                Serial.print(" stopped at ");
                Serial.print(blindsTimerStorage[1][blind]);
                Serial.print("ms / ");
                Serial.print(blindsTimerStorage[0][blind]);
                Serial.println("%.");
            }

            // set the controllino outputs with the correct status
            digitalWrite(directionPin, directionPinStatus);
            digitalWrite(controllPin, controllPinStatus);
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
    // check if the timer has finished and turn appropriate PIN status to LOW
    for (int b = 0;b < BLINDS_TOTAL; b++)
    {
        if (roletyTimer[b].done() && digitalRead(BLINDS[b]) != LOW) 
        {
            digitalWrite(BLINDS[b], LOW);
            digitalWrite(BLINDS[b] + 1, LOW);
        }
    }

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
