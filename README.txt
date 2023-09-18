LATHE DISPLAY DEPLOYMENT

Arduino based code for an Arduino Uno and Adafruit TFT/
Resistive Touch Screen display board combo that implements the Lathe Helper
display application as described in the March/April 2021 issue of The Home
 Machinist

	Author: Glenn Tober
	Source: https://github.com/glennto/LH_Project
	Email: glennt@palmnet.net
	Copyright:	Glenn Tober 12/2020


INTENDED USE

Purpose is to accept operator material type, cutting diameter and tool type inputs and then visually display a computed spindle rpm setting and a corresponding motor belt setting that best achieves a tool surface cutting speed appropriate to a particular machining task. This instrument's simple function is to replace numerous paper references and calculations otherwise needed to perform this common shop function.

This instrument also displays actual spindle rpm as an aid to operations. All interactions are made via a touch screen display element.


IMPORTANT WARNING

This device does not directly control lathe, mill or drill operations in any manner. The visually displayed spindle rpm value produced by this instrument is subject to error due to operator, programming or other unforeseen events and must be interpreted for safe use prior to use by the machine operator.
  

DOCUMENTATION

All documentation, excepting item 2 below, is hosted on the above referenced github site.

	1. This README.TXT file is the project baseline document.
	2. The original project description as published in the March/April 2021 	issue of The Home Shop Machinist (a Village Press Publication).
	3. Additional Integration Checkout and Test instructions found in 	LH_IC&T_Instructions.md
	4. Instructions for software conditional compile directives and other 	possible configuration/tuning changes described in LH_Config_Instructions.md
	5. Project addendum/errata information described in
	LH_Addendum.md

No operations manual other than item 2 above is provided.


HARDWARE REQUIREMENTS

Target hardware includes an Arduino Uno R3 with a mated 1651 Adafruit TFT touch screen display board. A standard Arduino 7VDC external wall power unit/cable connector and an Arduino compatible USB programming cable are required.

A suitable pulsing rotation sensor is required to display actual spindle rpm. A 3144 Allegro Hall Effect sensor (which actually senses the passing of a magnet attached to rotating shaft -ie, a 'one tooth gear sensor') is recommended. This three terminal open collector device is direct connected to Uno +5V, ground and signal and requires no other components. Power and rpm speed sensor connections are facilitated by a 2027 Adafruit Proto board sandwiched between the Uno R3 and the TFT display boards.

A desktop or laptop computer with a USB port and a suitable operating system (Windows, MAC or Linux) is required to compile and download the software to the Uno.

A reference mechanical design for an instrument enclosure, a spindle rpm sensing assembly and the interconnecting power and signal cables is described in reference 2 above. The instrument enclosure design can done independently or not at all.


SOFTWARE REQUIREMENTS

The application software (LH_Helper_VX.X) should be compiled and loaded using the Arduino Integrated Development Environment (IDE) in communication with a connected Uno R3. The following Adafruit libraries are required by the software:

	#include "SPI.h"
	#include "Adafruit_GFX.h"
	#include "Adafruit_ILI9341.h"
	#include "Adafruit_STMPE610.h"

The code must dedicate the following Arduino Uno pins:

	#define STMPE_CS 8	// Pin 8 Touchscreen chip select
	#define TFT_DC 9	// Pin 9 TFT data 
	#define TFT_CS 10	// Pin 10 TFT chip select
	int pin = D2 // Pin 2 offboard rpm sensor interrupt input

The first three pin connections are made when the factory boards are mated and configured in software. The D2 interrupt input must be wired to a suitable pulse rpm sensor configured in software using the INPUT_PULLUP option in the pin interrupt declaration statement.

The baseline 'Option A' software in LH_Helper_VX.X encodes a materials/SFPM list derived from the Little Machineshop table of values:

	https://littlemachineshop.com/Reference/CuttingSpeeds.php#turning

Refernce 2 above describes an alternate list defined as 'Option B'. The Option A/B choice is made by a #define OPTION_A statement in the software (or NOT).

Future clarifications/errata material will be posted in the file:

LH_Project_Addendum.md 


VERSION HISTORY

  Version 0.10 01/15/2021
  
  Fixed listing problem in lathe pulley codes ('1-4' was listed twice, changed incorrect one to '1-1')
  This version is distribution release version.

  Version 0.50 09/15/2023

  Added LH_Panel.jpg

