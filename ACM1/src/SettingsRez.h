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
const char *controllino = "ACM1/#";
boolean retain = true;

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

/* define analog multi button structure to allow for streamlined handling
    am_button[0].name = A0 = input name on the controllino
    am_button[0].definition > reference to the analogMultiButton object
    am_button[0].total_buttons > number of buttons associated with multiButton
    am_button[0].buttons[0].name = s10_5_a > name of the switch and button on it
    am_button[0].buttons[0].total_topics > number of topics associated with particular button
    am_button[0].buttons[0].topics[0] = "ACM0/light/#" > topic(s) to turn up
*/
typedef struct SubButton
{
    const char *name;
    int total_topics;
    const char *topics[3];
    const char *payload;
} SubButton;

typedef struct AmB
{
    String name;
    AnalogMultiButton *definition;
    SubButton buttons[BUTTONS_TOTAL];
    int total_buttons;
} AmB;

AmB am_button[ANALOG_BUTTONS_TOTAL] = {
    {"A6", &buttonsA6, {
                           {},
                           {"s10_5_a", 2, {"ACM0/light/kitchen/island/toggle", "ACM0/light/kitchen/led/toggle"}},
                           {"s10_5_b", 1, {"ACM0/light/kitchen/fridge/toggle"}},
                           {"s10_5_c", 1, {"ACM1/blind/kitchen/cmd"}, "open"},
                           {"s10_5_d", 1, {"ACM1/blind/kitchen/cmd"}, "close"},
                           {"s10_5_e", 1, {"ACM0/light/hallway/downstairs/main/toggle"}},
                           {"s10_5_f", 1, {"ACM0/light/kitchen/dining/toggle"}},
                       },
     BUTTONS_TOTAL},
    {"A7", &buttonsA7, {
                           {},
                           {"s10_3_a", 2, {"ACM0/light/living_room/hallway/toggle", "ACM0/light/living_room/center/toggle"}},
                           {"s10_3_b", 1, {"ACM0/light/hallway/downstairs/main/toggle"}},
                           {"s10_3_c", 1, {"ACM1/blind/living_room/cmd"}, "close"},
                           {"s10_3_d", 1, {"ACM1/blind/living_room/cmd"}, "open"},
                           {"s10_3_e", 1, {"ACM0/light/outside/porch/toggle"}},
                           {"s10_3_f", 3, {"ACM0/light/kitchen/fridge/toggle", "ACM0/light/kitchen/island/toggle", "ACM0/light/kitchen/led/toggle"}},
                       },
     BUTTONS_TOTAL}};

// ---------------------------------------------------------------------------------
// PUSH BUTTONS SETUP
// ---------------------------------------------------------------------------------

// define push button multi array
typedef struct p_btn
{
    const char *name;
    PushButton definition;
    int total_topics;
    const char *topics[3];
    const char *payload;
} p_btn;

// push buttons definition array
p_btn p_button[] = {
    {"s10_1_in0", PushButton(CONTROLLINO_IN0, PRESSED_WHEN_HIGH), 1, {"ACM0/light/guest_room/toggle"}},
    {"s10_1_in1", PushButton(CONTROLLINO_IN1, PRESSED_WHEN_HIGH), 1, {"ACM0/light/tech_room/toggle"}},
    {"s11_3_a8", PushButton(CONTROLLINO_A8, PRESSED_WHEN_HIGH), 1, {"ACM0/light/pantry/toggle"}},
    {"s11_2_a9", PushButton(CONTROLLINO_A9, PRESSED_WHEN_HIGH), 1, {"ACM0/light/bathroom/downstairs/mirror/toggle"}},
    {"s11_1_a10", PushButton(CONTROLLINO_A10, PRESSED_WHEN_HIGH), 1, {"ACM0/light/bathroom/downstairs/main/toggle"}},
    {"s10_8_a11", PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH), 1, {"ACM0/light/kitchen/island/toggle"}},
    {"s10_7_a12", PushButton(CONTROLLINO_A12, PRESSED_WHEN_HIGH), 1, {"ACM0/light/kitchen/led/toggle"}},
    {"s10_4_f", PushButton(CONTROLLINO_A13, PRESSED_WHEN_HIGH), 1, {"ACM0/light/living_room/four/toggle"}},
    {"s10_4_d", PushButton(CONTROLLINO_A14, PRESSED_WHEN_HIGH), 1, {"ACM0/light/hallway/stairs/led/toggle"}},
    {"s10_4_a", PushButton(CONTROLLINO_A15, PRESSED_WHEN_HIGH), 1, {"ACM0/light/living_room/one/toggle"}},
    {"s10_4_b", PushButton(CONTROLLINO_I16, PRESSED_WHEN_HIGH), 1, {"ACM0/light/living_room/two/toggle"}},
    {"s10_4_c", PushButton(CONTROLLINO_I17, PRESSED_WHEN_HIGH), 1, {"ACM1/light/hallway/stairs/above/toggle"}},
    {"s10_4_e", PushButton(CONTROLLINO_I18, PRESSED_WHEN_HIGH), 1, {"ACM0/light/living_room/three/toggle"}}};

const int PUSH_BUTTONS_TOTAL = 13;

// ---------------------------------------------------------------------------------
// OUTPUT DEFINITION - without blinds
// ---------------------------------------------------------------------------------

const int OUTPUTS_TOTAL = 23;
typedef struct c_output
{
    const char *name;
    int pin;
    String entity;
} c_output;

c_output c_outputs[OUTPUTS_TOTAL] = {
    {"el_82_83", CONTROLLINO_D0, "light/hallway/upstairs"},
    {"el_84", CONTROLLINO_D1, "light/hallway/stairs/above"},
    {"el_97", CONTROLLINO_D2, "light/attic"},
    {"el_86", CONTROLLINO_D3, "light/adka"},
    {"el_85", CONTROLLINO_D4, "light/misko"},
    {"el_87_88", CONTROLLINO_D5, "light/closet/main"},
    {"el_98_99", CONTROLLINO_D6, "light/closet/led"},
    {"el_91", CONTROLLINO_D7, "light/parents/dad"},
    {"el_92", CONTROLLINO_D8, "light/parents/mom"},
    {"el_93", CONTROLLINO_D9, "light/parents/main"},
    {"el_94", CONTROLLINO_D12, "light/bathroom/upstairs/main"},
    {"el_95", CONTROLLINO_D13, "light/bathroom/upstairs/mirror/mom"},
    {"el_96", CONTROLLINO_D14, "light/bathroom/upstairs/mirror/dad"},
    {"el_80", CONTROLLINO_D10, "light/toilet/upstairs/mirror"},
    {"el_81", CONTROLLINO_D11, "light/toilet/upstairs/main"},
    {"el_x34", CONTROLLINO_R14, "outlet/bathroom/downstairs"},
    {"el_x35", CONTROLLINO_R15, "outlet/bathroom/upstairs"},
    {"el_z30", CONTROLLINO_D23, "lock/outside/gate"}};

// ---------------------------------------------------------------------------------
// BLINDS SETUP
// ---------------------------------------------------------------------------------

unsigned int controllPin = 0;
unsigned int directionPin = 0;

const int BLINDS_TOTAL = 6;
Shutters spalna;
Shutters chodba;
Shutters detska1;
Shutters detska2;
Shutters kuchyna;
Shutters obyvacka;

typedef struct c_blind
{
    const char *name;
    Shutters *blind;
    int pin;
    String entity;
} c_blind;

c_blind blinds[BLINDS_TOTAL] = {
    {"el_z1", &spalna, CONTROLLINO_R2, "blind/parents"},
    {"el_z2", &chodba, CONTROLLINO_R4, "blind/hallway/upstairs"},
    {"el_z3", &detska1, CONTROLLINO_R6, "blind/adka"},
    {"el_z4", &detska2, CONTROLLINO_R8, "blind/misko"},
    {"el_z5", &kuchyna, CONTROLLINO_R10, "blind/kitchen"},
    {"el_z6", &obyvacka, CONTROLLINO_R12, "blind/living_room"},
};
const byte eepromOffset = 0;
const unsigned long upCourseTime = 68 * 1000L;
const unsigned long downCourseTime = 63 * 1000L;
const float calibrationRatio = 0.1;

#endif