/*
  Name: Kluis
  Date: 26-11-2021
  Author: Daan Heetkamp, Bram Geerdink
  Description: Zorg dat er doormiddel van een rotary knop een kluis open en dicht kan gaan.
  Revision: V0.0
*/

// ----- include libraries -----

// ----- Declare Constants -----



/******Transistors******/
#define input1 3
#define input2 4
#define input3 5
#define input4 6



// ----- Declare Objects -----

// ----- Declare subroutines and/or functions -----

// ----- Declare Global Variables -----



// Setup
void setup()
{
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(input3, OUTPUT);
  pinMode(input4, OUTPUT);
}

// Main loop
void loop()
{
  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
  digitalWrite(input3, LOW);
  digitalWrite(input4, LOW);

  delay(1000);

  digitalWrite(input1, LOW);
  digitalWrite(input2, HIGH);
  digitalWrite(input3, LOW);
  digitalWrite(input4, LOW);

  delay(1000);

  digitalWrite(input1, HIGH);
  digitalWrite(input2, HIGH);
  digitalWrite(input3, LOW);
  digitalWrite(input4, LOW);

  delay(1000);

  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
  digitalWrite(input3, LOW);
  digitalWrite(input4, HIGH);

  delay(1000);
}
