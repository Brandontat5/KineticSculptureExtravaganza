
//      ******************************************************************
//      *                                                                *
//      *                            Play Mode                           *
//      *                                                                *
//      ******************************************************************

#include "Extravaganza.h"

//
// Play mode uses the distance sensing ultrasonic sensor to allow your Kinetic Sculpture to interact with its audience.
//
// You can program the motor speed and colors that are created based on the distance detected by the ultrasonic sensor.
//
// You can make the light sculpture more reactive by decreasing PERIOD_OF_ULTRASONIC_MEASUREMENTS_MS in Ultrasonic.h,
// or smoother by increasing PERIOD_OF_ULTRASONIC_MEASUREMENTS_MS.
//
// Your Kinetic Sculpture must interact using disc motion and backlight displays.
//
// Play should run whenever it detects something and base its response on how close the detected object is.
//

//
// you may update this to make the transitions more or less responsive
//
const int kPeriodOfUltrasonicMeasurementsInMS = 2000; // update this but make it a multiple of 100 ms

//
// this is the number of cm per index as it transitions, you may update this
//
const int cmPerIndex = 5; 

//
// you may change this if your sculpture has an object closer than 2.5 meters to it
// do not exceed 255 as it is a byte variable
//
const byte kMaxDistanceToUserInCM = 250;

//
// function prototypes
//
void showPlayMode();

// ---------------------------------------------------------------------------------
//                                The Play Mode
// ---------------------------------------------------------------------------------

//
// run the Play mode, return when no longer in this mode
//
void showPlayMode()
{
  while (true)
  {
    //
    // take a measurement to determine distance
    //
    ultrasonicStartMeasurement();
    while (ultrasonicIsFinished() == false)
    {
      //
      // check for button presses...
      //
      executeTasks();

      //
      // return if no longer in this mode
      //
      if (sculptureMode != playMode)
        return;
    }

    //
    // get measurement from ultrasonic
    //
    byte thisDistance = constrain(ultrasonicGetDistanceInCM(), 0, kMaxDistanceToUserInCM); // constrain to a byte

    byte idx = 0;

    //
    // interpolate the correct distance entry (unless the mesaurement is invalid)
    //
    if (thisDistance != 0) idx = thisDistance / cmPerIndex % ExtravaganzaTableLength;

    //
    // get the disk velocities from the table and transition from the current velocities
    //   to new velocities over the given duration period
    //
    diskVelocitiesStartTransition(
      pgm_read_float(&ExtravaganzaTable[idx].discVelocities[front]),
      pgm_read_float(&ExtravaganzaTable[idx].discVelocities[back]),
      kPeriodOfUltrasonicMeasurementsInMS);

    //
    // get the backlight color from the table and transition from the current RGB values
    //   to new values over the given duration period
    //
    backlightStartTransition(
      pgm_read_byte(&ExtravaganzaTable[idx].rgb[red]),
      pgm_read_byte(&ExtravaganzaTable[idx].rgb[green]),
      pgm_read_byte(&ExtravaganzaTable[idx].rgb[blue]),
      kPeriodOfUltrasonicMeasurementsInMS);

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
      if (sculptureMode != playMode)
        return;
    }
  }
}

