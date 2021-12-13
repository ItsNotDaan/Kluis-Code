/*
  Name: Kluis
  Date:
  Author: Daan Heetkamp, Bram Geerdink

  Description: Een kluis die dicht en open kan gaan.

  Revision: 1.4

*/

// ----- include libraries -----
#include <Wire.h>
#include <Servo.h>

// ----- Declare Constants -----

// ----- Declare Objects -----

// ----- Declare subroutines and/or functions -----

// ----- Declare Global Variables -----

/**************LED/Display*******************/

int groenLED = 1; //De groene led.
int roodLED = 2; //De rode led.

int sevenSeg1 = 3; //Het eerste seven segmenten display.
int sevenSeg2 = 4; //Het tweede seven segmenten display.
int sevenSeg3 = 5; //Het derde seven segmenten display.

/**************IC/Encoder*******************/
int rotaryLinks = 6; //De linker kant van de rotary encoder.
int rotaryRechts = 7; //De rechter kant van de rotary encoder.

int klokPin = 8;  // Latch pin of 74HC595 is connected to Digital pin 5 SRCLK
int latchPin = 9;  // Clock pin of 74HC595 is connected to Digital pin 6 RCLK
int dataPin = 10;  // Data pin of 74HC595 is connected to Digital pin 4 SER

/**************Buzzer/Servo*******************/
int geluidBuzzer = 11;
int slotServo = 12;

/**************Knoppen*******************/
int knopSlot = 14; //14 = A0.
int knopRotary = 15; //15 = A1.
int knopGroen = 16; //16 = A2.
int knopRood = 17; //17 = A3.

/**************Globale Variablen*******************/
byte data;    //Stuurt de data van de rotary encoder naar het schuifregister
byte hoofdCode[3] = {1, 2, 3};
byte pinCode[3] = {0, 0, 0};
byte bcdWaarde[3] = {0, 0, 0}; //Maak een lege array
byte rotaryWaarde = 0; //Dit slaat de huidige waarde van de rotary encoder op.
byte displayWaarde = 0; //De waarde die op het scherm komt te staan.
byte laatsteDraaiwaarde = 0; //Dit is om te ontdekken of er gedraaid wordt.
byte bcdLine = 2; //Dit is om te zorgen dat de waardes niet tussen 0 en 9 komen.
bool rotaryGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool groenGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool roodGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool slotGedrukt = LOW;
int count1 = 0;
int count2 = 0;
int count3 = 0;

byte codeIngevoerd = 0;

// Setup
void setup()
{
  Serial.begin(9600);

  pinMode(groenLED, OUTPUT);
  pinMode(roodLED, OUTPUT);
  pinMode(sevenSeg1, OUTPUT);
  pinMode(sevenSeg2, OUTPUT);
  pinMode(sevenSeg3, OUTPUT);

  pinMode(rotaryLinks, INPUT);
  pinMode(rotaryRechts, INPUT);
  pinMode(klokPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(geluidBuzzer, OUTPUT);
  pinMode(slotServo, OUTPUT);

  pinMode(knopSlot, INPUT_PULLUP);
  pinMode(knopGroen, INPUT_PULLUP);
  pinMode(knopRood, INPUT_PULLUP);
  pinMode(knopRotary, INPUT_PULLUP);
}

/*  To do
Servo
*Buzzer* DONE
Led groen
Led Rood
Knop groen
Knop rood
Knop Slot

*/


// Main loop
void loop()
{
  /**************Deur open?*******************/
  bool deurOpen;
  if(kijkSlotKnop() == true) //Deur open?
  {
    deurOpen = true;
  }
  while(deurOpen == true)//Zolang de deur open is.
  {
    buzzerActie(3)//Alarm
    if (kijkSlotKnop() == false)//Deur weer dicht?
    {
      //Slot dicht.
      deurOpen = false; //
    }
    //Deur moet dicht.
  }

  /**************Check code*******************/
  kijkRotaryKnop();

  int codeModus = kijkGroeneKnop();
  if(codeModus == 1) //Goeie code
  {
    //reset de waardes in het systeem.
    for(int i = 0; i < 3; i++)
    {
      bcdWaarde[i] = 0;
    }
    bcdLine = 0;
    /**************Deur open + code kunnen veranderen*******************/
    "Servo open"
    bool wacht = true;
    while(wacht == true)
    {
      if((kijkSlotKnop() == false) && (kijkGroeneKnop() == true)) //Deur dicht en groene knop ingedrukt? Deur dicht.
      {
        "servo dicht"
        wacht = false;
      }

      if(kijkRodeKnop() == true) //Rode knop ingedrukt? Code veranderen
      {
        for(int i = 0; i < 3; i++)
        {
          bcdWaarde[i] = 0;
        }
        bcdLine = 0;

        bool wacht2 = true;
        while(wacht2 == true)
        {
          kijkRotaryencoder();
          schermAansturen();

          if(knopGroen == LOW) //Set nieuwe waarde als code.
          {
            for(int i = 0; i < 3; i++)
            {
              pinCode[i] = bcdWaarde[i];//Set de bcdWaarde als de code.
            }
            wacht2 = false;//uit de while.
          }
        }
      }

    }
  }

  else if(codeModus == 2)//drie keer fout
  {
    for(int i = 0; i < 3; i++)
    {
      bcdWaarde[i] = 0;
    }
    bcdLine = 0;

    bool wacht = true;
    while(wacht == true)
    {
      buzzerActie(3) //alarm

      kijkRotaryencoder();
      schermAansturen();

      int goeieCode = 0;
      if(knopGroen == LOW) //Set nieuwe waarde als code.
      {
        for(int i = 0; i < 3; i++)
        {
          if (bcdWaarde[i] == hoofdCode[i]) //is de ingevulde waarde de hoofdcode?
          {
            goeieCode++;
          }
        }
        if(goeieCode == 3)//code goed?
        {
          wacht = false;
        }
        else//Reset de lines.
        {
          for(int i = 0; i < 3; i++)
          {
            bcdWaarde[i] = 0;
          }
          bcdLine = 0;
        }
      }
    }
  }

  if (kijkRodeKnop() == true)
  {

  }




  /**************Rotaryencoder*******************/
  kijkRotaryencoder();

  /**************Rotary data to Register*******************/
  schermAansturen();
  Serial.println(bcdLine);
}

//////////////////////////////////////////////////////////////////////
void updateShiftRegister()
{
  digitalWrite(latchPin, LOW); //Er mag data toegevoegd worden in het register.
  shiftOut(dataPin, klokPin, MSBFIRST, data); //Shift de waarde van data in het register.
  //MSB eerst. dus 0000 0110 wordt 0110 0000 als er wordt gekeken vanaf rechts.
  digitalWrite(latchPin, HIGH); //Er mag geen data toegevoegd worden in het register.
}

void kijkSlotKnop()
{
  if(slotGedrukt == HIGH && digitalRead(knopSlot) == LOW)//Deur open??
  {

  }
  slotGedrukt = digitalRead(knopSlot);

  bool gedrukt = false;
  if(digitalRead(knopSlot) == LOW)
  {
    gedrukt = true; //return true als
  }
  return gedrukt;
}

void kijkRotaryKnop()
{
  //Rotary knop
  if(rotaryGedrukt == HIGH && digitalRead(knopRotary) == LOW) //Is er gedrukt?
  {
    BuzzerActie(0);
    bcdWaarde[bcdLine] = rotaryWaarde; //De waarde van de knop de waarde van bcdLine.
    bcdLine++; //Zoek data van het volgende scherm.
    if(bcdLine == 3)// Als die 3 is moet die naar nummer 0 gaan.
    {
      bcdLine = 0;
    }
    rotaryWaarde = bcdWaarde[bcdLine]; //Stop de huidige data van dit scherm op het scherm.
    Serial.println("Knop is gedrukt");
  }
  rotaryGedrukt = digitalRead(knopRotary); //Zodat er geen redruk komt.

  bool gedrukt = false;
  if(digitalRead(knopRotary) == LOW)
  {
    gedrukt = true; //return true als
  }
  bool gedrukt;
}

void kijkGroeneKnop()
{
  int modus = 0;
  //Groene knop. Deze moet gedrukt worden als de waarde op het scherm ingevoerd moet worden.
  if(groenGedrukt == HIGH && digitalRead(knopGroen) == LOW) //Is er gedrukt?
  {
    buzzerActie(0);
    codeIngevoerd++;

    if(bcdWaarde == juist)
    {
      codeIngevoerd = 0; //Reset het aantal keer dat de code is ingevoer.
      modus = 1; //open de kluis
      open kluis
      buzzerActie(1); //Goede code melding.
    }
    else
    {
      for (int i = 0; i < 3; i++) //reset de waardes op het scherm.
      {
        bcdWaarde[i] = 0;
      }
      if(codeIngevoerd == 3) //Drie keer foute code?
      {
        buzzerActie(2) //Foute code alarm
        modus = 2;
      }
    }
  }
  groenGedrukt = digitalRead(knopGroen); //Zodat er geen redruk komt.
  return modus;
}

void kijkRodeKnop()
{
  //Rode knop
  if(roodGedrukt == HIGH && digitalRead(knopRood) == LOW) //Is er gedrukt?
  {
    BuzzerActie(0);

    Serial.println("Knop is gedrukt");
  }
  roodGedrukt = digitalRead(knopRood); //Zodat er geen redruk komt.

  bool gedrukt = false;
  if(digitalRead(knopRood) == LOW)
  {
    gedrukt = true; //return true als  knop rood is gedrukt
  }
  return gedrukt;
}

void kijkRotaryencoder()
{
  if(digitalRead(rotaryLinks) != laatsteDraaiwaarde) //Is er gedraaid?
  {
    if(digitalRead(rotaryRechts) == laatsteDraaiwaarde) //Draai naar rechts?
    {
      if(rotaryWaarde == 18){} //18? Doe niets. Elke draai doet die 2x. Dus daarom 18 ipv 9.
      else //Niet 18? Dan +1
      {
        rotaryWaarde++; //Tel 1 op
        Serial.print(rotaryWaarde);
      }
    }
    else //Draai naar links.
    {
      if(rotaryWaarde == 0){} //0? Doe niets
      else // Niet 0? Dan -1
      {
        rotaryWaarde--; //Trek 1 af.
        Serial.print(rotaryWaarde);
      }
    }
  }
  laatsteDraaiwaarde = digitalRead(rotaryLinks); //Sla waarde op van de laatste positie.
}

void schermAansturen()
{
  /**************Rotary data to Register*******************/
  for(int i = 0; i < 3; i++) //Doe 3 keer
  {
    if(bcdLine == i) //Is dit scherm geselecteerd?
    {
      data = rotaryWaarde; //Stop de waarde in data
    }
    else
    {
      data = bcdWaarde[i]; //Stop de opgeslagen waarde in data.
    }

    data =  data / 2; //Deel de data door 2. 18/2 = 9.

    digitalWrite(sevenSeg1, LOW); //Zet alle schermen uit
    digitalWrite(sevenSeg2, LOW);
    digitalWrite(sevenSeg3, LOW);

    switch(i) //Zet seven segment aan welke data nodig heeft..
    {
      case 0:
        count1 = count1 + 1;

        if (((count1 >= 1) && (count1 <= 50)) && (bcdLine == 2) || (bcdLine == 0) || (bcdLine == 1) )
        {
        digitalWrite(sevenSeg1, HIGH);

        }
        else if (((count1 > 100) && (count1 <= 100)) && (bcdLine == 2))
        {
         digitalWrite(sevenSeg1, LOW);
        }

        if (count1 > 100 && (bcdLine == 2))
        {
          count1 = 1;
        }
        delay(1); //Dit is om het andere scherm te laten doven.


      break;
      case 1:
        count2 = count2 + 1;

        if (((count2 >= 1) && (count2 <= 50)) && (bcdLine == 0) || (bcdLine == 2) || (bcdLine == 1) )
        {
        digitalWrite(sevenSeg2, HIGH);

        }
        else if (((count2 > 100) && (count2 <= 100)) && (bcdLine == 0))
        {
         digitalWrite(sevenSeg2, LOW);
        }

        if (count2 > 100 && (bcdLine == 0))
        {
          count2 = 1;
        }
        delay(1); //Dit is om het andere scherm te laten doven.

      break;
      case 2:
        count3 = count3 + 1;

        if (((count3 >= 1) && (count3 <= 50)) && (bcdLine == 1) || (bcdLine == 0) || (bcdLine == 2) )
        {
        digitalWrite(sevenSeg3, HIGH);

        }
        else if (((count3 > 100) && (count3 <= 100)) && (bcdLine == 1))
        {
         digitalWrite(sevenSeg3, LOW);
        }

        if (count3 > 100 && (bcdLine == 1))
        {
          count3 = 1;
        }
        delay(1); //Dit is om het andere scherm te laten doven.

      break;
    }
    updateShiftRegister(); //Plaatst de data op het actieve 7-segmenten display.
  }
}

void buzzerActie(int actie)
{
  switch(actie)
  {
    int a;
    case 0: //Standaard knop geluid
      tone(geluidBuzzer, 150, 50);
    break;
    case 1: //Kluis unlocked
      a = 500;
      for(int i = 0; i < 5; i++)
      {
        tone(geluidBuzzer,a,150);
        delay(150);
        a = a + 100;
      }
      tone(geluidBuzzer,1000,300);
      delay(400);
      tone(geluidBuzzer,1000,300);
      delay(400);
      tone(geluidBuzzer,1000,600);
      delay(600);
    break;
    case 2: //Kluis gelocked
      a = 500;
      for(int i = 0; i < 5; i++)
      {
      tone(geluidBuzzer,a,150);
      delay(150);
      a = a + 100;
      }
      tone(geluidBuzzer,600,300);
      delay(400);
      tone(geluidBuzzer,400,300);
      delay(400);
      tone(geluidBuzzer,200,600);
      delay(600);
    break;
    case 3: //Kluis deur open
      tone(geluidBuzzer,500,500);
      delay(1000);
    break;
  }
}
