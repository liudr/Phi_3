/** \file
 *  \brief     This is a sample program to test the phi-3 shield.
 *  \details   The program will blink the two LEDs and beep the speaker. It will then display a long message that you can read by scrolling with up/down and dismiss with B. Then it displays a menu for you to choose and prints your choice after you press B. Then you will enter a number. It gets saved to SD card. This requires a number of libraries.
 *  \author    Dr. John Liu
 *  \version   1.0
 *  \date      02/17/2017
 *  \pre       Compatible with Arduino IDE 1.6.X
 *  \bug       Not tested on Arduino IDE higher than 1.6.X!
 *  \warning   PLEASE DO NOT REMOVE THIS COMMENT WHEN REDISTRIBUTING! No warranty!
 *  \copyright Dr. John Liu. GNU GPL V 3.0.
 *  \par Contact
 * Obtain the documentation or find details of the phi_interfaces, phi_prompt TUI library, Phi-3 shield, and Phi-panel hardware or contact Dr. Liu at:
 *
 * <a href="http://liudr.wordpress.com/phi_interfaces/">http://liudr.wordpress.com/phi_interfaces/</a>
 *
 * <a href="http://liudr.wordpress.com/phi-panel/">http://liudr.wordpress.com/phi-panel/</a>
 *
 * <a href="http://liudr.wordpress.com/phi_prompt/">http://liudr.wordpress.com/phi_prompt/</a>
 *
 * <a href="http://liudr.wordpress.com/phi-2-shield/">http://liudr.wordpress.com/phi-3-shield/</a>
 *  \par Library dependency
 * This sketch requires sdfat, RTClib, Adafruit_GPS, phi_interfaces, phi_prompt, and NewliquidCrystal libraries
 *
 *  \par Update
 * 02/17/2017: Initial release of the code. Tested with I2C LCD, DS3231, LEDs, speaker, and buttons.
 * \par Function keys
 * The roles of the function keys are different in different inputs.
 * \par Up
 * Scroll one line up in long message. Increment one character in place in input panel or input number. Move one item up in the select list.
 * \par Down
 * Scroll one line down in long message. Decrement one character in place in input panel or input number. Move one item down in the select list.
 * \par Left
 * Scroll one page up in long message. Move one character to the left in input panel or input number. Move one column to the left in the select list.
 * \par Right
 * Scroll one page down in long message. Move one character to the right in input panel or input number. Move one item column to the right in the select list.
 * \par Enter
 * Dismiss a long message, return 1. Submit result in input panel or input number and dismiss the input, return 1. Select the highlighted item in the select list and dismiss the list.
 * \par Escape
 * Dismiss a long message, return -1. Dismiss the input and return -1. Dismiss the list and return -1.
 */

#include <SoftwareSerial.h> // Adafruit_GPS depends on this library
#include <Wire.h> // RTClib depends on this library
#include <SPI.h> // RTClib depends on this library
#include <RTClib.h>
#include <SdFat.h>
#include <Adafruit_GPS.h>
#include <LiquidCrystal_I2C.h>
#include <phi_interfaces.h>
#include <phi_prompt.h>

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

const boolean use_GPS=false;
boolean usingInterrupt = false;

char log_file_name[]="LOG.TXT";
byte pins[]={button_up,button_down,button_left,button_right,button_B,button_A}; // The digital pins connected to the 6 buttons.
phi_button_groups my_btns("UDLRBA", pins, total_buttons);
char * function_keys[]={"U","D","L","R","B","A"}; ///< All function key names are gathered here fhr phi_prompt.
multiple_button_input * keypads[]={&my_btns,0}; // This adds all available keypads as inputs for phi_prompt library

Adafruit_GPS GPS(&Serial2);
RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // This works with a particular I2C LCD with blue square potentiometer and a back light enable jumper. Refer to diagrams. Some other variations use 0x20, or 0x27 instead of 0x3F. Use the I2C bus scanner to determine the address.

SdFat sd;
SdFile logfile; 

void setup() 
{
  Serial.begin(9600);
  Serial2.begin(9600);
  lcd.begin(lcd_rows, lcd_columns);               // initialize the lcd 
  lcd.setBacklight(1); // Turn on LCD back light
  init_phi_prompt(&lcd,keypads,function_keys, lcd_columns, lcd_rows, '~'); // Supply the liquid crystal object, input keypads, and function key names. Also supply the column and row of the lcd, and indicator as '>'. You can also use '\x7e', which is a right arrow.
  pinMode(LED0, INPUT); // The high-brightness LEDs are VERY bright! To use them less bright, declare the pin as INPUT. To use full brightness, declear as OUTPUT.
  pinMode(LED1, INPUT); // The high-brightness LEDs are VERY bright! To use them less bright, declare the pin as INPUT. To use full brightness, declear as OUTPUT.

  //Flash the LEDs a couple of times and then buzz.
  digitalWrite(LED0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED1, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for 0.5 second
  digitalWrite(LED0, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for 0.5 second
  digitalWrite(LED0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED1, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for 0.5 second
  digitalWrite(LED0, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for 0.5 second
  digitalWrite(LED1, LOW);    // turn the LED off by making the voltage LOW
  tone(SPKR,540,250);

  if (use_GPS)
  {
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
    useInterrupt(true);
  
    while(1) // Waiting for GPS fix
    {
      if (GPS.fix)
      {
        lcd.clear();
        lcd.print(GPS.year);
        lcd.print("-"); lcd.print(GPS.month);
        lcd.print("-"); lcd.print(GPS.day);
        wait_on_escape(2000); // This acts as delay but you can dismiss the delay by pressing any button
        break;
      }
      else 
      {
        lcd.clear(); // Clears screen
        lcd.print("Waiting for GPS fix");
        wait_on_escape(500); // This acts as delay but you can dismiss the delay by pressing any button
        lcd.setCursor(0,1);
        lcd.print(".");
        wait_on_escape(500); // This acts as delay but you can dismiss the delay by pressing any button
        lcd.print(".");
        wait_on_escape(500); // This acts as delay but you can dismiss the delay by pressing any button
        lcd.print(".");
        wait_on_escape(1000); // This acts as delay but you can dismiss the delay by pressing any button
      }
    }
  }
  init_sd_card();
}

void loop() 
{
  char input_buffer[]="40000";
  int ret_val=255;
  long user_input;
  // Displays a long message
  simple_text_area("This is a sample code for displaying information, menu, and collecting user inputs on phi-3 shield with phi_prompt library.\nAuthor: Dr. Liu\nDate: 2017-02-17");
  
  // Use a select list as a more elegant menu. You select it with up/down/enter keys.
  ret_val=simple_select_list("Main menu:\nDisplay GPS info\nRecord GPS info \nErase data\nDisplay records\nParameters\nShow credit\n");
  lcd.clear();
  lcd.print("Your choice:");
  lcd.print(ret_val);
  wait_on_escape(2000); // This acts as delay but you can dismiss the delay by pressing any button

  // Use a simple input panel to collect input. You can use sscanf or else to convert numbers in the string to integers.
  lcd.clear();
  lcd.print("Enter 5-digit:");
  ret_val=simple_input_panel(input_buffer,'0','9',0);
  lcd.clear();
  lcd.print("Your input:");
  lcd.print(input_buffer);
  wait_on_escape(2000); // This acts as delay but you can dismiss the delay by pressing any button
  
  // Use sscanf to turn text input into number, that is if the input is number
  lcd.clear();
  lcd.print("Your value:");
  sscanf(input_buffer,"%ld",&user_input);
  lcd.print(user_input);
  start_logging();
  logfile.println(user_input);
  stop_logging();
  lcd.setCursor(0,1);
  lcd.print("Was logged to SD.");
  wait_on_escape(2000); // This acts as delay but you can dismiss the delay by pressing any button
  
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) 
{
  GPS.read();
  if (GPS.newNMEAreceived()) GPS.parse(GPS.lastNMEA());
}

void useInterrupt(boolean v) {
  if (v) {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

void init_sd_card()
{
  pinMode(SD_card_chip_select, OUTPUT);
  delay(50);
  
  Serial.println();
  Serial.print(F("Initializing SD card"));delay(1000);
  
  if (!sd.begin(SD_card_chip_select, SPI_HALF_SPEED)) // see if the card is present and can be initialized
  {
    Serial.println();
    Serial.print(F("Can't start SD card.")); while(1);
  }
  if (use_GPS)
  {
    SdFile::dateTimeCallback(GPS_file_date_time); // Attach callback function to provide date time to files.
  }
  else
  {
    SdFile::dateTimeCallback(RTC_file_date_time); // Attach callback function to provide date time to files.
  }
  Serial.println();
  Serial.print(F("Card initialized"));delay(1000);
}

void GPS_file_date_time(uint16_t* date, uint16_t* time) 
{
  int yr, mo, dy, hr, mi, se;
  
  noInterrupts(); // Make sure background parsing won't change values while they are being extracted for output.
  yr=GPS.year; // 2-digit such as 17 for 2017
  mo=GPS.month;
  dy=GPS.day;
  hr=GPS.hour;
  mi=GPS.minute;
  se=GPS.seconds;
  interrupts();

  *date=FAT_DATE(yr+2000,mo,dy); // Add 2000 to yr
  *time=FAT_TIME(hr,mi,se);
}

void RTC_file_date_time(uint16_t* date, uint16_t* time) 
{
  DateTime now = RTC.now();
  *date=FAT_DATE(now.year(),now.month(),now.day());
  *time=FAT_TIME(now.hour(),now.minute(),now.second());
}

byte start_logging()
{
  if (sd.exists(log_file_name))
  {
    if (!logfile.open(log_file_name, O_WRITE | O_CREAT | O_AT_END))
    {
      Serial.println();
      Serial.print(F("Can't create file.")); 
      while(1);
    }
  }
  else
  {
    if (!logfile.open(log_file_name, O_WRITE | O_CREAT | O_AT_END))
    {
      Serial.println();
      Serial.print(F("Can't create file.")); 
      while(1);
    }
  }
}

byte stop_logging()
{
  logfile.sync();
  logfile.close();
}



