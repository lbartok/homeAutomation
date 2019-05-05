
 
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
// const int BUTTONS_TOTAL = 7;

byte mac[] = {0xDE, 0xED, 0xBB, 0xFE, 0xAF, 0xAA};
IPAddress ip(192, 168, 69, 181);
IPAddress server(192, 168, 69, 10);

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;


// set how many buttons you have connected

const int BUTTONS_TOTAL = 7;
const int BUTTONS_VALUES_1[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};

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

    // const char* json = "{\"action\":\"toggle\",\"output\":[1,2,3,4,9,9,9,9,9,9]}";

    deserializeJson(root, payload);

    const char* action = root["action"]; // "toggle"

        // const char* action = root["action"];
        Serial.println(action);



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
        Serial.println("-----------------------");
    // }

    

    // payload[length] = '\0';
    // String s = String((char *)payload);
    // Serial.println(s);
    
    Serial.println("-------------------------");
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
AnalogMultiButton buttonsA15(CONTROLLINO_A15, BUTTONS_TOTAL, BUTTONS_VALUES_1);

PushButton buttonA5 = PushButton(CONTROLLINO_A5, PRESSED_WHEN_HIGH);
PushButton buttonA6 = PushButton(CONTROLLINO_A6, PRESSED_WHEN_HIGH);
PushButton buttonA7 = PushButton(CONTROLLINO_A7, PRESSED_WHEN_HIGH);
PushButton buttonA8 = PushButton(CONTROLLINO_A8, PRESSED_WHEN_HIGH);
PushButton buttonA9 = PushButton(CONTROLLINO_A9, PRESSED_WHEN_HIGH);
PushButton buttonA11 = PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH);

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
void onButtonPressedA5(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[2]}");
    client.subscribe("ACM0");
    Serial.println(" button A5 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA6(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[8]}");
    client.subscribe("ACM0");
    Serial.println(" button A6 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA7(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[42]}");
    client.subscribe("ACM0");
    Serial.println(" button A7 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA8(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[42]}");
    client.subscribe("ACM0");
    Serial.println(" button A8 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA9(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[42]}");
    client.subscribe("ACM0");
    Serial.println(" button A9 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA11(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[78]}");
    client.subscribe("ACM0");
    Serial.println(" button A9 pressed");
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
    buttonA5.configureButton(configurePushButton);
    buttonA6.configureButton(configurePushButton);
    buttonA7.configureButton(configurePushButton);
    buttonA8.configureButton(configurePushButton);
    buttonA9.configureButton(configurePushButton);
    buttonA11.configureButton(configurePushButton);

    // When the button is first pressed, call the function onButtonPressed (further down the page)
    buttonA5.onPress(onButtonPressedA5);
    buttonA6.onPress(onButtonPressedA6);
    buttonA7.onPress(onButtonPressedA7);
    buttonA8.onPress(onButtonPressedA8);
    buttonA9.onPress(onButtonPressedA9);
    buttonA11.onPress(onButtonPressedA11);
}

void loop()
{
    // Analog
    buttonsA0.update();
    buttonsA1.update(); 
    buttonsA2.update(); 
    buttonsA4.update(); 
    buttonsA13.update(); 
    buttonsA14.update(); 
    buttonsA15.update(); 
    
    // Digital
    buttonA5.update(); // D9 @ACM0
    buttonA6.update(); // D1 @ACM0
    buttonA7.update();
    buttonA8.update();
    buttonA9.update();
    buttonA11.update();

// buttonsA0
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
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z3dole\"]}");
        client.subscribe("ACM0");
        Serial.println("Button A0-3 is pressed");
    }
// Check if pressed
    if (buttonsA0.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z3hore\"]}");
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
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z4dole\"]}");
        client.subscribe("ACM0");
        Serial.println("Button A1-3 is pressed");
    }
// Check if pressed
    if (buttonsA1.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z4hore\"]}");
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
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z2dole\"]}");
        client.subscribe("ACM0");
        Serial.println("Button A4-3 is pressed");
    }
// Check if pressed
    if (buttonsA4.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z2hore\"]}");
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
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z1hore\"]}");
        client.subscribe("ACM0");
        Serial.println("Button A13-3 is pressed");
    }
// Check if pressed
    if (buttonsA13.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"rolety\",\"output\":[\"Z1dole\"]}");
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
