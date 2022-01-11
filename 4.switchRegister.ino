/*
  Name:
  Date:
  Author: Daan Heetkamp

  Description:

  Revision:

*/

// ----- include libraries -----
//hi
// ----- Declare Constants -----

// ----- Declare Objects -----

// ----- Declare subroutines and/or functions -----

// ----- Declare Global Variables -----


/**************Switch Register*******************/
int latchPin = 3;  // Clock pin of 74HC595 is connected to Digital pin 6 RCLK
int klokPin = 2;  // Latch pin of 74HC595 is connected to Digital pin 5 SRCLK
int dataPin = 4;  // Data pin of 74HC595 is connected to Digital pin 4 SER


byte data;    // Variable to hold the pattern of which LEDs are currently turned on or off

bool eindWaarde = false;

// Setup
void setup()
{
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(klokPin, OUTPUT);

  data = 0; // Initially turns all the LEDs off, by giving the variable 'leds' the value 0
  updateShiftRegister();

  data = B00000000; //8
}

// Main loop
void loop()
{
  //bitSet(data, 2); //data 0000 0100 zet een bit hoog.
  //updateShiftRegister(); //shift de data in het register.
  if(data == 9){
    eindWaarde = true; //9? aftrekken
  } else if(data == 0){
    eindWaarde = false;// 0? optellen
  }

  if(eindWaarde == false){
    data = data + B00000001;
    updateShiftRegister();
  }
  else if(eindWaarde == true){
    data = data - B00000001;
    updateShiftRegister();
  }
  delay(1000);
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, klokPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}
