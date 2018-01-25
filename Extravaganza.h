#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//                                                RGB Color Definitions                                               //
//                                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum Colour {red, green, blue};

#define rgbPureGreen {0, 255, 0}
#define rgbPinkForAllGenders {188, 42, 167}
#define rgbRazzleDazzle {3, 121, 198}
#define rgbmistyBlue {71, 158, 239}
#define rgbPogchamp {108, 249, 89}
#define rgbMellowYellllllow {255,255,0}


#define rgbSoftBlue {0, 207, 180}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//                                            Motor Velocities Definitions                                            //
//                                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum Disc {front, back};

#define velocitiesOff {0, 0}
#define velocitiesDizzyUp {-10, 10}
#define velocitiesBlaster {15, 20}
#define trippingBalls {20.0, -20.0}
#define moreBallTripping {-20.0, 20.0}
#define Chiaroscuro {250, -250.0}
#define reversedChiaroscuro {-250.0, 250.0}
#define cruuuising {30.0, 15.0}
#define reverseCruuuising {10.0, 20.0}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//                                                 Extravaganza Table                                                 //
//                                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  const byte rgb[3];
  const float discVelocities[2];
  const unsigned int transitionDuration; // length of time in ms to transition to this entry
  const unsigned int postTransitionDuration; // length of time to remain at this entry after transition
} ExtravaganzaEntry;

const ExtravaganzaEntry PROGMEM ExtravaganzaTable[] = {
  {rgbmistyBlue,               Chiaroscuro, 5000, 1500},
  {rgbPureGreen,                cruuuising, 5000, 1500},
  {rgbMellowYellllllow,         cruuuising, 5000, 1500},
  {rgbPogchamp,         reversedChiaroscuro, 2500, 500},
  {rgbMellowYellllllow,         cruuuising, 5000, 1500},
  {rgbPureGreen,                cruuuising, 5000, 1500},
  {rgbmistyBlue,                Chiaroscuro, 5000, 750},
  {rgbPureGreen,         reverseCruuuising, 5000, 1500},
  {rgbPogchamp,         reversedChiaroscuro, 2500, 500}
};

const int ExtravaganzaTableLength = sizeof(ExtravaganzaTable) / sizeof(ExtravaganzaEntry);

//
// function prototypes
//
void showActionMode();
void showLightsMode();


// ---------------------------------------------------------------------------------
//                                The Action Mode
// ---------------------------------------------------------------------------------

//
// run the Action mode, return when no longer in this mode
//
void showActionMode()
{
  int idx = 0;
  int lastTableIdx;

  //
  // run through the table, executing the motions and background color changes
  //
  lastTableIdx = ExtravaganzaTableLength - 1;

  while (true)
  {
    for (idx = 0; idx <= lastTableIdx; idx++)
    {
      //
      // get the disk velocities from the table and transition from the current velocities
      //   to new velocities over the given duration period
      //
      diskVelocitiesStartTransition(
        pgm_read_float(&ExtravaganzaTable[idx].discVelocities[front]),
        pgm_read_float(&ExtravaganzaTable[idx].discVelocities[back]),
        pgm_read_word(&ExtravaganzaTable[idx].transitionDuration));

      //
      // get the backlight color from the table and transition from the current RGB values
      //   to new values over the given duration period
      //
      backlightStartTransition(
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[red]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[green]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[blue]),
        pgm_read_word(&ExtravaganzaTable[idx].transitionDuration));

      //
      // update the LCD display with the table entry number currently being executed
      //
      LCDSetCursorXY(26, 3);
      LCDPrintUnsignedIntWithPadding(idx, 3, ' ');


      //
      // execute other task while waiting for transition to complete
      //
      while (diskVelocitiesTransitionIsFinished() == false)
      {
        //
        // check for button presses...
        //
        executeTasks();

        //
        // return if no longer in this mode
        //
        if (sculptureMode != actionMode)
          return;
      }



      //
      // get the disk velocities from the table and transition from the current velocities
      //   to new velocities over the given duration period
      //
      diskVelocitiesStartTransition(
        pgm_read_float(&ExtravaganzaTable[idx].discVelocities[front]),
        pgm_read_float(&ExtravaganzaTable[idx].discVelocities[back]),
        pgm_read_word(&ExtravaganzaTable[idx].postTransitionDuration));

      //
      // get the backlight color from the table and transition from the current RGB values
      //   to new values over the given duration period
      //
      backlightStartTransition(
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[red]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[green]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[blue]),
        pgm_read_word(&ExtravaganzaTable[idx].postTransitionDuration));

      //
      // update the LCD display with the table entry number currently being executed
      //
      LCDSetCursorXY(26, 3);
      LCDPrintUnsignedIntWithPadding(idx, 3, ' ');


      //
      // execute other task while waiting for transition to complete
      //
      while (diskVelocitiesTransitionIsFinished() == false)
      {
        //
        // check for button presses...
        //
        executeTasks();

        //
        // return if no longer in this mode
        //
        if (sculptureMode != actionMode)
          return;
      }
    }
  }
}


// ---------------------------------------------------------------------------------
//                                The Lights Mode
// ---------------------------------------------------------------------------------

//
// run the Lights mode, return when no longer in this mode
//
void showLightsMode()
{
  int idx = 0;
  int lastTableIdx;

  //
  // run through the table, executing the motions and background color changes
  //
  lastTableIdx = ExtravaganzaTableLength - 1;

  while (true)
  {
    for (idx = 0; idx <= lastTableIdx; idx++)
    {

      diskVelocitiesStartTransition(0, 0, pgm_read_word(&ExtravaganzaTable[idx].transitionDuration));

      //
      // get the backlight color from the table and transition from the current RGB values
      //   to new values over the given duration period
      //
      backlightStartTransition(
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[red]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[green]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[blue]),
        pgm_read_word(&ExtravaganzaTable[idx].transitionDuration));

      //
      // update the LCD display with the table entry number currently being executed
      //
      LCDSetCursorXY(26, 3);
      LCDPrintUnsignedIntWithPadding(idx, 3, ' ');


      //
      // execute other task while waiting for transition to complete
      //
      while (diskVelocitiesTransitionIsFinished() == false)
      {
        //
        // check for button presses...
        //
        executeTasks();

        //
        // return if no longer in this mode
        //
        if (sculptureMode != lightMode)
          return;
      }


      diskVelocitiesStartTransition(0, 0, pgm_read_word(&ExtravaganzaTable[idx].postTransitionDuration));

      //
      // get the backlight color from the table and transition from the current RGB values
      //   to new values over the given duration period
      //
      backlightStartTransition(
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[red]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[green]),
        pgm_read_byte(&ExtravaganzaTable[idx].rgb[blue]),
        pgm_read_word(&ExtravaganzaTable[idx].postTransitionDuration));

      //
      // update the LCD display with the table entry number currently being executed
      //
      LCDSetCursorXY(26, 3);
      LCDPrintUnsignedIntWithPadding(idx, 3, ' ');


      //
      // execute other task while waiting for transition to complete
      //
      while (diskVelocitiesTransitionIsFinished() == false)
      {
        //
        // check for button presses...
        //
        executeTasks();

        //
        // return if no longer in this mode
        //
        if (sculptureMode != lightMode)
          return;
      }
    }
  }
}
