/*
  Name: Rotaryencoder
  Date: 26-11-2021
  Author: Daan Heetkamp, Bram Geerdink
  Description: Zorg dat er doormiddel van een rotary knop nummers worden laten zien.
  Revision: V0.1
*/

// ----- include libraries -----

// ----- Declare Constants -----

/******rotaryKnop******/



// ----- Declare Objects -----

// ----- Declare subroutines and/or functions -----

// ----- Declare Global Variables -----
int rotaryKnop = A1;
int rotaryLinks = A2;
int rotaryRechts = A3;

byte rotaryWaarde = 0;
byte displayWaarde = 0;
byte bcdWaarde[3] = {0,0,0};
byte laatsteDraaiwaarde = 0;
byte bcdLine = 1;
bool laatsteGedrukt = false;



// Setup
void setup()
{
  Serial.begin(9600);

  pinMode(rotaryKnop, INPUT);
  pinMode(rotaryLinks, INPUT);
  pinMode(rotaryRechts, INPUT);

  laatsteDraaiwaarde = digitalRead(rotaryLinks);
}

// Main loop
void loop()
{
  if(laatsteGedrukt == HIGH && digitalRead(rotaryKnop) == LOW)
  {
    bcdWaarde[bcdLine] = rotaryWaarde;
    bcdLine++;
    if(bcdLine == 4)// Als die 4 is moet die naar nummer 1 gaan.
    {
      bcdLine = 1;
    }
    Serial.println("Knop is gedrukt");
    delay(100);
  }
  laatsteGedrukt = digitalRead(rotaryKnop);

  if(digitalRead(rotaryLinks) != laatsteDraaiwaarde)
  {
    if(digitalRead(rotaryRechts) == laatsteDraaiwaarde)
    {
      if(rotaryWaarde == 18){
      }else {
        rotaryWaarde++;
        Serial.print(rotaryWaarde);
      }
    }else{
      if(rotaryWaarde == 0){
      }else {
        rotaryWaarde--;
        Serial.print(rotaryWaarde);
      }
    }
  }
  laatsteDraaiwaarde = digitalRead(rotaryLinks);
  displayWaarde = rotaryWaarde / 2;

  //Serial.print("De waarde van de knop is:");
  Serial.println(displayWaarde);
  /*
  Serial.print(" - ");
  Serial.print(bcdWaarde[1]);
  Serial.print(" - ");
  Serial.print(bcdWaarde[2]);
  Serial.print(" - ");
  Serial.println(bcdWaarde[3]);*/
}
