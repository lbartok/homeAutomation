/* IMPORT LIBRARIES */

#include <SPI.h>
#include <avr/wdt.h>
#include <Controllino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

/* DEFINE VARIABLES */

//THIS IS JUST ASSIGNMENT >> Btn0 >> "0", etc.
#define Btn0        0 
#define Btn1        1
#define Btn2        2
#define Btn3        3
#define BtnNone 4

/* GLOBAL VARIABLE ASSIGNMENT */

//SET ETHERNET VARIABLES
byte Mac[]        = {    0xAE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress Ip(192, 168, 1, 91);
IPAddress Servers(192, 168, 1, 21);
EthernetClient EthClient;

void MQTTCallback(char* topic, byte* payload, unsigned int length) {
    String Converter = "";
    for (int i=0;i<length;i++) {
        Converter = Converter + ((char)payload[i]);
    }
    MQTTEventHandlerIn(Converter.substring(0,3),Converter.substring(4,8),Converter.substring(9,12).toInt());

    }

PubSubClient Clients(Servers, 1883, MQTTCallback, EthClient);

//SET TIMERS VARIABLES FOR MAIN PROGRAM
unsigned long CurrentTime = 0;
unsigned long PreviousTime1 = 0;
unsigned long PreviousTime2 = 0;
unsigned long PreviousTime3 = 0;
unsigned long PreviousTime4 = 0;
unsigned long PreviousTime5 = 0;


//ASSIGNMENT OF ANALOG INPUT EXAMPLE PORT AI0 = 54
String AnalogInputMapped [2][12] = {
    {"AI00","AI01","AI02","AI03","AI04","AI05","AI06","AI07","AI08","AI09","AI10","AI11"},
    {"54","55","56","57","58","59","60","61","62","63","64","65"}
    };

//ASSIGNMENT OF DIGITAL INPUT PORT DO0 = 2
String DigitalOutputMapped [4][18] = {
    {"D000","DO01","DO02","DO03","DO04","DO05","DO06","DO07","RO00","RO01","RO02","RO03","RO04","RO05","RO06","RO07","RO08","RO09"},
    {"2","3","4","5","6","7","8","9","22","23","24","25","26","27","28","29","30","31"},
    {"PORTE","PORTE","PORTG","PORTE","PORTH","PORTH","PORTH","PORTH","PORTA","PORTA","PORTA","PORTA","PORTA","PORTA","PORTA","PORTA","PORTC","PORTC"},
    {"4","5","5","3","3","4","5","6","0","1","2","3","4","5","6","7","7","6"}
    };

/* GENERAL ASSIGNMENT LIGHT SECTION */

//ASSIGNMENT OF ANALOG IMPUTS TO LIGHT MODULE
int InputGroupLightStandard [4]=    {54,55,56,57};

//ARRAY THAT MAPS ANALOG IMPUTS TO DIGITAL OUTPUTS
//{{ANALOG INPUT FRAGMENT BTN ASSINGED TO DIGITAL OUTPUT PIN}, {VALUE INCRESES WHEN ANALOG INPUT FRAGMENT BTN PRESENTED AS PUSHED},{N/A} 
// {{OUTPUT PIN DETAILS EXAMPLE: DO0},{AI BTN 0 - 3 BUTTON IS PRESSED = 1+, BTN 0 - 3 ARE RELEASED = 0},{N/A}
int InputGroupLightStandardMapped [4][3][4] = {
    {{2,3,4,5},{0,0,0,0},{0,0,0,0}},
    {{99,99,99,99},{0,0,0,0},{0,0,0,0}}, 
    {{99,99,99,99},{0,0,0,0},{0,0,0,0}},
    {{99,99,99,99},{0,0,0,0},{0,0,0,0}}    
    };

/* GENERAL ASSIGNMENT SHADE SECTION */

//ASSIGNMENT OF ANALOG IMPUTS TO SHADE MODULE
int InputGroupShadeStandard [4]=    {58,58,59,59};

//ARRAY THAT MAPS ANALOG IMPUTS TO DIGITAL OUTPUTS
//{{ANALOG INPUT FRAGMENT BTN},{DIGITAL OUTPUT UP, DIGITAL OUTPUT DOWN},{VALUE INCRESES WHEN ANALOG INPUT FRAGMENT BTN PRESENTED AS PUSHED FOR UP/DOWN},{TIMER TIMESTAMPS UP/DOWN}}
long InputGroupShadeStandardMapped [4][4][2] = {
    {{0,1},{22,23},{0,0},{0,0}},
    {{2,3},{24,25},{0,0},{0,0}}, 
    {{0,1},{26,27},{0,0},{0,0}},
    {{2,3},{28,29},{0,0},{0,0}}    
    };

//SHADE GENERAL TIMER UP/DOWN (IN THAT TIME SHADE MUST REACH MAXIMUM UP OR MAXIMUM DOWN)
int ShadeTime = 5000;

//WINDPROTECTION TRUE/FALSE
boolean WindProtection = false;

/* GENERAL ASSIGNMENT FAN SECTION */

//ASSIGNMENT OF ANALOG IMPUTS TO FAN MODULE
int InputGroupFanStandard [2]=    {60,60};

//ARRAY THAT MAPS ANALOG IMPUTS TO DIGITAL OUTPUTS
//{{ANALOG INPUT FRAGMENT BTN},{DIGITAL OUTPUT},{VALUE INCRESES WHEN ANALOG INPUT FRAGMENT BTN PRESENTED AS PUSHED},{TIMER TIMESTAMP}}
long InputGroupFanStandardMapped [2][4][1] = {
    {{0},{30},{0},{0}},
    {{1},{31},{0},{0}}
    };

//FAN GENERAL TIMER
int FanTime = 5000;

/* INITIAL SETUP */

void setup() {

    //INITIATE SERIAL PORT
    Serial.begin(9600);
    Serial.setTimeout(50);

    Ethernet.begin(Mac, Ip);

    //ACTIVATE ALL ANALOG INPUTS FROM AnalogInputMapped ARRAY
    for (int i=0; i<sizeof(AnalogInputMapped[0])/sizeof(*AnalogInputMapped[0]);i++) { // Assign I/O to Analog PINs
        pinMode(AnalogInputMapped[1][i].toInt(), INPUT); //Set all PINs from AnalogInputMapped array as INPUT
    }

    //ACTIVATE ALL DIGITAL OUTPUTS FROM ANALOG INPUT MAPPED ARRAY
    for (int i=0; i<sizeof(DigitalOutputMapped[0])/sizeof(*DigitalOutputMapped[0]);i++) { // Assign I/O to Relay PINs (10 RELAYS AVAILABLE)
        pinMode(DigitalOutputMapped[1][i].toInt(), OUTPUT); // Set all PINs from DigitalOutputMapped array as OUTPUT
    }

    if (!Clients.connected()) {
        if (Clients.connect("ArduinoClient")) {
            Clients.publish("Module2CMAOut","hello");
            Clients.subscribe("Module2CMAIn");
        }
    }





    // START WATCHDOG
    wdt_enable(WDTO_4S);

    }


// THIS DEFINITION CHECKS IF BUTTON IS PRESSED OR NOT, IT RETURNS VALUE 0-3 IF BUTTON PRESSED AND 4 IF NOT.
int ReadAnalogButtons(int Pin) { // DEFINITION REQUIRES ANALOG PIN NUMBER AND RETURNS BTN NUMBER.
    int KeyIn = 0;
    KeyIn = analogRead(Pin);
    if (KeyIn <= 200) return BtnNone;    
    else if ((KeyIn > 200) && (KeyIn < 350)) return Btn0;    
    else if ((KeyIn > 350) && (KeyIn < 550)) return Btn1;    
    else if ((KeyIn > 550) && (KeyIn < 750)) return Btn2;    
    else if (KeyIn >= 750) return Btn3;    
    return BtnNone;
    }

//IT SEARCHES FOR POSSITION IN ARRAY BASED ON PROVIDED KEY.
int FindElement(String Array[], int LastElement, String SearchKey) {//DEFINITION REQUIRES ARRAY NAME AND POINTER, AMOUNT OF VALUES IN THE ROW, SEARCHING KEY 
    int Index = -1;
    for(int i=0; i<LastElement; i++) {
        if(Array[i] == SearchKey) { Index = i;}
    }
    return Index;
    }

/* LIGHT STANDARD DEFINITION */

void LightStandard(int MapPosition, int PinElement) {
    int ReadKey = PinElement;
    if ((ReadKey < 0) or (ReadKey > 3)) { ReadKey = ReadAnalogButtons(InputGroupLightStandard[MapPosition]); }
    switch (ReadKey) {
        case BtnNone: {
            for (int i=0; i<4; i++) {
                if ((InputGroupLightStandardMapped [MapPosition][0][i] != 99) && (InputGroupLightStandardMapped[MapPosition][1][i] > 0)) {
                    int Index = FindElement(DigitalOutputMapped[1],sizeof(DigitalOutputMapped[1])/sizeof(*DigitalOutputMapped[1]), String(InputGroupLightStandardMapped [MapPosition][0][i]));
                    int PinStatus = -1;
                    if (DigitalOutputMapped[2][Index] == "PORTA") { PinStatus = bitRead(PORTA,DigitalOutputMapped[3][Index].toInt());}
                    else if (DigitalOutputMapped[2][Index] == "PORTB") { PinStatus = bitRead(PORTB,DigitalOutputMapped[3][Index].toInt());}
                    else if (DigitalOutputMapped[2][Index] == "PORTC") { PinStatus = bitRead(PORTC,DigitalOutputMapped[3][Index].toInt());}
                    else if (DigitalOutputMapped[2][Index] == "PORTD") { PinStatus = bitRead(PORTD,DigitalOutputMapped[3][Index].toInt());}
                    else if (DigitalOutputMapped[2][Index] == "PORTE") { PinStatus = bitRead(PORTE,DigitalOutputMapped[3][Index].toInt());}
                    else if (DigitalOutputMapped[2][Index] == "PORTF") { PinStatus = bitRead(PORTF,DigitalOutputMapped[3][Index].toInt());}
                    else if (DigitalOutputMapped[2][Index] == "PORTG") { PinStatus = bitRead(PORTG,DigitalOutputMapped[3][Index].toInt());}
                    else if (DigitalOutputMapped[2][Index] == "PORTH") { PinStatus = bitRead(PORTH,DigitalOutputMapped[3][Index].toInt());}

                    if (PinStatus == 1) { 
                        digitalWrite(InputGroupLightStandardMapped[MapPosition][0][i], LOW);
                        InputGroupLightStandardMapped[MapPosition][2][i] = 0;
                        MQTTEventHandlerOut("LHT", InputGroupLightStandardMapped[MapPosition][0][i], 0);
                    }
                    else if (PinStatus == 0) {
                        digitalWrite(InputGroupLightStandardMapped[MapPosition][0][i], HIGH);
                        InputGroupLightStandardMapped[MapPosition][2][i] = 1;
                        MQTTEventHandlerOut("LHT", InputGroupLightStandardMapped[MapPosition][0][i], 1);
                    }
                    InputGroupLightStandardMapped[MapPosition][1][i] = 0;
                } 
            }
        break;
        }
        case Btn0: { InputGroupLightStandardMapped[MapPosition][1][0]++; break;}
        case Btn1: { InputGroupLightStandardMapped[MapPosition][1][1]++; break;}
        case Btn2: { InputGroupLightStandardMapped[MapPosition][1][2]++; break;}
        case Btn3: { InputGroupLightStandardMapped[MapPosition][1][3]++; break;}

    }
    }

/* SHADE STANDARD DEFINITION */

void ShadeStandard(int MapPosition, int PinElement) {
    int ReadKey = PinElement;
    if ((ReadKey < 0) or (ReadKey > 3)) { ReadKey = ReadAnalogButtons(InputGroupShadeStandard[MapPosition]); }
    switch (ReadKey) {
        case BtnNone: {
            if ((InputGroupShadeStandardMapped[MapPosition][3][0] > 0 ) && (InputGroupShadeStandardMapped[MapPosition][2][1] > 0)) { 
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][0], LOW);
                InputGroupShadeStandardMapped[MapPosition][3][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][3][1] = 0; 
                InputGroupShadeStandardMapped[MapPosition][2][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][2][1] = 0;
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][0], 0);
            }
            else if ((InputGroupShadeStandardMapped[MapPosition][3][1] > 0 ) && (InputGroupShadeStandardMapped[MapPosition][2][0] > 0)) { 
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][1], LOW);
                InputGroupShadeStandardMapped[MapPosition][3][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][3][1] = 0; 
                InputGroupShadeStandardMapped[MapPosition][2][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][2][1] = 0;
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][1], 0);
            }
            else if ((InputGroupShadeStandardMapped[MapPosition][2][1] > 0 ) && (InputGroupShadeStandardMapped[MapPosition][2][0] > 0)) { 
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][1], LOW);
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][0], LOW);
                InputGroupShadeStandardMapped[MapPosition][3][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][3][1] = 0; 
                InputGroupShadeStandardMapped[MapPosition][2][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][2][1] = 0;    
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][1], 0);
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][0], 0);
            }
            else if ((InputGroupShadeStandardMapped[MapPosition][2][0] > 0 ) && (InputGroupShadeStandardMapped[MapPosition][3][0] == 0)) { 
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][0], HIGH);
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][1], LOW);
                InputGroupShadeStandardMapped[MapPosition][3][0] = CurrentTime;
                InputGroupShadeStandardMapped[MapPosition][3][1] = 0; 
                InputGroupShadeStandardMapped[MapPosition][2][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][2][1] = 0;                 
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][0], 1);
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][1], 0);
            }
            else if ((InputGroupShadeStandardMapped[MapPosition][2][1] > 0 ) && (InputGroupShadeStandardMapped[MapPosition][3][1] == 0)) { 
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][0], LOW);
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][1], HIGH);
                InputGroupShadeStandardMapped[MapPosition][3][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][3][1] = CurrentTime; 
                InputGroupShadeStandardMapped[MapPosition][2][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][2][1] = 0;        
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][0], 0);
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][1], 1);
            }
            break;
        }

        case Btn0: { 
            if (InputGroupShadeStandardMapped[MapPosition][0][0] == 0) {InputGroupShadeStandardMapped[MapPosition][2][0]++;}
            break;
        }
        case Btn1: {
            if (InputGroupShadeStandardMapped[MapPosition][0][1] == 1) {InputGroupShadeStandardMapped[MapPosition][2][1]++;}
            break;
        }
        case Btn2: { 
            if (InputGroupShadeStandardMapped[MapPosition][0][0] == 2) {InputGroupShadeStandardMapped[MapPosition][2][0]++;}
            break;
        }
        case Btn3: {
            if (InputGroupShadeStandardMapped[MapPosition][0][1] == 3) {InputGroupShadeStandardMapped[MapPosition][2][1]++;}
            break;
        }
    }
    for (int i=0; i<2; i++) {
        if (((InputGroupShadeStandardMapped[MapPosition][3][i] + ShadeTime) < CurrentTime) && (InputGroupShadeStandardMapped[MapPosition][3][i] != 0)) {
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][0], LOW);
                digitalWrite(InputGroupShadeStandardMapped[MapPosition][1][1], LOW);
                InputGroupShadeStandardMapped[MapPosition][3][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][3][1] = 0; 
                InputGroupShadeStandardMapped[MapPosition][2][0] = 0;
                InputGroupShadeStandardMapped[MapPosition][2][1] = 0;        
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][0], 0);
                MQTTEventHandlerOut("SHD", InputGroupShadeStandardMapped[MapPosition][1][1], 0);
        }
    }

    }

/* FAN STANDARD DEFINITION */

void FanStandard(int MapPosition, int PinElement) {
    int ReadKey = PinElement;
    if ((ReadKey < 0) or (ReadKey > 3)) { ReadKey = ReadAnalogButtons(InputGroupFanStandard[MapPosition]); }
    switch (ReadKey) {
        case BtnNone: {
            if ((InputGroupFanStandardMapped[MapPosition][2][0] > 0 ) && (InputGroupFanStandardMapped[MapPosition][3][0] > 0)) { 
                digitalWrite(InputGroupFanStandardMapped[MapPosition][1][0], LOW);
                InputGroupFanStandardMapped[MapPosition][2][0] = 0;
                InputGroupFanStandardMapped[MapPosition][3][0] = 0;
                MQTTEventHandlerOut("FAN", InputGroupFanStandardMapped[MapPosition][1][0], 0);
            }
            else if ((InputGroupFanStandardMapped[MapPosition][2][0] > 0 ) && (InputGroupFanStandardMapped[MapPosition][3][0] == 0)) { 
                digitalWrite(InputGroupFanStandardMapped[MapPosition][1][0], HIGH);
                InputGroupFanStandardMapped[MapPosition][2][0] = 0;
                InputGroupFanStandardMapped[MapPosition][3][0] = CurrentTime;
                MQTTEventHandlerOut("FAN", InputGroupFanStandardMapped[MapPosition][1][0], 1);
            }
            break;
        }
        case Btn0: { 
            if (InputGroupFanStandardMapped[MapPosition][0][0] == 0) {InputGroupFanStandardMapped[MapPosition][2][0]++;}
            break;
        }
        case Btn1: {
            if (InputGroupFanStandardMapped[MapPosition][0][0] == 1) {InputGroupFanStandardMapped[MapPosition][2][0]++;}
            break;
        }
        case Btn2: { 
            if (InputGroupFanStandardMapped[MapPosition][0][0] == 2) {InputGroupFanStandardMapped[MapPosition][2][0]++;}
            break;
        }
        case Btn3: {
            if (InputGroupFanStandardMapped[MapPosition][0][0] == 3) {InputGroupFanStandardMapped[MapPosition][2][0]++;}
            break;
        }
    }

    if (((InputGroupFanStandardMapped[MapPosition][3][0] + FanTime) < CurrentTime) && (InputGroupFanStandardMapped[MapPosition][3][0] != 0)) {
        digitalWrite(InputGroupFanStandardMapped[MapPosition][1][0], LOW);
        InputGroupFanStandardMapped[MapPosition][3][0] = 0;    
        InputGroupFanStandardMapped[MapPosition][2][0] = 0;
        MQTTEventHandlerOut("FAN", InputGroupFanStandardMapped[MapPosition][1][0], 0);
     }
     }

void MQTTEventHandlerIn(String Module, String Port, int Parameter) {

    if (Module == 'LHT') {
        int Index = FindElement(AnalogInputMapped[0],sizeof(AnalogInputMapped[0])/sizeof(*AnalogInputMapped[0]), Port);
        if (Index >= 0) {
            for(int i=0; i<sizeof(InputGroupLightStandard)/sizeof(*InputGroupLightStandard); i++) {
                if (AnalogInputMapped[1][Index] == String(InputGroupLightStandard[i])) { LightStandard(i,Parameter); break;}
            }     
        }

    }

    if (Module == 'SHD') {
        Serial.println(Module + "    " + Port);
        int Index = FindElement(AnalogInputMapped[0],sizeof(AnalogInputMapped[0])/sizeof(*AnalogInputMapped[0]), Port);
        Serial.println("Index " + String(Index));
        if (Index >= 0) {
            for(int i=0; i<sizeof(InputGroupShadeStandard)/sizeof(*InputGroupShadeStandard); i++) {
                if (AnalogInputMapped[1][Index] == String(InputGroupShadeStandard[i])) { ShadeStandard(i,Parameter);}
            }
        }

    }


    if (Module == 'FAN') {
        Serial.println(Module + "    " + Port);
        int Index = FindElement(AnalogInputMapped[0],sizeof(AnalogInputMapped[0])/sizeof(*AnalogInputMapped[0]), Port);
        Serial.println("Index " + String(Index));
        if (Index >= 0) {
            for(int i=0; i<sizeof(InputGroupFanStandard)/sizeof(*InputGroupFanStandard); i++) {
                if (AnalogInputMapped[1][Index] == String(InputGroupFanStandard[i])) { FanStandard(i,Parameter);}
            }
        }
    }



    }




void MQTTEventHandlerOut(String Module, int Port, int Status ) {
    if (Clients.connected()) {
        String UpdatedPort = ("000" + String(Port)).substring(String(Port).length(), String(Port).length() + 3);
        String UpdatedStatus = ("000" + String(Status)).substring(String(Status).length(), String(Status).length() + 3);
        String Message = Module + "_" + UpdatedPort + "_" + UpdatedStatus;
        char Buffer [Message.length() + 1];
        Message.toCharArray(Buffer,Message.length() + 1);
        Clients.publish("Module2CMAOut", Buffer, Message.length() + 1);
    }
}





void loop() {

    long cap = millis();



    //INITIATE MAJOR TIMER
    CurrentTime = millis(); //Set initial timer

    //PERFORM EVERY 50MS USING MAJOR TIMER, LIGHT STANDARD
    if ((CurrentTime - PreviousTime1) >= 50) {        
        PreviousTime1 = CurrentTime;
        for (int i=0; i<sizeof(InputGroupLightStandard)/sizeof(*InputGroupLightStandard);i++) {
            LightStandard(i, -1);
        }
    }

    //PERFORM EVERY 50MS USING MAJOR TIMER, SHADE STANDARD
    if ((CurrentTime - PreviousTime2) >= 50) { 
        PreviousTime2 = CurrentTime;
        if (WindProtection == false) {
            for (int i=0; i<sizeof(InputGroupShadeStandard)/sizeof(*InputGroupShadeStandard);i++) {
                ShadeStandard(i, -1);
            }
        }
    }

    //PERFORM EVERY 50MS USING MAJOR TIMER, FAN STANDARD
    if ((CurrentTime - PreviousTime3) >= 50) { 
        PreviousTime3 = CurrentTime;
        for (int i=0; i<sizeof(InputGroupFanStandard)/sizeof(*InputGroupFanStandard);i++) {
            FanStandard(i, -1);
        }
    }


    long cap1 = millis();


    if ((CurrentTime - PreviousTime4) >= 600000) { 
        PreviousTime4 = CurrentTime;
        Serial.println(String(Clients.connected()));
        if (!Clients.connected()) {
            if (Clients.connect("ArduinoClient")) {
                Clients.publish("Module2CMAOut","hello");
                Clients.subscribe("Module2CMAIn");
            }
        }
    }

    if ((CurrentTime - PreviousTime5) >= 200) { 
        PreviousTime5 = CurrentTime;
        Clients.loop();
    } 



    if (cap1 - cap > 2) { Serial.println("time before " + String(cap) +" time after "+ String(cap1)); }

    wdt_reset();

    }

