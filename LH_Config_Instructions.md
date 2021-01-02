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

**Configuring the Software**

You may wish to modify some of the performance features of the software. Some configuration changes only require a small change to one line of code while others are more extensive

*Simple Modifications*

Locate the *Simple Configuration Definitions * section in the code.

1. Use an RPM sensor with more than one ‘tooth’ per revolution
Locate the ‘byte const NBR_TEETH=1’ code line and change 1 to the number of teeth sensed in one revolution.

2. Disable the spindle belt configuration display function
Locate the #define PULLEY_PULLEY_CODES code line and insert a ‘//’ in front of the # sign.

3.  Disable the  actual RPM sensor display function
Locate the ‘#define ‘DISPLAY_ACTUAL_RPM’ code line and insert a ‘//’ in front of the # sign.

3. Adjust the power on display choice for any of the change buttons
Locate the line of code containing:
‘# define material_List_Index =  0, diameter_List_Index = 3, tool_List_Index = 0;’ and then change any one of the integer values on this line if you want a different material, a different cutting diameter or a different tool type to be displayed on power up.  The new value must be zero or greater and must not exceed the maximum values of length indices defined by another line of code:

material_list_Length = 11, diameter_list_Length = 11 and tool_list_Length = 1;

List length values start with zero and are always one less than the actual number of items in any given list.  You will need to examine each of the three list definitions and determine the new index value that locates a choice for power on that you want (counting left to right starting at zero).

*Advanced Modifications*

Locate the *Advanced Configuration Definitions * section in the code.

This class of changes allows you to configure the software to advanced levels of customization better able to meet your specific needs. Advanced modifications entail changes to list variables defined at the beginning of the program and require care when editing.  Changes modify what the program displays on the LCD screen. Each list has a name equated to a string of defining list values. Most of these lists are associated with one or more other lists of the same type. Changes must be done with care to ensure that relationships between related lists are maintained. The configuration list names are shown in Table 1.  List variable names are assigned a group number as shown in the table.  All of the variable names sharing the same group number must have list lengths which are the same as defined by the the code line:

material_List_Length = 11, diameter_List_Length = 11, Rpm_List_Length = 9, tool_List_Length = 1

that defines Group I, II, III and IV list lengths, respectively.

List length counting begins with zero. Within each group successive elements will be associated with the same successive element in all other lists in that group.

The metal names listed in material_list_Top[] are ‘double indexed’ into the metals_List[] table to save program space (the ‘steel’ name is listed multiple times in a typical material_list_Top[] definition and would require more program constant space if repeated over and over).  Instead of putting metal names as strings into material_list_Top[], put an index number (0-8) that references the name you want from the metals_List[] definition.

Using the Option A, an index of ‘1’ set by a materials change button depression, for example, yields the following display value

	Steel		[displayed top of button area]
	10(0/1/2)*	[displayed bottom of button area]

and later in the program, the use of either of the following SFPM values (dependent on tool selection).

	175			[CS program value for HSS tooling]
	or 842			[CS program value for Carbide tooling]

as defined by the second element in each list in the group (as well as looking up the string ‘   steel’ using the  index of ‘1’ in the metals_List[]). Since you always want the program to associate this particular steel with those particular values of CS, all list changes must be made in a consistent manner across a group.		



| List Name                   | Group Number |
| --------------------------- | ------------ |
| material_list_Top[]         | Group I      |
| material_list_Bot[]         | Group I      |
| material_list_SFM_HSS[]     | Group I      |
| material_list_SFM_CARBIDE[] | Group I      |
| diameter_List[]             | Group II     |
| diameter_list_Sizes[]       | Group II     |
| pulley_rpm_List[]           | Group III    |
| belt_config_List[]          | Group III    |
| tool_List[]                 | Group IV     |
| metals_List[]               | (stand alone)|

*Table 1. Program List Names and Groupings*



In the future program changes may be made by the author at any time including those that correct errors or those that enhance performance. Updates will be posted on the Github web site labeled as a version/revision later than the current (older) version.  You can continue to use an older version or download/install the newer one. The *README.txt* file on the download site will describe the latest changes and provide current version numbers.

For instructions on how to load, compile and install the software please see (LH_IC&T_Instructions.md)
