
//      ******************************************************************
//      *                                                                *
//      *                 Push Buttons Hardware Control                  *
//      *                                                                *
//      ******************************************************************


//
// function prototypes
//
void buttonsInitialize(void);
byte checkButton(byte buttonIdx);

// ---------------------------------------------------------------------------------
//                                Push Button Functions
// ---------------------------------------------------------------------------------

//
// types of button events
//
const int BUTTON_NO_EVENT = 0;
const int BUTTON_PUSHED = 1;
const int BUTTON_RELEASED = 2;
const int BUTTON_REPEAT = 3;


//
// values for the button numbers (also indexes into ButtonsTable)
//
const byte PUSH_BUTTON_UP = 0;
const byte PUSH_BUTTON_MODE = 1;
const byte PUSH_BUTTON_DOWN = 2;

const byte REMOTE_BUTTON_A = 3;
const byte REMOTE_BUTTON_B = 4;
const byte REMOTE_BUTTON_C = 5;
const byte REMOTE_BUTTON_D = 6;

const byte BUTTON_TABLE_LAST_ENTRY = 6;



//
// values for State in ButtonsTable
//
const int WAITING_FOR_BUTTON_DOWN = 0;
const int DEBOUNCE_AFTER_BUTTON_DOWN = 1;
const int WAITING_FOR_BUTTON_UP = 2;
const int WAITING_FOR_BUTTON_UP_AFTER_AUTO_REPEAT = 3;
const int DEBOUNCE_AFTER_BUTTON_UP = 4;


//
// delay periods for dealing with buttons in milliseconds
//
const long BUTTON_DEBOUNCE_PERIOD = 30;
const long BUTTON_AUTO_REPEAT_DELAY = 800;
const long BUTTON_AUTO_REPEAT_RATE = 130;


//
// structure for storing the state of a push button
//
typedef struct _BUTTON_TABLE_ENTRY
{
  byte ButtonPinNumber;
  byte State;
  byte ButtonActiveLowFlg;
  unsigned long EventStartTime;
} BUTTON_TABLE_ENTRY;

static BUTTON_TABLE_ENTRY ButtonsTable[BUTTON_TABLE_LAST_ENTRY + 1];


/* ------------------------------------------------------------------------ */

//
// initialize the buttons
//
void buttonsInitialize(void)
{
  BUTTON_TABLE_ENTRY buttonTableEntry;
  
  
  //
  // setup the IO pins
  //
  pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  pinMode(REMOTE_BUTTON_A_PIN, INPUT);
  pinMode(REMOTE_BUTTON_B_PIN, INPUT);
  pinMode(REMOTE_BUTTON_C_PIN, INPUT);
  pinMode(REMOTE_BUTTON_D_PIN, INPUT);


  //
  // initialize the button table for each push button
  //
  buttonTableEntry.State = WAITING_FOR_BUTTON_DOWN;
  buttonTableEntry.ButtonActiveLowFlg = true;

  buttonTableEntry.ButtonPinNumber = BUTTON_UP_PIN;
  ButtonsTable[PUSH_BUTTON_UP] = buttonTableEntry;

  buttonTableEntry.ButtonPinNumber = BUTTON_MODE_PIN;
  ButtonsTable[PUSH_BUTTON_MODE] = buttonTableEntry;

  buttonTableEntry.ButtonPinNumber = BUTTON_DOWN_PIN;
  ButtonsTable[PUSH_BUTTON_DOWN] = buttonTableEntry;

  //
  // initialize the button table for each remote button
  //
  buttonTableEntry.ButtonActiveLowFlg = false;

  buttonTableEntry.ButtonPinNumber = REMOTE_BUTTON_A_PIN;
  ButtonsTable[REMOTE_BUTTON_A] = buttonTableEntry;

  buttonTableEntry.ButtonPinNumber = REMOTE_BUTTON_B_PIN;
  ButtonsTable[REMOTE_BUTTON_B] = buttonTableEntry;

  buttonTableEntry.ButtonPinNumber = REMOTE_BUTTON_C_PIN;
  ButtonsTable[REMOTE_BUTTON_C] = buttonTableEntry;

  buttonTableEntry.ButtonPinNumber = REMOTE_BUTTON_D_PIN;
  ButtonsTable[REMOTE_BUTTON_D] = buttonTableEntry;
}



//
// check for an event from a push button
//		Enter:	buttonIdx = index of which button to test
//		Exit:	event value returned, BUTTON_NO_EVENT returned if no event
//
byte checkButton(byte buttonIdx)
{
  byte buttonValue;
  unsigned long currentTime;
  
  //
  // read the button, invert polarity if set
  //
  buttonValue = digitalRead(ButtonsTable[buttonIdx].ButtonPinNumber);
  if (ButtonsTable[buttonIdx].ButtonActiveLowFlg)
    buttonValue = !buttonValue;

  //
  // check if nothing is going on (waiting for button to go down and it's not pressed)
  //
  if ((ButtonsTable[buttonIdx].State == WAITING_FOR_BUTTON_DOWN) && (buttonValue == false))
    return(BUTTON_NO_EVENT);                      // return no new event
  
  //
  // something is going on, read the time
  //
  currentTime = millis();		
  
  //
  // check the state that the button was in last
  //
  switch(ButtonsTable[buttonIdx].State)
  {
    //
    // check if waiting for button to go down
    //
    case WAITING_FOR_BUTTON_DOWN:
    {
      if (buttonValue == true)                   // check if button is now down
      {                                          // button is down, start timer
        ButtonsTable[buttonIdx].EventStartTime = currentTime;
        ButtonsTable[buttonIdx].State = DEBOUNCE_AFTER_BUTTON_DOWN;
        return(BUTTON_PUSHED);                  // return button "pressed" event
      }
      break;
    }	
    
    //
    // check if waiting for timer after button has gone down
    //		
    case DEBOUNCE_AFTER_BUTTON_DOWN:
    {                                           // check if it has been up long enough
      if (currentTime >= (ButtonsTable[buttonIdx].EventStartTime + BUTTON_DEBOUNCE_PERIOD))
      {                                         // debouncing period over, start auto repeat timer
        ButtonsTable[buttonIdx].EventStartTime = currentTime;
        ButtonsTable[buttonIdx].State = WAITING_FOR_BUTTON_UP;
      }
      break;
    }

    //
    // check if waiting for button to go back up
    //			
    case WAITING_FOR_BUTTON_UP:
    {
      if (buttonValue == false)                 // check if button is now up
      {                                         // button up, start debounce timer
        ButtonsTable[buttonIdx].EventStartTime = currentTime;
        ButtonsTable[buttonIdx].State = DEBOUNCE_AFTER_BUTTON_UP;
        return(BUTTON_RELEASED);                // return button "released" event
      }
                                                // button still down, check if time to auto repeat
      if (currentTime >= (ButtonsTable[buttonIdx].EventStartTime + BUTTON_AUTO_REPEAT_DELAY))
      {                                         // reset auto repeat timer
        ButtonsTable[buttonIdx].EventStartTime = currentTime;
        ButtonsTable[buttonIdx].State = WAITING_FOR_BUTTON_UP_AFTER_AUTO_REPEAT;
        return(BUTTON_REPEAT);                  // return button "auto repeat" event
      }
      break;
    }

    //
    // check if waiting for button to repeat
    //
    case WAITING_FOR_BUTTON_UP_AFTER_AUTO_REPEAT:
    {
      if (buttonValue == false)                 // check if button is now up
      {                                         // button up, start debounce timer
        ButtonsTable[buttonIdx].EventStartTime = currentTime;
        ButtonsTable[buttonIdx].State = DEBOUNCE_AFTER_BUTTON_UP;
        return(BUTTON_RELEASED);                // return button "released" event
      }
                                                // button still down, check if time to auto repeat
      if (currentTime >= (ButtonsTable[buttonIdx].EventStartTime + BUTTON_AUTO_REPEAT_RATE))
      {                                         // reset auto repeat timer
        ButtonsTable[buttonIdx].EventStartTime = currentTime;
        return(BUTTON_REPEAT);                  // return button "auto repeat" event
      }
      break;
    }			
    
    //
    // check if waiting for debouncing after button has gone up
    //
    case DEBOUNCE_AFTER_BUTTON_UP:
    {                                            // after button goes up, wait before check for down
      if (currentTime >= (ButtonsTable[buttonIdx].EventStartTime + BUTTON_DEBOUNCE_PERIOD))
        ButtonsTable[buttonIdx].State = WAITING_FOR_BUTTON_DOWN;
      break;
    }
  }
  
  return(BUTTON_NO_EVENT);                      // return no new event
}

