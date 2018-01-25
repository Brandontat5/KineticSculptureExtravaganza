
//      ******************************************************************
//      *                                                                *
//      *                       DPEA Kinetic Sculpture                   *
//      *                   Core Architecture Structure                  *
//      *                                                                *
//      *       Rev 0     Stan Reifel                3/17/2014           *
//      *       Rev 1     Kyle Stewart               4/16/2017           *
//      *                                                                *
//      ******************************************************************

//
// the mode of the machine
//

enum Modes {actionMode, lightMode, playMode, meterStickMode, setContrastMode, setTimeMode, stoppedMode};

//
//  name displayed on the LCD when each mode is selected
//
#define ACTION_LCD_NAME "ACTION"
#define LIGHTS_LCD_NAME "LIGHT SHOW"
#define PLAY_LCD_NAME "PLAY"

//
// global vars
//
byte timeDisplayCount;
int sculptureMode;

//
// main architecture function prototypes
//
void executeTasks();
void setSculptureMode(int mode);
void showSetContrastMode();
void showSetTimeMode();
void showStoppedMode();
void stopBetweenModes();
void backgroundProcessingInitialize();



// ---------------------------------------------------------------------------------
//                             Execute Tasks and Set Mode
// ---------------------------------------------------------------------------------

//
// execute other tasks while waiting for the current motion to complete
//
void executeTasks()
{
  byte event;
  int newSculptureMode;
  
  //
  // check if the Mode button has been press indicating the mode has changed
  //
  event = checkButton(PUSH_BUTTON_MODE);
  if (event == BUTTON_PUSHED)
  { 
    if (sculptureMode == actionMode) sculptureMode = lightMode; // don't transition into lightsMode
    
    newSculptureMode = sculptureMode + 1;
    if (newSculptureMode >= stoppedMode)
      newSculptureMode = playMode;
      
    setSculptureMode(newSculptureMode);
  }

  //
  // check if the Mode button has been press indicating the mode has changed
  //
  event = checkButton(PUSH_BUTTON_UP);
  if (event == BUTTON_PUSHED)
  { 
    newSculptureMode = actionMode;
      
    setSculptureMode(newSculptureMode);
  }

  //
  // check if the Mode button has been press indicating the mode has changed
  //
  event = checkButton(PUSH_BUTTON_DOWN);
  if (event == BUTTON_PUSHED)
  { 
    newSculptureMode = lightMode;
      
    setSculptureMode(newSculptureMode);
  }
  
  //
  // check if the remote A button has been pressed: Action mode
  //
  event = checkButton(REMOTE_BUTTON_A);
  if (event == BUTTON_PUSHED)
     setSculptureMode(actionMode);
  
  
  //
  // check if the remote B button has been pressed: Light Show mode
  //
  event = checkButton(REMOTE_BUTTON_B);
  if (event == BUTTON_PUSHED)
     setSculptureMode(lightMode);
  
  
  //
  // check if the remote C button has been pressed: Play mode
  //
  event = checkButton(REMOTE_BUTTON_C);
  if (event == BUTTON_PUSHED)
    setSculptureMode(playMode);

  //
  //
  event = checkButton(REMOTE_BUTTON_D);
  if (event == BUTTON_PUSHED)
    setSculptureMode(stoppedMode);
    
  //
  // periodically update the time shown on the LCD
  //
  if(timeDisplayCount == 0)
  {
    displayTimeOnLCD(); 
    timeDisplayCount = 20;
  } 
  timeDisplayCount--;  
  
  
  //
  // delay a bit before checking for next event
  //
  delay(20);
}



//
// set the sculpture's mode
//  Enter: mode = stoppedMode, actionMode, DISPLAY_MODE_STROBOCOPIC...
//
void setSculptureMode(int mode)
{
  char *s;

  //
  // check if already in the desired mode
  //
  if (sculptureMode == mode)
    return;
    
  //
  // set the new mode
  //
  sculptureMode = mode;
  
  //
  // display the mode name on the LCD
  //
  switch(mode)
  {
    case actionMode:
      s = ACTION_LCD_NAME;
      break;
      
    case lightMode:
      s = LIGHTS_LCD_NAME;
      break;
      
      
    case playMode:
      s = PLAY_LCD_NAME;
      break;
      
      
    case meterStickMode:
      s = "METER STICK";
      break;
     
    case setContrastMode:
      s = "SET CONTRAST";
      break;
      
    case setTimeMode:
      s = "SET TIME";
      break;
            
    default:
      s = "??????";
      break;
  }
  
  //
  //display the name of the owner on the screen
  //
  LCDPrintCenteredString(SCULPTURE_LCD_NAME, 0);

  //
  // display the mode name on the screen
  //
  LCDPrintCenteredString(s, 2);

  //
  // blank the LCD lines used by the modes
  //
  LCDPrintCenteredString(" ", 3);
  LCDPrintCenteredString(" ", 4);
}


// ---------------------------------------------------------------------------------
//                                The Meter Stick Mode
// ---------------------------------------------------------------------------------

//
// run the Meter Stick mode, return when no longer in this mode
//
void showMeterStickMode()
{
  int distance;
  unsigned long nextEventTime;
  bool waitingForEcho;
  int barGraphLength;
  const int BAR_GRAPH_WIDTH = 84;
  const int MAX_BAR_GRAPH_CM = 300;

  //
  // initialize timer
  //
  nextEventTime =  millis() + 120;
  waitingForEcho = false;

  //
  // loop to run this mode until the mode is changed with a button press
  //
  while(true)
  { 
    //
    // check if time to start an ultrasonic measurement
    //
    if (millis() >= nextEventTime)
    {  
      ultrasonicStartMeasurement();
      nextEventTime =  millis() + 100;
      waitingForEcho = true;
    }

    //
    // check if a new measurement is complete
    //
    if (waitingForEcho)
    {
      if (ultrasonicIsFinished())
      {
        //
        // measurement complete, display results
        //
        distance = ultrasonicGetDistanceInCM();
        LCDSetCursorXY(26, 4);
        LCDPrintUnsignedIntWithPadding(distance, 3, ' ');
        LCDPrintString("CM");
        
        //
        // draw a bar graph showing the distance
        //
        barGraphLength = (distance * BAR_GRAPH_WIDTH) / MAX_BAR_GRAPH_CM;

        if (barGraphLength > BAR_GRAPH_WIDTH) 
          barGraphLength = 84;

        if (barGraphLength < 1) 
          barGraphLength = 1;
        
        LCDDrawRowOfPixels(0, barGraphLength-1, 3, 0x3c);
        
        if (barGraphLength < BAR_GRAPH_WIDTH)
        LCDDrawRowOfPixels(barGraphLength, BAR_GRAPH_WIDTH-1, 3, 0x0);

        waitingForEcho = false;
      }
    }
    
    //
    // check for button presses and update the clock
    // 
    executeTasks();

    //
    // return if no longer in this mode
    //
    if (sculptureMode != meterStickMode)
      return;
  }
}


// ---------------------------------------------------------------------------------
//                             Set the LCD Contrast Mode
// ---------------------------------------------------------------------------------

//
// run the Set LCD Contrast mode, return when no longer in this mode
//
void showSetContrastMode()
{
  byte event;
  int contrastValue;
  unsigned long startTime;

  //
  // initially draw the contrast display
  //
  updateContrastModeDisplay();

  //
  // turn on the LED so we know we're in the contrast setting mode
  //   in the case where the LCD can not been seen
  //
  digitalWrite(LED_PIN, HIGH);
  startTime =  millis();

  //
  // loop to run this mode until the mode is changed with a button press
  //
  while(true)
  { 
    //
    // check for the UP button and adjust the contract value with each press
    //
    event = checkButton(PUSH_BUTTON_UP);
    if ((event == BUTTON_PUSHED) || (event == BUTTON_REPEAT))
    {
      //
      // up button pressed, read the current value from EEPROM and add 1 to it
      //
      contrastValue = getContrastByteFromEEPROM();
      contrastValue++;
      
      if (contrastValue >= 127)
        contrastValue = 127;

      //
      // save the value in EEPROM, update the LCD contrast value and update the bar graph
      //
      EEPROM.write(EEPROM_CONTRAST_BYTE_ADDRESS, contrastValue);
      LCDSetContrast(contrastValue);
      updateContrastModeDisplay();
    }
    
    
    //
    // check for the DOWN button and adjust the contract value with each press
    //
    event = checkButton(PUSH_BUTTON_DOWN);
    if ((event == BUTTON_PUSHED) || (event == BUTTON_REPEAT))
    {
      //
      // down button pressed, read the current value from EEPROM and subtract 1 from it
      //
      contrastValue = getContrastByteFromEEPROM();
      contrastValue--;
      
      if (contrastValue < 0)
        contrastValue = 0;

      //
      // save the value in EEPROM, update the LCD contrast value and update the bar graph
      //
      EEPROM.write(EEPROM_CONTRAST_BYTE_ADDRESS, contrastValue);
      LCDSetContrast(contrastValue);
      updateContrastModeDisplay();
    }   
    
    
    //
    // blink the LED showing that in the "set contrast" mode
    //
    if ((millis() - startTime) < 1500)
      digitalWrite(LED_PIN, HIGH);
    else
      digitalWrite(LED_PIN, LOW);
      
    if ((millis() - startTime) > 3000)
      startTime = millis();
    
    
    //
    // check for button presses and update the clock
    // 
    executeTasks();

    //
    // return if no longer in this mode
    //
    if (sculptureMode != setContrastMode)
    {
      digitalWrite(LED_PIN, LOW);        // turn off LED and return
      return;
    }
  }
}


// ---------------------------------------------------------------------------------
//                                Set The Time Mode
// ---------------------------------------------------------------------------------

//
// run the Set Time mode, return when no longer in this mode
//
void showSetTimeMode()
{
  byte event;
  int timeIncrement;
  int repeatCount;
  
  //
  // display help info on the LCD
  //
  LCDPrintCenteredString("Use Up & Down", 3);
  LCDPrintCenteredString("buttons.", 4);


  //
  // loop to run this mode until the mode is changed with a button press
  //
  while(true)
  { 
    //
    // check for a UP button Pressed event
    //
    event = checkButton(PUSH_BUTTON_UP);
    if (event == BUTTON_PUSHED)
    {
      timeIncrement = 1;
      repeatCount = 0;
      advanceRTCTime(timeIncrement);
    }

    //
    // check for a UP button Repeat event
    //
    if (event == BUTTON_REPEAT)
    {
      repeatCount++;
      if (repeatCount == 15)
        timeIncrement = 5;
      if (repeatCount == 30)
        timeIncrement = 10;
      advanceRTCTime(timeIncrement);     
    }
        
    //
    // check for a DOWN button Pressed event
    //
    event = checkButton(PUSH_BUTTON_DOWN);
    if (event == BUTTON_PUSHED)
    {
      timeIncrement = 1;
      repeatCount = 0;
      reverseRTCTime(timeIncrement);
    }  
    
    //
    // check for a DOWN button Repeat event
    //
    if (event == BUTTON_REPEAT)
    {
      repeatCount++;
      if (repeatCount == 15)
        timeIncrement = 5;
      if (repeatCount == 30)
        timeIncrement = 10;
      reverseRTCTime(timeIncrement);
    }   
    
    
    //
    // check for other buttons presses and update the clock display
    // 
    executeTasks();

    //
    // return if no longer in this mode
    //
    if (sculptureMode != setTimeMode)
      return;
  }
}


// ---------------------------------------------------------------------------------
//                                  The Stopped Mode
// ---------------------------------------------------------------------------------

//
// run the Stopped mode, return when no longer in this mode
//
void showStoppedMode()
{
  //
  // loop to run this mode until the mode is changed with a button press
  //
  while(true)
  { 
    //
    // check for button presses and update the clock
    // 
    executeTasks();

    //
    // return if no longer in this mode
    //
    if (sculptureMode != stoppedMode)
      return;
  }
}


// ---------------------------------------------------------------------------------
//                               Mode Support Functions
// ---------------------------------------------------------------------------------

//
// transition the disks to a stopped discVelocities and the backlight to off
//
void stopBetweenModes()
{
  diskVelocitiesStartTransition(0, 0, 500);
  backlightStartTransition(0, 0, 0, 500);
  
  while(diskVelocitiesTransitionIsFinished() == false)
    executeTasks();
    
  motorZeroIntegralTerms();
}




// ---------------------------------------------------------------------------------
//              Background Processing - Timer 3 Interrupt Service Routine 
// ---------------------------------------------------------------------------------

//
// initialize background processing
//
void backgroundProcessingInitialize()
{
  //
  // configure Timer3 to interrupt every 10 ms
  //
  cli();                               // disable interrupts during configuration
  TCCR3A = 0;                          // clear all bits in Timer/Counter Control Reg A
  TCCR3B = 0;                          // clear all bits in Timer/Counter Control Reg B
  TCCR3B |= (1 << WGM32);              // enable CTC mode (Clear Timer on Compare Match)
  TCCR3B |= ((1 << CS30) | (1 << CS31));  // set the prescaler to divide by 64 giving a 250Khz count rate
  TCNT3  = 0;                          // initialize the counter to 0
  OCR3A = 2499;                        // set compare register to count to 2500-1, giving a 100Hz interrupt
  TIMSK3 |= (1 << OCIE3A);             // enable the interrupt when compare register matches
}



//
// the background process called every 10ms (interrupt service routine for Timer3)
//
ISR(TIMER3_COMPA_vect)
{
  //
  // enable global interrupts so faster interrupts can happen while processing background tasks
  //
  sei(); 

//digitalWrite(TEST_D9_PIN, HIGH);


  //
  // update the backlight LEDs as they transition from one color to the next
  //
  backlightTransition();
   
  //
  // update the desired disk velocities as they transition from one speed to the next
  //
  diskVelocitiesTransition();
  
  //
  // undate the motor servos 
  //
  motorProportionalIntegralControl1();
  motorProportionalIntegralControl2();
 
  
//digitalWrite(TEST_D9_PIN, LOW);
}



