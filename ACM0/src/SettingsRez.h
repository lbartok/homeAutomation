/*
 * SettingsRez.h
 * A library to capture variables and settings for homeAutomation for "rez".
 * Controllino ACM0
 */

#ifndef SETTINGS_REZ_H
#define SETTINGS_REZ_H

#include "acm0_setup.h"
#include <Controllino.h>
// For analog button detection
#include "AnalogMultiButton.h"
// For pushing digital buttons
#include <Button.h>
#include <PushButton.h>

// ---------------------------------------------------------------------------------
// ANALOG BUTTONS SETUP
// ---------------------------------------------------------------------------------
// set how many buttons you have connected to one analog PIN
const int BUTTONS_TOTAL = 7;
const int BUTTONS_VALUES_1[BUTTONS_TOTAL] = {0, 14, 136, 252, 399, 551, 673};
const int BUTTONS_VALUES_2[BUTTONS_TOTAL] = {0, 6, 71, 143, 257, 406, 562};

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttonsA0(CONTROLLINO_A0, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA1(CONTROLLINO_A1, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA2(CONTROLLINO_A2, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA4(CONTROLLINO_A4, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA13(CONTROLLINO_A13, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA14(CONTROLLINO_A14, BUTTONS_TOTAL, BUTTONS_VALUES_1);
AnalogMultiButton buttonsA15(CONTROLLINO_A15, BUTTONS_TOTAL, BUTTONS_VALUES_2);
// these cannot be defined within array, hence array of their pointers
AnalogMultiButton *ANALOG_BUTTONS_DEF[] = {
    &buttonsA0, &buttonsA1, &buttonsA2, &buttonsA4, &buttonsA13, &buttonsA14, &buttonsA15};
const int ANALOG_BUTTONS_TOTAL = 7;

// define analog multi button structure to allow for streamlined handling
// am_button[0].name = A0 = input name on the controllino
// am_button[0].definition > reference to the analogMultiButton object
// am_button[0].total_buttons > number of buttons associated with multiButton
// am_button[0].buttons[0].name = s10_5_a > name of the switch and button on it
// am_button[0].buttons[0].total_topics > number of topics associated with particular button
// am_button[0].buttons[0].topics[0] = "ACM0/light/#" > topic(s) to turn up

typedef struct SubButton
{
    const char *name;
    int total_topics;
    const char *topics[2];
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
    {"A0", &buttonsA0, {
                           {},
                           {"s13_8_a", 2, {"NR/light/misko/toggle"}},
                           {"s13_8_b", 2, {"NR/light/adka/toggle"}},
                           {"s13_8_c", 1, {"ACM1/blind/adka/cmd"}, "open"},
                           {"s13_8_d", 1, {"ACM1/blind/adka/cmd"}, "close"},
                           {"s13_2_a", 1, {"NR/light/parents/main/toggle"}},
                           {"s13_2_b", 1, {"ACM1/light/parents/dad/toggle"}},
                       },
     BUTTONS_TOTAL},
    {"A1", &buttonsA1, {
                           {},
                           {"s13_7_a", 2, {"NR/light/misko/toggle"}},
                           {"s13_7_b", 2, {"NR/light/adka/toggle"}},
                           {"s13_7_c", 1, {"ACM1/blind/misko/cmd"}, "close"},
                           {"s13_7_d", 1, {"ACM1/blind/misko/cmd"}, "open"},
                           {"s13_1_a", 1, {"ACM1/light/parents/mom/toggle"}},
                           {"s13_1_b", 1, {"NR/light/parents/main/toggle"}},
                       },
     BUTTONS_TOTAL},
    {"A2", &buttonsA2, {
                           {},
                           {"s13_6_a", 1, {"NR/light/hallway/upstairs/toggle"}},
                           {"s13_6_b", 1, {"ACM1/light/hallway/stairs/above/toggle"}},
                           {"s13_6_c", 1, {"ACM0/light/hallway/stairs/led/toggle"}},
                           {"s13_6_d", 1, {"ACM0/light/outside/door/toggle"}},
                           {"s12_8_a", 1, {"NR/light/parents/main/toggle"}},
                           {"s12_8_b", 1, {"ACM1/light/closet/main/toggle"}},
                       },
     BUTTONS_TOTAL},
    {"A4", &buttonsA4, {
                           {},
                           {"s13_4_a", 1, {"NR/light/hallway/upstairs/toggle"}},
                           {"s13_4_b", 1, {"ACM1/light/hallway/stairs/above/toggle"}},
                           {"s13_4_c", 1, {"ACM1/blind/hallway/upstairs/cmd"}, "open"},
                           {"s13_4_d", 1, {"ACM1/blind/hallway/upstairs/cmd"}, "close"},
                           {"s12_6_a", 1, {"NR/light/bathroom/upstairs/main/toggle"}},
                           {"s12_6_b", 1, {"ACM1/light/closet/main/toggle"}},
                       },
     BUTTONS_TOTAL},
    {"A13", &buttonsA13, {
                             {},
                             {"s12_2_a", 1, {"ACM1/light/toilet/upstairs/mirror/toggle"}},
                             {"s12_2_b", 1, {"NR/light/toilet/upstairs/main/toggle"}},
                             {"s13_3_a", 1, {"ACM1/blind/parents/cmd"}, "close"},
                             {"s13_3_b", 1, {"ACM1/blind/parents/cmd"}, "open"},
                             {"s13_3_c", 2, {"ACM1/light/parents/mom/toggle", "ACM1/light/parents/dad/toggle"}},
                             {"s13_3_d", 1, {"NR/light/parents/main/toggle"}},
                         },
     BUTTONS_TOTAL},
    {"A14", &buttonsA14, {
                             {},
                             {"s11_4_a", 1, {"ACM0/light/outside/door/toggle"}},
                             {"s11_4_b", 1, {"ACM1/lock/outside/gate/toggle"}},
                             {"s11_4_c", 1, {"ACM0/light/hallway/downstairs/entrance/toggle"}},
                             {"s11_4_d", 1, {"ACM0/light/hallway/downstairs/main/toggle"}},
                             {"s11_7_a", 1, {"ACM0/light/outside/garage/toggle"}},
                             {"s11_7_b", 1, {"ACM0/light/outside/porch/toggle"}},
                         },
     BUTTONS_TOTAL},
    {"A15", &buttonsA15, {
                             {},
                             {"s10_6_a", 1, {"ACM0/light/kitchen/dining/toggle"}},
                             {"s10_6_b", 1, {"ACM0/light/hallway/downstairs/main/toggle"}},
                             {"s10_6_c", 2, {"NR/light/kitchen/island/toggle", "ACM0/light/kitchen/led/toggle"}},
                             {"s10_6_d", 1, {"ACM0/light/kitchen/fridge/toggle"}},
                             {"s11_6_a", 1, {"ACM0/light/outside/door/toggle"}},
                             {"s11_6_b", 1, {"ACM0/light/outside/garage/toggle"}},
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
    const char *topics[1];
    const char *payload;
} p_btn;

// push buttons definition array
p_btn p_button[] = {
    {"s13_5_a5", PushButton(CONTROLLINO_A5, PRESSED_WHEN_HIGH), 1, {"NR/light/hallway/upstairs/toggle"}},
    {"s12_7_a6", PushButton(CONTROLLINO_A6, PRESSED_WHEN_HIGH), 1, {"ACM1/light/closet/led/toggle"}},
    {"s12_5_a7", PushButton(CONTROLLINO_A7, PRESSED_WHEN_HIGH), 1, {"ACM1/light/bathroom/upstairs/mirror/mom/toggle"}},
    {"s12_4_a8", PushButton(CONTROLLINO_A8, PRESSED_WHEN_HIGH), 1, {"ACM1/light/bathroom/upstairs/mirror/dad/toggle"}},
    {"s12_3_a9", PushButton(CONTROLLINO_A9, PRESSED_WHEN_HIGH), 1, {"NR/light/bathroom/upstairs/main/toggle"}},
    {"s11_5_a11", PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH), 1, {"ACM0/light/outside/door/toggle"}},
};
const int PUSH_BUTTONS_TOTAL = 6;

// ---------------------------------------------------------------------------------
// OUTPUT DEFINITION
// ---------------------------------------------------------------------------------

const int OUTPUTS_TOTAL = 24;
typedef struct c_output
{
    const char *name;
    int pin;
    String entity;
} c_output;

c_output c_outputs[OUTPUTS_TOTAL] = {
    {"el_49", CONTROLLINO_D0, "light/bathroom/downstairs/mirror"},
    {"el_51", CONTROLLINO_D1, "light/tech_room"},
    {"el_52", CONTROLLINO_D2, "light/hallway/downstairs/entrance"},
    {"el_53", CONTROLLINO_D3, "light/bathroom/downstairs/main"},
    {"el_54", CONTROLLINO_D4, "light/pantry"},
    {"el_55", CONTROLLINO_D5, "light/kitchen/led"},
    {"el_56", CONTROLLINO_D6, "light/kitchen/fridge"},
    {"el_57", CONTROLLINO_D7, "light/kitchen/island"},
    {"el_58", CONTROLLINO_D8, "light/kitchen/dining"},
    {"el_61", CONTROLLINO_D9, "light/guest_room"},
    {"el_62_63", CONTROLLINO_D10, "light/hallway/downstairs/main"},
    {"el_64", CONTROLLINO_D11, "light/living_room/hallway"},
    {"el_66", CONTROLLINO_D12, "light/living_room/led/ceiling"},
    {"el_68", CONTROLLINO_D15, "light/hallway/stairs/led"},
    {"el_65", CONTROLLINO_D13, "light/living_room/led/tv"},
    {"el_67", CONTROLLINO_D14, "light/living_room/center"},
    {"el_71", CONTROLLINO_D16, "light/living_room/one"},
    {"el_72", CONTROLLINO_D17, "light/living_room/two"},
    {"el_73", CONTROLLINO_D18, "light/living_room/three"},
    {"el_74", CONTROLLINO_D19, "light/living_room/four"},
    {"el_75", CONTROLLINO_D20, "light/living_room/five"},
    {"el_76", CONTROLLINO_D21, "light/outside/door"},
    {"el_77", CONTROLLINO_D22, "light/outside/garage"},
    {"el_78", CONTROLLINO_D23, "light/outside/porch"}};

#endif