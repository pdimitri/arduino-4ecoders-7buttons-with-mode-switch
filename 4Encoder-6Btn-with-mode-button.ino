//Buttonbox with Encoders sketch
//Supports up to 21 buttons and up to 4 encoders
//based on the Sketch from TOPMO3
//
//
//Arduino IDE 1.6.6 (or above) !
//
//Should be used with an Arduino Pro Micro or any other micro controller with an ATmega32U4 chipset, that may act as an USB Device
//
//To identify the pin mumbers on your Arduino refer to this pin layout https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro
//
//Joystick library from  Matthew Heironimus, https://github.com/MHeironimus/ArduinoJoystickLibrary
// 
//Encoders code from Ben Buxton
//More info: http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
// 
//Thank you guys! :)
//



//Uncomment this for HALFSTEP encoder operation
//#define HALF_STEP

#include <Keypad.h>
#include <Joystick.h>
#include <Bounce2.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 4
#define NUM_JOYSTICK_BUTTONS 32


const int NUM_MODES = 3;
const int NUM_ENCODER_BUTTONS = 2;
const int NUM_SWITCHES = 8;

struct rotariesdef {
  byte pin1;
  byte pin2;
  int buttons [NUM_MODES][NUM_ENCODER_BUTTONS];
  volatile unsigned char state;
};

struct switchdef {
  byte pin;
  byte joystick_button[NUM_MODES];
};

struct hatdef {
  byte hat_nr;
  int angle;
};

byte MODE = 0;
byte LAST_MODE = 0;
unsigned long previousMillis = 0;
int ledState = LOW;

//=====================================================================
//Please adjust to your personal choice of PIN Connections from here...
//=====================================================================

//Select the digital input pin numbers where you attached the mode switch (push button) and the LED (use a 22O Ohm resistor in Front of LED)
const int MODE_PIN = 10;
byte LED_PIN = 21; 


//The first two numbers in each line are the digital inputs where you attached the CLK and DT connectors of the encoders 
rotariesdef rotaries[NUMROTARIES] {
  {0,1,{{1,2},{3,4},{5,6}},0},
  {2,3,{{7,8},{9,10},{11,12}},0},
  {4,5,{{13,14},{15,16},{17,18}},0},
  {6,7,{{19,20},{21,22},{23,24}},0},
};

//The first number in each line are the pins with simple push buttons attached
switchdef switches[NUM_SWITCHES] = {
  {8,{25,26,27}},
  {9,{28,29,30}},
  {14,{31,32,33}},
  {15,{34,35,36}},
  {16,{37,38,39}},
  {18,{40,41,42}},
  {19,{43,44,45}},
  {20,{47,48,49}},
};

//=====================================================================
//...to here
//=====================================================================

hatdef hatswitches[16] = {
  {0,0},
  {0,45},
  {0,90},
  {0,135},
  {0,180},
  {0,225},
  {0,270},
  {0,315},
  {1,0},
  {1,45},
  {1,90},
  {1,135},
  {1,180},
  {1,225},
  {1,270},
  {1,315}
};

// Last state of the button
int lastButtonState[NUM_SWITCHES] = {0,0,0,0,0,0,0,0};

bool hatValueChanged[2] = {false, false};
int lastHatPin = 0;
int lastHatState[2] = {0,0};
bool valueChanged[2] = {false, false};



//initialize an Joystick with 34 buttons;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 32, JOYSTICK_DEFAULT_HATSWITCH_COUNT,
  false, false, false, false, false, false,
  false, false, false, false, false);

Bounce  myButton  = Bounce();

unsigned int interval = 1000;

void blinker(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_PIN, ledState);
  }
}


void CheckAllButtons(void) {
      byte hatArrayIndex;

      
      for (int index = 0; index < NUM_SWITCHES; index++)
      {
        int currentButtonState = !digitalRead(switches[index].pin);

        if (currentButtonState != lastButtonState[index])
        {
            //switches[index].joystick_button[LAST_MODE] is the number of the joystick button that should be emulated.
            //because a maximum of 32 Buttons is supported by the joystick library, we also emulate two 
            //hat switches, which gives us up to 16 additional "buttons"
            //remember: with the push buttons of the encoders, all in all we already use up to 32 emulated "buttons" for the four encoders in their three different modes
            
            if(switches[index].joystick_button[LAST_MODE] < 32){
              Joystick.setButton(switches[index].joystick_button[LAST_MODE], currentButtonState);
            } else{
              hatArrayIndex = switches[index].joystick_button[LAST_MODE] - 32;
              if(currentButtonState==LOW){
                    Joystick.setHatSwitch(hatswitches[hatArrayIndex].hat_nr, -1);
              } else {
                Joystick.setHatSwitch(hatswitches[hatArrayIndex].hat_nr, hatswitches[hatArrayIndex].angle);
              }
            }
            lastButtonState[index] = currentButtonState;
        }
      }
}



void CheckMODE_PIN(){
  myButton.update();
  if (myButton.risingEdge()) {
    if (LAST_MODE < (NUM_MODES - 1)){
      LAST_MODE++;
    } else {
      LAST_MODE=0;
    }
    
    switch (LAST_MODE) {
    case 0:    // your hand is on the sensor
      digitalWrite(LED_PIN, LOW);
      break;
    case 1:    // your hand is close to the sensor
      digitalWrite(LED_PIN, HIGH);
      break;
    }
    
  }
}


#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
// Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif


/* Read input pins and process for events. Call this either from a
 * loop or an interrupt (eg pin change or timer).
 *
 * Returns 0 on no event, otherwise 0x80 or 0x40 depending on the direction.
 */
unsigned char rotary_process(int _i) {
   unsigned char pinstate = (digitalRead(rotaries[_i].pin2) << 1) | digitalRead(rotaries[_i].pin1);
  rotaries[_i].state = ttable[rotaries[_i].state & 0xf][pinstate];
  return (rotaries[_i].state & 0x30);
}

void CheckAllEncoders(void) {
  for (int i=0;i<NUMROTARIES;i++) {
    unsigned char result = rotary_process(i);
    if (result == DIR_CCW) {
        Joystick.setButton(rotaries[i].buttons[LAST_MODE][0]-1, 1); delay(50); Joystick.setButton(rotaries[i].buttons[LAST_MODE][0]-1, 0);
    }
    
    if (result == DIR_CW) {
        Joystick.setButton(rotaries[i].buttons[LAST_MODE][1]-1, 1); delay(50); Joystick.setButton(rotaries[i].buttons[LAST_MODE][1]-1, 0);
      }
    };
}



/* Call this once in setup(). */
void rotary_init() {
  for (int i=0;i<NUMROTARIES;i++) {
    pinMode(rotaries[i].pin1, INPUT);
    pinMode(rotaries[i].pin2, INPUT);
    #ifdef ENABLE_PULLUPS
      digitalWrite(rotaries[i].pin1, HIGH);
      digitalWrite(rotaries[i].pin2, HIGH);
    #endif
  }
}

void buttons_init() {
    for (int index = 0; index < NUM_SWITCHES; index++) {
      pinMode(switches[index].pin,INPUT_PULLUP);
    }
}




void setup() {
  Serial.begin(9600);
  
  pinMode(MODE_PIN,INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  myButton.attach(MODE_PIN);
  myButton.interval(5); // 5ms zum Entprellen
  
  buttons_init();
  Joystick.begin();

}



void loop() { 
  
  CheckMODE_PIN();
  CheckAllEncoders();
  CheckAllButtons();

  if(LAST_MODE == 2){
    blinker();
  }
}
