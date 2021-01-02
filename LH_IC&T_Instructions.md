**Notice**

    Lathe Helper Project Software as described in the February Issue of The Home Shop Machinist
    
    Copyright (C)  2020 Glenn Tober (glennt@palmnet.net)
    Source: https://github/glennto/LH_Display
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.Configuring the Software

**Installation Checkout  and Test**

The following paragraphs provide essential information about installing/setting up, testing and configuring the Lathe_Helper software. Complete information is beyond the scope of this article (refer to vendor documentation for this).

You must download and install a version of the Arduino Integrated Development Environment (IDE) suitable for your computer (IDE is an industry name for a computer program that aids software development and software installation).  Arduino IDE versions are available for Windows, Mac and Linux operating systems.  The IDE is required for installation of the Lathe Helper software.

Once the IDE is installed use the USB cable and connect the Arduino Uno board (by itself) to your computer.  The board will power up on its own.  Now look under the IDE ‘Tools’ menu and see that the ‘Port’ button is active.  Selecting the ‘Port’ button should that either one port is already selected or reveal that one or more port options must be selected. The proper computer USB port option must be selected before going further. You may have to trial and error them to see which one works.  The USB port is properly configured when you can go under the IDE ‘Tools  - Get Board Info’ and see the Uno board listed.  On my Linux machine the proper USB port is labeled ‘/dev/ttyACMO(Arduino Uno)’.  On a windows computer the port name will contains a similar ‘tty..’ identifier.  Next go to IDE ‘Tools’ and see that ‘Board Arduino Uno’ is displayed.

With the IDE properly configured select one of the example programs under the IDE ‘File - Examples – Basic’ menu (‘Blink’ is a good choice).  In the new IDE window click on the left most upper check mark and the file should compile successfully.  Next click on the upload button to the right of the compile button and the ‘Blink’ program will be uploaded to the Uno. The Uno board will then execute the test program and you can verify that, for example, the Uno LED is indeed blinking.  Your Arduino IDE and Uno interface are now working.

When the IDE was installed a number of  ‘library packages’ were installed with it. A few additional libraries are required to support the Adafruit LCD resistive touch screen used by Lathe Helper. Adding these libraries is straightforward.  Select “Tools Manage Libraries’.  One at a time enter each of the following library names into the filter window and select ‘Install’ for any one which is not already installed:

1. SD library
2. Adafruit_GFX library
3. Adafruit_ILI9341 library
4. Adafruit_STMPE610 library

**Testing the Hardware**

Mate the TFT Display board to the Uno board and reconnect the Uno board to the computer. Go to ‘Files Examples’ and successively open, compile and load/run (one at a time) the following example programs:

1. ‘graphicstest’ under Adafruit_ILI9341
2. ‘touchtest’ under Adafruit_STMPE610

If (1) displays some pretty graphics and (2) lets you push a ‘button’ area on the screen and see some results then you are good to go. 

The only step left is to locate and open the downloaded LH_Display_VXX.ino file.  Check the compile box followed by the load check box.  After the credits screen goes away and you see the Lath Helper screen on the LCD display the software is installed.  You will not need the IDE again unless you change the software or for any reason the installation becomes corrupted.  Note that power on/off cycles of the Lathe Helper instrument does not effect the installed software.

For instructions on how to make certain modifications to the software please see (LH_Config_Instructions)
