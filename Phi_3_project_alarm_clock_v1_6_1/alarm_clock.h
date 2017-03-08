#ifndef alarm_clock_h
#define alarm_clock_h
#include <Arduino.h>

#define Max_alarms 12
#define alarm_EEPROM_storage 0//(1024-3*Max_alarms) // Where the alarm is saved. This is assuming ATMEGA328 is used with 1K EEPROM so the 1023.
#define bytes_per_alarm 3 // We store 3 bytes for each alarm, hour, minute, and frequency
#define alarm_clock_alarm_off 0
#define alarm_clock_alarm_daily 1
#define alarm_clock_alarm_weekday 2
#define alarm_clock_alarm_weekend 3
#define alarm_clock_alarm_once 4

class alarm_clock
{
public:
  alarm_clock(boolean noo);
  byte run(); // Returns which alarm is triggered, 0-Max_alarms, 255 means no alarm is triggered.
  byte set_alarm(byte alarm_num, byte hr, byte mnt, byte dow); // Returns 255 if parameters are invalid. Returns 0 if valid.
  void turn_on(byte alarm_num);
  void turn_off(byte alarm_num);
  void alarm();
  boolean within(byte a, byte dow);
  void save_alarm(byte ala); ///< This saves the alarm setting of alarm ala to the ATMEGA328 EEPROM.
  
  typedef struct {
  byte hr;
  byte mnt;
  byte dow;
  boolean on_off; // This variable is not used. Instead dow is used to turn the alarm off or on.
  } entry;

  entry alarms[Max_alarms];
  boolean alarm_is_on; // Alarm is on. alarm() will be called if this is true.
  byte triggered_alarm; // Which alarm is triggered.
  byte snooze;
};
#endif
