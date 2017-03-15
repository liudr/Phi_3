/*
 * Test code for i2c liquid crystal display. See instruction in the next comment.
 */
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Some models have address 0x27 and other 0x3F. Please uncomment the correct line below
//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Blue potentiometer and BL jumper.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Blue potentiometer and BL jumper.

void setup()
{
  lcd.begin(20,4);               // initialize the lcd 
  lcd.setBacklight(1);           // Turn on back light
}

void loop()
{
   lcd.home ();
   lcd.print("Hello World!");
   lcd.setCursor(0,1);
   lcd.print(millis()/1000);
   delay (1000);
}

