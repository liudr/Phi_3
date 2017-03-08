/*
.___  ___.  _______ .__   __.  __    __  
|   \/   | |   ____||  \ |  | |  |  |  | 
|  \  /  | |  |__   |   \|  | |  |  |  | 
|  |\/|  | |   __|  |  . `  | |  |  |  | 
|  |  |  | |  |____ |  |\   | |  `--'  | 
|__|  |__| |_______||__| \__|  \______/  
*/

// Menu texts
const char PROGMEM top_menu_item00[]="Display clock";
const char PROGMEM top_menu_item01[]="Adjust time";
const char PROGMEM top_menu_item02[]="Set alarms";
const char PROGMEM top_menu_item03[]="Display settings";
const char PROGMEM top_menu_item04[]="Show credit";
const char PROGMEM * const top_menu_items[] = {top_menu_item00, top_menu_item01, top_menu_item02, top_menu_item03, top_menu_item04};

//This program is the main menu. It handles inputs from the keys, updates the menu or executes a certain menu function accordingly.
int clock_style=0; // This is the style of the menu
void top_menu()
{
  int menu_pointer_1=0; // This stores the menu choice the user made.
  phi_prompt_struct myMenu; // This structure stores the main menu.

// Initialize the top menu
  myMenu.ptr.list=(char**)&top_menu_items; // Assign the list to the pointer
  myMenu.low.i=0; // Default item highlighted on the list
  myMenu.high.i=4; // Last item of the list is size of the list - 1.
  myMenu.width=lcd_columns-1; // Length in characters of the longest list item.
  myMenu.step.c_arr[0]=lcd_rows-1; // rows to auto fit entire screen
  myMenu.step.c_arr[1]=1; // one col list
  myMenu.step.c_arr[2]=0; // y for additional feature such as an index
  myMenu.step.c_arr[3]=lcd_columns-4; // x for additional feature such as an index
  myMenu.col=0; // Display prompt at column 0
  myMenu.row=1; // Display prompt at row 1
  myMenu.option=45;
  
  while(1) // This loops every time a menu item is selected.
  {
    lcd.clear();  // Refresh menu if a button has been pushed
    lcd.print("Main Menu");

    select_list(&myMenu); // Use the select_list to ask the user to select an item of the list, that is a menu item from your menu.
    menu_pointer_1=myMenu.low.i; // Get the selected item number and store it in the menu pointer.
    switch (menu_pointer_1) // See which menu item is selected and execute that correspond function
    {
      case 0:
      lcd.clear();
      top_menu_function_1();
      break;
      case 1:
      lcd.clear();
      top_menu_function_2();
      break;
      case 2:
      lcd.clear();
      top_menu_function_3();
      break;
      
      case 3:
      lcd.clear();
      top_menu_function_4();
      break;
      
      case 4:
      lcd.clear();
      top_menu_function_5();
      break;
      
      default:
      break;
    }

  }
}

// Menu functions go here. The functions are called when their menu items are selected. They are called only once so if you want to do something repeatedly, make sure you have a while loop.
void top_menu_function_1() //This runs the clock
{
  lcd.clear();
  center_text("Anykey to return");
  wait_on_escape(1000);
  lcd.clear();
  int temp1;
  while (1)
  {
    clock1.run();
    if (!clock1.alarm_is_on)
    {
      temp1=wait_on_escape(1000);
      switch (temp1)
      {
        case 0:
        break;
        
        default:
        return;
        break;
      }
    }
  }  
}

void top_menu_function_2() //Set the clock
{
  int user_input;
  phi_prompt_struct myIntegerInput, myListInput; // This structure stores the main menu.

  render_RTC(0);
  RTC_get(rtc,true);
  // Set hour
  user_input=rtc[2]; // Current value
  myIntegerInput.ptr.i_buffer=&user_input; // Pass the address of the buffer
  myIntegerInput.low.i=0; // Lower limit
  myIntegerInput.high.i=23; // Upper limit
  myIntegerInput.step.i=1; // Step size
  myIntegerInput.col=7; // Display prompt at column 7
  myIntegerInput.row=1; // Display prompt at row 1
  myIntegerInput.width=2; // The number occupies 2 characters space
  myIntegerInput.option=1; // Option 0, space pad right, option 1, zero pad left, option 2, space pad left.
  lcd.clear();
  center_text("Hour"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) rtc[2]=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  // Set minute
  user_input=rtc[1]; // Current value
  myIntegerInput.low.i=0; // Lower limit
  myIntegerInput.high.i=59; // Upper limit
  lcd.clear();
  center_text("Minute"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) rtc[1]=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  // Set second
  user_input=rtc[0]; // Current value
  lcd.clear();
  center_text("Second"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) rtc[0]=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  // Set year
  user_input=rtc[6]; // Current value
  myIntegerInput.ptr.i_buffer=&user_input; // Pass the address of the buffer
  myIntegerInput.low.i=2000; // Lower limit
  myIntegerInput.high.i=2099; // Upper limit
  myIntegerInput.step.i=1; // Step size
  myIntegerInput.col=6; // Display prompt at column 7
  myIntegerInput.row=1; // Display prompt at row 1
  myIntegerInput.width=4; // The number occupies 2 characters space
  myIntegerInput.option=0; // Option 0, space pad right, option 1, zero pad left, option 2, space pad left.
  lcd.clear();
  center_text("Year"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) rtc[6]=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  // Set month
  myListInput.ptr.list=(char**)&month_items; // Assign the list to the pointer
  myListInput.low.i=rtc[5]-1; // Default item highlighted on the list
  myListInput.high.i=11; // Last item of the list is size of the list - 1.
  myListInput.width=3; // Length in characters of the longest list item.
  myListInput.col=0; // Display prompt at column 0
  myListInput.row=1; // Display prompt at row 1
  myListInput.option=1; 
  myListInput.step.c_arr[0]=lcd_rows-1; // rows to auto fit entire screen
  myListInput.step.c_arr[1]=lcd_columns/4; // multi col list
  lcd.clear();
  center_text("Month"); // Prompt user for input
  if (select_list(&myListInput)!=-1) rtc[5]=myListInput.low.i+1; // select_list stores user choice in myListInput.low. If the user didn't press escape (return -1) then update the user choice with the value in myListInput.low.
  else return;

  // Set day
  user_input=rtc[4]; // Current value
  myIntegerInput.low.i=1; // Lower limit
  myIntegerInput.high.i=31; // Upper limit
  lcd.clear();
  center_text("Date"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) rtc[4]=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  RTC.adjust(DateTime(rtc[6],rtc[5],rtc[4],rtc[2],rtc[1],rtc[0]));
}

void top_menu_function_3() //Set alarms
{
  int temp1, ala=0;
  char msg[17];
  int user_input;
  phi_prompt_struct myIntegerInput, myListInput; // This structure stores the main menu.
  lcd.clear();
  sprintf(msg,"Alarm %02d:",0);
  lcd.print(msg);

  user_input=0; // Current value
  myIntegerInput.ptr.i_buffer=&user_input; // Pass the address of the buffer
  myIntegerInput.low.i=0; // Lower limit
  myIntegerInput.high.i=Max_alarms-1; // Upper limit
  myIntegerInput.step.i=1; // Step size
  myIntegerInput.col=7; // Display prompt at column 7
  myIntegerInput.row=1; // Display prompt at row 1
  myIntegerInput.width=2; // The number occupies 2 characters space
  myIntegerInput.option=1; // Option 0, space pad right, option 1, zero pad left, option 2, space pad left.
  lcd.clear();
  center_text("Which alarm?"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) ala=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  user_input=clock1.alarms[ala].hr; // Current value
  myIntegerInput.high.i=23; // Upper limit
  lcd.clear();
  center_text("Hour"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) clock1.alarms[ala].hr=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  user_input=clock1.alarms[ala].mnt; // Current value
  myIntegerInput.high.i=59; // Upper limit
  lcd.clear();
  center_text("Minute"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) clock1.alarms[ala].mnt=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;

  myListInput.ptr.list=(char**)&alarm_items; // Assign the list to the pointer
  myListInput.low.i=clock1.alarms[ala].dow; // Default item highlighted on the list
  myListInput.high.i=4; // Last item of the list is size of the list - 1.
  myListInput.width=7; // Length in characters of the longest list item.
  myListInput.col=0; // Display prompt at column 0
  myListInput.row=1; // Display prompt at row 1
  myListInput.option=1; // Option 0, display classic list, option 1, display 2X2 list, option 2, display list with index, option 3, display list with index2.
  myListInput.step.c_arr[0]=lcd_rows-1; // rows to auto fit entire screen
  myListInput.step.c_arr[1]=lcd_columns/8; // multi col list
  lcd.clear();
  center_text("Type"); // Prompt user for input
  if (select_list(&myListInput)!=-1) clock1.alarms[ala].dow=myListInput.low.i; // select_list stores user choice in myListInput.low. If the user didn't press escape (return -1) then update the user choice with the value in myListInput.low.
  else return;

  clock1.save_alarm(ala); ///< Save the setting of this particular alarm to EEPROM.
}

void top_menu_function_4() // Select clock display style
{
  int user_input;
  phi_prompt_struct myIntegerInput; // This structure stores the main menu.

  user_input=clock_style; // Current value
  myIntegerInput.ptr.i_buffer=&user_input; // Pass the address of the buffer
  myIntegerInput.low.i=0; // Lower limit
  myIntegerInput.high.i=1; // Upper limit
  myIntegerInput.step.i=1; // Step size
  myIntegerInput.col=7; // Display prompt at column 7
  myIntegerInput.row=1; // Display prompt at row 1
  myIntegerInput.width=1; // The number occupies 2 characters space
  myIntegerInput.option=0; // Option 0, space pad right, option 1, zero pad left, option 2, space pad left.
  lcd.clear();
  center_text("Clock style"); // Prompt user for input
  if (input_integer(&myIntegerInput)!=-1) clock_style=user_input; // If the user didn't press escape (return -1) then update the ultimate storage with the value in the buffer.
  else return;
}

void top_menu_function_5() //Show credit
{
  show_credit();
}

