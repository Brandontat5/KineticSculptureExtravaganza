
//      ******************************************************************
//      *                                                                *
//      *                  LCD and RTC Hardware Control                  *
//      *                                                                *
//      ******************************************************************

#include <Arduino.h>
//
// Real Time Clock function prototypes
//
void advanceRTCTime(int minutesForward);
void reverseRTCTime(int minutesBackward);
void RTCInitialise();
void RTCSetTimeAndDate(byte year, byte month, byte dayOfMonth, byte dayOfWeek, byte hour, byte minute, byte second);
void RTCGetTime(byte *hour, byte *minute, byte *second);
void RTCGetTimeAndDate(byte *year, byte *month, byte *dayOfMonth, byte *dayOfWeek, byte *hour, byte *minute, byte *second);
byte RTC_DecToBCD(byte val);
byte RTC_BCDToDec(byte val);


//
// Liquid Crystal Display function prototypes
//

void updateContrastModeDisplay();
byte getContrastByteFromEEPROM();
void LCDInitialise();
void LCDPrintInt(int n);
void LCDPrintUnsignedInt(unsigned int n);
void LCDPrintUnsignedIntWithPadding(unsigned int n, byte widthToPrint, char padCharacter);
void LCDPrintCenteredString(char *s, int lineNumber);
void LCDPrintString(char *s);
void LCDPrintCharacter(byte character);
void LCDClearDisplay();
void LCDDrawRowOfPixels(int X1, int X2, int lineNumber, byte byteOfPixels);
void LCDSetCursorXY(int column, int lineNumber);
void LCDSetContrast(int contrastValue);
void LCDWriteCommand(byte command);
void LCDWriteData(byte data);
void displayTimeOnLCD();



//      ******************************************************************
//      *                                                                *
//      *               Real Time Clock Hardware Control                 *
//      *                                                                *
//      ******************************************************************


//
// advance the RTC time by n minutes
//
void advanceRTCTime(int minutesForward)
{
  byte second;
  byte minute;
  byte hour;

  //
  // read the current time
  //
  RTCGetTime(&hour, &minute, &second);
  
  //
  // advance the time
  //
  minute += minutesForward;
  minute = minute - (minute % minutesForward);
  
  if (minute >= 60)
  {
    minute = 0;
    hour++;
    
    if (hour >= 24)
      hour = 0;
  }

  //
  // set the new time
  //
  RTCSetTimeAndDate(2013, 1, 1, 1, hour, minute, 0);

  //
  // update the display
  //
  displayTimeOnLCD();
}



//
// reverse the RTC time by n minutes
//
void reverseRTCTime(int minutesBackward)
{
  byte second;
  byte minute;
  byte hour;
  int hours;
  int minutes;

  //
  // read the current time
  //
  RTCGetTime(&hour, &minute, &second);
  hours = hour;
  minutes = minute;
  
  //
  // reverse the time
  //
  minutes = minutes - (minutes % minutesBackward);
  if (minutes == minute)
    minutes -= minutesBackward;
  
  if (minutes < 0)
  {
    minutes += 60;
    hours--;
    
    if (hours <= 0)
      hours = 23;
  }
  hour = hours;
  minute = minutes;

  //
  // set the new time
  //
  RTCSetTimeAndDate(2013, 1, 1, 1, hour, minute, 0);

  //
  // update the display
  //
  displayTimeOnLCD();
}




// ---------------------------------------------------------------------------------
//                             Real Time Clock Functions
// ---------------------------------------------------------------------------------

//
// constants for the RTC
//
#define DS1307_I2C_ADDRESS 0x68


// ---------------------------------------------------------------------------------

//
// initialize the motors
//
void RTCInitialise()
{
  //
  // Initialize communication with the Real Time Clock
  //
  Wire.begin();
}


//
// set the real time clock's time and date
//
void RTCSetTimeAndDate(byte year, byte month, byte dayOfMonth, byte dayOfWeek, byte hour, byte minute, byte second)
{ 
  Wire.beginTransmission(DS1307_I2C_ADDRESS);  // open I2C line in write mode
 
  Wire.write((byte)0x00);                      // set the register pointer to (0x00)
  Wire.write(RTC_DecToBCD(second));            // write seven bytes
  Wire.write(RTC_DecToBCD(minute));
  Wire.write(RTC_DecToBCD(hour));      
  Wire.write(RTC_DecToBCD(dayOfWeek));
  Wire.write(RTC_DecToBCD(dayOfMonth));
  Wire.write(RTC_DecToBCD(month));
  Wire.write(RTC_DecToBCD(year));
  
  Wire.endTransmission();                      // end write mode
}



//
// get the real time clock's time (without the date)
//
void RTCGetTime(byte *hour, byte *minute, byte *second)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);   // open I2C line in write mode
  Wire.write((byte)0x00);                       // set the register pointer to (0x00)
  Wire.endTransmission();                       // end write rransmission 
 
  Wire.requestFrom(DS1307_I2C_ADDRESS, 3);      // open the I2C line in send mode
 
  *second     = RTC_BCDToDec(Wire.read() & 0x7f); // read 3 bytes of data
  *minute     = RTC_BCDToDec(Wire.read());
  *hour       = RTC_BCDToDec(Wire.read() & 0x3f);  
}



//
// get the real time clock's time and date
//
void RTCGetTimeAndDate(byte *year, byte *month, byte *dayOfMonth, byte *dayOfWeek, byte *hour, byte *minute, byte *second)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);   // open I2C line in write mode
  Wire.write((byte)0x00);                       // set the register pointer to (0x00)
  Wire.endTransmission();                       // end write rransmission 
 
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);      // open the I2C line in send mode
 
  *second     = RTC_BCDToDec(Wire.read() & 0x7f); // read seven bytes of data
  *minute     = RTC_BCDToDec(Wire.read());
  *hour       = RTC_BCDToDec(Wire.read() & 0x3f);  
  *dayOfWeek  = RTC_BCDToDec(Wire.read());
  *dayOfMonth = RTC_BCDToDec(Wire.read());
  *month      = RTC_BCDToDec(Wire.read());
  *year       = RTC_BCDToDec(Wire.read());
}


//
// Convert normal decimal numbers to binary coded decimal
//
byte RTC_DecToBCD(byte val)
{
  return ((val/10*16) + (val%10));
}
 
 
// 
// Convert binary coded decimal to normal decimal numbers
//
byte RTC_BCDToDec(byte val)
{
  return ((val/16*10) + (val%16));
}


//      ******************************************************************
//      *                                                                *
//      *            Liquid Crystal Display Hardware Control             *
//      *                                                                *
//      *****************************************************************

// ---------------------------------------------------------------------------------
//                             Set the LCD Contrast Mode
// ---------------------------------------------------------------------------------

//
// update the display showing the contrast value and bar graph
//
void updateContrastModeDisplay()
{
  int barGraphLength;
  int contrastValue;
  const int BAR_GRAPH_WIDTH = 84;
  const int MAX_BAR_GRAPH_CM = 300;
 
  //
  // get the constrast value stored in EEPROM
  //
  contrastValue = getContrastByteFromEEPROM();

  //
  // draw a bar graph showing the value
  //
  barGraphLength = (contrastValue * BAR_GRAPH_WIDTH) / 128;
  
  if (barGraphLength > BAR_GRAPH_WIDTH) 
    barGraphLength = 84;
  
  if (barGraphLength < 1) 
    barGraphLength = 1;
  
  LCDDrawRowOfPixels(0, barGraphLength-1, 3, 0x3c);
  
  if (barGraphLength < BAR_GRAPH_WIDTH)
  LCDDrawRowOfPixels(barGraphLength, BAR_GRAPH_WIDTH-1, 3, 0x0);

  //
  // display the contract as a number
  //
  LCDSetCursorXY(33, 4);
  LCDPrintUnsignedIntWithPadding(contrastValue, 3, ' ');
}


//
// get the contrast byte stored in EEPROM
//  Exit:  contrast byte returned
//
byte getContrastByteFromEEPROM()
{
  byte contrastValue;
  
  //
  // get the value saved in EEPROM
  //
  contrastValue = EEPROM.read(EEPROM_CONTRAST_BYTE_ADDRESS);
  
  //
  // if the value has never been set, use the default value
  //
  if (contrastValue == 0xff)
    contrastValue = 59;
    
  return(contrastValue);
}

// ---------------------------------------------------------------------------------
//                                   LCD Functions
// ---------------------------------------------------------------------------------


//
// constants for the LCD display
//
const int LCD_COMMAND_BYTE = LOW;
const int LCD_DATA_BYTE = HIGH;
const int LCD_NUMBER_PIXELS_ACROSS = 84;
const int LCD_NUMBER_PIXELS_DOWN = 48;


//
// ASCII font, 5 x 8 pixels, this font is stored in program memory rather than RAM
//

const int Font[][5] PROGMEM = 
  {
   {0x00, 0x00, 0x00, 0x00, 0x00},     // 20  
   {0x00, 0x00, 0x5f, 0x00, 0x00},     // 21 !
   {0x00, 0x07, 0x00, 0x07, 0x00},     // 22 "
   {0x14, 0x7f, 0x14, 0x7f, 0x14},     // 23 #
   {0x24, 0x2a, 0x7f, 0x2a, 0x12},     // 24 $
   {0x23, 0x13, 0x08, 0x64, 0x62},     // 25 %
   {0x36, 0x49, 0x55, 0x22, 0x50},     // 26 &
   {0x00, 0x05, 0x03, 0x00, 0x00},     // 27 '
   {0x00, 0x1c, 0x22, 0x41, 0x00},     // 28 (
   {0x00, 0x41, 0x22, 0x1c, 0x00},     // 29 )
   {0x14, 0x08, 0x3e, 0x08, 0x14},     // 2a *
   {0x08, 0x08, 0x3e, 0x08, 0x08},     // 2b +
   {0x00, 0x50, 0x30, 0x00, 0x00},     // 2c ,
   {0x08, 0x08, 0x08, 0x08, 0x08},     // 2d -
   {0x00, 0x60, 0x60, 0x00, 0x00},     // 2e .
   {0x20, 0x10, 0x08, 0x04, 0x02},     // 2f /
   {0x3e, 0x51, 0x49, 0x45, 0x3e},     // 30 0
   {0x00, 0x42, 0x7f, 0x40, 0x00},     // 31 1
   {0x42, 0x61, 0x51, 0x49, 0x46},     // 32 2
   {0x21, 0x41, 0x45, 0x4b, 0x31},     // 33 3
   {0x18, 0x14, 0x12, 0x7f, 0x10},     // 34 4
   {0x27, 0x45, 0x45, 0x45, 0x39},     // 35 5
   {0x3c, 0x4a, 0x49, 0x49, 0x30},     // 36 6
   {0x01, 0x71, 0x09, 0x05, 0x03},     // 37 7
   {0x36, 0x49, 0x49, 0x49, 0x36},     // 38 8
   {0x06, 0x49, 0x49, 0x29, 0x1e},     // 39 9
   {0x00, 0x36, 0x36, 0x00, 0x00},     // 3a :
   {0x00, 0x56, 0x36, 0x00, 0x00},     // 3b ;
   {0x08, 0x14, 0x22, 0x41, 0x00},     // 3c <
   {0x14, 0x14, 0x14, 0x14, 0x14},     // 3d =
   {0x00, 0x41, 0x22, 0x14, 0x08},     // 3e >
   {0x02, 0x01, 0x51, 0x09, 0x06},     // 3f ?
   {0x32, 0x49, 0x79, 0x41, 0x3e},     // 40 @
   {0x7e, 0x11, 0x11, 0x11, 0x7e},     // 41 A
   {0x7f, 0x49, 0x49, 0x49, 0x36},     // 42 B
   {0x3e, 0x41, 0x41, 0x41, 0x22},     // 43 C
   {0x7f, 0x41, 0x41, 0x22, 0x1c},     // 44 D
   {0x7f, 0x49, 0x49, 0x49, 0x41},     // 45 E
   {0x7f, 0x09, 0x09, 0x09, 0x01},     // 46 F
   {0x3e, 0x41, 0x49, 0x49, 0x7a},     // 47 G
   {0x7f, 0x08, 0x08, 0x08, 0x7f},     // 48 H
   {0x00, 0x41, 0x7f, 0x41, 0x00},     // 49 I
   {0x20, 0x40, 0x41, 0x3f, 0x01},     // 4a J
   {0x7f, 0x08, 0x14, 0x22, 0x41},     // 4b K
   {0x7f, 0x40, 0x40, 0x40, 0x40},     // 4c L
   {0x7f, 0x02, 0x0c, 0x02, 0x7f},     // 4d M
   {0x7f, 0x04, 0x08, 0x10, 0x7f},     // 4e N
   {0x3e, 0x41, 0x41, 0x41, 0x3e},     // 4f O
   {0x7f, 0x09, 0x09, 0x09, 0x06},     // 50 P
   {0x3e, 0x41, 0x51, 0x21, 0x5e},     // 51 Q
   {0x7f, 0x09, 0x19, 0x29, 0x46},     // 52 R
   {0x46, 0x49, 0x49, 0x49, 0x31},     // 53 S
   {0x01, 0x01, 0x7f, 0x01, 0x01},     // 54 T
   {0x3f, 0x40, 0x40, 0x40, 0x3f},     // 55 U
   {0x1f, 0x20, 0x40, 0x20, 0x1f},     // 56 V
   {0x3f, 0x40, 0x38, 0x40, 0x3f},     // 57 W
   {0x63, 0x14, 0x08, 0x14, 0x63},     // 58 X
   {0x07, 0x08, 0x70, 0x08, 0x07},     // 59 Y
   {0x61, 0x51, 0x49, 0x45, 0x43},     // 5a Z
   {0x00, 0x7f, 0x41, 0x41, 0x00},     // 5b [
   {0x02, 0x04, 0x08, 0x10, 0x20},     // 5c 
   {0x00, 0x41, 0x41, 0x7f, 0x00},     // 5d ]
   {0x04, 0x02, 0x01, 0x02, 0x04},     // 5e ^
   {0x40, 0x40, 0x40, 0x40, 0x40},     // 5f _
   {0x00, 0x01, 0x02, 0x04, 0x00},     // 60 `
   {0x20, 0x54, 0x54, 0x54, 0x78},     // 61 a
   {0x7f, 0x48, 0x44, 0x44, 0x38},     // 62 b
   {0x38, 0x44, 0x44, 0x44, 0x20},     // 63 c
   {0x38, 0x44, 0x44, 0x48, 0x7f},     // 64 d
   {0x38, 0x54, 0x54, 0x54, 0x18},     // 65 e
   {0x08, 0x7e, 0x09, 0x01, 0x02},     // 66 f
   {0x0c, 0x52, 0x52, 0x52, 0x3e},     // 67 g
   {0x7f, 0x08, 0x04, 0x04, 0x78},     // 68 h
   {0x00, 0x44, 0x7d, 0x40, 0x00},     // 69 i
   {0x20, 0x40, 0x44, 0x3d, 0x00},     // 6a j 
   {0x7f, 0x10, 0x28, 0x44, 0x00},     // 6b k
   {0x00, 0x41, 0x7f, 0x40, 0x00},     // 6c l
   {0x7c, 0x04, 0x18, 0x04, 0x78},     // 6d m
   {0x7c, 0x08, 0x04, 0x04, 0x78},     // 6e n
   {0x38, 0x44, 0x44, 0x44, 0x38},     // 6f o
   {0x7c, 0x14, 0x14, 0x14, 0x08},     // 70 p
   {0x08, 0x14, 0x14, 0x18, 0x7c},     // 71 q
   {0x7c, 0x08, 0x04, 0x04, 0x08},     // 72 r
   {0x48, 0x54, 0x54, 0x54, 0x20},     // 73 s
   {0x04, 0x3f, 0x44, 0x40, 0x20},     // 74 t
   {0x3c, 0x40, 0x40, 0x20, 0x7c},     // 75 u
   {0x1c, 0x20, 0x40, 0x20, 0x1c},     // 76 v
   {0x3c, 0x40, 0x30, 0x40, 0x3c},     // 77 w
   {0x44, 0x28, 0x10, 0x28, 0x44},     // 78 x
   {0x0c, 0x50, 0x50, 0x50, 0x3c},     // 79 y
   {0x44, 0x64, 0x54, 0x4c, 0x44},     // 7a z
   {0x00, 0x08, 0x36, 0x41, 0x00},     // 7b {
   {0x00, 0x00, 0x7f, 0x00, 0x00},     // 7c |
   {0x00, 0x41, 0x36, 0x08, 0x00},     // 7d }
   {0x10, 0x08, 0x08, 0x10, 0x08},     // 7e ~
   {0x78, 0x46, 0x41, 0x46, 0x78}      // 7f 
  };

// ---------------------------------------------------------------------------------

//
// initialize the LCD display
//
void LCDInitialise()
{
  //
  // setup the IO pins
  //
  pinMode(LCD_CHIP_ENABLE_PIN, OUTPUT);
  pinMode(LCD_RESET_PIN, OUTPUT);
  pinMode(LCD_DATA_CONTROL_PIN, OUTPUT);
  pinMode(LCD_DATA_IN_PIN, OUTPUT);
  pinMode(LCD_CLOCK_PIN, OUTPUT);
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);

  //
  // reset the display
  //
  digitalWrite(LCD_RESET_PIN, LOW);
  digitalWrite(LCD_RESET_PIN, HIGH);

  //
  // configure the display
  //
  LCDWriteCommand(0x21);      // set H bit to program config registers

  LCDWriteCommand(0xC0);      // set LCD Vop (Contrast) (0x80 - 0xff)

  LCDWriteCommand(0x06);      // set LCD temp coefficent (0x04 - 0x07)

  LCDWriteCommand(0x14);      // set LCD bias mode to 1:48 (0x10 - 0x17)
  LCDWriteCommand(0x20);      // clear H bit to access X & Y registers
  LCDWriteCommand(0x0C);      // set "normal mode"

  //
  // turn on the LCD backlight
  //
//digitalWrite(LCD_BACKLIGHT_PIN, LOW);      // changed from LOW to HIGH for 2014-2015 school year
  digitalWrite(LCD_BACKLIGHT_PIN, HIGH);
}



//
// print an signed int to the LCD display
//  Enter:  n = number to print 
//
void LCDPrintInt(int n)
{
  if (n < 0)
  {
    LCDPrintCharacter('-');
    n = -n;
  }
  
  LCDPrintUnsignedIntWithPadding(n, 0, ' ');
}



//
// print an unsigned int to the LCD display
//  Enter:  n = number to print 
//
void LCDPrintUnsignedInt(unsigned int n)
{
  LCDPrintUnsignedIntWithPadding(n, 0, ' ');
}



//
// print an unsigned int to the LCD display, pad with blank spaces left of the number if desired
//  Enter:  n = number to print 
//          widthToPrint = total width of characters to print (0 to 5), a value of 0 does not pad
//          padCharacter = character to used for padding (typically a '0' or ' ')
//
void LCDPrintUnsignedIntWithPadding(unsigned int n, byte widthToPrint, char padCharacter)
{
  char buf[6];
  char *str;
  unsigned int m;
  char c;
  int characterCount;

  str = &buf[sizeof(buf) - 1];
  *str = '\0';
  characterCount = 0;

  //
  // convert number to a string of digits
  //
  do 
  {
    m = n;
    n /= 10;
    c = m - (10 * n);
    *--str = c + '0';
    characterCount++;
  } while(n);

  //
  // add padding before the number if desired
  //
  characterCount = widthToPrint - characterCount;
  while (characterCount > 0)
  {
    *--str = padCharacter;
    characterCount--;
  }
  
  //
  // display the string
  //
  LCDPrintString(str);
}



//
// print a string to the LCD display
//  Enter:  s -> a null terminated string 
//          lineNumber = character line (0 - 5, 0 = top row)
//
void LCDPrintCenteredString(char *s, int lineNumber)
{
  int charCount;
  int padding;
  int paddingCount;
  
  //
  // move cursor to the beginning of the line
  //
  LCDSetCursorXY(0, lineNumber);

  //
  // count number of characters in the string
  //
  charCount = 0;
  while(s[charCount] != 0)
    charCount++;

  //
  // blank pixels on left of string
  //
  padding = (LCD_NUMBER_PIXELS_ACROSS - (charCount * 6)) / 2;
  paddingCount = padding;
  while(paddingCount > 0)
  { 
    LCDWriteData(0x00);
    paddingCount--;
  }
  
  //
  // print the string
  //
  LCDPrintString(s);

  //
  // blank pixels on right of string
  //
  padding = LCD_NUMBER_PIXELS_ACROSS - (padding + (charCount * 6));
  paddingCount = padding;
  while(paddingCount > 0)
  { 
    LCDWriteData(0x00);
    paddingCount--;
  }
}



//
// print a string to the LCD display
//  Enter:  s -> a null terminated string 
//
void LCDPrintString(char *s)
{
  //
  // loop, writing one character at a time until the end of the string is reached
  //
  while (*s)
  {
    LCDPrintCharacter(*s++);
  }
}



//
// print one ASCII charater to the display
//  Enter:  c = character to display
//
void LCDPrintCharacter(byte character)
{
  int pixelColumn;
  
  //
  // make sure character is in range of the font table
  //
  if ((character < 0x20) or (character > 0x7f))
    character = 0x20;
  
  //
  // from the character, get the index into the font table
  //
  character -= 0x20;
  
  //
  // write all 5 columns of the character
  //
  for (pixelColumn = 0; pixelColumn < 5; pixelColumn++)
  {
    LCDWriteData(pgm_read_word(&Font[character][pixelColumn]));   
  }
  
  //
  // write a column of blank pixels after the character
  //
  LCDWriteData(0x00);
}



//
// clear the LCD display by writing blank pixels
//
void LCDClearDisplay()
{
  int characterColumn;
  int lastCharacterColumn;
  
  //
  // determine how many bytes must be written to clear the display
  //
  lastCharacterColumn = LCD_NUMBER_PIXELS_ACROSS * LCD_NUMBER_PIXELS_DOWN / 8;
  
  //
  // write enough blank pixels to clear the display
  //
  for (int characterColumn = 0; characterColumn < lastCharacterColumn; characterColumn++)
  {
    LCDWriteData(0x00);
  }
}



//
// draw a row of pixels, repeating the same column of 8 pixels across
//  Enter:  X1 = starting pixel column (0 - 83, 0 = left most column)
//          X2 = ending pixel column (0 - 83)
//          lineNumber = character line (0 - 5, 0 = top row)
//          byteOfPixels = byte of pixels to repeat
//
void LCDDrawRowOfPixels(int X1, int X2, int lineNumber, byte byteOfPixels)
{
  int pixelColumn;

  //
  // move to the first pixel
  //
  LCDSetCursorXY(X1, lineNumber);


  //
  // write the pixels
  //
  for (pixelColumn = 0; pixelColumn <= X2-X1; pixelColumn++)
  {
    LCDWriteData(byteOfPixels);   
  }
}



//
// set the coords where the next character will be written to the LCD display
//  Enter:  column = pixel column (0 - 83, 0 = left most column)
//          lineNumber = character line (0 - 5, 0 = top row)
//
void LCDSetCursorXY(int column, int lineNumber)
{
  LCDWriteCommand(0x80 | column);
  LCDWriteCommand(0x40 | lineNumber);
}



//
// set the LCD screen contrast value 
//  Enter:  contrastValue = value to set for the screen's contrast (0 - 127)
//
void LCDSetContrast(int contrastValue)
{
  LCDWriteCommand(0x21);                      // set H bit to program config registers
  LCDWriteCommand(contrastValue + 0x80);      // set LCD Vop (Contrast) (0x80 - 0xff)
  LCDWriteCommand(0x20);      // clear H bit to access X & Y registers
}



//
// write a single command byte to the LCD display
//
void LCDWriteCommand(byte command)
{
  digitalWrite(LCD_DATA_CONTROL_PIN, LCD_COMMAND_BYTE);
  digitalWrite(LCD_CHIP_ENABLE_PIN, LOW);
  shiftOut(LCD_DATA_IN_PIN, LCD_CLOCK_PIN, MSBFIRST, command);
  digitalWrite(LCD_CHIP_ENABLE_PIN, HIGH);
}



//
// write a single data byte to the LCD display
//
//void LCDWriteData(byte data)
//{
//  digitalWrite(LCD_DATA_CONTROL_PIN, LCD_DATA_BYTE);
//  digitalWrite(LCD_CHIP_ENABLE_PIN, LOW);
//  shiftOut(LCD_DATA_IN_PIN, LCD_CLOCK_PIN, MSBFIRST, data);
//  digitalWrite(LCD_CHIP_ENABLE_PIN, HIGH);
//}


//
// write a single data byte to the LCD display
// NOTE: This version of the function is optimised for speed
//
void LCDWriteData(byte data)
{
  byte i;

  //
  // set the "LCD_DATA_CONTROL_PIN" to "Data"
  //
  bitSet(PORTF, 2); 
  
  //
  // enable the chip enable for the display
  //
  bitClear(PORTF, 3);

  //
  // clock the data byte out, starting with the MSB, one bit at a time
  // write to the IO ports directly for a faster speed
  //
  for (i = 0; i < 8; i++)  
  {
    if (data & 0x80)
      bitSet(PORTF, 1); 
    else 
      bitClear(PORTF, 1);
    
    data = data << 1;
    
    bitSet(PORTF, 0);	
    bitClear(PORTF, 0);
  }

  //
  // disable the chip enable for the display
  //
  bitSet(PORTF, 3);
}


//
// display the time on the LCD
//
void displayTimeOnLCD()
{
  byte second;
  byte minute;
  byte hour;
  char *AMPMString;
  
  //
  // read the real time clock to get the current time
  //
  RTCGetTime(&hour, &minute, &second);
  
  //
  // convert 24 hour time to 12 hour
  //
  if (hour >= 12)
    AMPMString = " PM";
  else
    AMPMString = " AM";

  if (hour == 0)
    hour = 12;

  if (hour > 12)
    hour -= 12;
 
  //
  // display time on the LCD
  //
  LCDSetCursorXY(8, 5);
  LCDPrintUnsignedIntWithPadding(hour, 2, ' ');
  LCDPrintCharacter(':');
  
  LCDPrintUnsignedIntWithPadding(minute, 2, '0');
  LCDPrintCharacter(':');
  
  LCDPrintUnsignedIntWithPadding(second, 2, '0');
  
  LCDPrintString(AMPMString);
}
