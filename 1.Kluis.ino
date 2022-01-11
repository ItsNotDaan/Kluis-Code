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
int pos;

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
byte pinCode[4] = {0, 0, 0, 0}; //Dit is de pincode.
byte bcdWaarde[4] = {0, 0, 0, 0}; //Maak een lege array
byte rotaryWaarde = 0; //Dit slaat de huidige waarde van de rotary encoder op.
byte laatsteDraaiwaarde = 0; //Dit is om te ontdekken of er gedraaid wordt.
byte bcdLine = 0; //Dit is om te zorgen dat de waardes niet tussen 0 en 9 komen.

bool rotaryGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool groenGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool roodGedrukt = LOW; //Dit is voor de knop voor een soort latch.
bool slotGedrukt = LOW; //Dit is voor de knop voor een soort latch.

unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;

unsigned long huidigeTijd;

// Setup
void setup()
{
  //Serial.begin(9600);

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
  if(digitalRead(knopSlot) == LOW)//Deur open??
  {
    bool deurOpen = true;


    for(pos = 0; pos <= 90; pos += 1) { //Deur openen
      slotServo.write(pos);
      delay(10);
    }
    //Serial.println("Deur is open en moet dicht");

    while(deurOpen == true)//Zolang de deur open is.
    {
      tone(geluidBuzzer,500,500); //Alarm aan voor 500ms
      //Serial.println("Deur open");
      huidigeTijd = millis(); //tijd hoelang het programma al draait. Long omdat het om tijd gaat
      while((millis() - huidigeTijd < 1000)) //doe voor het aantal seconden alles wat in de while staat.
      {
        if(millis() - huidigeTijd < 250) //Tijd onder de 250ms?
        {
          digitalWrite(roodLED, HIGH);
          digitalWrite(groenLED, LOW);
        }
        if((millis() - huidigeTijd < 500) && (millis() - huidigeTijd > 250)) //Tijd onder de 500ms?
        {
          digitalWrite(roodLED, LOW);
          digitalWrite(groenLED, HIGH);
        }
        if((millis() - huidigeTijd < 750) && (millis() - huidigeTijd > 500)) //Tijd onder de 750ms?
        {
          digitalWrite(roodLED, HIGH);
          digitalWrite(groenLED, LOW);
        }
        if((millis() - huidigeTijd < 1000) && (millis() - huidigeTijd > 750)) //Tijd onder de 1000ms?
        {
          digitalWrite(roodLED, LOW);
          digitalWrite(groenLED, HIGH);
        }

        kijkRotaryencoder();
        kijkRotaryKnop();
        schermAansturen();

        //Serial.println("Deur moet dicht");
        if((digitalRead(knopSlot) == HIGH) && (slotGedrukt == LOW))//Deur Dicht??
        {
          deurOpen = false; //stop de while loop.
          //Serial.println("Deur is dicht");
        //  slotServo.write(90);//90 is deur dicht.
          for(pos = 90; pos >= 0; pos -= 1) { //Deur Dicht
            slotServo.write(pos);
            delay(10);
          }
        }
        slotGedrukt = digitalRead(knopSlot);
      }
    }
    digitalWrite(roodLED, LOW); //Rode led uit
    digitalWrite(groenLED, LOW); //Groene led uit
  }

  /*****************Input Code Function***********************/

    /**************Rotaryencoder*******************/
    kijkRotaryencoder();
    kijkRotaryKnop();

    /**********Rotary data to Register*************/
    schermAansturen();

  /**********************************************************/

  //KNIPPER GROEN
  if(millis() - huidigeTijd > 1000) //tijd boven de 1000ms?
  {
    huidigeTijd = millis(); //sla de huidige tijd op.
  }
  if(millis() - huidigeTijd < 500) //Tijd onder de 500ms?
  {
    digitalWrite(groenLED, HIGH); //groene led aan
  }
  if(millis() - huidigeTijd > 500) //Tijd boven de 500ms?
  {
    digitalWrite(groenLED, LOW); //groene led uit
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
    //Serial.println("Deur gaat open");
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
        //Serial.println("Slot gaat erop, deur dicht.");
        tone(geluidBuzzer,2000,300);
        delay(200);
        tone(geluidBuzzer,2000,400);
        delay(400); //Goede code melding.

      //  slotServo.write(90);//90 is deur dicht.
        for(pos = 90; pos >= 0; pos -= 1) { //Deur dicht
          slotServo.write(pos);
          delay(10);
        }
        wacht = false;
      }
      groenGedrukt = digitalRead(knopGroen);

      if((digitalRead(knopRood) == HIGH) && (roodGedrukt == LOW) && (digitalRead(knopGroen) == LOW)) //Rode knop ingedrukt en de groene niet? Code veranderen
      {
        //Serial.println("Verander de code");

        tone(geluidBuzzer,1900,800);

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

          /*****************Input Code Function***********************/

            /**************Rotaryencoder*******************/
            kijkRotaryencoder();
            kijkRotaryKnop();

            /**********Rotary data to Register*************/
            schermAansturen();

          /**********************************************************/

          if(digitalRead(knopGroen) == HIGH && (groenGedrukt == LOW)) //Set nieuwe waarde als code.
          {
            //Serial.println("Code verandert");

            tone(geluidBuzzer,1900,300);
            delay(400);
            tone(geluidBuzzer,1900,300);
            delay(400);


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
    //Serial.println("Drie keer foute code");
    for(int i = 0; i < 4; i++)
    {
      bcdWaarde[i] = 0;
    }
    bcdLine = 0;

    unsigned long alarmTijd = millis();
    bool wacht = true;
    while(wacht == true)
    {
      tone(geluidBuzzer,1250,500);
      huidigeTijd = millis(); //tijd hoelang het programma al draait. Long omdat het om tijd gaat
      while (millis() - huidigeTijd < 1000) //doe voor het aantal seconden alles wat in de while staat.
      {
        //Serial.println("Maak een nieuwe code.");

        if (millis() - alarmTijd > 10000)//Minuut het alarm gehad?
        {
            wacht = false;
        }

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

        /*****************Input Code Function***********************/

          /**************Rotaryencoder*******************/
          kijkRotaryencoder();
          kijkRotaryKnop();

          /**********Rotary data to Register*************/
          schermAansturen();

        /**********************************************************/

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
            //Serial.println("De code is goed.");
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
}

//////////////////////////////////////////////////////////////////////

void kijkRotaryKnop() //Kijk of de rotary encoder knop wordt gedrukt.
{
  //Rotary knop
  if(rotaryGedrukt == LOW && digitalRead(knopRotary) == HIGH) //Is er gedrukt?
  {
    tone(geluidBuzzer, 2000, 50); //Laat horen dat de knop is gedrukt
    bcdWaarde[bcdLine] = rotaryWaarde; //De waarde van de knop de waarde van bcdLine.
    bcdLine++; //Zoek data van het volgende scherm.

    if(bcdLine == 4)// Als die 3 is moet die naar nummer 0 gaan.
    {
      bcdLine = 0;
    }
    rotaryWaarde = bcdWaarde[bcdLine]; //Stop de huidige data van dit scherm op het scherm.

    //Serial.println("Cijfer geset");
  }
  rotaryGedrukt = digitalRead(knopRotary); //Zodat er geen redruk komt.
}

int kijkGroeneKnop() //Kijk of de groene knop wordt gedrukt.
{
  byte modus = 0; //Dit is om te kijken wat de modus is.
  byte codeIngevoerd = 0; //Dit is om te kijken hoe vaak de code is ingevoerd.
  //Groene knop. Deze moet gedrukt worden als de waarde op het scherm ingevoerd moet worden.
  if(groenGedrukt == LOW && digitalRead(knopGroen) == HIGH) //Is er gedrukt?
  {
    tone(geluidBuzzer, 2000, 50); //Laat horen dat de knop is gedrukt
    //Serial.println("Groene knop gedrukt");
    codeIngevoerd++; //Het aantal keer dat de groene knop is gedrukt

    int goeieCode = 0; //Variable om te kijken of de ingevoerde code juist is.
    for(int i = 0; i < 4; i++) //doe vier keer.
    {
      if (bcdWaarde[i] == pinCode[i]) //is de ingevulde waarde de hoofdcode?
      {
        goeieCode++; //als de code drie keer goed is, is goeieCode 4.
      }
    }

    if(goeieCode == 4) //Code goed?
    {
      digitalWrite(groenLED, HIGH); //Groene led aan.
      modus = 1; //open de kluis

      for (pos = 0; pos <= 90; pos += 1) { //Deur open;
        slotServo.write(pos);
        delay(10);
      }

      /***********Laat een deuntje horen dat de code goed is************/
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
      //Serial.println("Code niet goed");
      if(codeIngevoerd == 3) //Drie keer foute code?
      {
        digitalWrite(roodLED, HIGH);
        /***********Laat een deuntje horen dat de code vaak niet goed is************/
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
        /***********Laat een deuntje horen dat de code niet goed is************/
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
  return modus; //Stuur de waarde van modus terug.
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
        //Serial.print(rotaryWaarde);
      }
    }
    else //Draai naar links.
    {
      if(rotaryWaarde == 0){} //0? Doe niets
      else // Niet 0? Dan -1
      {
        rotaryWaarde--; //Trek 1 af.
        //Serial.print(rotaryWaarde);
      }
    }
  }
  laatsteDraaiwaarde = digitalRead(rotaryLinks); //Sla waarde op van de laatste positie.
}

void schermAansturen() //laat waardes zien op het scherm
{
  byte data;    //Stuurt de data van de rotary encoder naar het schuifregister

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
