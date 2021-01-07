/***************************************************
  Arduino based code for an Arduino Uno and an Adafruit TFT/
  Resistive Touch Screen display board that, together with a suitable gear 
  tooth rpm sensor, provides a display of recommended spindle speed (rpm) best suited
  for machining the selected material, cutting diameter and tool type.

  Lathe Helper Project Software as described in the March/April 2021 issue of 
  The Home Shop Machinist published by the Village Press.
  
     Source:  https//github/glennto/LH_Project
     glennt@palmnet.net  1/6/2021

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

  This program uses operator supplied touch screen selections to compute and display
  a recommended metal lathe or mill spindle rpm setting that provides the
  target tool point cutting 'Surface Feet Per Minute' (SFPM or SPM) rate.

  Operator selections include (1) material type, (2) cutting diameter and
  (3) tool type.

  Target SFPM if found by catalog lookup based on material and tool type.
  Target rpm is computed and displayed from a formula using target SFPM
  and user selected cutting diameter.

  As an aid to setting spindle rpm on a belt driven lathe (ie, a lathe
  with a fixed rpm drive motor and a set of speed selection pulleys) the closest matching
  belt pulley arrangement that provides the closest match to target rpm is
  also displayed (using a simple code to indicate pulley selection).

  This program accepts a pulsed input signal from a spindle gear tooth sensor and computes
  and displays actual spindle rpm.

  This code is baselined for a six inch Atlas Lathe with a 2-4 pulley drive train
  arrangement and a 1725 rpm motor.

  Adopting this code for lathes with different or no pulleys (ie, more modern variable
  speed lathes), adding/changing the material catalog, configuring the teeth
  on the rpm sensor or adjusting the motor (fixed) rpm should be easily managed via
  straight forward code changes (commented in place below).

  This device is not intended to directly control or automate lathe operations in any way.

                 Version 1.8  12/27/2020
  
  Added conditional compile section for selecting between material lists Option A or Option B (set with define OPTION_A).
     Reference The Home Shop Machinist Article which defines Option A to use Little Machine Shop Article material list
     and Option B material list derived from Author's buying habits.
  In this version the Option A material list was also rearranged to match the Option A definition in the article  
  Cleaned up code comments everywhere. Rearrange splash screen.
  Create configuration item MAXRPM that applies a ceiling to computed rpm (typically case of smaller cutting diamteters)
                    
  Sketch uses 19122 bytes (59%) of program storage space. Maximum is 32256 bytes.
  Global variables use 1233 bytes (60%) of dynamic memory, leaving 815 bytes for local variables. Maximum is 2048 bytes..

                    Version 1.9  12/31/2020
  
  This version addresses program size problem with Option B (mangled display text).  Search for and modify coding
  to reduce compiled code size (eliminated " dia" redundancy in diameter list and spaces in metals list).
  Added F() option to tft.serial.print() functions that use strings (moves string from dynamic to program memory where there is
  more room).

  Note: This is the first verified version of the Lathe Helper software (checked against an independant spread sheet for
  each option and HSS/Carbide tooling at full range of material types and diameters).

  Following for Option A and (B):

  (A) Sketch uses 19298 bytes (59%) of program storage space. Maximum is 32256 bytes.
  Global variables use 1133 bytes (55%) of dynamic memory, leaving 915 bytes for local variables. Maximum is 2048 bytes.

  (B) Sketch uses 19358 bytes (60%) of program storage space. Maximum is 32256 bytes.
  Global variables use 1169 bytes (57%) of dynamic memory, leaving 879 bytes for local variables. Maximum is 2048 bytes..
 ************************************************************************************************************************/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_STMPE610.h"

// Lathe Helper definitions next
String VERSION="Ver 1.9";

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

//  Instantiate a ts TFT class
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// Control pins for the Adafruit TFT shield (default).
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//  set screen rotation (1-4, [N E S W])
uint8_t rotation = 1;

//  following defines the red change rectangle upper left side offsets and box size in pixels (all three)
#define BUTTON_X 178
#define BUTTON_Y 0
#define BUTTON_W 150
#define BUTTON_H 50
//  and the location for each change button from top of display to rectangle upper edge
#define MATERIAL_CHANGE_BUTTON_Y_OFFSET 55
#define DIAMETER_CHANGE_BUTTON_Y_OFFSET 115
#define TOOL_CHANGE_BUTTON_Y_OFFSET 175
//  and offsets for both rpm text display locations
#define RPM_TARGET_Y_OFFSET 90
#define RPM_ACTUAL_Y_OFFSET 170
#define RPM_X_OFFSET 35

// *********************************Simple Configuration Definitions********************************************
//  Adjust next value to change the number of rpm sensor teeth to something other than 1
const byte NBR_TEETH = 1;

//  comment out the following line if you don't want belt pulley settings displayed (ie, your lathe
//  has a variable speed direct drive)
#define DISPLAY_PULLEY_CODES

//  comment out the following line if you don't want measured rpm displayed (probably because your
//  lathe is already providing spindle rpm to you)
#define DISPLAY_ACTUAL_RPM

//  Set the power on display values for the change buttons. Do this to make your favorite material,
//  your most used cutting diameter or your most used tool type be displayed at power on.
//  For example values (0,3,0) inserted below after the equal sign will result in
//  starting display values of (material=aluminum, cutting diameter=1" and tool type=HSS)
byte material_List_Index =  0, diameter_List_Index = 3, tool_List_Index = 0;

//  Sometimes smaller cutting diameters produce an excessively high computed rpm so we limit high rpm
#define MAXRPM 4000

//  Comment out next define if you want to use the 'OPTION B' material definitions
#define OPTION_A

//************************************end simple configuration items*********************************************

//************************************Advanced Configuration Definitions*****************************************

//  Define a list for material types (to reduce code space)
const String  metals_List[] = {"Aluminum", "Steel", "Stainless", "Cast Alum", "Cast Iron",
                                "Tool Steel", "Brass", "Bronze", "Delrin"
                                };

// List length statements 1 of 2 (zero based index):
const byte diameter_list_Length = 13, rpm_list_Length = 9, tool_list_Length = 1;

//  material_list_Top[] is for metal type
//  material_list_Bot[] is for matching ASTM metal designation
//  Edit, add or delete items as desired but make sure to adjust XX_List_Length values above accordingly (for adds/deletes)
//  Sequential values in next two lists define matching metal types/ASTM codes.  Therefore, the two lists must always
//  be the same length.  The bottom one of a pair can be null ("") if ASTM code is not important to identify material.
//
//  Limited space for displaying AISI material codes required a coding scheme
//  The designator code for ASTM numbers is coded as a combination of either actual values in a location,
//  alternate values in a location (enclosed in parenthesis and separated by a /) or a wild card position that can contain
//  any alphanumberic character (designated with an *).
//
//  Example: the ASTM codes for Toolsteels WX, OX and AX are encoded as '(W/O/A)X'.
//  in some cases two designators separated by a space are encoded.


//****************Next Section Determines Compile with Material Choice OPTION_A or Not ('OPTION_B')***********
#ifdef OPTION_A  //Compile with Option A
String option = "A";
// List length statements 2 of 2 (zero based index):
const byte material_list_Length = 14;

//  The following list uses index values into metals_List[], above, instead of actual metal names (to save program space)
const byte material_list_Top[]  = {0, 1, 6, 3, 4, 7, 5, 1, 1, 1, 1, 1, 1, 2, 8};

//  Next list values encode the Little Machine Shop CS Table material types
const String material_list_Bot[]  = {"(5/6/7)0**", "10(0/1/2)*", "", "", "", "", "(W/O/A)*", "11(3/4/5)*","12**",
                                       "41(4/5)*", "110* 111*", "10(3/4/5)*", "11L* 12L*", "(2/3/4)0*", ""
                                    };
//  SFM values for any material type are dependant on tool type.
//  The following lists must be the same length as the material_List_(Top/Bot) above
int material_list_SFM_HSS[] = {550, 175, 325, 675, 180, 225, 120, 145, 280, 135, 225, 118, 230,125, 35};
int material_list_SFM_CARBIDE[] = {2820, 842, 1170, 2820, 410, 715, 400, 735, 930, 560, 950, 820, 810, 570, 80};

#else   //Compile with Option B
String option = "B";
const byte material_list_Length = 18;

const byte material_list_Top[]  = {0, 1, 6, 1, 3, 4, 7, 5, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 8};

const String material_list_Bot[]  = {"(5/6/7)0**", "A36", "", "1018", "", "", "", "(W/O/A)*", "1045","1144",
                                       "1215", "4140", "8620", "41L40", "11L17/12L14", "316", "416", "30(3/4)", ""
                                    };
                                    
int material_list_SFM_HSS[] = {550, 120, 325, 130, 675, 180, 225, 120, 100, 135, 280, 100, 100, 110,
                              200, 60, 180, 125, 35};
int material_list_SFM_CARBIDE[] = {2820, 790, 1170, 800, 2820, 410, 715, 400, 375, 560, 930, 350, 
                                  350, 425, 800, 240, 720, 570, 80};
#endif  //End OPTION_A or B block

//  turning point diameters (in text and decimal formats) are:
const String diameter_List[] = {"1/8\"", "1/4\"", "3/8\"", "1/2\"", "3/4\"", "1\"",
                                 "1 1/4\"", "1 1/2\"",
                                 "1 3/4\"", "2\"", "2 1/2\"", "3\"", "3 1/2\"", "4\""
                               };
                                                           
//  decimal equivalents of the diameter_List[]:
const float diameter_list_Sizes[] = {0.125, 0.250, 0.375, 0.500, 0.750, 1.00, 1.250, 1.50, 1.750, 2.000, 2.500, 3.000, 3.500, 4.000};

//  the following lists are specific to the Atlas 6" lathe and possibly to a subset of these lathes
//  (and possibly only the authors lathe if the pulley diameters are unique).
//  These lists define spindle rpm for a 1725 rpm motor with two pulleys
//  and a driving an idler shaft with four pulleys. Values must be arrainged in descending order.
//  The two lists must match. 
const int pulley_rpm_List[] = {3500, 2943, 2139, 1347, 1163, 942, 845, 532, 372, 0};
//  Corresponding pulley codes are "motor pulley (1/2) - idle pulley (1/2/3/4)"
const String belt_config_List[] = {"1-4+", "2-1", "2-2", "2-3", "1-4", "2-4", "1-2", "1-3", "1-4", "X-X"};

//  The last list is for the two different types of tooling
const String tool_List[] = {"  HSS", "CARBIDE"};

//************************************end advanced variable definitions*********************************************

//************************************Lathe_Helper Core Program section************************************************
#ifdef DISPLAY_ACTUAL_RPM // Only include next block if rpm sensor activated
//  variables used to compute actual rpm
unsigned long Time, save_Time;
//use digital pin 2 for the rpm sensor pulse input signal
const int pin_D2 = 2;
// state is a toggle which is flipped everytime interrupt routine detects a falling rpm sensor signal
volatile int state = LOW;
int save_State = state;
// default (starting) rpm value
int rpm = 0;
// rpm counter variable and max count to reduce actual rpm display jitter
int count = 1;
const int count_Max = 50;
#endif  //End DISPLAY_ACTUAL_RPM block

// an rpm string cache to prevent redundant displays of the same valued target rpm
String save_String = "";

//  starting SFM value
int material_SFM = material_list_SFM_HSS[material_List_Index];


//*******************************Program Functions Sections****************************************
void display_RPMS()
//  display rpm values in two locations
{
  if (tool_List_Index == 0) {
    material_SFM = material_list_SFM_HSS[material_List_Index];
  }
  else {
    material_SFM = material_list_SFM_CARBIDE[material_List_Index];
  }
  float rpm_Desired = (3.820 * material_SFM) / diameter_list_Sizes[diameter_List_Index];
  
  //  Some smaller diameters produce excessively high rpms so need to limit the max
  if(rpm_Desired >= MAXRPM) {
    rpm_Desired = MAXRPM;
  }
  String rpm_Set = String(int(rpm_Desired)) + " ";  

#ifndef DISPLAY_PULLEY_CODES //Won't display pulley_Set if not wanted
  String  pulley_Set = "";
#else
  String pulley_Set = construct_Belt_Config(rpm_Desired);
#endif //end DIPLAY_PULLEY_CODES block

  //  first location is for target rpm

  if (save_String != rpm_Set) { //  wrote only if changed
    tft.fillRect(RPM_X_OFFSET, RPM_TARGET_Y_OFFSET, 130, 40, ILI9341_BLACK);
    tft.setCursor(RPM_X_OFFSET + 5, RPM_TARGET_Y_OFFSET + 15);
    write_Text(rpm_Set + pulley_Set, 2);
    save_String = rpm_Set;
  }

  // Second location is for actual rpm
#ifdef DISPLAY_ACTUAL_RPM // Only include next block if rpm sensor activated
  tft.fillRect(RPM_X_OFFSET, RPM_ACTUAL_Y_OFFSET, 130, 40, ILI9341_BLACK);
  tft.setCursor(RPM_X_OFFSET + 5, RPM_ACTUAL_Y_OFFSET + 15);
  write_Text(String(rpm), 2);
#endif //end DISPLAY_ACTUAL_RPM block

  return;
}

void write_Text(String text, byte size) {
  //  write white text of size at current cursor location
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(size);
  tft.println(text);

  return;
}

void write_Change_Button(int y_offset, String top_Text, String bot_Text)
//  routine writes one of three red change button windows
//  y_offset differentiates between one of three button windows (material, diameter and tool type).
//  Wipes a (red) push button window clean and then displays top and bottom (if not null) texts

{
  //  next red fill rectangle sized to cover all previous white text in change button box
  tft.fillRect(BUTTON_X, BUTTON_Y + y_offset, BUTTON_W, BUTTON_H, ILI9341_RED);
  if (bot_Text == "") {
    //  locate single text field in vertical center of button window
    //  was BUTTON_X + 12
    tft.setCursor(BUTTON_X + 7 , BUTTON_Y + y_offset + (BUTTON_H * 0.4));
    write_Text(" " + top_Text, 2);
  }
  else {
    //  split upper and lower text fields vertically in button window
    //  was BUTTON_X + 12
    tft.setCursor(BUTTON_X + 10 , BUTTON_Y + y_offset + (BUTTON_H * 0.2));
    write_Text(" " + top_Text, 1);

    //  was BUTTON_X + 12
    tft.setCursor(BUTTON_X + 10 , BUTTON_Y + y_offset + (BUTTON_H * 0.6));
    write_Text(bot_Text, 2);
  }

  return;
}

void write_Static_Text()
//  routine blanks the screen and paints all static text
{
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.println(VERSION + option);
  tft.setCursor(10, 100);
  // Use F() option to force string storage in program memory and not FLASH RAM
  tft.println(F("Written by Glenn Tober"));
  tft.setCursor(10, 120);
  tft.println(F("License: "));
  tft.setCursor(10, 140);
  tft.println(F("GNU GPL VER3"));
  


  delay(5000);

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 10);
  write_Text("LATHE HELPER", 3);

  //  text we want red cannot call write_Text. Maybe fix this?
  tft.setCursor(0, 70);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.println(F(" Target RPM:"));

#ifdef DISPLAY_ACTUAL_RPM // Only include next block if rpm sensor activated
  tft.setCursor(0, 150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.println(F(" Actual RPM:"));
#endif // End DISPLAY_ACTUAL_RPM block

  return;
}

void Touch(int y_offset)
//  routine is polled from within loop() to test for button pushes and act accordingly
{
  if (!ts.bufferEmpty()) {
    TS_Point p = ts.getPoint();
    // Scale using the calibration #'s
    // and rotate coordinate system
    p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    int y = tft.height() - p.x;
    int x = p.y;

    if ((x > BUTTON_X) && (x < (BUTTON_X + BUTTON_W))) {
      if ((y > BUTTON_Y + y_offset) && (y <= (BUTTON_Y + y_offset + BUTTON_H))) {
        if (y_offset == MATERIAL_CHANGE_BUTTON_Y_OFFSET) {
          //  the material change area was depressed
          if (x >= BUTTON_X + BUTTON_W / 2) {
            //right side of area was pushed
            material_List_Index = material_List_Index + 1;
            if (material_List_Index > material_list_Length) {
              material_List_Index = 0;
            }
          }
          else {
            //right side of change area was depressed
            if (material_List_Index == 0) {
              material_List_Index = material_list_Length;
            }
            else {
              material_List_Index = material_List_Index - 1;
            }
          }

          write_Change_Button(MATERIAL_CHANGE_BUTTON_Y_OFFSET, metals_List[material_list_Top[material_List_Index]], material_list_Bot[material_List_Index]);
        }

        if (y_offset == DIAMETER_CHANGE_BUTTON_Y_OFFSET) {
          //The diameter change area was depressed
          if (x >= BUTTON_X + BUTTON_W / 2) {
            //right side of area was pushed
            diameter_List_Index = diameter_List_Index + 1;
            if (diameter_List_Index > diameter_list_Length) {
              diameter_List_Index = 0;
            }
          }
          else {
            //left side of change area was depressed
            if (diameter_List_Index == 0) {
              diameter_List_Index = diameter_list_Length;
            }
            else {
              diameter_List_Index = diameter_List_Index - 1;
            }
          }
          write_Change_Button(DIAMETER_CHANGE_BUTTON_Y_OFFSET, diameter_List[diameter_List_Index] + " dia", "");

        }

        if (y_offset == TOOL_CHANGE_BUTTON_Y_OFFSET) {
          //  the tool change area was depressed
          tool_List_Index = tool_List_Index + 1;
          if (tool_List_Index > (tool_list_Length)) {
            tool_List_Index = 0;
          }
          write_Change_Button(TOOL_CHANGE_BUTTON_Y_OFFSET, tool_List[tool_List_Index], "");
        }
      }
    }
    display_RPMS();
  }

  return;
}

#ifdef DISPLAY_PULLEY_CODES  // Only include next block if pulley codes are activated
String construct_Belt_Config(int rpm)
//  routine accepts a calculated rpm value and translates this to closest match for my lathe
{
  int diff = 2900;
  int match = 0;
  for (int x = 0; x <= (rpm_list_Length); x++) {
    if (abs(rpm - pulley_rpm_List[x]) <= diff) {
      diff = abs(rpm - pulley_rpm_List[x]);
      match = x;
    }
  }
  if (match == rpm_list_Length && rpm >= 10) {
    match = match - 1;
  }

  return belt_config_List[match];
}
#endif  //End DISPLAY_PULLEY_CODES block

#ifdef DISPLAY_ACTUAL_RPM // Only include next block if rpm sensor activated
void rpm_event()
//  this is the interrupt service routine which is entered every time there
//  is a falling edge from the rpm sensor input to pin2 interrupt
{
  state = !state;     //flip states so that we can detect this event in loop() below
}
#endif  //End DISPLAY_PULLEY_CODES block


//************************Program Setup Section Runs Once at Startup*******************************
void setup() {
  //  Uncomment next three lines if you want IDE monitor diagnostics
  // Serial.begin(9600);
  //  Serial.print("Lathe Display ");
  //  Serial.println(VERSION);
  //  instantiate tft and ts classes for screen display and touch screen respectively
  tft.begin();
  if (!ts.begin()) {
    write_Change_Button(TOOL_CHANGE_BUTTON_Y_OFFSET, "TS Failure!", "");
  }

  // read diagnostics (optional) to help debug  tft problems)
  /**
    uint8_t x = tft.readcommand8(ILI9341_RDMODE);
    Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDMADCTL);
    Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDPIXFMT);
    Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDIMGFMT);
    Serial.print("Image Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDSELFDIAG);
    Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
   **/

  // Before leaving setup write all default settings to the screen:
  tft.setRotation(rotation);
  write_Static_Text();
  write_Change_Button(MATERIAL_CHANGE_BUTTON_Y_OFFSET, metals_List[material_list_Top[material_List_Index]], material_list_Bot[material_List_Index]);
  write_Change_Button(DIAMETER_CHANGE_BUTTON_Y_OFFSET, diameter_List[diameter_List_Index] + " dia", "");
  write_Change_Button(TOOL_CHANGE_BUTTON_Y_OFFSET, "HSS", "");

  display_RPMS();

#ifdef DISPLAY_ACTUAL_RPM // Only include next block if rpm sensor activated
  //  setup interrupt structure for rpm detection
  //  Allegro Hall Effect sensor has Open Collector output so use input  internal pull up resistor
  pinMode(pin_D2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_D2), rpm_event, FALLING);
  save_State = state;
  Time = micros();
  //  Serial.print("measured rpm = ");
  //  Serial.println(String(rpm));
#endif  // End DISPLAY_ACTUAL_RPM block

  // Leave setup
}


// **********************Program Loop Section Runs Over and Over After Startup()**********************
void loop() {
  // Check for touches on each red button
  Touch(MATERIAL_CHANGE_BUTTON_Y_OFFSET);
  Touch(DIAMETER_CHANGE_BUTTON_Y_OFFSET);
  Touch(TOOL_CHANGE_BUTTON_Y_OFFSET);

#ifdef DISPLAY_ACTUAL_RPM // Only include next block if rpm sensor activated
  if (save_State != state) {  //detected an rpm pulse
    save_Time = micros();
    rpm = int((60 * 1000000) / ((save_Time - Time) * NBR_TEETH));
    Time = save_Time;
    state = save_State;
    count = count + 1;
    if (count >= count_Max) {
      display_RPMS();
      count = 1;
    }
  }
#endif //End DISPLAY_ACTUAL_RPM block
}
