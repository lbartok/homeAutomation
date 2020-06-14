/*
 * SettingsRez.h
 * A library to capture variables and settings for homeAutomation for "rez".
 * Controllino ACM1
*/

#ifndef SETTINGS_REZ_H
#define SETTINGS_REZ_H

#include <Controllino.h>
// For analog button detection
#include "AnalogMultiButton.h"
// For pushing digital buttons
#include <Button.h>
#include <PushButton.h>

// ---------------------------------------------------------------------------------
// MQTT setup
// ---------------------------------------------------------------------------------
byte mac[] = {0xDE, 0xED, 0xBB, 0xFE, 0xAF, 0xBB};
IPAddress ip(192, 168, 69, 180);
IPAddress server(192, 168, 69, 10);
const char *controllino = "ACM1";

// ---------------------------------------------------------------------------------
// ANALOG BUTTONS SETUP
// ---------------------------------------------------------------------------------
// set how many buttons you have connected to one analog PIN
const int BUTTONS_TOTAL = 7;
const int BUTTONS_VALUES_1[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};
const int BUTTONS_VALUES_2[BUTTONS_TOTAL] = {0, 6, 71, 143, 257, 406, 562};

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttonsA6(CONTROLLINO_A6, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA7(CONTROLLINO_A7, BUTTONS_TOTAL, BUTTONS_VALUES_1);
// these cannot be defined within array, hence array of their pointers
AnalogMultiButton *ANALOG_BUTTONS_DEF[] = {
    &buttonsA6, &buttonsA7};
const int ANALOG_BUTTONS_TOTAL = 2;
// analog buttons actor array (needs to be same order as ANALOG_BUTTONS_DEF)
const char *ANALOG_BUTTONS_ACT[2][ANALOG_BUTTONS_TOTAL][BUTTONS_TOTAL] = {
    {//section for topic to which we post MQTT message
     {
         //A6
         "",     //Button 0
         "ACM0", //Button 1
         "ACM0", //Button 2
         "ACM1", //Button 3
         "ACM1", //Button 4
         "ACM0", //Button 5
         "ACM0"  //Button 6
     },
     {
         //A7
         "",     //Button 0
         "ACM0", //Button 1
         "ACM0", //Button 2
         "ACM1", //Button 3
         "ACM1", //Button 4
         "ACM0", //Button 5
         "ACM0"  //Button 6
     }},
    {//section for actual message to post in MQTT
     {
         //A6
         "",                                                         //Button 0
         "{\"action\":\"toggle\",\"output\":[9]}",                   //Button 1
         "{\"action\":\"toggle\",\"output\":[10]}",                  //Button 2
         "{\"action\":\"rolety\",\"prcnt\":\"100\",\"output\":[4]}", //Button 3
         "{\"action\":\"rolety\",\"prcnt\":\"0\",\"output\":[4]}",   //Button 4
         "{\"action\":\"toggle\",\"output\":[12]}",                  //Button 5
         "{\"action\":\"toggle\",\"output\":[8]}"                    //Button 6
     },
     {
         //A7
         "",                                                         //Button 0
         "{\"action\":\"toggle\",\"output\":[13, 44]}",              //Button 1
         "{\"action\":\"toggle\",\"output\":[12]}",                  //Button 2
         "{\"action\":\"rolety\",\"prcnt\":\"100\",\"output\":[4]}", //Button 3
         "{\"action\":\"rolety\",\"prcnt\":\"0\",\"output\":[4]}",   //Button 4
         "{\"action\":\"toggle\",\"output\":[80]}",                  //Button 5
         "{\"action\":\"toggle\",\"output\":[8, 9]}"                 //Button 6
     }}};

// ---------------------------------------------------------------------------------
// PUSH BUTTONS SETUP
// ---------------------------------------------------------------------------------

// push buttons definition array
const int PUSH_BUTTONS_TOTAL = 14;
PushButton PUSH_BUTTONS_DEF[PUSH_BUTTONS_TOTAL] = {
    PushButton(CONTROLLINO_IN0, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_IN1, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A5, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A8, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A9, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A10, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A12, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A13, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A14, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A15, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_I16, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_I17, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_I18, PRESSED_WHEN_HIGH)};

// push buttons actor array (needs to be same order as PUSH_BUTTON_DEF)
const char *PUSH_BUTTONS_ACT[2][PUSH_BUTTONS_TOTAL] = {
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
        "ACM0"  //I18
    },
    {
        "{\"action\":\"toggle\",\"output\":[11]}",                              //IN0
        "{\"action\":\"toggle\",\"output\":[3]}",                               //IN1
        "{\"action\":\"toggle\",\"output\":[78]}",                              //A5
        "{\"action\":\"toggle\",\"output\":[6]}",                               //A8
        "{\"action\":\"toggle\",\"output\":[2]}",                               //A9
        "{\"action\":\"toggle\",\"output\":[5]}",                               //A10
        "{\"action\":\"toggle\",\"output\":[7]}",                               //A11
        "{\"action\":\"toggle\",\"output\":[7]}",                               //A12
        "{\"action\":\"toggle\",\"output\":[13, 44]}",                          //A13
        "{\"action\":\"rolety\",\"prcnt\":\"0\",\"output\":[0, 1, 2, 3, 4]}",   //A14
        "{\"action\":\"toggle\",\"output\":[45]}",                              //A15
        "{\"action\":\"toggle\",\"output\":[3]}",                               //I16
        "{\"action\":\"rolety\",\"prcnt\":\"100\",\"output\":[0, 1, 2, 3, 4]}", //I17
        "{\"action\":\"toggle\",\"output\":[12]}"                               //I18
    }};

// ---------------------------------------------------------------------------------
// OUTPUT DEFINITION
// ---------------------------------------------------------------------------------

// outputs definition array for being able to read the state through function
const int TOTAL_OUTPUT_DEF_ARRAY = 17;
const int OUTPUT_DEF_ARRAY[] = {
    CONTROLLINO_D0,
    CONTROLLINO_D1,
    CONTROLLINO_D2,
    CONTROLLINO_D3,
    CONTROLLINO_D4,
    CONTROLLINO_D5,
    CONTROLLINO_D6,
    CONTROLLINO_D7,
    CONTROLLINO_D8,
    CONTROLLINO_D9,
    CONTROLLINO_D10,
    CONTROLLINO_D11,
    CONTROLLINO_D12,
    CONTROLLINO_D13,
    CONTROLLINO_D14,
    CONTROLLINO_R14,
    CONTROLLINO_R15};

// ---------------------------------------------------------------------------------
// BLINDS SETUP
// ---------------------------------------------------------------------------------

unsigned int controllPin = 0;
unsigned int directionPin = 0;

const int BLINDS_TOTAL = 5;
const int BLINDS[BLINDS_TOTAL] = {
    CONTROLLINO_R2, CONTROLLINO_R4, CONTROLLINO_R6, CONTROLLINO_R8, CONTROLLINO_R10 //, CONTROLLINO_R12
};

const byte eepromOffset = 0;
const unsigned long upCourseTime = 68 * 1000L;
const unsigned long downCourseTime = 63 * 1000L;
const float calibrationRatio = 0.1;

// Blinds need to be defined one-by-one first
Shutters spalna;
Shutters chodba;
Shutters detska1;
Shutters detska2;
Shutters kuchyna;
Shutters obyvacka;
// the order of the blinds needs to be the same as in the BLINDS[] array above
Shutters *blindsArray[BLINDS_TOTAL] = {
    &spalna, &chodba, &detska1, &detska2, &kuchyna //, &obyvacka
};

#endif