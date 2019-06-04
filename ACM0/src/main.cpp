
 
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


// MQTT setup
byte mac[] = {0xDE, 0xED, 0xBB, 0xFE, 0xAF, 0xAA};
IPAddress ip(192, 168, 69, 181);
IPAddress server(192, 168, 69, 10);

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;

// analog buttons' setup
// set how many buttons you have connected
const int BUTTONS_TOTAL = 7;
const int BUTTONS_VALUES_1[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};
const int BUTTONS_VALUES_2[BUTTONS_TOTAL] = {0, 6, 71, 143, 257, 406, 562};

// main toggle function for lights (and others)
void toggle(int pin)
{
    digitalWrite(pin, !digitalRead(pin));
}

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length) {
    // const size_t capacity = JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(2) + 50;
    // DynamicJsonBuffer jsonBuffer(capacity);

    // const char *json = "{\"action\":\"toggle\",\"output\":[1,2,3,4,9,9,9,9,9,9]}";
    // JsonObject& root = jsonBuffer.parseObject(json);

    // handle message arrived
    // Serial.print("[");
    // Serial.print(topic);
    // Serial.print("] ");
    // for (unsigned int i = 0; i < length; i++)
    // {
    //     Serial.print((char)payload[i]);
    // }
    // Serial.println();

    // JsonObject &root = jsonBuffer.parseObject(payload);
    // if (root.success()){

    const size_t capacity = JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument root(capacity);
    deserializeJson(root, payload);

    const char* action = root["action"];
    if (strcmp(action, "toggle") == 0)
    {
        for (unsigned int i = 0; i < root["output"].size(); i++)
        {
            const int button =  root["output"][i];
            if(button != 0 ) {
                toggle(button);
                Serial.print(button);
                Serial.print("--");
                Serial.println(digitalRead(button));
            }
        }
    }

    if (strcmp(action, "info") == 0)
    {
        Serial.println("ACM0 reconnected...'info' command received.");
    }

    //*********************************************************************************
    //********* The following is only temporary section for testing purposes! *********
    // mockup function to simulate button press through MQTT
    if (strcmp(action, "switch") == 0)
    {
        // dir tells the code what 'voltage' to push to analog input (max is 255)
        const int dir = root["dir"];
        const long dirMapped = map(dir, 0, 1022, 0, 255);
        // pin is PIN number based on the controllino.h scheme for (analog-)inputs
        const int MQTTpin = root["pin"];

        if (dir > 0)
        {
            analogWrite(MQTTpin, dirMapped); //max analogWrite is 255
        } else {
            digitalWrite(MQTTpin, LOW);
        }

        Serial.print("Expect pin #");
        Serial.print(MQTTpin);
        Serial.print(" read state: ");
        Serial.print(dirMapped);
        Serial.print("/");
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
    if (client.connect("deviceACM0"))
    {
        // Once connected, publish an announcement...
        client.publish("ACM0", "{\"action\":\"info\"}");
        // ... and resubscribe
        client.subscribe("ACM0");
    }
    return client.connected();
}


// you can also define constants for each of your buttons, which makes your code easier to read
// define these in the same order as the numbers in your BUTTONS_VALUES array, so whichever button has the smallest analogRead() number should come first

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttonsA0(CONTROLLINO_A0, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA1(CONTROLLINO_A1, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA2(CONTROLLINO_A2, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA4(CONTROLLINO_A4, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA13(CONTROLLINO_A13, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA14(CONTROLLINO_A14, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA15(CONTROLLINO_A15, BUTTONS_TOTAL, BUTTONS_VALUES_2);
// these cannot be defined within array, hence array of their pointers
AnalogMultiButton * ANALOG_BUTTONS_DEF[] = {
    &buttonsA0, &buttonsA1, &buttonsA2, &buttonsA4, &buttonsA13, &buttonsA14, &buttonsA15
};
const int ANALOG_BUTTONS_TOTAL = 7;
// analog buttons actor array (needs to be same order as ANALOG_BUTTONS_DEF)
const char* ANALOG_BUTTONS_ACT[2][ANALOG_BUTTONS_TOTAL][BUTTONS_TOTAL] = {
    { //section for topic to which we post MQTT message
        { //A0
            "",     //Button 0
            "ACM1", //Button 1
            "ACM1", //Button 2
            "ACM1", //Button 3
            "ACM1", //Button 4
            "ACM1", //Button 5
            "ACM1"  //Button 6
        },{ //A1
            "",     //Button 0
            "ACM1", //Button 1
            "ACM1", //Button 2
            "ACM1", //Button 3
            "ACM1", //Button 4
            "ACM1", //Button 5
            "ACM1"  //Button 6
        },{ //A2
            "",     //Button 0
            "ACM1", //Button 1
            "ACM1", //Button 2
            "ACM0", //Button 3
            "",     //Button 4
            "ACM1", //Button 5
            "ACM1"  //Button 6
        },{ //A4
            "",     //Button 0
            "ACM1", //Button 1
            "ACM1", //Button 2
            "ACM1", //Button 3
            "ACM1", //Button 4
            "ACM1", //Button 5
            "ACM1"  //Button 6
        },{ //A13
            "",     //Button 0
            "ACM1", //Button 1
            "ACM1", //Button 2
            "ACM1", //Button 3
            "ACM1", //Button 4
            "ACM1", //Button 5
            "ACM1"  //Button 6
        },{ //A14
            "",     //Button 0
            "ACM0", //Button 1
            "ACM0", //Button 2
            "ACM0", //Button 3
            "",     //Button 4
            "ACM0", //Button 5
            "ACM0"  //Button 6
        },{ //A15
            "",     //Button 0
            "ACM0", //Button 1
            "ACM0", //Button 2
            "ACM0", //Button 3
            "ACM0", //Button 4
            "ACM0", //Button 5
            "ACM0"  //Button 6
        }
    },
    { //section for actual message to post in MQTT 
        { //A0
            "",                                             //Button 0
            "{\"action\":\"toggle\",\"output\":[5, 6]}",    //Button 1
            "{\"action\":\"toggle\",\"output\":[5, 6]}",    //Button 2
            "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[2]}", //Button 3
            "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[2]}",   //Button 4
            "{\"action\":\"toggle\",\"output\":[11]}",      //Button 5
            "{\"action\":\"toggle\",\"output\":[9]}"        //Button 6
        },{ //A1
            "",                                             //Button 0
            "{\"action\":\"toggle\",\"output\":[5, 6]}",    //Button 1
            "{\"action\":\"toggle\",\"output\":[5, 6]}",    //Button 2
            "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[3]}", //Button 3
            "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[3]}",   //Button 4
            "{\"action\":\"toggle\",\"output\":[11]}",      //Button 5
            "{\"action\":\"toggle\",\"output\":[10]}"       //Button 6
        },{ //A2
            "",                                         //Button 0
            "{\"action\":\"toggle\",\"output\":[2]}",   //Button 1
            "{\"action\":\"toggle\",\"output\":[3]}",   //Button 2
            "{\"action\":\"toggle\",\"output\":[45]}",  //Button 3
            "",                                         //Button 4
            "{\"action\":\"toggle\",\"output\":[11]}",  //Button 5
            "{\"action\":\"toggle\",\"output\":[7]}"    //Button 6
        },{ //A4
            "",                                         //Button 0
            "{\"action\":\"toggle\",\"output\":[2]}",   //Button 1
            "{\"action\":\"toggle\",\"output\":[3]}",   //Button 2
            "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[1]}", //Button 3
            "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[1]}",   //Button 4
            "{\"action\":\"toggle\",\"output\":[7]}",   //Button 5
            "{\"action\":\"toggle\",\"output\":[42]}"   //Button 6
        },{ //A13
            "",                                         //Button 0
            "{\"action\":\"toggle\",\"output\":[13]}",  //Button 1
            "{\"action\":\"toggle\",\"output\":[13]}",  //Button 2
            "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[0]}", //Button 3
            "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[0]}",   //Button 4
            "{\"action\":\"toggle\",\"output\":[11]}",  //Button 5
            "{\"action\":\"toggle\",\"output\":[11]}"   //Button 6
        },{ //A14
            "",                                         //Button 0
            "{\"action\":\"toggle\",\"output\":[12]}",  //Button 1
            "{\"action\":\"toggle\",\"output\":[4]}",   //Button 2
            "{\"action\":\"toggle\",\"output\":[78]}",  //Button 3
            "",                                         //Button 4
            "{\"action\":\"toggle\",\"output\":[79]}",  //Button 5
            "{\"action\":\"toggle\",\"output\":[80]}"   //Button 6
        },{ //A15
            "",                                         //Button 0
            "{\"action\":\"toggle\",\"output\":[9]}",   //Button 1
            "{\"action\":\"toggle\",\"output\":[10]}",  //Button 2
            "{\"action\":\"toggle\",\"output\":[7]}",   //Button 3
            "{\"action\":\"toggle\",\"output\":[12]}",  //Button 4
            "{\"action\":\"toggle\",\"output\":[78]}",  //Button 5
            "{\"action\":\"toggle\",\"output\":[79]}"   //Button 6
        }
    }
};
//IT SEARCHES FOR POSSITION IN ARRAY BASED ON PROVIDED KEY.
//DEFINITION REQUIRES ARRAY NAME AND POINTER, AMOUNT OF VALUES IN THE ROW, SEARCHING KEY
int findAMBElement(AnalogMultiButton* array[], int lastElement, AnalogMultiButton* searchKey) { 
  int index = -1;
  for(int fE = 0; fE < lastElement; fE++) {
    if(array[fE] == searchKey) { 
        index = fE;
    }
  }
  return index;
}

void checkAnalogButtonsPress(AnalogMultiButton * buttonsObject) {
    // find pressed analog button object
    int pressedButtonsObject = findAMBElement(ANALOG_BUTTONS_DEF, ANALOG_BUTTONS_TOTAL, buttonsObject);
    AnalogMultiButton deRefObject = *buttonsObject;
    // cycle through all possible buttons
    for (unsigned int aB = 0; aB < BUTTONS_TOTAL; aB++) {
        if (deRefObject.onPress(aB)) {
            client.publish(ANALOG_BUTTONS_ACT[0][pressedButtonsObject][aB], ANALOG_BUTTONS_ACT[1][pressedButtonsObject][aB]);
            client.subscribe("ACM0");

            Serial.println("Button #");
            Serial.print(aB);
            Serial.print(" (aB) from switch array #");
            Serial.print(pressedButtonsObject);
            Serial.println(" has been pressed!");
            Serial.println("----");
            Serial.print("Publish topic: ");
            Serial.println(ANALOG_BUTTONS_ACT[0][pressedButtonsObject][aB]);
            Serial.print("Publish payload: ");
            Serial.println(ANALOG_BUTTONS_ACT[1][pressedButtonsObject][aB]);
            Serial.println("----");
            
        }
    }
}

// push buttons definition array
const int PUSH_BUTTONS_TOTAL = 6;
PushButton PUSH_BUTTONS_DEF[PUSH_BUTTONS_TOTAL] = {
    PushButton(CONTROLLINO_A5,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A6,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A7,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A8,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A9,  PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH)
};
// push buttons actor array (needs to be same order as PUSH_BUTTONS_DEF)
const char* PUSH_BUTTONS_ACT[2][PUSH_BUTTONS_TOTAL] = {
    { //section for topic to which we post in MQTT
        "ACM1", //A5
        "ACM1", //A6
        "ACM1", //A7
        "ACM1", //A8
        "ACM1", //A9
        "ACM0"  //A11
    },
    { //section for actual message to post in MQTT
        "{\"action\":\"toggle\",\"output\":[2]}",      //A5
        "{\"action\":\"toggle\",\"output\":[8]}",      //A6
        "{\"action\":\"toggle\",\"output\":[42]}",     //A7
        "{\"action\":\"toggle\",\"output\":[42]}",     //A8
        "{\"action\":\"toggle\",\"output\":[42]}",     //A9
        "{\"action\":\"toggle\",\"output\":[78]}",     //A11
    }
};


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
    for (unsigned int p = 0; p < PUSH_BUTTONS_TOTAL; p++)
    {
        if (PUSH_BUTTONS_DEF[p].is(btn))
        {
            client.publish(PUSH_BUTTONS_ACT[0][p], PUSH_BUTTONS_ACT[1][p]);
            client.subscribe("ACM0");

            Serial.print("PushButton #");
            Serial.print(p);
            Serial.println(" from pushButton array pressed.");
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

    client.subscribe("ACM0");

    // Setup ethernet 
    Ethernet.begin(mac, ip);
    delay(200);
    lastReconnectAttempt = 0;

    // Configure the button as you'd like - not necessary if you're happy with the defaults
    for (unsigned int p1 = 0; p1 < PUSH_BUTTONS_TOTAL; p1++)
    {
        PUSH_BUTTONS_DEF[p1].configureButton(configurePushButton);
    }

    // When the button is first pressed, call the function checkPressedPushButton (above)
    for (unsigned int p2 = 0; p2 < PUSH_BUTTONS_TOTAL; p2++)
    {
        PUSH_BUTTONS_DEF[p2].onPress(checkPressedPushButton);
    }
}




void loop()
{
    // Digital
    for (unsigned int p3 = 0; p3 < PUSH_BUTTONS_TOTAL; p3++)
    {
        PUSH_BUTTONS_DEF[p3].update();
    }

    // Analog
    for (unsigned int aB2 = 0; aB2 < ANALOG_BUTTONS_TOTAL; aB2++)
    {
        AnalogMultiButton AMBobject = *ANALOG_BUTTONS_DEF[aB2];
        AMBobject.update();
    }
    
    // Check if button is pressed
    for (unsigned int aB3 = 0; aB3 < ANALOG_BUTTONS_TOTAL; aB3++)
    {
        checkAnalogButtonsPress(ANALOG_BUTTONS_DEF[aB3]);
    }

/*
// Check if pressed
    if (buttonsA0.onPress(1))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[5, 6]}");
        client.subscribe("ACM0");
        Serial.println("Button A0-1 is pressed");
    }
// Check if pressed
    if (buttonsA0.onPress(2))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[5, 6]}");
        client.subscribe("ACM0");
        Serial.println("Button A0-2 is pressed");
    }
// Check if pressed
    if (buttonsA0.onPress(3))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[2]}");
        client.subscribe("ACM0");
        Serial.println("Button A0-3 is pressed");
    }
// Check if pressed
    if (buttonsA0.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[2]}");
        client.subscribe("ACM0");
        Serial.println("Button A0-4 is pressed");
    } 
// Check if pressed
    if (buttonsA0.onPress(5))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[11]}");
        client.subscribe("ACM0");
        Serial.println("Button A0-5 is pressed");
    }
// Check if pressed
    if (buttonsA0.onPress(6))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[9]}");
        client.subscribe("ACM0");
        Serial.println("Button A0-6 is pressed");
    } 
//*/
/*
// buttonsA1 
// Check if pressed
    if (buttonsA1.onPress(1))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[5, 6]}");
        client.subscribe("ACM0");
        Serial.println("Button A1-1 is pressed");
    }
// Check if pressed
    if (buttonsA1.onPress(2))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[5, 6]}");
        client.subscribe("ACM0");
        Serial.println("Button A1-2 is pressed");
    }
// Check if pressed
    if (buttonsA1.onPress(3))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[3]}");
        client.subscribe("ACM0");
        Serial.println("Button A1-3 is pressed");
    }
// Check if pressed
    if (buttonsA1.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[3]}");
        client.subscribe("ACM0");
        Serial.println("Button A1-4 is pressed");
    }
// Check if pressed
    if (buttonsA1.onPress(5))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[11]}");
        client.subscribe("ACM0");
        Serial.println("Button A1-5 is pressed");
    }
// Check if pressed
    if (buttonsA1.onPress(6))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[10]}");
        client.subscribe("ACM0");
        Serial.println("Button A1-6 is pressed");
    }
//*/
/*
// buttonsA2
// Check if pressed
    if (buttonsA2.onPress(1))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[2]}");
        client.subscribe("ACM0");
        Serial.println("Button A2-1 is pressed");
    }
// Check if pressed
    if (buttonsA2.onPress(2))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[3]}");
        client.subscribe("ACM0");
        Serial.println("Button A2-2 is pressed");
    }
// Check if pressed
    if (buttonsA2.onPress(3))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[45]}");
        client.subscribe("ACM0");
        Serial.println("Button A2-3 is pressed");
    }
// Check if pressed
    if (buttonsA2.onPress(4))
    {
        Serial.println("Button A2-4 is pressed");
    }
// Check if pressed
    if (buttonsA2.onPress(5))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[11]}");
        client.subscribe("ACM0");
        Serial.println("Button A2-5 is pressed");
    }
// Check if pressed
    if (buttonsA2.onPress(6))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[7]}");
        client.subscribe("ACM0");
        Serial.println("Button A2-6 is pressed");
    }
//*/
/*
// buttonsA4
// Check if pressed
    if (buttonsA4.onPress(1))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[2]}");
        client.subscribe("ACM0");
        Serial.println("Button A4-1 is pressed");
    }
// Check if pressed
    if (buttonsA4.onPress(2))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[3]}");
        client.subscribe("ACM0");
        Serial.println("Button A4-2 is pressed");
    }
// Check if pressed
    if (buttonsA4.onPress(3))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[1]}");
        client.subscribe("ACM0");
        Serial.println("Button A4-3 is pressed");
    }
// Check if pressed
    if (buttonsA4.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[1]}");
        client.subscribe("ACM0");
        Serial.println("Button A4-4 is pressed");
    }
// Check if pressed
    if (buttonsA4.onPress(5))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[7]}");
        client.subscribe("ACM0");
        Serial.println("Button A4-5 is pressed");
    }
// Check if pressed
    if (buttonsA4.onPress(6))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[42]}");
        client.subscribe("ACM0");
        Serial.println("Button A4-6 is pressed");
    }
//*/
/*
// buttonsA13
// Check if pressed
    if (buttonsA13.onPress(1))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[13]}");
        client.subscribe("ACM0");
        Serial.println("Button A13-1 is pressed");
    }
// Check if pressed
    if (buttonsA13.onPress(2))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[13]}");
        client.subscribe("ACM0");
        Serial.println("Button A13-2 is pressed");
    }
// Check if pressed
    if (buttonsA13.onPress(3))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"down\",\"output\":[0]}");
        client.subscribe("ACM0");
        Serial.println("Button A13-3 is pressed");
    }
// Check if pressed
    if (buttonsA13.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"direction\":\"up\",\"output\":[0]}");
        client.subscribe("ACM0");
        Serial.println("Button A13-4 is pressed");
    }
// Check if pressed
    if (buttonsA13.onPress(5))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[11]}");
        client.subscribe("ACM0");
        Serial.println("Button A13-5 is pressed");
    }
// Check if pressed
    if (buttonsA13.onPress(6))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[11]}");
        client.subscribe("ACM0");
        Serial.println("Button A13-6 is pressed");
    }
//*/
/*
// buttonsA14
// Check if pressed
    if (buttonsA14.onPress(1))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[12]}");
        client.subscribe("ACM0");
        Serial.println("Button A14-1 is pressed");
    }
// Check if pressed
    if (buttonsA14.onPress(2))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[4]}");
        client.subscribe("ACM0");
        Serial.println("Button A14-2 is pressed");
    }
// Check if pressed
    if (buttonsA14.onPress(3))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[78]}");
        client.subscribe("ACM0");
        Serial.println("Button A14-3 is pressed");
    }
// Check if pressed
    if (buttonsA14.onPress(4))
    {
        Serial.println("Button A14-4 is pressed");
    }
// Check if pressed
    if (buttonsA14.onPress(5))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[79]}");
        client.subscribe("ACM0");
        Serial.println("Button A14-5 is pressed");
    }
// Check if pressed
    if (buttonsA14.onPress(6))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[80]}");
        client.subscribe("ACM0");
        Serial.println("Button A14-6 is pressed");
    }
//*/
/*
// buttonsA15
// Check if pressed
    if (buttonsA15.onPress(1))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[9]}");
        client.subscribe("ACM0");
        Serial.println("Button A15-1 is pressed");
    }
// Check if pressed
    if (buttonsA15.onPress(2))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[10]}");
        client.subscribe("ACM0");
        Serial.println("Button A15-2 is pressed");
    }
// Check if pressed
    if (buttonsA15.onPress(3))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[7]}");
        client.subscribe("ACM0");
        Serial.println("Button A15-3 is pressed");
    }
// Check if pressed
    if (buttonsA15.onPress(4))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[12]}");
        client.subscribe("ACM0");
        Serial.println("Button A15-4 is pressed");
    }
// Check if pressed
    if (buttonsA15.onPress(5))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[78]}");
        client.subscribe("ACM0");
        Serial.println("Button A15-5 is pressed");
    }
// Check if pressed
    if (buttonsA15.onPress(6))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[79]}");
        client.subscribe("ACM0");
        Serial.println("Button A15-6 is pressed");
    }
//*/
 
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
