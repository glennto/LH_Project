/***************************************************
  Arduino based test code for an Arduino Uno and an Adafruit TFT/
  Resistive Touch Screen display board that, together with a suitable gear 
  tooth rpm sensor, displays spindle speed (rpm) as derived from a spindle 
  gear tooth sensor input on digital pin 2 of an Arduino Uno R3

  Part of Lathe Helper Project Software as described in the March/April 2021 issue of 
  The Home Shop Machinist published by the Village Press.
  
     Source:  https//github/glennto/LH_Project
     glennt@palmnet.net  1/6/2021

  Note: this program is for test purposes and does not form any part of the 
  Lathe Helper main application program.

****************************************************/
  
/**********************************************

>   Notice
    copyright:  Glenn Tober
    glennt@palmnet.net

>   This program is free software; you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation; either version 2 of the License, or
>   (at your option) any later version.

>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.

>   You should have received a copy of the GNU General Public License along
>   with this program; if not, write to the Free Software Foundation, Inc.,
>   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

>   Program compiled under the Arduino IDE
>   Program structure adopted from Adafruit graphics test code.
>   Code links to unmodified routines in Arduino and Adafruit SD, SPI, STMPE610, ILI9341, and GFX libraries.

  Scope:
  
  This program is intended to be embedded in an Arduino Uno R3 with a mating Adafruit TFT touch screen display.
  In support of power wiring and an optional gear tooth rpm sensor a passive Adafruit Proto Board
  is sandwitched in between the Uno and the TFT boards




                      Version 1.00 01/06/2021
  

 ************************************************************************************************************************/

#define NBRTEETH 1
#define RPM_TARGET_Y_OFFSET 90
#define RPM_ACTUAL_Y_OFFSET 170
#define RPM_X_OFFSET 35

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_STMPE610.h"

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


//  variables used to compute actual rpm
unsigned long Time = micros(), save_Time = Time;

int pin_D2 = 2;    // will use pin 2 for the rpm sensor pulse input
int LED = 4;    // pin D4 is wired to the red LED

volatile int state = LOW;
int save_State = state;
// default (starting) rpm value
int rpm = 0;
// rpm counter variable and max count to reduce actual rpm display jitter
int count = 1;
const int count_Max = 50;

void display_RPMS()
//  display rpm values in two locations
{
//  second location
  tft.fillRect(RPM_X_OFFSET, RPM_ACTUAL_Y_OFFSET, 130, 40, ILI9341_BLACK);  
  tft.setCursor(RPM_X_OFFSET + 5, RPM_ACTUAL_Y_OFFSET + 15); 
  write_Text(String(rpm), 3);
  digitalWrite(LED,state);

  return;
}



void rpm_event()
//  this is the interrupt service routine which is entered everytime there
//  is a falling edge from the rpm sensor
  {
  state = !state;     //flip states so that we can detect this event in loop() below
  
}

void write_Text(String text, byte size){
  //  write white text of size at current cursor location
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(size);
  tft.println(text);

  return;
}


void write_Static_Text() 
//  routine blanks the screen and paints all static text
  { 
  tft.setCursor(0, 10);  
  write_Text("LATHE HELPER", 3);
  
//  text we want red cannot call write_Text. Maybe fix this?  
  tft.setCursor(0, 70);  
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.println(" Target RPM:");

  tft.setCursor(0, 150);  
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.println(" Actual RPM:");

  return;
  }


//********************setup*************************
void setup() {
//setup code here, to run once:
  Serial.begin(9600);
  Serial.print("RPM Interrupt tester ");
//  Serial.println(VERSION);

//  instantiate tft and ts classes for screen display and touch screen respectively 
  tft.begin();
  if (!ts.begin()) { 
    Serial.println("Unable to start touchscreen.");
  } 


//  setup interrupt structure for rpm detection
  pinMode(pin_D2, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(pin_D2), rpm_event, FALLING);
  save_State = state;
  unsigned long Time = micros();
  write_Static_Text();
  display_RPMS();
  digitalWrite(LED, digitalRead(pin_D2)); // displays interrupt input value
}

//************************loop**************************
void loop() {
//main code here, to run repeatedly:

  save_Time = micros();

  if(save_State != state){    //detected an rpm pulse
     rpm = ((60 * 1000000)/((save_Time - Time)*NBRTEETH));
     Time = save_Time;
     state = save_State;
     count = count + 1;
     if (count >= count_Max){
      display_RPMS();
      count = 1;
     }
     digitalWrite(LED, state);
  }

  if (int((60 * 1000000) / (Time - save_Time + 100)) >= 3) {
    rpm = 0;
    display_RPMS();
    Time = micros();
  }

}
