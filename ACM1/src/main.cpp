
 
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


byte mac[] = {0xDE, 0xED, 0xBB, 0xFE, 0xAF, 0xBB};
IPAddress ip(192, 168, 1, 80);
IPAddress server(192, 168, 1, 10);

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;


// set how many buttons you have connected
const int BUTTONS_TOTAL = 7;
const int BUTTONS_VALUES_1[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};

unsigned int workingTime = 61000;
Neotimer timerZ3dole = Neotimer(workingTime); // 60 second timer - to finish all the movements

void toggle(int pin)
{
    digitalWrite(pin, !digitalRead(pin));
}

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length) {

    const size_t capacity = JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument root(capacity);
    deserializeJson(root, payload);

    const char* action = root["action"]; // "toggle"

    Serial.println(action);

    if (strcmp(action, "toggle")==0 )
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
        Serial.println("-- Executed");
    }

    if (strcmp(action, "rolety")==0)
    {
        if(root.containsKey("time")){
            workingTime = root["time"];
            Serial.print("TimeExist and it is: ");
            Serial.println(workingTime);
        } else {
            workingTime = 61000;
            Serial.print("TimeExist and it is: ");
            Serial.println(workingTime);
        }

        timerZ3dole = Neotimer(workingTime);
        timerZ3dole.start();
        const char* output = root["output"][0];
        Serial.println(output);
        Serial.println(digitalRead(CONTROLLINO_R2));

        if(digitalRead(CONTROLLINO_R2)==LOW){
            if( strcmp(output, "Z3dole")==0 ){
                Serial.println("IdemDole");
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
            } 
            if( strcmp(output, "Z3hore")==0 ){
                Serial.println("idemHore");
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
            } 
        } else {
            digitalWrite(CONTROLLINO_R2, LOW);
            digitalWrite(CONTROLLINO_R3, LOW);
            digitalWrite(CONTROLLINO_R4, LOW);
            digitalWrite(CONTROLLINO_R5, LOW);
            digitalWrite(CONTROLLINO_R6, LOW);
            digitalWrite(CONTROLLINO_R7, LOW);
            digitalWrite(CONTROLLINO_R8, LOW);
            digitalWrite(CONTROLLINO_R9, LOW);
            digitalWrite(CONTROLLINO_R10, LOW);
            digitalWrite(CONTROLLINO_R11, LOW);
            digitalWrite(CONTROLLINO_R12, LOW);
            digitalWrite(CONTROLLINO_R13, LOW);
            timerZ3dole.reset();
            Serial.println("idem hocikam-stop");            
        }
    }

    // if (strcmp(action, "roletyHore")==0)
    // {
    //     digitalWrite(CONTROLLINO_R2, HIGH);
    //     digitalWrite(CONTROLLINO_R3, LOW);
    //     digitalWrite(CONTROLLINO_R4, HIGH);
    //     digitalWrite(CONTROLLINO_R5, LOW);
    //     digitalWrite(CONTROLLINO_R6, HIGH);
    //     digitalWrite(CONTROLLINO_R7, LOW);
    //     digitalWrite(CONTROLLINO_R8, HIGH);
    //     digitalWrite(CONTROLLINO_R9, LOW);
    //     digitalWrite(CONTROLLINO_R10, HIGH);
    //     digitalWrite(CONTROLLINO_R11, LOW);
    //     digitalWrite(CONTROLLINO_R12, HIGH);
    //     digitalWrite(CONTROLLINO_R13, LOW);
    //     Serial.println("R Hore");
    // }

    // if (strcmp(action, "roletyDole") ==0)
    // {
    //     digitalWrite(CONTROLLINO_R2, HIGH);
    //     digitalWrite(CONTROLLINO_R3, HIGH);
    //     digitalWrite(CONTROLLINO_R4, HIGH);
    //     digitalWrite(CONTROLLINO_R5, HIGH);
    //     digitalWrite(CONTROLLINO_R6, HIGH);
    //     digitalWrite(CONTROLLINO_R7, HIGH);
    //     digitalWrite(CONTROLLINO_R8, HIGH);
    //     digitalWrite(CONTROLLINO_R9, HIGH);
    //     digitalWrite(CONTROLLINO_R10, HIGH);
    //     digitalWrite(CONTROLLINO_R11, HIGH);
    //     digitalWrite(CONTROLLINO_R12, HIGH);
    //     digitalWrite(CONTROLLINO_R13, HIGH);
    //     Serial.println("R Dole");
    // }
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
PushButton buttonA5  = PushButton(CONTROLLINO_A5,  PRESSED_WHEN_HIGH);
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
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[11]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button IN0 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedIN1(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[3]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button IN1 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA13(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[13, 44]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A13 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA14(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"roletyHore\"}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A14 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA15(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[45]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A15 pressed");
}
                   
// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedI16(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[3]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button I16 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedI17(Button &btn)
{
    client.publish("ACM1", "{\"action\":\"roletyDole\"}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button I17 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedI18(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[12]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button I18 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA8(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[6]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A8 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA9(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[2]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A9 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA10(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[5]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A10 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA11(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[7]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A11 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA12(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[7]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A12 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressedA5(Button &btn)
{
    client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[78]}");
    client.subscribe("ACM1");
    // Serial.print(btn);
    Serial.println(" button A5 pressed");
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
// void onButtonPressedA6(Button &btn)
// {
//     // Check if pressed
//     if (btn == 1)
//     {
//         client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[9]}");
//         client.subscribe("ACM1");
//         Serial.println("Button buttonsA6 1 is pressed");
//     }
//     // Check if pressed
//     if (btn == 2)
//     {
//         client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[10]}");
//         client.subscribe("ACM1");
//         Serial.println("Button buttonsA6 2 is pressed");
//     }
//     // Check if pressed
//     if (btn == 3)
//     {
//         client.publish("ACM1", "{\"action\":\"rolety\",\"command\":\"Z6_dole\"}");
//         client.subscribe("ACM1");
//         Serial.println("Button buttonsA6 3 is pressed");
//     }
//     // Check if pressed
//     if (btn == 4)
//     {
//         client.publish("ACM1", "{\"action\":\"rolety\",\"command\":\"Z6_hore\"}");
//         client.subscribe("ACM1");
//         Serial.println("Button buttonsA6 4 is pressed");
//     }
//     // Check if pressed
//     if (btn == 5)
//     {
//         client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[12]}");
//         client.subscribe("ACM1");
//         Serial.println("Button buttonsA6 5 is pressed");
//     }
//     // Check if pressed
//     if (btn == 6)
//     {
//         client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[7]}");
//         client.subscribe("ACM1");
//         Serial.println("Button buttonsA6 6 is pressed");
//     }
// }

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
    buttonA5.configureButton(configurePushButton);
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
    buttonA13.onPress(onButtonPressedA13);
    buttonA14.onPress(onButtonPressedA14);
    buttonA15.onPress(onButtonPressedA15);
    buttonI16.onPress(onButtonPressedI16);
    buttonI17.onPress(onButtonPressedI17);
    buttonI18.onPress(onButtonPressedI18);
    buttonA8.onPress(onButtonPressedA8);
    buttonA9.onPress(onButtonPressedA9);
    buttonA10.onPress(onButtonPressedA10);
    buttonA11.onPress(onButtonPressedA11);
    buttonA12.onPress(onButtonPressedA12);
    buttonA5.onPress(onButtonPressedA5);
 
    // Tryout if it will work this way. If not, needs to be removed and section in loop used
    // buttonsA6.onPress(onButtonPressedA6);

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
    // timerZ3dole.reset();
  if( timerZ3dole.done() && digitalRead(CONTROLLINO_R2)!=LOW ){

    digitalWrite(CONTROLLINO_R2, LOW);
    digitalWrite(CONTROLLINO_R3, LOW);
    digitalWrite(CONTROLLINO_R4, LOW);
    digitalWrite(CONTROLLINO_R5, LOW);
    digitalWrite(CONTROLLINO_R6, LOW);
    digitalWrite(CONTROLLINO_R7, LOW);
    digitalWrite(CONTROLLINO_R8, LOW);
    digitalWrite(CONTROLLINO_R9, LOW);
    digitalWrite(CONTROLLINO_R10, LOW);
    digitalWrite(CONTROLLINO_R11, LOW);
    digitalWrite(CONTROLLINO_R12, LOW);
    digitalWrite(CONTROLLINO_R13, LOW);

    Serial.println("Timer Z3dole finished");
  }


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
    buttonA5.update();
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


// Check if pressed
    // if (buttonsA7.onPress(0))
    // {
    //     Serial.println("Button 0 is pressed");
    // }
// Check if pressed
    if (buttonsA7.onPress(1))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[13, 44]}");
        client.subscribe("ACM1");
        Serial.println("Button 1 is pressed");
    }
// Check if pressed
    if (buttonsA7.onPress(2))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[12]}");
        client.subscribe("ACM1");
        Serial.println("Button 2 is pressed");
    }
// Check if pressed
    if (buttonsA7.onPress(3))
    {
        // R2, R3 | R4 , R5 | R6, R7 | R8, R9 | R10, R11 | R12, R13
        // client.publish("ACM1", "{\"action\":\"toggle\",\"output\":[24,25,26,27,28,29,30,31,32,33,34,35]}");
        client.publish("ACM1", "{\"action\":\"roletyDole\"}");
        client.subscribe("ACM1");
        Serial.println("Button 3 is pressed Roleta Dole");
    }
// Check if pressed
    if (buttonsA7.onPress(4))
    {
        client.publish("ACM1", "{\"action\":\"roletyHore\"}");
        client.subscribe("ACM1");
        
        Serial.println("Button 4 is pressed  Roleta hore");
    }
// Check if pressed
    if (buttonsA7.onPress(5))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[80]}");
        client.subscribe("ACM1");
        Serial.println("Button 5 is pressed");
    }
// Check if pressed
    if (buttonsA7.onPress(6))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[8, 9]}");
        client.subscribe("ACM1");
        Serial.println("Button 6 is pressed");
    }

 /* Moved this section up to where the button pressed is defined */
    // Chrck if pressed
    // if (buttonsA6.onPress(0))
    // {
    //     Serial.println("Button 0 is pressed");
    // }
    // Chrck if pressed
    if (buttonsA6.onPress(1))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[9]}");
        client.subscribe("ACM1");
        Serial.println("Button buttonsA6 1 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(2))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[10]}");
        client.subscribe("ACM1");
        Serial.println("Button buttonsA6 2 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(3))
    {
        // client.publish("ACM1", "{\"action\":\"rolety\",\"command\":\"Z6_dole\"}");
        client.publish("ACM1", "{\"action\":\"roletyHore\"}");
        client.subscribe("ACM1");
        Serial.println("Button buttonsA6 3 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(4))
    {
        // client.publish("ACM1", "{\"action\":\"rolety\",\"command\":\"Z6_hore\"}");
        client.publish("ACM1", "{\"action\":\"roletyDole\"}");
        client.subscribe("ACM1");
        Serial.println("Button buttonsA6 4 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(5))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[12]}");
        client.subscribe("ACM1");
        Serial.println("Button buttonsA6 5 is pressed");
    }
    // Chrck if pressed
    if (buttonsA6.onPress(6))
    {
        client.publish("ACM0", "{\"action\":\"toggle\",\"output\":[7]}");
        client.subscribe("ACM1");
        Serial.println("Button buttonsA6 6 is pressed");
    }
/* */

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
