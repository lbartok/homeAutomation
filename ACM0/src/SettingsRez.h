/*
 * SettingsRez.h
 * A library to capture variables and settings for homeAutomation for "rez".
 * Controllino ACM0
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
byte mac[] = {0xDE, 0xED, 0xBB, 0xFE, 0xAF, 0xAA};
IPAddress ip(192, 168, 69, 181);
IPAddress server(192, 168, 69, 10);

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

// try and error for a union
typedef struct SubButton
{
    const char *name;
    const char *topics[6];
} SubButton;

// a_button[0].name = A0 = input name on the controllino
// a_button[0].definition > reference to the analogMultiButton object
// a_button[0].total_buttons > number of buttons associated with multiButton
// a_button[0].buttons[0].name = s10_5_a > name of the switch and button on it
// a_button[0].buttons[0].total_topics > number of topics associated with particular button
// a_button[0].buttons[0].topics[0] = "ACM0/light/#" > topic(s) to turn up

typedef struct AmB
{
    String name;
    AnalogMultiButton *definition;
    SubButton buttons[BUTTONS_TOTAL];
} AmB;

AmB am_button[ANALOG_BUTTONS_TOTAL] = {
    {
        "A0", &buttonsA0, 
        {
            {},
            {"s13_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_b", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_c", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_d", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_b", {"ACM0/light/", "ACM0/light/"}}
        }
    },
    {
        "A1", &buttonsA1, 
        {
            {},
            {"s13_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_b", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_c", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_d", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_b", {"ACM0/light/", "ACM0/light/"}}
        }
    },
    {
        "A2", &buttonsA2, 
        {
            {},
            {"s13_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_b", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_c", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_d", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_b", {"ACM0/light/", "ACM0/light/"}}
        }
    },
    {
        "A4", &buttonsA4, 
        {
            {},
            {"s13_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_b", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_c", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_d", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_b", {"ACM0/light/", "ACM0/light/"}}
        }
    },
    {
        "A13", &buttonsA13, 
        {
            {},
            {"s13_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_b", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_c", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_d", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_b", {"ACM0/light/", "ACM0/light/"}}
        }
    },
    {
        "A14", &buttonsA14, 
        {
            {},
            {"s13_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_b", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_c", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_d", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_b", {"ACM0/light/", "ACM0/light/"}}
        }
    },
    {
        "A15", &buttonsA15, 
        {
            {},
            {"s13_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_b", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_c", {"ACM0/light/", "ACM0/light/"}}, 
            {"s13_d", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_a", {"ACM0/light/", "ACM0/light/"}}, 
            {"s18_b", {"ACM0/light/", "ACM0/light/"}}
        }
    }
};
//initialize_AmB();
// analog buttons actor array (needs to be same order as ANALOG_BUTTONS_DEF)
const char *ANALOG_BUTTONS_ACT[2][ANALOG_BUTTONS_TOTAL][BUTTONS_TOTAL] = {
    {//section for topic to which we post MQTT message
     {
         //A0
         "",     //Button 0
         "ACM1", //Button 1
         "ACM1", //Button 2
         "ACM1", //Button 3
         "ACM1", //Button 4
         "ACM1", //Button 5
         "ACM1"  //Button 6
     },
     {
         //A1
         "",     //Button 0
         "ACM1", //Button 1
         "ACM1", //Button 2
         "ACM1", //Button 3
         "ACM1", //Button 4
         "ACM1", //Button 5
         "ACM1"  //Button 6
     },
     {
         //A2
         "",     //Button 0
         "ACM1", //Button 1
         "ACM1", //Button 2
         "ACM0", //Button 3
         "ACM0", //Button 4
         "ACM1", //Button 5
         "ACM1"  //Button 6
     },
     {
         //A4
         "",     //Button 0
         "ACM1", //Button 1
         "ACM1", //Button 2
         "ACM1", //Button 3
         "ACM1", //Button 4
         "ACM1", //Button 5
         "ACM1"  //Button 6
     },
     {
         //A13
         "",     //Button 0
         "ACM1", //Button 1
         "ACM1", //Button 2
         "ACM1", //Button 3
         "ACM1", //Button 4
         "ACM1", //Button 5
         "ACM1"  //Button 6
     },
     {
         //A14
         "",     //Button 0
         "ACM0", //Button 1
         "ACM0", //Button 2
         "ACM0", //Button 3
         "ACM0", //Button 4
         "ACM0", //Button 5
         "ACM0"  //Button 6
     },
     {
         //A15
         "",                                          //Button 0
         "ACM0/light/kitchen/island/cmd",             //Button 1
         "ACM0/light/kitchen/dining/cmd",             //Button 2
         "ACM0/light/hallway/downstairs/main/toggle", //Button 3
         "ACM0/light/kitchen/fridge/toggle",          //Button 4
         "ACM0",                                      //Button 5
         "ACM0"                                       //Button 6
     }},
    {//section for actual message to post in MQTT
     {
         //A0
         "",                                                         //Button 0
         "{\"action\":\"toggle\",\"output\":[5, 6]}",                //Button 1
         "{\"action\":\"toggle\",\"output\":[5, 6]}",                //Button 2
         "{\"action\":\"rolety\",\"prcnt\":\"100\",\"output\":[2]}", //Button 3
         "{\"action\":\"rolety\",\"prcnt\":\"0\",\"output\":[2]}",   //Button 4
         "{\"action\":\"toggle\",\"output\":[11]}",                  //Button 5
         "{\"action\":\"toggle\",\"output\":[9]}"                    //Button 6
     },
     {
         //A1
         "",                                                         //Button 0
         "{\"action\":\"toggle\",\"output\":[5, 6]}",                //Button 1
         "{\"action\":\"toggle\",\"output\":[5, 6]}",                //Button 2
         "{\"action\":\"rolety\",\"prcnt\":\"100\",\"output\":[3]}", //Button 3
         "{\"action\":\"rolety\",\"prcnt\":\"0\",\"output\":[3]}",   //Button 4
         "{\"action\":\"toggle\",\"output\":[11]}",                  //Button 5
         "{\"action\":\"toggle\",\"output\":[10]}"                   //Button 6
     },
     {
         //A2
         "",                                        //Button 0
         "{\"action\":\"toggle\",\"output\":[2]}",  //Button 1
         "{\"action\":\"toggle\",\"output\":[3]}",  //Button 2
         "{\"action\":\"toggle\",\"output\":[45]}", //Button 3
         "{\"action\":\"toggle\",\"output\":[78]}", //Button 4
         "{\"action\":\"toggle\",\"output\":[11]}", //Button 5
         "{\"action\":\"toggle\",\"output\":[7]}"   //Button 6
     },
     {
         //A4
         "",                                                         //Button 0
         "{\"action\":\"toggle\",\"output\":[2]}",                   //Button 1
         "{\"action\":\"toggle\",\"output\":[3]}",                   //Button 2
         "{\"action\":\"rolety\",\"prcnt\":\"100\",\"output\":[1]}", //Button 3
         "{\"action\":\"rolety\",\"prcnt\":\"0\",\"output\":[1]}",   //Button 4
         "{\"action\":\"toggle\",\"output\":[42]}",                  //Button 5
         "{\"action\":\"toggle\",\"output\":[7]}"                    //Button 6
     },
     {
         //A13
         "",                                                         //Button 0
         "{\"action\":\"toggle\",\"output\":[13]}",                  //Button 1
         "{\"action\":\"toggle\",\"output\":[13]}",                  //Button 2
         "{\"action\":\"rolety\",\"prcnt\":\"100\",\"output\":[0]}", //Button 3
         "{\"action\":\"rolety\",\"prcnt\":\"0\",\"output\":[0]}",   //Button 4
         "{\"action\":\"toggle\",\"output\":[11]}",                  //Button 5
         "{\"action\":\"toggle\",\"output\":[11]}"                   //Button 6
     },
     {
         //A14
         "",                                        //Button 0
         "{\"action\":\"toggle\",\"output\":[12]}", //Button 1
         "{\"action\":\"toggle\",\"output\":[4]}",  //Button 2
         "{\"action\":\"toggle\",\"output\":[78]}", //Button 3
         "{\"action\":\"toggle\",\"output\":[78]}", //Button 4
         "{\"action\":\"toggle\",\"output\":[79]}", //Button 5
         "{\"action\":\"toggle\",\"output\":[80]}"  //Button 6
     },
     {
         //A15
         "",                                        //Button 0
         "on",                                      //Button 1
         "on",                                      //Button 2
         "toggle",                                  //Button 3
         "toggle",                                  //Button 4
         "{\"action\":\"toggle\",\"output\":[78]}", //Button 5
         "{\"action\":\"toggle\",\"output\":[79]}"  //Button 6
     }}};

// ---------------------------------------------------------------------------------
// PUSH BUTTONS SETUP
// ---------------------------------------------------------------------------------

// push buttons definition array
const int PUSH_BUTTONS_TOTAL = 6;
PushButton PUSH_BUTTONS_DEF[PUSH_BUTTONS_TOTAL] = {
    PushButton(CONTROLLINO_A5, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A6, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A7, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A8, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A9, PRESSED_WHEN_HIGH),
    PushButton(CONTROLLINO_A11, PRESSED_WHEN_HIGH)};
// push buttons actor array (needs to be same order as PUSH_BUTTONS_DEF)
const char *PUSH_BUTTONS_ACT[2][PUSH_BUTTONS_TOTAL] = {
    {
        //section for topic to which we post in MQTT
        "ACM1", //A5
        "ACM1", //A6
        "ACM1", //A7
        "ACM1", //A8
        "ACM1", //A9
        "ACM0"  //A11
    },
    {
        //section for actual message to post in MQTT
        "{\"action\":\"toggle\",\"output\":[2]}",  //A5
        "{\"action\":\"toggle\",\"output\":[8]}",  //A6
        "{\"action\":\"toggle\",\"output\":[42]}", //A7
        "{\"action\":\"toggle\",\"output\":[42]}", //A8
        "{\"action\":\"toggle\",\"output\":[42]}", //A9
        "{\"action\":\"toggle\",\"output\":[78]}", //A11
    }};

// ---------------------------------------------------------------------------------
// OUTPUT DEFINITION
// ---------------------------------------------------------------------------------

typedef struct c_output
{
    const char *name;
    int pin;
    String entity;
} c_output; 

c_output c_outputs[24] = {{"el_49", CONTROLLINO_D0, "/light/bathroom/downstairs/mirror"},
                 {"el_51", CONTROLLINO_D1, "/light/tech_room"},
                 {"el_52", CONTROLLINO_D2, "/light/hallway/downstairs/entrance"},
                 {"el_53", CONTROLLINO_D3, "/light/bathroom/downstairs/main"},
                 {"el_54", CONTROLLINO_D4, "/light/pantry"},
                 {"el_55", CONTROLLINO_D5, "/light/kitchen/led"},
                 {"el_56", CONTROLLINO_D6, "/light/kitchen/fridge"},
                 {"el_57", CONTROLLINO_D7, "/light/kitchen/island"},
                 {"el_58", CONTROLLINO_D8, "/light/kitchen/dining"},
                 {"el_61", CONTROLLINO_D9, "/light/guest_room"},
                 {"el_62_63", CONTROLLINO_D10, "/light/hallway/downstairs/main"},
                 {"el_64", CONTROLLINO_D11, "/light/living_room/hallway"},
                 {"el_66", CONTROLLINO_D12, "/light/living_room/led/ceiling"},
                 {"el_68", CONTROLLINO_D15, "/light/hallway/stairs/led"},
                 {"el_65", CONTROLLINO_D13, "/light/living_room/led/tv"},
                 {"el_67", CONTROLLINO_D14, "/light/living_room/center"},
                 {"el_71", CONTROLLINO_D16, "/light/living_room/1"},
                 {"el_72", CONTROLLINO_D17, "/light/living_room/2"},
                 {"el_73", CONTROLLINO_D18, "/light/living_room/3"},
                 {"el_74", CONTROLLINO_D19, "/light/living_room/4"},
                 {"el_75", CONTROLLINO_D20, "/light/living_room/5"},
                 {"el_76", CONTROLLINO_D21, "/light/outside/door"},
                 {"el_77", CONTROLLINO_D22, "/light/outside/garage"},
                 {"el_78", CONTROLLINO_D23, "/light/outside/porch"}};


// outputs definition array for being able to read the state through function
// TODO: Need to rewrite this to struct array of int and string
const int TOTAL_OUTPUT_DEF_ARRAY = 24;
const int OUTPUT_DEF_ARRAY[TOTAL_OUTPUT_DEF_ARRAY] = {
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
    CONTROLLINO_D15,
    CONTROLLINO_D16,
    CONTROLLINO_D17,
    CONTROLLINO_D18,
    CONTROLLINO_D19,
    CONTROLLINO_D20,
    CONTROLLINO_D21,
    CONTROLLINO_D22,
    CONTROLLINO_D23};

const int TOTAL_ENTITY_DEF_ARRAY = 24;
const char *ENTITY_DEF_ARRAY[TOTAL_ENTITY_DEF_ARRAY] = {
    "light/bathroom/downstairs/mirror",
    "light/tech_room",
    "light/hallway/downstairs/entrance",
    "light/bathroom/downstairs/main",
    "light/pantry",
    "light/kitchen/led",
    "light/kitchen/fridge",
    "light/kitchen/island",
    "light/kitchen/dining",
    "light/guest_room",
    "light/hallway/downstairs/main",
    "light/living_room/hallway",
    "light/living_room/led/ceiling",
    "light/hallway/stairs/led",
    "light/living_room/led/tv",
    "light/living_room/center",
    "light/living_room/1",
    "light/living_room/2",
    "light/living_room/3",
    "light/living_room/4",
    "light/living_room/5",
    "light/outside/door",
    "light/outside/garage",
    "light/outside/porch"};
#endif