
 
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

byte mac[] = {0xDE, 0xED, 0xBB, 0xFE, 0xAF, 0xBB};
IPAddress ip(192, 168, 1, 80);
IPAddress server(192, 168, 1, 10);

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
    const size_t capacity = JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(2) + 50;
    DynamicJsonBuffer jsonBuffer(capacity);

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

    JsonObject &root = jsonBuffer.parseObject(payload);
    if (root.success()){
        const char* action = root["action"];
        Serial.println(action);

        if (action == "toggle")
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

        if (action == "roletyHore")
        {
            digitalWrite(CONTROLLINO_R2, HIGH);
            digitalWrite(CONTROLLINO_R3, LOW);
            digitalWrite(CONTROLLINO_R4, HIGH);
            digitalWrite(CONTROLLINO_R5, LOW);
            digitalWrite(CONTROLLINO_R6, HIGH);
            digitalWrite(CONTROLLINO_R7, LOW);
            digitalWrite(CONTROLLINO_R8, HIGH);
            digitalWrite(CONTROLLINO_R9, LOW);
            digitalWrite(CONTROLLINO_R10, HIGH);
            digitalWrite(CONTROLLINO_R11, LOW);
            digitalWrite(CONTROLLINO_R12, HIGH);
            digitalWrite(CONTROLLINO_R13, LOW);
            Serial.println("R Hore");
        }
        if (action == "roletyDole")
        {
            digitalWrite(CONTROLLINO_R2, HIGH);
            digitalWrite(CONTROLLINO_R3, HIGH);
            digitalWrite(CONTROLLINO_R4, HIGH);
            digitalWrite(CONTROLLINO_R5, HIGH);
            digitalWrite(CONTROLLINO_R6, HIGH);
            digitalWrite(CONTROLLINO_R7, HIGH);
            digitalWrite(CONTROLLINO_R8, HIGH);
            digitalWrite(CONTROLLINO_R9, HIGH);
            digitalWrite(CONTROLLINO_R10, HIGH);
            digitalWrite(CONTROLLINO_R11, HIGH);
            digitalWrite(CONTROLLINO_R12, HIGH);
            digitalWrite(CONTROLLINO_R13, HIGH);
            Serial.println("R Dole");
        }

        Serial.println("--------- ----------- Success -----------------------");
    } else {
        Serial.println("--------- ----------- FAIL!!! -----------------------");
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


// you can also define constants for each of your buttons, which makes your code easier to read
// define these in the same order as the numbers in your BUTTONS_VALUES array, so whichever button has the smallest analogRead() number should come first

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttonsA6(CONTROLLINO_A6, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA7(CONTROLLINO_A7, BUTTONS_TOTAL, BUTTONS_VALUES_1);

PushButton buttonIN0 = PushButton(CONTROLLINO_IN0, PRESSED_WHEN_HIGH);
PushButton buttonIN1 = PushButton(CONTROLLINO_IN1, PRESSED_WHEN_HIGH);
PushButton buttonA8  = PushButton(CONTROLLINO_A8,  PRESSED_WHEN_HIGH);
PushButton buttonA9  = PushButton(CONTROLLINO_A9,  PRESSED_WHEN_HIGH);
PushButton buttonA10 = PushButton(CONTROLLINO_A10, PRESSED_WHEN_HIGH);
PushButton buttonA11 = PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH);
PushButton buttonA12 = PushButton(CONTROLLINO_A12, PRESSED_WHEN_HIGH);
PushButton buttonA13 = PushButton(CONTROLLINO_A13, PRESSED_WHEN_HIGH);
PushButton buttonA14 = PushButton(CONTROLLINO_A14, PRESSED_WHEN_HIGH);
PushButton buttonA15 = PushButton(CONTROLLINO_A15, PRESSED_WHEN_HIGH);
PushButton buttonI16 = PushButton(CONTROLLINO_I16, PRESSED_WHEN_HIGH);
PushButton buttonI17 = PushButton(CONTROLLINO_I17, PRESSED_WHEN_HIGH);
PushButton buttonI18 = PushButton(CONTROLLINO_I18, PRESSED_WHEN_HIGH);

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
void onButtonPressedIN0(Button &btn)
{
    Serial.println("button pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedIN1(Button &btn)
{
    Serial.println("button pressed");
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
    buttonIN0.configureButton(configurePushButton);
    buttonIN1.configureButton(configurePushButton);
    buttonA8.configureButton(configurePushButton);
    buttonA9.configureButton(configurePushButton);
    buttonA10.configureButton(configurePushButton);
    buttonA11.configureButton(configurePushButton);
    buttonA12.configureButton(configurePushButton);
    buttonA13.configureButton(configurePushButton);
    buttonA14.configureButton(configurePushButton);
    buttonA15.configureButton(configurePushButton);
    buttonI16.configureButton(configurePushButton);
    buttonI17.configureButton(configurePushButton);
    buttonI18.configureButton(configurePushButton);

    // When the button is first pressed, call the function onButtonPressed (further down the page)
    buttonIN0.onPress(onButtonPressedIN0);
    buttonIN1.onPress(onButtonPressedIN1);

    pinMode(CONTROLLINO_R2, OUTPUT);
    pinMode(CONTROLLINO_R3, OUTPUT);
    pinMode(CONTROLLINO_R4, OUTPUT);
    pinMode(CONTROLLINO_R5, OUTPUT);
    pinMode(CONTROLLINO_R6, OUTPUT);
    pinMode(CONTROLLINO_R7, OUTPUT);
    pinMode(CONTROLLINO_R8, OUTPUT);
    pinMode(CONTROLLINO_R9, OUTPUT);
    pinMode(CONTROLLINO_R10, OUTPUT);
    pinMode(CONTROLLINO_R11, OUTPUT);
    pinMode(CONTROLLINO_R12, OUTPUT);
    pinMode(CONTROLLINO_R13, OUTPUT);
}

void loop()
{


    // Analog
    buttonsA6.update();
        // D7           @ACM0
        // D8           @ACM0
        // R10 & R11    @ACM1
        // R10          @ACM1
        // R10          @ACM1
        // R10          @ACM1

    buttonsA7.update(); 
        // D11 & D14    @ACM0
        // D10          @ACM0
        // R10 & R11    @ACM1
        // R10          @ACM1
        // D23          @ACM0
        // D6 & D7      @ACM0

// Digital
    buttonIN0.update(); // D9 @ACM0
    buttonIN1.update(); // D1 @ACM0
    buttonA8.update();
    buttonA9.update();
    buttonA10.update();
    buttonA11.update();
    buttonA12.update();
    buttonA13.update();
    buttonA14.update(); // R10, R12 @ACM1
    buttonA15.update(); // D15
    buttonI16.update(); // D1 @ACM1
    buttonI17.update(); // R10,R11, R12,R13 @ACM1
    buttonI18.update(); // D10 @ACM0


// Chrck if pressed
    // if (buttonsA7.onPress(0))
    // {
    //     Serial.println("Button 0 is pressed");
    // }
// Chrck if pressed
    if (buttonsA7.onPress(1))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[3,5,9,10,45]}");
        client.subscribe("ACM1");
        Serial.println("Button 1 is pressed");
    }
// Chrck if pressed
    if (buttonsA7.onPress(2))
    {
        Serial.println("Button 2 is pressed");
    }
// Chrck if pressed
    if (buttonsA7.onPress(3))
    {
        // R2, R3 | R4 , R5 | R6, R7 | R8, R9 | R10, R11 | R12, R13
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[24,25,26,27,28,29,30,31,32,33,34,35]}");
        client.subscribe("ACM1");
        Serial.println("Button 3 is pressed Roleta Dole");
    }
// Chrck if pressed
    if (buttonsA7.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[24,26,28,30,32,34]}");
        client.subscribe("ACM1");
        Serial.println("Button 4 is pressed  Roleta hore");
    }
// Chrck if pressed
    if (buttonsA7.onPress(5))
    {
        Serial.println("Button 5 is pressed");
    }
// Chrck if pressed
    if (buttonsA7.onPress(6))
    {
        Serial.println("Button 6 is pressed");
    }

    // Chrck if pressed
    // if (buttonsA6.onPress(0))
    // {
    //     Serial.println("Button 0 is pressed");
    // }
    // Chrck if pressed
    if (buttonsA6.onPress(1))
    {
        Serial.println("Button buttonsA6 1 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(2))
    {
        Serial.println("Button buttonsA6 2 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(3))
    {
        Serial.println("Button buttonsA6 3 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(4))
    {
        Serial.println("Button buttonsA6 4 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(5))
    {
        Serial.println("Button buttonsA6 5 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(6))
    {
        Serial.println("Button buttonsA6 6 is pressed");
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
