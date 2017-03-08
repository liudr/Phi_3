#include "alarm_clock.h"
#include <EEPROM.h>

void render_RTC(int temp)
{
  int rtc[7];
  char msg[17];
  int user_input;
  phi_prompt_struct myListInput; // This structure stores the main menu.

  RTC_get(rtc,true);

  switch(temp)
  {
    case 0:
    myListInput.ptr.list=(char**)&month_items; // Assign the list to the pointer
    myListInput.low.i=rtc[5]-1; // Default item highlighted on the list
    myListInput.high.i=11; // Last item of the list is size of the list - 1.
    myListInput.width=3; // Length in characters of the longest list item.
    myListInput.col=lcd_columns/2-8; // Display prompt at column 0
    myListInput.row=lcd_rows/2-1; // Display prompt at row 1
    myListInput.option=0; 
    myListInput.step.c_arr[0]=1; // rows to auto fit entire screen
    myListInput.step.c_arr[1]=1; // one col list
    render_list(&myListInput);
    sprintf(msg,"/%02d/%4d/",rtc[4],rtc[6]);
    lcd.print(msg);
    
    myListInput.ptr.list=(char**)&dow_items; // Assign the list to the pointer
    myListInput.low.i=rtc[3]; // Default item highlighted on the list Sunday is 0 Monday is 1
    myListInput.high.i=6; // Last item of the list is size of the list - 1.
    myListInput.col=lcd_columns/2-8+12; // Display prompt at column 0
    render_list(&myListInput);

    lcd.setCursor(lcd_columns/2-4,lcd_rows/2);
    sprintf(msg,"%2d:%02d:%02d",rtc[2],rtc[1],rtc[0]);
    lcd.print(msg);
    break;
    
    case 1:
    sprintf(msg,"%02d%02d",rtc[2],rtc[1]);
    render_big_msg(msg,lcd_columns/2-8,0);
    if ((lcd_rows==4)&&(lcd_columns==20))
    {
      strcpy_P(msg,(char*)pgm_read_word(dow_items+rtc[3])); // Sunday is 0 Monday is 1
      sprintf(msg+3,"%02d",rtc[4]);
      render_big_msg(msg,0,2);
    }
    if (rtc[0]%2)
    {
      lcd.setCursor(lcd_columns/2-1,0);
      lcd.write('.');
      lcd.setCursor(lcd_columns/2-1,1);
      lcd.write('.');
    }
    else
    {
      lcd.setCursor(lcd_columns/2-1,0);
      lcd.write(' ');
      lcd.setCursor(lcd_columns/2-1,1);
      lcd.write(' ');
    }
    break;
    default:
    break;
  }
}

alarm_clock::alarm_clock(boolean noo) // This parameter is needed otherwise the compiler won't work with a constructor without parameters.
{
  alarm_is_on=false;
  snooze=10;
}

byte alarm_clock::run()
{
  int rtc[7];
  render_RTC(clock_style);
  RTC_get(rtc,true);
  for (int i=0;i<Max_alarms;i++)
  {
    if ((alarms[i].hr==rtc[2])&&(alarms[i].mnt==rtc[1])&&(rtc[0]==0)&&within(i,rtc[3]))
    {
      alarm_is_on=true;
      triggered_alarm=i;
      // Do something if alarm is triggered
    }
  }
  if (alarm_is_on) alarm();
}

byte alarm_clock::set_alarm(byte alarm_num, byte hr, byte mnt, byte dow)
{
  if (alarm_num>=Max_alarms) return 255;
  if (hr>23) return 255;
  if (mnt>59) return 255;
  alarms[alarm_num].hr=hr;
  alarms[alarm_num].mnt=mnt;
  alarms[alarm_num].dow=dow;
  return 0;
}

void alarm_clock::alarm()
{
  int temp1;
  for (int i=0;i<total_beeps;i++)
  {
    tone(SPKR,770);
    temp1=wait_on_escape(75);
    if ((temp1==phi_prompt_enter)||(temp1==phi_prompt_escape))
    {
      noTone(SPKR);
      alarm_is_on=false; // Turn off alarm
      // Do some actions here when alarm is turned off
      return;
    }
    noTone(SPKR);
    temp1=wait_on_escape(75);
    if ((temp1==phi_prompt_enter)||(temp1==phi_prompt_escape))
    {
      noTone(SPKR);
      alarm_is_on=false; // Turn off alarm
      // Do something here when alarm is turned off
      return;
    }
  }
  temp1=wait_on_escape(150);
  if ((temp1==phi_prompt_enter)||(temp1==phi_prompt_escape))
  {
    noTone(SPKR);
    alarm_is_on=false; // Turn off alarm
    // Do some actions here when alarm is turned off
    return;
  }
}

boolean alarm_clock::within(byte ala, byte dow) // This function checks if the day of the week is within alarm clock's trigger setting.
{
  switch(alarms[ala].dow)
  {
    case 0: // Alarm is off
    return false;
    break;
    
    case 1: // Alarm is on daily
    return true;
    break;
    
    case 2: // Alarm is on M-F
    if ((dow>=monday)&&(dow<=friday)) return true;
    else return false;
    break;
    
    case 3: // Alarm is on Weekend
    if ((dow==saturday)||(dow==sunday)) return true;
    else return false;
    
    case 4: // Alarm is on once so set it to off and return true
    alarms[ala].dow=0;
    save_alarm(ala); //Save the status of this once clock to EEPROM so it stays off after on triggering.
    return true;
    break;
    
    default:
    return false;
    break;
  }
}

void alarm_clock::turn_on(byte alarm_num) // This function is not used.
{
  if (alarm_num<Max_alarms) alarms[alarm_num].on_off=true;
} 

void alarm_clock::turn_off(byte alarm_num) // This function is not used.
{
  if (alarm_num<Max_alarms) alarms[alarm_num].on_off=false;
} 

void alarm_clock::save_alarm(byte ala) // This function saves the alarm setting of the alarm ala to the ATMEGA328 EEPROM.
{
  EEPROM.write(alarm_EEPROM_storage+3*ala,alarms[ala].hr); // Alarm hour
  delay(100);

  EEPROM.write(alarm_EEPROM_storage+3*ala+1,alarms[ala].mnt); // Alarm minute
  delay(100);

  EEPROM.write(alarm_EEPROM_storage+3*ala+2,alarms[ala].dow); // Alarm frequency M-F
  delay(100);
} 

