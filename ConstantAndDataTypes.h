
//      ******************************************************************
//      *                                                                *
//      *                       DPEA Kinetic Sculpture                   *
//      *                      Constants and Data Types                  *
//      *                                                                *
//      *       Rev 0     Stan Reifel                3/17/2014           *
//      *       Rev 1     Kyle Stewart               4/16/2014           *
//      *                                                                *
//      ******************************************************************
#ifndef CONSTANTS_H
#define CONSTANTS_H

//
// THERE ARE REALLY ONLY TWO THINGS THAT SHOULD BE CHANGED IN THIS FILE.
//
//  1. TURNING ON AND OFF THE BACKLIGHT BLINK - DESCRIBED BELOW.
//
//  2. ENTER YOUR FIRST AND LAST NAME IN THE SPACE BELOW - SO THAT YOUR SCULPTURE DISPLAYS YOUR NAME ON THE TOP OF THE LCD
//
//  3. MODIFYING AND OR ADDING TO THE COLOR CONSTANTS DEFINED BELOW.
//
//  IT IS ADVISED YOU LEAVE EVERYTHING ELSE AS IT IS. 
//
//                          MR. HARLOW


//
// Setting this constant to "true" will cause the backlight to blink once after a program
// is downloaded.  This is very useful when developing.  Once developement is complete,
// the constant should be changed to "false".  (It must be set to true or false using 
// lower case without quotes)
//
#define BLINK_AFTER_DOWNLOADING true


// Use this space to put your First Last Name on the TOP of the LCD of your Sculpture.
// You MUST enter the letters in the format given.
//
// IF your Last Name is really long - Put your First Inital and Last Name.
//
// You have a MAXIMUM of 14 Letters
//



//
//  ALL YOUR CUSTOM COLOR CONSTANTS SHOULD BE ENTERED ABOVE.
//
//  IT IS ADVISED YOU LEAVE ALL THE CODE BELOW THIS LINE AS IT IS. 
//
//                          MR. HARLOW
//
//
// motor pin assignments
//
const int MOTOR_DIRECTION_1_PIN = 15;
const int MOTOR_PWM_1_PIN = 11;
const int MOTOR_TACHOMETER_1_PIN = 19;

const int MOTOR_DIRECTION_2_PIN = 14;
const int MOTOR_PWM_2_PIN = 12;
const int MOTOR_TACHOMETER_2_PIN = 18;


//
// backlight LEDs pin assignments
//
const int BACKLIGHT_RED_PIN = 6;                // port H, bit 3
const int BACKLIGHT_BLUE_PIN = 7;               // port H, bit 4
const int BACKLIGHT_GREEN_PIN = 8;              // port H, bit 5


//
// LCD display pin assignments, NOTE: THESE VALUES CAN NOT BE CHANGED AS PORTS ARE HARD CODED IN THE SOFTWARE
//
const int LCD_CLOCK_PIN = 54;                   // port F, bit 0
const int LCD_DATA_IN_PIN = 55;                 // port F, bit 1
const int LCD_DATA_CONTROL_PIN = 56;            // port F, bit 2
const int LCD_CHIP_ENABLE_PIN = 57;             // port F, bit 3
const int LCD_RESET_PIN = 58;                   // port F, bit 4


//
// ultrasonics pin assignments
//
const int ULTRASONIC_TRIGGER_PIN = 4;
const int ULTRASONIC_ECHO_PIN = 3;


//
// miscellaneous pin assignments
//
const int LCD_BACKLIGHT_PIN = 10;
const int TEST_D9_PIN = 9;
const int LED_PIN = 13;

//
// buttons and remote pin assignments
//
const int BUTTON_UP_PIN = 61;
const int BUTTON_MODE_PIN = 60;
const int BUTTON_DOWN_PIN = 59;

const int REMOTE_BUTTON_A_PIN = 63;
const int REMOTE_BUTTON_B_PIN = 65;
const int REMOTE_BUTTON_C_PIN = 62;
const int REMOTE_BUTTON_D_PIN = 64;


//
// EERROM storage locations
//
const int EEPROM_CONTRAST_BYTE_ADDRESS = 0;

//
// motion constants constants
//
const float GEAR_REDUCTION_TO_FINAL_STAGE = 127;
const long LINES_ON_TACHOMETER_DISK = 25;
const float MINIMUM_VELOCITY_IN_RPM = 0.9;


//
// servo control constants
//
const int MOTOR_KP_PROP_CONTROL = 5;
const int MOTOR_KINT_SPEED_ERROR_MAX = 2000;
const int MOTOR_MAX_PWM_FROM_INTEGRAL_TERM = 125;
const long MOTOR_KI_PROP_INT_CONTROL = MOTOR_KINT_SPEED_ERROR_MAX / MOTOR_MAX_PWM_FROM_INTEGRAL_TERM;
const int MOTOR_MAX_PWM = 220;

//
// table normalize for eye nonlinearity with the function: x ^ 1.8
//
const int LEDTable[] PROGMEM = {
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 
2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 
6, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 
13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 21, 
21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 
32, 32, 33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 
44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 
58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 
74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 86, 87, 88, 89, 90, 
91, 92, 93, 95, 96, 97, 98, 99, 100, 102, 103, 104, 105, 107, 108, 109, 
110, 111, 113, 114, 115, 116, 118, 119, 120, 122, 123, 124, 126, 127, 128, 129, 
131, 132, 134, 135, 136, 138, 139, 140, 142, 143, 145, 146, 147, 149, 150, 152, 
153, 154, 156, 157, 159, 160, 162, 163, 165, 166, 168, 169, 171, 172, 174, 175, 
177, 178, 180, 181, 183, 184, 186, 188, 189, 191, 192, 194, 195, 197, 199, 200, 
202, 204, 205, 207, 208, 210, 212, 213, 215, 217, 218, 220, 222, 224, 225, 227, 
229, 230, 232, 234, 236, 237, 239, 241, 243, 244, 246, 248, 250, 251, 253, 255
};

const int LEDTableLength = sizeof(LEDTable) / sizeof(int);

#endif
