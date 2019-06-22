
 
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


// MQTT setup
byte mac[] = {0xDE, 0xED, 0xBB, 0xFE, 0xAF, 0xBB};
IPAddress ip(192, 168, 69, 180);
IPAddress server(192, 168, 69, 10);

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;

// analog buttons' setup
// set how many buttons you have connected
const int BUTTONS_TOTAL = 7;
const int BUTTONS_VALUES_1[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};
const int BUTTONS_VALUES_2[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};

// Blinds variables
const char* direction = "up";
long percent = -1;
long workingTimePercent;
bool tiltBlind = 0;
// set maxTime variable to max time needed to get blinds all the way
long maxTime = 65535; // max time used for calculating working time
long workingTime = maxTime;
// set how many blinds there are on the house and initialize Neotimer and symbol array
const int BLINDS_TOTAL = 6;
Neotimer roletyTimer[BLINDS_TOTAL];
int symbol[BLINDS_TOTAL];
// blinds timer storage where 0 is all the way UP and 100 all the way DOWN
long blindsTimerStorage[2][BLINDS_TOTAL];
// set blinds' controll pin array
const int BLINDS[BLINDS_TOTAL] = {
    CONTROLLINO_R2, CONTROLLINO_R4, CONTROLLINO_R6, CONTROLLINO_R8,
    CONTROLLINO_R10, CONTROLLINO_R12
};

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
            // legacy check - can be removed as it should not be used anymore
            // check for existence of 'time' parameter in JSON
            if(root.containsKey("time")){
                workingTime = root["time"];
                Serial.print("TimeExist and it is: ");
                Serial.println(workingTime);
            } else {
                workingTime = maxTime;
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
                workingTime = symbol[blind] * (workingTimePercent / 100) * maxTime;
                blindsTimerStorage[0][blind] = percent;
                blindsTimerStorage[1][blind] += (workingTime * symbol[blind]);
            }
            roletyTimer[blind] = Neotimer(workingTime);
            roletyTimer[blind].start();

            // check if the blind is off
            if (digitalRead(controllPin) == LOW)
            {
                // check desired direction and set the PIN correctly
                if (strcmp(direction, "up") == 0) 
                {
                    directionPinStatus = LOW;
                } else {
                    directionPinStatus = HIGH;                    
                }
                // set the controll PIN status to HIGH
                controllPinStatus = HIGH;
            } else {
                // clear the PINs and turn the blind off
                directionPinStatus = LOW;
                controllPinStatus = LOW;
                blindsTimerStorage[1][blind] -= blindsTimerStorage[1][blind] - (symbol[blind] * roletyTimer[blind].stop()); // amount of milis blinds ran
                blindsTimerStorage[0][blind] = (blindsTimerStorage[1][blind] / maxTime) * 100;
                roletyTimer[blind].reset();
                // for serial troubleshooting purposes only
                Serial.print("Blind #");
                Serial.print(blind);
                Serial.print(" stopped at ");
                Serial.print(blindsTimerStorage[1][blind]);
                Serial.println("ms / ");
                Serial.print(blindsTimerStorage[0][blind]);
                Serial.println("% and timer is reset.");
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
    //*********************************************************************************
    //********* The following is only temporary section for testing purposes! *********
    // mockup function to simulate button press through MQTT
    if (strcmp(action, "switch") == 0)
    {
        // dir tells the code whether to turn ON (1) or OFF (0)
        const int dir = root["dir"];
        // pin is PIN number based on the controllino.h scheme for (analog-)inputs
        const int MQTTpin = root["pin"];

        if (dir >= 1)
        {
            analogWrite(MQTTpin, 140); //max analogWrite is 255
        } else {
            digitalWrite(MQTTpin, LOW);
        }

        Serial.print("Expect pin #");
        Serial.print(MQTTpin);
        Serial.print(" read state: ");
        Serial.println(dir);
        Serial.print(MQTTpin);
        Serial.print(" read state: (analog) ");
        Serial.print(analogRead(MQTTpin));
        Serial.print(" / (digital) ");
        Serial.println(digitalRead(MQTTpin));
        Serial.println("----------------------------");
    }
    //**********************End of section for testing purposes.*********************
    //*******************************************************************************
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


// you can also define constants for each of your buttons, which makes your code easier to read
// define these in the same order as the numbers in your BUTTONS_VALUES array, so whichever button has the smallest analogRead() number should come first

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttonsA6(CONTROLLINO_A6, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA7(CONTROLLINO_A7, BUTTONS_TOTAL, BUTTONS_VALUES_1);
// these cannot be defined within array, hence array of their pointers
AnalogMultiButton * ANALOG_BUTTONS_DEF[] = {
    &buttonsA6, &buttonsA7
};
const int ANALOG_BUTTONS_TOTAL = 2;
// analog buttons actor array (needs to be same order as ANALOG_BUTTONS_DEF)
const char* ANALOG_BUTTONS_ACT[2][ANALOG_BUTTONS_TOTAL][BUTTONS_TOTAL] = {
    { //section for topic to which we post MQTT message
        { //A6
            "",     //Button 0
            "ACM0", //Button 1
            "ACM0", //Button 2
            "ACM1", //Button 3
            "ACM1", //Button 4
            "ACM0", //Button 5
            "ACM0"  //Button 6
        },{ //A7
            "",     //Button 0
            "ACM0", //Button 1
            "ACM0", //Button 2
            "ACM1", //Button 3
            "ACM1", //Button 4
            "ACM0", //Button 5
            "ACM0"  //Button 6
        }
    },
    { //section for actual message to post in MQTT 
        { //A6
            "",                                             //Button 0
            "{\"action\":\"toggle\",\"output\":[9]}",       //Button 1
            "{\"action\":\"toggle\",\"output\":[10]}",      //Button 2
            "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[4, 5]}", //Button 3
            "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[4, 5]}",   //Button 4
            "{\"action\":\"toggle\",\"output\":[12]}",      //Button 5
            "{\"action\":\"toggle\",\"output\":[7]}"        //Button 6
        },{ //A7
            "",                                             //Button 0
            "{\"action\":\"toggle\",\"output\":[13, 44]}",  //Button 1
            "{\"action\":\"toggle\",\"output\":[12]}",      //Button 2
            "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[4, 5]}", //Button 3
            "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[4, 5]}",   //Button 4
            "{\"action\":\"toggle\",\"output\":[80]}",      //Button 5
            "{\"action\":\"toggle\",\"output\":[8, 9]}"     //Button 6
        }
    }
};

// push buttons definition array
const int PUSH_BUTTONS_TOTAL = 14;
PushButton PUSH_BUTTONS_DEF[PUSH_BUTTONS_TOTAL] = {
    PushButton(CONTROLLINO_IN0, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_IN1, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A5,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A8,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A9,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A10, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A12, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A13, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A14, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A15, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_I16, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_I17, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_I18, PRESSED_WHEN_HIGH)    
};
// push buttons actor array (needs to be same order as PUSH_BUTTON_DEF)
const char* PUSH_BUTTONS_ACT[PUSH_BUTTONS_TOTAL][PUSH_BUTTONS_TOTAL] = {
    {
        "ACM0", //IN0
        "ACM0", //IN1
        "ACM0", //A5
        "ACM0", //A8
        "ACM0", //A9
        "ACM0", //A10
        "ACM0", //A11
        "ACM0", //A12
        "ACM0", //A13
        "ACM1", //A14
        "ACM0", //A15
        "ACM1", //I16
        "ACM1", //I17
        "ACM0" //I18
    },
    {
        "{\"action\":\"toggle\",\"output\":[11]}",      //IN0
        "{\"action\":\"toggle\",\"output\":[3]}",       //IN1
        "{\"action\":\"toggle\",\"output\":[78]}",      //A5
        "{\"action\":\"toggle\",\"output\":[6]}",       //A8
        "{\"action\":\"toggle\",\"output\":[2]}",       //A9
        "{\"action\":\"toggle\",\"output\":[5]}",       //A10
        "{\"action\":\"toggle\",\"output\":[7]}",       //A11
        "{\"action\":\"toggle\",\"output\":[7]}",       //A12
        "{\"action\":\"toggle\",\"output\":[13, 44]}",  //A13
        "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[0, 1, 2, 3, 4, 5]}", //A14
        "{\"action\":\"toggle\",\"output\":[45]}",      //A15
        "{\"action\":\"toggle\",\"output\":[3]}",       //I16
        "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[0, 1, 2, 3, 4, 5]}", //I17
        "{\"action\":\"toggle\",\"output\":[12]}"       //I18
    }
};

// pass a fourth parameter to set the debounce time in milliseconds
// this defaults to 20 and can be increased if you're working with particularly bouncy buttons

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
    for (unsigned int p = 0; p < PUSH_BUTTONS_TOTAL; p++)
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
    for (unsigned int p1 = 0; p1 < PUSH_BUTTONS_TOTAL; p1++)
    {
        PUSH_BUTTONS_DEF[p1].configureButton(configurePushButton);
    }

    // When the button is first pressed, call the function onButtonPressed (further down the page)
    for (unsigned int p2 = 0; p2 < PUSH_BUTTONS_TOTAL; p2++)
    {
        PUSH_BUTTONS_DEF[p2].onPress(checkPressedButton);
    }
    
    // initialize pinMode for blinds
    for (unsigned int pM = 0; pM < BLINDS_TOTAL; pM++) {
        pinMode(BLINDS[pM], OUTPUT);
        pinMode(BLINDS[pM] + 1, OUTPUT);
    }
}


void loop()
{
    // rolety / blinds
    // check if the timer has finished and turn appropriate PIN status to LOW
    for (unsigned int b = 0;b < BLINDS_TOTAL; b++)
    {
        if (roletyTimer[b].done() && digitalRead(BLINDS[b]) != LOW) 
        {
            digitalWrite(BLINDS[b], LOW);
            digitalWrite(BLINDS[b] + 1, LOW);
            if (tiltBlind) {
                const char* publishBlindMessage = "";
                publishBlindMessage = strcat(b, "{\"action\":\"rolety\",\"direction\":\"up\",\"time\":1000,\"output\":[");
                publishBlindMessage = strcat("]}", publishBlindMessage);
                Serial.println("Tilting blind with:");
                Serial.println(publishBlindMessage);
                client.publish("ACM1", publishBlindMessage);
                client.subscribe("ACM1");
                tiltBlind = 0;
            }
        }
    }

    // Digital
    for (unsigned int p3 = 0; p3 < PUSH_BUTTONS_TOTAL; p3++)
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
