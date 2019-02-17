
#include <Arduino.h>
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
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xAF, 0xA1};
IPAddress ip(192, 168, 1, 81);
IPAddress server(192, 168, 1, 10);

// set how many buttons you have connected
const int BUTTONS_TOTAL = 7;
const int BUTTONS_VALUES_1[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};

// you can also define constants for each of your buttons, which makes your code easier to read
// define these in the same order as the numbers in your BUTTONS_VALUES array, so whichever button has the smallest analogRead() number should come first

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttonsA0(CONTROLLINO_A0, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA1(CONTROLLINO_A1, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA2(CONTROLLINO_A2, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA4(CONTROLLINO_A4, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA6(CONTROLLINO_A6, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA7(CONTROLLINO_A7, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA13(CONTROLLINO_A13, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA14(CONTROLLINO_A14, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA15(CONTROLLINO_A15, BUTTONS_TOTAL, BUTTONS_VALUES_1);
PushButton buttonIN0 = PushButton(CONTROLLINO_IN0, PRESSED_WHEN_HIGH);
PushButton buttonIN1 = PushButton(CONTROLLINO_IN1, PRESSED_WHEN_HIGH);

// pass a fourth parameter to set the debounce time in milliseconds
// this defaults to 20 and can be increased if you're working with particularly bouncy buttons

void setup()
{
    // begin serial so we can see which buttons are being pressed through the serial monitor
    Serial.begin(9600);
    Serial.println("Init");

    // Configure the button as you'd like - not necessary if you're happy with the defaults
    buttonIN0.configureButton(configurePushButton);
    buttonIN1.configureButton(configurePushButton);

    // When the button is first pressed, call the function onButtonPressed (further down the page)
    buttonIN0.onPress(onButtonPressed);
    buttonIN1.onPress(onButtonPressed);
}

void loop()
{
    // update the AnalogMultiButton object every loop
    buttonsA0.update();
    buttonsA1.update();
    buttonsA2.update();
    buttonsA4.update();
    buttonsA6.update();
    buttonsA7.update();
    buttonsA13.update();
    buttonsA14.update();
    buttonsA15.update();
    buttonIN0.update();
    buttonIN1.update();

    if (buttonsA1.onPress(1))
    {
        Serial.println("A1 - 1 was pressed now");
    }

    if (buttonsA1.onPress(2))
    {
        Serial.println("A1 - 2 was pressed now");
    }

    if (buttonsA1.onPress(3))
    {
        Serial.println("A1 - 3 was pressed now");
    }



    delay(10);
}

// Use this function to configure the internal Bounce object to suit you. See the documentation at: https://github.com/thomasfredericks/Bounce2/wiki
// This function can be left out if the defaults are acceptable - just don't call configureButton
void configurePushButton(Bounce &bouncedButton)
{
    // Set the debounce interval to 15ms - default is 10ms
    bouncedButton.interval(15);
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressed(Button &btn)
{
    // client.publish("outTopic2", "Button pressed");
    Serial.println("button pressed");
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
