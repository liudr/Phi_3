/*
Phi-3 shield for Arduino
Program title: Phi clock version 1.6 
----------------------------------------
Instructions:
0. Please download phi_interfaces library (https://github.com/liudr/phi_interfaces). Download zip file and unzip the files and example subfolders in arduino\sketchbooks\libraries\phi_interfaces\
1. Please download phi_prompt library (https://github.com/liudr/phi_prompt/). Download zip file and unzip the files and example subfolders in arduino\sketchbooks\libraries\phi_prompt\
2. Please download phi_big_font library (http://liudr.wordpress.com/phi_big_font/) and unzip the three files and example subfolders in arduino\sketchbooks\libraries\phi_big_font\
3. Please download RTClib from your library manager.
4. Please download NewliquidCrystal (https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home).
5. Upload the sketch to arduino and select NO to load alarms if you are running it for the first time.
----------------------------------------
Programmed by Dr. John Liu
Revision: 02/20/2017
Free software for educational and personal uses.
No warrantee!
Commercial use without authorization is prohibited.
Find details of the Phi-3 shield, and Phi-prompt or contact Dr. Liu at
http://liudr.wordpress.com/phi-3-shield/
http://liudr.wordpress.com/phi_interfaces/
http://liudr.wordpress.com/phi_prompt/
All rights reserved.
----------------------------------------
List of functions:
* Display real time clock with Year month, date, day of week, hour, minute, and second.
* Four individual alarms (change Max_alarms to get more alarms)
* Alarms are stored on Arduino EEPROM and won't be lost when the power is out.
* Alarms Daily, Weekday, Weekend, Once, Off.
* Adjustable alarms. Press B to choose alarm and adjust the time.
* Adjustable clock and date. Press A to adjust clock and date.
* TODO - Add snooze function.
* TODO - Add special function on the alarm to drive outputs other than the buzzer and LED. You can be creative!
----------------------------------------
Thanks:

EEPROM Author: hkhijhe

Dependent libraries:
RTClib
NewliquidCrystal
Phi_prompt version 1.6.0
Phi_interfaces version 1.6.0
Phi_big_font version 1.6.0

Updates:
 * 02/20/2017: Updated to run on Phi-3 shield
 * 08/23/2015: Added some custom features
 * 03/05/2015: Updated for compatibily with Arduino IDE 1.6.0
 * 08/26/2012: I corrected the bug Andrew found about the "ONCE" alarm not turned off in EEPROM settings.
 * I rewrote the code to save the status of a "ONCE" alarm to EEPROM as "OFF" once it is triggered. This was made possible with the addition of method save_alarm().
 * Now the set alarm function in the Example_menu also calls this save_alarm method.
*/

#include <SoftwareSerial.h> // Adafruit_GPS depends on this library
#include <Wire.h> // RTClib depends on this library
#include <SPI.h> // RTClib depends on this library
#include <RTClib.h>
//#include <SdFat.h>
//#include <Adafruit_GPS.h>
#include <LiquidCrystal_I2C.h>
#include <phi_interfaces.h>
#include <phi_prompt.h>
#include <phi_big_font.h>
#include "alarm_clock.h"
#include <EEPROM.h>

const int lcd_rows=4;
const int lcd_columns=20;

const int total_buttons=6;
const int button_up=6;
const int button_down=5;
const int button_left=7;
const int button_right=4;
const int button_B=3;
const int button_A=2;

const int LED0=A5;
const int LED1=A6;
const int SPKR=A7;
const int SD_card_chip_select=10;

const int sunday=0;
const int monday=1;
const int tuesday=2;
const int wednesday=3;
const int thursday=4;
const int friday=5;
const int saturday=6;

byte pins[]={button_up,button_down,button_left,button_right,button_B,button_A}; // The digital pins connected to the 6 buttons.
phi_button_groups my_btns("UDLRBA", pins, total_buttons);
char * function_keys[]={"U","D","L","R","B","A"}; ///< All function key names are gathered here fhr phi_prompt.
multiple_button_input * keypads[]={&my_btns,0}; // This adds all available keypads as inputs for phi_prompt library

RTC_DS3231 RTC;
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // This works with a particular I2C LCD with blue square potentiometer and a back light enable jumper. Refer to diagrams. Some other variations use 0x27 instead of 0x3F.

const int total_beeps=4; // How many short beeps are grouped together, such as bi-bi-bi-bi   bi-bi-bi-bi for most alarm clocks. Some alarm clocks beeps constantly without short beeps and pauses. In this case, use 1.

const char PROGMEM msg_00[]="LCD ALARM CLOCK\nDr. Liu 02/20/2017\nV1.6 Phi-3 shield\nliudr.wordpress.com";

const char PROGMEM month_00[]="JAN";
const char PROGMEM month_01[]="FEB";
const char PROGMEM month_02[]="MAR";
const char PROGMEM month_03[]="APR";
const char PROGMEM month_04[]="MAY";
const char PROGMEM month_05[]="JUN";
const char PROGMEM month_06[]="JUL";
const char PROGMEM month_07[]="AUG";
const char PROGMEM month_08[]="SEP";
const char PROGMEM month_09[]="OCT";
const char PROGMEM month_10[]="NOV";
const char PROGMEM month_11[]="DEC";
const char PROGMEM * const month_items[]= {month_00,month_01,month_02,month_03,month_04,month_05,month_06,month_07,month_08,month_09,month_10,month_11};

const char PROGMEM dow_00[]="SUN";
const char PROGMEM dow_01[]="MON";
const char PROGMEM dow_02[]="TUE";
const char PROGMEM dow_03[]="WED";
const char PROGMEM dow_04[]="THU";
const char PROGMEM dow_05[]="FRI";
const char PROGMEM dow_06[]="SAT";
const char PROGMEM * const dow_items[]= {dow_00,dow_01,dow_02,dow_03,dow_04,dow_05,dow_06};

const char PROGMEM alarm_00[]="Off";
const char PROGMEM alarm_01[]="Daily";
const char PROGMEM alarm_02[]="Weekday";
const char PROGMEM alarm_03[]="Weekend";
const char PROGMEM alarm_04[]="Once";
const char PROGMEM * const alarm_items[]= {alarm_00,alarm_01,alarm_02,alarm_03,alarm_04};

const char PROGMEM lcd_chr0[]={64,64,64,64,64,64,64,64,0};// 0
const char PROGMEM lcd_chr1[]={64,64,64,64,64,31,31,31,0};//1
const char PROGMEM lcd_chr2[]={64,64,64,31,31,64,64,64,0};//2
const char PROGMEM lcd_chr3[]={64,64,64,31,31,31,31,31,0};//3 {64,64,64,31,31,64,31,31,0};
const char PROGMEM lcd_chr4[]={31,31,31,64,64,64,64,64,0};//4
const char PROGMEM lcd_chr5[]={31,31,64,64,64,31,31,31,0};//5
const char PROGMEM lcd_chr6[]={31,31,31,31,31,64,64,64,0};//6 {31,31,64,31,31,64,64,64,0}
const char PROGMEM lcd_chr7[]={31,31,31,31,31,31,31,31,0};//7 {31,31,64,31,31,64,31,31,0}
const char PROGMEM * const chr_item[] = {lcd_chr0, lcd_chr1, lcd_chr2, lcd_chr3, lcd_chr4, lcd_chr5, lcd_chr6, lcd_chr7};// The following lines instantiates a button group to control 6 buttons

alarm_clock clock1(false); // Create an alarm clock
int rtc[7];
void setup()
{
  tone(SPKR,770,500);
  byte ch_buffer[10]; // This buffer is required for custom characters on the LCD.
  lcd.begin(lcd_columns, lcd_rows); // No need to begin RTC since it is just wire.begin(), whcih is included in lcd.begin().
  init_phi_prompt(&lcd,keypads,function_keys, lcd_columns, lcd_rows, '~'); // Supply the liquid crystal object, input keypads, and function key names. Also supply the column and row of the lcd, and indicator as '>'. You can also use '\x7e', which is a right arrow.
  init_big_font(&lcd);

  pinMode(SPKR,OUTPUT);
  digitalWrite(SPKR,LOW);

  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) 
  {
    Serial.println("RTC is older than compile time!  Updating");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  if (RTC.lostPower()) 
  {
    Serial.println("RTC has lost power!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  if (yn_dialog("Load alarms from EEPROM?")==phi_prompt_dialog_NO)
  {
  //Set alarms
    for (byte i=0;i<Max_alarms;i++)
    {
      EEPROM.write(alarm_EEPROM_storage+bytes_per_alarm*i,6);
      EEPROM.write(alarm_EEPROM_storage+bytes_per_alarm*i+1,50);
      EEPROM.write(alarm_EEPROM_storage+bytes_per_alarm*i+2,alarm_clock_alarm_off);
    }
  }
  else
  {

// Set alarm on the clock from EEPROM.
    for (byte i=0;i<Max_alarms;i++)
    {
      clock1.set_alarm(i, EEPROM.read(alarm_EEPROM_storage+bytes_per_alarm*i), EEPROM.read(alarm_EEPROM_storage+bytes_per_alarm*i+1), EEPROM.read(alarm_EEPROM_storage+bytes_per_alarm*i+2));
    }
  }
  show_credit();
}

void loop()
{
  top_menu(); // See Example_menu.pde
}

void show_credit()
{
  // Display credits
  phi_prompt_struct myLongMsg;

  lcd.clear();
  lcd.noBlink();
  myLongMsg.ptr.msg_P=msg_00; // Assign the address of the text string to the pointer.
  myLongMsg.low.i=0; // Default text starting position. 0 is highly recommended.
  myLongMsg.high.i=strlen_P(msg_00); // Position of the last character in the text string, which is size of the string - 1.
  myLongMsg.step.c_arr[0]=lcd_rows; // rows to auto fit entire screen
  myLongMsg.step.c_arr[1]=lcd_columns; // one col list
  myLongMsg.col=0; // Display the text area starting at column 0
  myLongMsg.row=0; // Display the text area starting at row 0
  myLongMsg.option=0; // Option 0, display classic message, option 1, display message with scroll bar on right.

  text_area_P(&myLongMsg);
}

void RTC_get(int rtc[], boolean tf)
{
  DateTime now=RTC.now();
  rtc[0]=now.second();
  rtc[1]=now.minute();
  rtc[2]=now.hour();
  rtc[3]=now.dayOfTheWeek();
  rtc[4]=now.day();
  rtc[5]=now.month();
  rtc[6]=now.year();
}

