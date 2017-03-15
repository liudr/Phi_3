/*
 * Test code for i2c liquid crystal display and DS3231 RTC. See instruction in the next comment.
 */

// include the library code:
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// Some models have address 0x27 and other 0x3F. Please uncomment the correct line below
//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Blue potentiometer and BL jumper.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Blue potentiometer and BL jumper.

RTC_DS3231 RTC;

void setup() 
{
  // set up the LCD's number of rows and columns: 
  lcd.begin(20, 4);
  RTC.begin();
  Serial.begin(9600);
  lcd.setBacklight(HIGH);
  if (RTC.lostPower()) 
  {
    Serial.println("RTC lost power, lets set the time!");
    // The following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() 
{
  char buf[17];
  DateTime now = RTC.now();
  lcd.setCursor(0, 0);
  // print the number of seconds since reset:
  sprintf(buf,"%4d/%02d/%02d ",now.year(),now.month(),now.day());
  Serial.print(buf);
  lcd.print(buf);
  sprintf(buf,"%2d:%02d:%02d",now.hour(),now.minute(),now.second());
  Serial.println(buf);
  
  lcd.setCursor(0,1);
  lcd.print(buf);
  delay(1000);
}

