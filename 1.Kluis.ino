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
#include <EEPROM.h>

// ----- Declare Constants -----

// ----- Declare Objects -----

// ----- Declare subroutines and/or functions -----
void kijkRotaryKnop();
int kijkGroeneKnop();
void kijkRotaryencoder();
void schermAansturen();

// ----- Declare Global Variables -----

Servo slotServo;

/**************LED/Display*******************/

int groenLED = 1; //De groene led.
int roodLED = 2; //De rode led.

int sevenSeg1 = 5; //Het eerste seven segmenten display.
int sevenSeg2 = 3; //Het tweede seven segmenten display.
int sevenSeg3 = 4; //Het derde seven segmenten display.
int sevenSeg4 = 13; //Het derde seven segmenten display.

/**************IC/Encoder*******************/
int rotaryLinks = 6; //De linker kant van de rotary encoder.
int rotaryRechts = 7; //De rechter kant van de rotary encoder.

int klokPin = 8;  // Latch pin of 74HC595 is connected to Digital pin 5 SRCLK
int latchPin = 9;  // Clock pin of 74HC595 is connected to Digital pin 6 RCLK
int dataPin = 10;  // Data pin of 74HC595 is connected to Digital pin 4 SER

/**************Buzzer/Servo*******************/
int geluidBuzzer = 11;
//int slotServo = 12;

/**************Knoppen*******************/
int knopSlot = 14; //14 = A0.
int knopRotary = 15; //15 = A1.
int knopGroen = 16; //16 = A2.
int knopRood = 17; //17 = A3.

/**************Globale Variablen*******************/
byte data;    //Stuurt de data van de rotary encoder naar het schuifregister
byte pinCode[4] = {0, 0, 0, 0}; //Dit is de pincode.
byte bcdWaarde[4] = {0, 0, 0, 0}; //Maak een lege array
byte rotaryWaarde = 0; //Dit slaat de huidige waarde van de rotary encoder op.
byte displayWaarde = 0; //De waarde die op het scherm komt te staan.
byte laatsteDraaiwaarde = 0; //Dit is om te ontdekken of er gedraaid wordt.
byte bcdLine = 0; //Dit is om te zorgen dat de waardes niet tussen 0 en 9 komen.
byte codeIngevoerd = 0; //Dit is om te kijken hoe vaak de code is ingevoerd.

bool rotaryGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool groenGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool roodGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool slotGedrukt = LOW;

int count1 = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;

unsigned long huidigeTijd;

// Setup
void setup()
{
  Serial.begin(9600);

  pinMode(groenLED, OUTPUT);
  pinMode(roodLED, OUTPUT);
  pinMode(sevenSeg1, OUTPUT);
  pinMode(sevenSeg2, OUTPUT);
  pinMode(sevenSeg3, OUTPUT);
  pinMode(sevenSeg4, OUTPUT);

  pinMode(rotaryLinks, INPUT);
  pinMode(rotaryRechts, INPUT);
  pinMode(klokPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(geluidBuzzer, OUTPUT);
  slotServo.attach(12); //Servo is op poort 12.

  pinMode(knopSlot, INPUT_PULLUP);
  pinMode(knopGroen, INPUT_PULLUP);
  pinMode(knopRood, INPUT_PULLUP);
  pinMode(knopRotary, INPUT_PULLUP);

  /*for (int i = 0; i < 3; i++) //Stop de opgeslagen pincode in de pincode variable.
  {
    EEPROM.write(i, pinCode[i]);
  }*/

  for (int i = 0; i < 4; i++) //Stop de opgeslagen pincode in de pincode variable.
  {
    pinCode[i] = EEPROM.read(i);
  }
}

/*  To do
Servo DONE
*Buzzer* DONE
Led groen
Led Rood
Knop groen DONE
Knop rood DONE
Knop Slot DONE

*/


// Main loop
void loop()
{
  /**************Deur open?*******************/
  bool deurOpen = false;
  if(digitalRead(knopSlot) == LOW)//Deur open??
  {
    deurOpen = true;
    slotServo.write(0);//0 is deur open
    Serial.println("Deur is open en moet dicht");
  }

  while(deurOpen == true)//Zolang de deur open is.
  {
    tone(geluidBuzzer,500,500);
    huidigeTijd = millis(); //tijd hoelang het programma al draait. Long omdat het om tijd gaat
    while (millis() - huidigeTijd < 1000 || deurOpen == true) //doe voor het aantal seconden alles wat in de while staat.
    {
      if(millis() - huidigeTijd < 250)
      {
        digitalWrite(roodLED, HIGH);
        digitalWrite(groenLED, LOW);
      }
      if((millis() - huidigeTijd < 500) && (millis() - huidigeTijd > 250))
      {
        digitalWrite(roodLED, LOW);
        digitalWrite(groenLED, HIGH);
      }
      if((millis() - huidigeTijd < 750) && (millis() - huidigeTijd > 500))
      {
        digitalWrite(roodLED, HIGH);
        digitalWrite(groenLED, LOW);
      }
      if((millis() - huidigeTijd < 1000) && (millis() - huidigeTijd > 750))
      {
        digitalWrite(roodLED, LOW);
        digitalWrite(groenLED, HIGH);
      }
      kijkRotaryencoder();
      schermAansturen();
      //Serial.println("Deur moet dicht");
      if((digitalRead(knopSlot) == HIGH) && (slotGedrukt == LOW));//Deur Dicht??
      {
        deurOpen = false;
        Serial.println("Deur is dicht");
        slotServo.write(90);//90 is deur dicht.
      }
      slotGedrukt = digitalRead(knopSlot);
    }
    digitalWrite(roodLED, LOW);
    digitalWrite(groenLED, LOW);
  }

  //KNIPPER GROEN
  if(millis() - huidigeTijd > 1000)
  {
    huidigeTijd = millis();
  }
  if(millis() - huidigeTijd < 500)
  {
    digitalWrite(groenLED, HIGH);
  }
  if(millis() - huidigeTijd > 500)
  {
    digitalWrite(groenLED, LOW);
  }

  /**************Check code*******************/
  int codeModus = kijkGroeneKnop(); //Modus 0 is niets. Modus 1 open. modus 2 alarm.
  if(codeModus == 1) //Goeie code
  {
    //reset de waardes in het systeem.
    for(int i = 0; i < 4; i++)
    {
      bcdWaarde[i] = 0;
    }
    bcdLine = 0;

    /**************Deur open + code kunnen veranderen*******************/
    Serial.println("Deur gaat open");
    bool wacht = true;
    while(wacht == true)
    {
      if(millis() - huidigeTijd > 1000)
      {
        huidigeTijd = millis();
      }
      if(millis() - huidigeTijd < 500)
      {
        digitalWrite(roodLED, HIGH);
        digitalWrite(groenLED, HIGH);
      }
      if(millis() - huidigeTijd > 500)
      {
        digitalWrite(roodLED, LOW);
        digitalWrite(groenLED, LOW);
      }

      schermAansturen();
      if((digitalRead(knopSlot) == HIGH) && (digitalRead(knopGroen) == HIGH) && (groenGedrukt == LOW)) //Deur dicht en groene knop ingedrukt? Deur dicht.
      {
        Serial.println("Slot gaat erop, deur dicht.");
        tone(geluidBuzzer,2000,300);
        delay(200);
        tone(geluidBuzzer,2000,300);
        delay(200);
        tone(geluidBuzzer,2000,600);
        delay(600); //Goede code melding.

        slotServo.write(90);//90 is deur dicht.
        wacht = false;
      }
      groenGedrukt = digitalRead(knopGroen);

      if((digitalRead(knopRood) == HIGH) && (roodGedrukt == LOW) && (digitalRead(knopGroen) == LOW)) //Rode knop ingedrukt en de groene niet? Code veranderen
      {
        Serial.println("Verander de code");
        for(int i = 0; i < 4; i++)
        {
          bcdWaarde[i] = 0;
        }
        bcdLine = 0;

        bool wacht2 = true;
        while(wacht2 == true)
        {
          if(millis() - huidigeTijd > 1000)
          {
            huidigeTijd = millis();
          }
          if(millis() - huidigeTijd < 500)
          {
            digitalWrite(groenLED, HIGH);
          }
          if(millis() - huidigeTijd > 500)
          {
            digitalWrite(groenLED, LOW);
          }

          kijkRotaryKnop();
          kijkRotaryencoder();
          schermAansturen();

          if(digitalRead(knopGroen) == HIGH && (groenGedrukt == LOW)) //Set nieuwe waarde als code.
          {
            Serial.println("Code verandert");

            tone(geluidBuzzer,1750,300);
            delay(400);
            tone(geluidBuzzer,1900,300);
            delay(400);
            tone(geluidBuzzer,2000,600);
            delay(1000); //Goede code melding.

            for(int i = 0; i < 4; i++)
            {
              pinCode[i] = bcdWaarde[i];//Set de bcdWaarde als de code.
              if(EEPROM.read(i) != pinCode[i]) //Is de waarde op het bepaalde punt hetzelfde? dan niet schrijven.
              {
                EEPROM.write(i, pinCode[i]);
              }
            }
            wacht2 = false;//uit de while.
          }
          groenGedrukt = digitalRead(knopGroen);
        }
      }
      roodGedrukt = digitalRead(knopRood); //Zodat er geen redruk komt.
      digitalWrite(roodLED, LOW);
      digitalWrite(groenLED, LOW);
    }
  }

  else if(codeModus == 2)//drie keer fout
  {
    Serial.println("Drie keer foute code");
    for(int i = 0; i < 4; i++)
    {
      bcdWaarde[i] = 0;
    }
    bcdLine = 0;

    bool wacht = true;
    while(wacht == true)
    {
      tone(geluidBuzzer,1250,500);
      unsigned long huidigeTijd = millis(); //tijd hoelang het programma al draait. Long omdat het om tijd gaat
      while (millis() - huidigeTijd < 1000) //doe voor het aantal seconden alles wat in de while staat.
      {
        //Serial.println("Maak een nieuwe code.");

        if(millis() - huidigeTijd > 1000)
        {
          huidigeTijd = millis();
        }
        if(millis() - huidigeTijd < 500)
        {
          digitalWrite(groenLED, HIGH);
        }
        if(millis() - huidigeTijd > 500)
        {
          digitalWrite(groenLED, LOW);
        }

        kijkRotaryencoder();
        kijkRotaryKnop();
        schermAansturen();

        int goeieCode = 0;
        if((digitalRead(knopGroen) == HIGH) && (groenGedrukt == LOW)) //Set nieuwe waarde als code.
        {
          tone(geluidBuzzer, 2000, 50);
          for(int i = 0; i < 4; i++)
          {
            if (pinCode[i] == bcdWaarde[i]) //is de ingevulde waarde de hoofdcode?
            {
              goeieCode++;
            }
          }

          if(goeieCode == 4)//code goed?
          {
            tone(geluidBuzzer, 2000, 50);
            delay(50);
            tone(geluidBuzzer, 2000, 300);
            delay(300);
            wacht = false;
            Serial.println("De code is goed.");
          }
          else//Reset de lines.Code niet goed
          {
            for(int i = 0; i < 4; i++)
            {
              bcdWaarde[i] = 0;
            }
            bcdLine = 0;
            tone(geluidBuzzer,2000,200);
            delay(200);
            tone(geluidBuzzer,1800,200);
            delay(200);
            tone(geluidBuzzer,1750,400);
            delay(400);
          }
        }
        groenGedrukt = digitalRead(knopGroen);
      }
    }
  }


  /**************Rotaryencoder*******************/
  kijkRotaryencoder();
  kijkRotaryKnop();

  /**************Rotary data to Register*******************/
  schermAansturen();
  //Serial.println(bcdLine);
}

//////////////////////////////////////////////////////////////////////

void kijkRotaryKnop()
{
  //Rotary knop
  if(rotaryGedrukt == LOW && digitalRead(knopRotary) == HIGH) //Is er gedrukt?
  {
    tone(geluidBuzzer, 2000, 50);
    bcdWaarde[bcdLine] = rotaryWaarde; //De waarde van de knop de waarde van bcdLine.
    bcdLine++; //Zoek data van het volgende scherm.

    if(bcdLine == 4)// Als die 3 is moet die naar nummer 0 gaan.
    {
      bcdLine = 0;
    }
    rotaryWaarde = bcdWaarde[bcdLine]; //Stop de huidige data van dit scherm op het scherm.

    Serial.println("Cijfer geset");
  }
  rotaryGedrukt = digitalRead(knopRotary); //Zodat er geen redruk komt.
}

int kijkGroeneKnop()
{
  int modus = 0;

  //Groene knop. Deze moet gedrukt worden als de waarde op het scherm ingevoerd moet worden.
  if(groenGedrukt == LOW && digitalRead(knopGroen) == HIGH) //Is er gedrukt?
  {
    tone(geluidBuzzer, 2000, 50);
    Serial.println("Groene knop gedrukt");
    codeIngevoerd++;

    int goeieCode = 0;
    for(int i = 0; i < 4; i++)
    {
      if (bcdWaarde[i] == pinCode[i]) //is de ingevulde waarde de hoofdcode?
      {
        goeieCode++; //als de code drie keer goed is is goeieCode 3.
      }
    }

    if(goeieCode == 4) //Code goed?
    {
      digitalWrite(groenLED, HIGH);
      codeIngevoerd = 0; //Reset het aantal keer dat de code is ingevoer.
      modus = 1; //open de kluis

      slotServo.write(0);//0 is deur open

      int a = 1000;
      for(int i = 0; i < 5; i++)
      {
        tone(geluidBuzzer,a,150);
        delay(150);
        a = a + 250;
      }
      tone(geluidBuzzer,2000,300);
      delay(400);
      tone(geluidBuzzer,2000,300);
      delay(400);
      tone(geluidBuzzer,2000,600);
      delay(600); //Goede code melding.
      digitalWrite(groenLED, LOW);
    }
    else //Code niet goed?
    {
      Serial.println("Code niet goed");

      if(codeIngevoerd == 3) //Drie keer foute code?
      {
        digitalWrite(roodLED, HIGH);
        codeIngevoerd = 0;
        int a = 1000;
        for(int i = 0; i < 5; i++)
        {
        tone(geluidBuzzer,a,150);
        delay(150);
        a = a + 250;
        }
        tone(geluidBuzzer,2000,300);
        delay(400);
        tone(geluidBuzzer,1800,300);
        delay(400);
        tone(geluidBuzzer,1750,600);
        delay(600);
        digitalWrite(roodLED, LOW);
        modus = 2;
      }
      else{
        tone(geluidBuzzer,1800,300);
        delay(400);
        tone(geluidBuzzer,1750,600);
        delay(600);
      }
    }
    for(int i = 0; i < 4; i++) // reset scherm
    {
      bcdWaarde[i] = 0;
    }
    bcdLine = 0;
  }
  groenGedrukt = digitalRead(knopGroen); //Zodat er geen redruk komt.
  return modus;
}

void kijkRotaryencoder() //kijk of de rotary encoder draait.
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
  for(int i = 0; i < 4; i++) //Doe 3 keer
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
    digitalWrite(sevenSeg4, LOW);
    switch(i) //Zet seven segment aan welke data nodig heeft..
    {
      case 0:
        count1 = count1 + 1;

        if (((count1 >= 1) && (count1 <= 50)) && (bcdLine == 3) || (bcdLine == 0) || (bcdLine == 1) || (bcdLine == 2) )
        {
        digitalWrite(sevenSeg4, HIGH);

        }
        else if (((count1 > 100) && (count1 <= 100)) && (bcdLine == 3))
        {
         digitalWrite(sevenSeg4, LOW);
        }

        if (count1 > 100 && (bcdLine == 3))
        {
          count1 = 1;
          //Serial.println("case 0");
        }
        delay(1); //Dit is om het andere scherm te laten doven.
      break;

      case 1:
        count2 = count2 + 1;

        if (((count2 >= 1) && (count2 <= 50)) && (bcdLine == 0) || (bcdLine == 2) || (bcdLine == 1) || (bcdLine == 3) )
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
          //Serial.println("case 1");
        }
        delay(1); //Dit is om het andere scherm te laten doven.
      break;

      case 2:
        count3 = count3 + 1;

        if (((count3 >= 1) && (count3 <= 50)) && (bcdLine == 1) || (bcdLine == 0) || (bcdLine == 2) || (bcdLine == 3) )
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
          //Serial.println("case 2");
        }
        delay(1); //Dit is om het andere scherm te laten doven.
      break;

       case 3:
        count4 = count4 + 1;

        if ((((count4 >= 1) && (count4 <= 50)) && (bcdLine == 2)) || (bcdLine == 0) || (bcdLine == 1) || (bcdLine == 3) )
        {
        digitalWrite(sevenSeg1, HIGH);

        }
        else if (((count4 > 100) && (count4 <= 100)) && (bcdLine == 2))
        {
         digitalWrite(sevenSeg1, LOW);
        }

        if (count4 > 100 && (bcdLine == 2))
        {
          count4 = 1;
          //Serial.println("case 3");
        }
        delay(1); //Dit is om het andere scherm te laten doven.
      break;
    }

    digitalWrite(latchPin, LOW); //Er mag data toegevoegd worden in het register.
    shiftOut(dataPin, klokPin, MSBFIRST, data); //Shift de waarde van data in het register.
    //MSB eerst. dus 0000 0110 wordt 0110 0000 als er wordt gekeken vanaf rechts.
    digitalWrite(latchPin, HIGH); //Er mag geen data toegevoegd worden in het register.
  }
}
