
//      ******************************************************************
//      *                                                                *
//      *                LED Backlight Hardware Control                  *
//      *                                                                *
//      ******************************************************************

typedef struct {
  const byte red;
  const byte green;
  const byte blue;
} COLOR_ENTRY;

//
// function prototypes
//
void backlightInitialize(void);
void backlightStartTransitionRGBColor(COLOR_ENTRY color, unsigned long transitionDurationMS);
void backlightStartTransition(byte red, byte green, byte blue, unsigned long transitionDurationMS);
bool backlightTransitionIsFinished();
void backlightTransition();
void backlightSetColor(byte red, byte green, byte blue);

// ---------------------------------------------------------------------------------
//                           Background Lighting Functions
// ---------------------------------------------------------------------------------


//
// global variables by the backlight
//
byte backlightPrevRed;
byte backlightNextRed;
float backlightSlopoeRed;

byte backlightPrevGreen;
byte backlightNextGreen;
float backlightSlopoeGreen;

byte backlightPrevBlue;
byte backlightNextBlue;
float backlightSlopoeBlue;

unsigned long backlighttransitionDurationMS;
unsigned long backlightTransitionStartTimeMS;
unsigned long backlightTransitionFinishTimeMS;
bool backlightTransitionCompleteFlg;


// ---------------------------------------------------------------------------------

//
// initialize the backlight LEDs
//
void backlightInitialize(void)
{
  //
  // setup the IO pins
  //
  pinMode(BACKLIGHT_RED_PIN, OUTPUT);
  pinMode(BACKLIGHT_GREEN_PIN, OUTPUT);
  pinMode(BACKLIGHT_BLUE_PIN, OUTPUT);

  //
  // start with all LEDs off
  //
  backlightSetColor(0, 0, 0);

  backlightTransitionCompleteFlg = true;
}



//
// start a transition of the backlight from current color to a new one over a period of time
//  Enter:  color = rgb color 
//          transitionDurationMS = number of milliseconds for the transition (1 - 60000)
//
void backlightStartTransitionRGBColor(COLOR_ENTRY color, unsigned long transitionDurationMS)
{
  backlightStartTransition(color.red, color.green, color.blue, transitionDurationMS);
}



//
// start a transition of the backlight from current color to a new one over a period of time
//  Enter:  red = new value for red LED brightness (0 - 255)
//          green = new value for green LED brightness (0 - 255)
//          blue = new value for blue LED brightness (0 - 255)
//          transitionDurationMS = number of milliseconds for the transition (1 - 60000)
//
void backlightStartTransition(byte red, byte green, byte blue, unsigned long transitionDurationMS)
{
  unsigned long startTimeMS;
  unsigned long finishTimeMS;

  backlightTransitionCompleteFlg = true;

  startTimeMS = millis();

  finishTimeMS = startTimeMS + transitionDurationMS;
  if (finishTimeMS < startTimeMS)
    finishTimeMS = startTimeMS;

  if (transitionDurationMS != 0)
  {

    backlightPrevRed = backlightNextRed;
    backlightNextRed = red;
    backlightSlopoeRed = (float) (red - backlightPrevRed) / transitionDurationMS;

    backlightPrevGreen = backlightNextGreen;
    backlightNextGreen = green;
    backlightSlopoeGreen = (float) (green - backlightPrevGreen) / transitionDurationMS;

    backlightPrevBlue = backlightNextBlue;
    backlightNextBlue = blue;
    backlightSlopoeBlue = (float) (blue - backlightPrevBlue) / transitionDurationMS;


    cli();
    backlighttransitionDurationMS = transitionDurationMS;
    backlightTransitionStartTimeMS = startTimeMS;
    backlightTransitionFinishTimeMS = finishTimeMS;
    sei();

    backlightTransitionCompleteFlg = false;
  }
}



//
// check if the backlight transistion has finished
//  Exit:  true returned if finished, false returned if not finished
//
bool backlightTransitionIsFinished()
{
  return(backlightTransitionCompleteFlg);
}



//
// transition the backlight, this must be called every 20ms or so
//
void backlightTransition()
{
  unsigned long currentTime;
  unsigned long elapsedTime;
  byte red;
  byte green;
  byte blue;

  //
  // check if doing a transition, if not return
  //
  if (backlightTransitionCompleteFlg)
    return;

  //
  // doing a transition, now check if the period has ended
  //
  currentTime = millis();
  elapsedTime = currentTime - backlightTransitionStartTimeMS;
  if (currentTime >= backlightTransitionFinishTimeMS)
  {
    //
    // transition has finished, set the final colors
    //
    backlightSetColor(backlightNextRed, backlightNextGreen, backlightNextBlue);
    backlightTransitionCompleteFlg = true;

    return;
  }


  //
  // determine the colors for this point in time during the transition
  //
  red = backlightPrevRed + (byte) (backlightSlopoeRed * elapsedTime);

  green = backlightPrevGreen + (byte) (backlightSlopoeGreen * elapsedTime);

  blue = backlightPrevBlue + (byte) (backlightSlopoeBlue * elapsedTime);

  backlightSetColor(red, green, blue);
}



//
// set backlight color and brightness, normalizing to make fading more linear to the human eye
//  Enter:  red = red LED brightness (0 - 255)
//          green = green LED brightness (0 - 255)
//          blue = blue LED brightness (0 - 255)
//
void backlightSetColor(byte red, byte green, byte blue)
{
  analogWrite(BACKLIGHT_RED_PIN, pgm_read_word(&LEDTable[red]));

  analogWrite(BACKLIGHT_GREEN_PIN, pgm_read_word(&LEDTable[green]));

  analogWrite(BACKLIGHT_BLUE_PIN, pgm_read_word(&LEDTable[blue]));
}

