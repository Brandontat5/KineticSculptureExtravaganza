//      ******************************************************************
//      *                                                                *
//      *         DPEA Kinetic Machine - Student Software Version        *
//      *                                                                *
//      *            Stan Reifel                     3/18/2014           *
//      *            Kyle Stewart                    4/21/2017           *
//      *                                                                *
//      ******************************************************************

#define SCULPTURE_LCD_NAME "BRANDON TAT"


//
// UNLESS YOU REALLY KNOW WHAT YOU ARE DOING - DO NOT EDIT THE CODE IN THIS FILE!!!!
// IT IS PRESENTED HERE FOR YOU TO LOOK AT - BUT IS WAY BEYOND THE SCOPE OF THIS CLASS!!!!
//
// IT IS MY HOPE THAT AT SOME POINT (12th grade year) WE WILL BE ABLE TO RETURN TO THIS CODE AND EXAMINE
// AND LEARN SECTIONS OF IT.
//
// UNTIL THEN - IF YOU CHOOSE TO EDIT THIS CODE, MAKE SURE YOU HAVE MADE A BACKUP TO FALL
// BACK ON IF YOU MESS IT UP.
//                          MR. HARLOW

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "ConstantAndDataTypes.h"
#include "Buttons.h"
#include "RtcAndLcd.h"
#include "Backlight.h"
#include "Motors.h"
#include "Ultrasonic.h"
#include "Architecture.h"
#include "Extravaganza.h"
#include "Play.h"

// ---------------------------------------------------------------------------------
//                              Hardware and software setup
// ---------------------------------------------------------------------------------

//
// top level setup function
//
void setup()
{
  byte contrastValue;

  pinMode(TEST_D9_PIN, OUTPUT);            // configure the Test and LED output bits
  pinMode(LED_PIN, OUTPUT);

  //
  // initialize software modules
  //
  LCDInitialise();                          // initialize the LCD hardware and functions
  motorInitialise();                        // initialize the motor hardware and functions
  diskVelocitiesInitialize();               // initialize functions used to transition between disk velocities
  backlightInitialize();                    // initialize the backlight LEDs
  //strobeInitialize();                       // initialize the strobe LED functions
  buttonsInitialize();                      // initialize the buttons hardware and functions
  RTCInitialise();                          // initialize I2C communication with the real time clock
  ultrasonicInitialize();                   // initialize the ultrasonic hardware and functions
  backgroundProcessingInitialize();         // initialize background processing to run every 10ms

  LCDClearDisplay();                        // start with the LCD display blank

  //
  // set the LCD contrast using the value saved in EEPROM, read the value from the EEPROM,
  //  if it has not ever been set choose a default value
  //
  contrastValue = getContrastByteFromEEPROM();
  LCDSetContrast(contrastValue);


  //
  // update the display, including the sculputer's name
  //
  timeDisplayCount = 0;
  LCDPrintCenteredString(SCULPTURE_LCD_NAME, 0);

  //
  // start with the sculpture in the Action mode (NEED TO CHANGE THIS BACK LATER)
  //
  setSculptureMode(actionMode);



  //
  // enable interrupts to start background processing
  //
  sei();


  //
  // flash the LEDs showing it is ready
  //
#if BLINK_AFTER_DOWNLOADING
  backlightSetColor(70, 70, 70);
  delay(50);
  backlightSetColor(0, 0, 0);
#endif
}


// ---------------------------------------------------------------------------------
//                      The Main Loop and Top Level Functions
// ---------------------------------------------------------------------------------

//
// main loop, select the display show based on the User Mode Setting
//
void loop() {
  //
  // execute the mode that the sculpture is set to
  //
  switch (sculptureMode) {

    case actionMode:
      showActionMode();
      stopBetweenModes();
      break;

    case lightMode:
      showLightsMode();
      stopBetweenModes();
      break;

    case playMode:
      showPlayMode();
      stopBetweenModes();
      break;

    case meterStickMode:
      showMeterStickMode();
      break;

    case setContrastMode:
      showSetContrastMode();
      break;

    case setTimeMode:
      showSetTimeMode();
      break;
  }
}




// -------------------------------------- End --------------------------------------


