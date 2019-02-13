#include <SPI.h>
#include <avr/wdt.h>
#include <Controllino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte Mac[]        = {0xAE, 0xED, 0xBA, 0xFE, 0xFE, 0xC1};
IPAddress Ip(192, 168, 1, 91);
IPAddress Servers(192, 168, 1, 10);
EthernetClient EthClient;

//SET TIMERS VARIABLES FOR MAIN PROGRAM
unsigned long CurrentTime = 0;
unsigned long PreviousTime1 = 0;

int output_digital[]={CONTROLLINO_D0,CONTROLLINO_D1,CONTROLLINO_D2,CONTROLLINO_D3,CONTROLLINO_D4,CONTROLLINO_D5,CONTROLLINO_D6,CONTROLLINO_D7,CONTROLLINO_D8,CONTROLLINO_D9,CONTROLLINO_D10,CONTROLLINO_D11,
CONTROLLINO_D12,CONTROLLINO_D13,CONTROLLINO_D14,CONTROLLINO_D15,CONTROLLINO_D16,CONTROLLINO_D17,CONTROLLINO_D18,CONTROLLINO_D19,CONTROLLINO_D20,CONTROLLINO_D21,CONTROLLINO_D22,CONTROLLINO_D23,
CONTROLLINO_R2,CONTROLLINO_R3,CONTROLLINO_R4,CONTROLLINO_R5,CONTROLLINO_R6,CONTROLLINO_R7,CONTROLLINO_R8,CONTROLLINO_R9,CONTROLLINO_R10,CONTROLLINO_R11,
CONTROLLINO_R12,CONTROLLINO_R13,CONTROLLINO_R14,CONTROLLINO_R15};

int output_digital_size = sizeof(output_digital)/sizeof(*output_digital);

int input_analog[]={CONTROLLINO_A0,CONTROLLINO_A1,CONTROLLINO_A2,CONTROLLINO_A3,CONTROLLINO_A4,CONTROLLINO_A5,CONTROLLINO_A6,CONTROLLINO_A7,
CONTROLLINO_A8,CONTROLLINO_A9,CONTROLLINO_A10,CONTROLLINO_A11,CONTROLLINO_A12,CONTROLLINO_A13,CONTROLLINO_A14,CONTROLLINO_A15};

int input_analog_size = sizeof(input_analog)/sizeof(*input_analog);

// Here we define what switches what
// Every element starts with the number of lights to be turned by it | then whether it should use MQTT or not for that array | then the order of lights ot be turned on

int switch_A0[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor Ohm
 {0,0}, // Voltage    (4) for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A1[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A2[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A3[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A4[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A5[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A6[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A7[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A8[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A9[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A10[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A11[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A12[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A13[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A14[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A15[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A16[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A17[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A18[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A19[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A20[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A21[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A22[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};

int switch_A23[][7] = {
 {2,0, CONTROLLINO_D14,CONTROLLINO_D15}, // When just digital is in use
 {0,0}, // Voltage (0.4)  for button A or 220K resistor 
 {0,0}, // Voltage (4)    for button B or 22K resistor
 {0,0}, // Voltage (7.4)  for button C or 10K resistor
 {0,0}, // Voltage (11.7) for button D or 4K7 resistor
 {0,0}, // Voltage (16)   for button E or 2K2 resistor
 {0,0}, // Voltage (19.6) for button F or 1K resistor
 {0,0}
};


//this will not work, as second array is not defined
int switch_matrix[][16]={
  {CONTROLLINO_A0,CONTROLLINO_A1,CONTROLLINO_A2,CONTROLLINO_A3,CONTROLLINO_A4,CONTROLLINO_A5,CONTROLLINO_A6,CONTROLLINO_A7,CONTROLLINO_A8,CONTROLLINO_A9,CONTROLLINO_A10,CONTROLLINO_A11,CONTROLLINO_A12,CONTROLLINO_A13,CONTROLLINO_A14,CONTROLLINO_A15},
  {&switch_A0,&switch_A1,&switch_A2,&switch_A3,&switch_A4,&switch_A5,&switch_A6,&switch_A7,&switch_A8,&switch_A9,&switch_A10,&switch_A11,&switch_A12,&switch_A13,&switch_A14,&switch_A15}
};
int switch_matrix_size = sizeof(switch_matrix[0])/sizeof(*switch_matrix[0]);


// this array would keep the prev state 
int state_switch_matrix[][16] = {
  {CONTROLLINO_A0,CONTROLLINO_A1,CONTROLLINO_A2,CONTROLLINO_A3,CONTROLLINO_A4,CONTROLLINO_A5,CONTROLLINO_A6,CONTROLLINO_A7,CONTROLLINO_A8,CONTROLLINO_A9,CONTROLLINO_A10,CONTROLLINO_A11,CONTROLLINO_A12,CONTROLLINO_A13,CONTROLLINO_A14,CONTROLLINO_A15},
  {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},  // 100 when not pushed output_index value while holding
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                                  // time 1st pushed
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                                  // last measure
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}                                   // number of pushes
};


void toggle(int index ) {
  int *pointer = switch_matrix[1][index];
  int lightArray = *pointer;
  int use_mqtt = *(pointer + 1);

  //Serial.print("lightArray: ");
  //Serial.println(lightArray);

  for (int i=2; i<lightArray+2; i++) {
    if (use_mqtt == 1) {
      //start MQTT sub to gather state of the light(s)
      //load it to variable that can be itterated and find state of light stored in value (below)

    } else {
      int value = *(pointer + i);
      int state = digitalRead(value);
      state = !state;
      digitalWrite(value, state);
    }
  }
}

void lightSwitch(int index ){
  int value = analogRead(index);

  // When button is pushed (pulldown resistor makes this value alwais 0 unless)
  if (value != 0) {
    int output_index = defineIndex(value);
    int index2       = findElement(switch_matrix[0], switch_matrix_size, index);
    
    // set stateswith - last update time
    state_switch_matrix[index2][2] = millis();

    // if true - button is pushed
    if (state_switch_matrix[index2][0] != output_index) {

      state_switch_matrix[index2][0] = output_index;
      state_switch_matrix[index2][1] = millis();
      toggle(index2);


    }
  }
}

// input 0 -> 1024
// output array index -> controll int switch_AXX
int defineIndex(int voltage) {
  if (voltage > 8 && voltage <= 20) {  // Voltage (0.4) for button A or 220K resistor
    return 1;
  } else if (voltage > 20 && voltage <= 140) {  // Voltage (4) for button B or 22K resistor
    return 2;
  } else if (voltage > 140 && voltage <= 255) {  // Voltage (7.4) for button C or 10K resistor
    return 3;
  } else if (voltage > 255 && voltage <= 405) {  //Voltage (11.7) for button D or 4K7 resistor
    return 4;
  } else if (voltage > 405 && voltage <= 560) {  // Voltage (16) for button E or 2K2 resistor
    return 5;
  } else if (voltage > 560 && voltage <= 720) {  // Voltage (19.6) for button F or 1K resistor
    return 6;
  } else if (voltage > 720) {
    return 0;
  } else {
    return 100;
  }
}

//IT SEARCHES FOR POSSITION IN ARRAY BASED ON PROVIDED KEY.
//DEFINITION REQUIRES ARRAY NAME AND POINTER, AMOUNT OF VALUES IN THE ROW, SEARCHING KEY
int findElement(int array[], int lastElement, int searchKey) { 
  int index = -1;
  for(int i=0; i<lastElement; i++) {
    if(array[i] == searchKey) { index = i;}
  }
  return index;
}


void setup() {
    //INITIATE SERIAL PORT
  Serial.begin(9600);

  Serial.setTimeout(50);
  Ethernet.begin(Mac, Ip);
    
  // init Digital Output
  for(int i=0; i<output_digital_size; i++){
    pinMode(output_digital[i],OUTPUT);
  }
  
  // init Digital Input
  for(int i=0; i<input_analog_size; i++){
    pinMode(input_analog[i],INPUT);
  }
  }

void loop() {
  long cap = millis();
  //INITIATE MAJOR TIMER
  CurrentTime = millis(); //Set initial timer

  // Test of pushing A0 button
  // ***** to be removed *****
  //digitalWrite(input_analog[i], HIGH);

  // Button press detection
  if ((CurrentTime - PreviousTime1) >= 50) {        
    PreviousTime1 = CurrentTime;
    for (int i=0; i<input_analog_size;i++) {
      
      lightSwitch(input_analog[i]);
      
      delay(2);
    }
      // Test of pushing A0 button
      // ***** to be removed *****
      //digitalWrite(input_analog[i], LOW);
  }


  wdt_reset();
}




