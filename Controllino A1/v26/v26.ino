
#include <SPI.h>
#include <avr/wdt.h>
#include <Controllino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte Mac[]        = {    0xAE, 0xED, 0xBA, 0xFE, 0xFE, 0xC1 };
IPAddress Ip(192, 168, 1, 91);
IPAddress Servers(192, 168, 1, 10);
EthernetClient EthClient;

//SET TIMERS VARIABLES FOR MAIN PROGRAM
unsigned long CurrentTime = 0;
unsigned long PreviousTime1 = 0;

int output_digital[]={CONTROLLINO_D0,CONTROLLINO_D1,CONTROLLINO_D1,CONTROLLINO_D2,CONTROLLINO_D3,CONTROLLINO_D4,CONTROLLINO_D5,CONTROLLINO_D6,CONTROLLINO_D7,CONTROLLINO_D8,CONTROLLINO_D9,CONTROLLINO_D10,CONTROLLINO_D11,CONTROLLINO_D12,CONTROLLINO_D13,CONTROLLINO_D14,CONTROLLINO_D15,CONTROLLINO_D16,CONTROLLINO_D17,CONTROLLINO_D18,CONTROLLINO_D19,CONTROLLINO_D20,CONTROLLINO_D21,CONTROLLINO_D22,CONTROLLINO_D23};
int output_digital_size = sizeof(output_digital)/sizeof(output_digital[0]);

int input_digital[]={CONTROLLINO_A0,CONTROLLINO_A1,CONTROLLINO_A2,CONTROLLINO_A3,CONTROLLINO_A4,CONTROLLINO_A5,CONTROLLINO_A6,CONTROLLINO_A7,CONTROLLINO_A8,CONTROLLINO_A9,CONTROLLINO_A10,CONTROLLINO_A11,CONTROLLINO_A12,CONTROLLINO_A13,CONTROLLINO_A14,CONTROLLINO_A15};
int input_digital_size = sizeof(input_digital)/sizeof(input_digital[0]);

// Here we define what switches what
// Every element 
int switch_0[] = {CONTROLLINO_D0,CONTROLLINO_D16,CONTROLLINO_D23};
int switch_1[] = {CONTROLLINO_D1,CONTROLLINO_D17};
int switch_2[] = {CONTROLLINO_D2,CONTROLLINO_D18};
int switch_3[] = {CONTROLLINO_D3,CONTROLLINO_D19};
int switch_4[] = {CONTROLLINO_D4,CONTROLLINO_D20};
int switch_5[] = {CONTROLLINO_D5,CONTROLLINO_D21};
int switch_6[] = {CONTROLLINO_D6,CONTROLLINO_D22};
int switch_7[] = {CONTROLLINO_D7};
int switch_8[] = {CONTROLLINO_D8};
int switch_9[] = {CONTROLLINO_D9};
int switch_10[] = {CONTROLLINO_D10};
int switch_11[] = {CONTROLLINO_D11};
int switch_12[] = {CONTROLLINO_D12};
int switch_13[] = {CONTROLLINO_D13};
int switch_14[] = {CONTROLLINO_D14};
int switch_15[] = {CONTROLLINO_D15};
int switch_16[] = {};
int switch_17[] = {};
int switch_18[] = {};
int switch_19[] = {};
int switch_20[] = {};
int switch_21[] = {};
int switch_22[] = {};
int switch_23[] = {};



int switch_matrix[][16]={
  {CONTROLLINO_A0,CONTROLLINO_A1,CONTROLLINO_A2,CONTROLLINO_A3,CONTROLLINO_A4,CONTROLLINO_A5,CONTROLLINO_A6,CONTROLLINO_A7,CONTROLLINO_A8,CONTROLLINO_A9,CONTROLLINO_A10,CONTROLLINO_A11,CONTROLLINO_A12,CONTROLLINO_A13,CONTROLLINO_A14,CONTROLLINO_A15},
  {&switch_0,&switch_1,&switch_2,&switch_3,&switch_4,&switch_5,&switch_6,&switch_7,&switch_8,&switch_9,&switch_10,&switch_11,&switch_12,&switch_13,&switch_14,&switch_15}
};

void toggle(int index ){
    int value = switch_matrix[1][index];
    int lightArray = sizeof(value)/sizeof(value[0]);

    for (int i=0; i<lightArray;i++) {
      digitalRead(value[i]);
    }
}

void digitalLightSwitch(int index ){
  int value = digitalRead(switch_matrix[0][index]);
  if (value!=0) {
    toggle( index );
    delay(2000);
  }
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
  for(int i=0; i<input_digital_size; i++){
    pinMode(output_digital[i],INPUT);
  }
  

  // TEST   
  digitalWrite(CONTROLLINO_D1, LOW);
}

void loop() {
  long cap = millis();
  //INITIATE MAJOR TIMER
  CurrentTime = millis(); //Set initial timer

  // Button press detection
  if ((CurrentTime - PreviousTime1) >= 50) {        
    PreviousTime1 = CurrentTime;
    for (int i=0; i<input_digital_size;i++) {
      digitalLightSwitch(input_digital[i]);
    }
  }



// blink all 
//  for(int i=0; i<output_list_size; i++){
//    digitalWrite(output_list[i], HIGH);
//    delay(1000);
//    digitalWrite(output_list[i], LOW);
//  }

  wdt_reset();
}




