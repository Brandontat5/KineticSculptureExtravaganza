
//      ******************************************************************
//      *                                                                *
//      *                   Ultrasonic Hardware Control                  *
//      *                                                                *
//      ******************************************************************




//
// measurement constants
//
const long ULTRASONIC_TIMEOUT_PERIOD = 60000;
const byte kMinDistanceInCM = 3;

//
// function prototypes
//
void showMeterStickMode();
void ultrasonicInitialize();                     // initializes the ultrasonic
void ultrasonicStartMeasurement();               // starts an ultrasonic measurement
bool ultrasonicIsFinished();                     // returns true when measurement is done, otherwise false
int ultrasonicGetDistanceInCM();                 // returns the measurement distance in CM
void ultrasonicEchoISR();                        // Interrupt Service Routine to make measurements



// ---------------------------------------------------------------------------------
//                               Ultrasonic Functions
// ---------------------------------------------------------------------------------

//
// NOTES: Sound travels at 1130 feet per second, or 340.29 meters per second.
// that results in about 29.4 microseconds per cm.  Round trip is about 58.8 
// microseconds per cm to the object and back.
//
// The transducer needs a high level trigger signal of at least 10us to start
// a measurement.
//
// The Echo pin falls low when an echo is detected.
//
// A minimum of 60ms is recommend from the start of one measurment until the
// start of the next one.
//


//
// global variables used by the ultrasonics
//
byte nextRed, nextBlue, nextGreen;
unsigned long ultrasonicStartTime;
unsigned long ultrasonicEchoTime;
bool ultrasonicMeasurementCompleteFlg;

// ---------------------------------------------------------------------------------

//
// initialize the ultrasonics
//
void ultrasonicInitialize(void)
{  
  //
  // setup the IO pins
  //
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  //
  // start with the trigger off
  //
  digitalWrite(ULTRASONIC_TRIGGER_PIN, 0);

  //
  // attach the echo pin to an interrupt service routine
  //
  attachInterrupt(1, ultrasonicEchoISR, FALLING);    // interrupt 1 = D3 = echo signal from ultrasonic
}


//
// start an ultrasonic measurement
//
void ultrasonicStartMeasurement()
{
  ultrasonicMeasurementCompleteFlg = false;         // indicate that the measurement is not complete
  digitalWrite(ULTRASONIC_TRIGGER_PIN, 1);          // trigger ultrasonics to start a measurement
  ultrasonicStartTime = micros();                   // record the time when the measurement was started
  delayMicroseconds(50);                            // delay while trigger signal is on
  digitalWrite(ULTRASONIC_TRIGGER_PIN, 0);          // turn trigger off
}



//
// check if the ultrasonic measurement has finished
//  Exit:  true returned if finished, false returned if not finished
//
bool ultrasonicIsFinished()
{
  //
  // check if the echo has been received
  //
  if (ultrasonicMeasurementCompleteFlg)
    return(true);

  //
  // no echo, check for timeout
  //
  if ((micros() - ultrasonicStartTime) > ULTRASONIC_TIMEOUT_PERIOD)
  {
    ultrasonicMeasurementCompleteFlg = true;
    ultrasonicEchoTime = 0L;
    return(true);
  }

  //
  // return false, echo not received yet
  //
  return(false);
}


//
// get the results of the last ultrasonic measurement
//  Exit:  distance in centimeters returned, 0 returned if no echo
//
int ultrasonicGetDistanceInCM()
{
  int distanceInCM;

  ultrasonicMeasurementCompleteFlg = false;

  //
  // check if the measurement timed out, if so return 0
  //
  if (ultrasonicEchoTime == 0)
    return(0);

  //
  // compute the distance to the object in CM
  //
  //distanceInCM = ((ultrasonicEchoTime - 950L) / 58L);
  distanceInCM = ((ultrasonicEchoTime - 500L) / 58L);
  distanceInCM = constrain(distanceInCM, kMinDistanceInCM, distanceInCM);

  //
  // return the distance
  //
  return(distanceInCM);
}


//
// interrupt service routine for the ultrasonic echo
//
void ultrasonicEchoISR()
{
  unsigned long echoTime;

  //
  // determine how long in microseconds it took to get the echo
  //
  ultrasonicEchoTime = micros() - ultrasonicStartTime;
  ultrasonicMeasurementCompleteFlg = true;         // indicate that the measurement is complete
}


// -------------------------------------- End --------------------------------------
