
//      ******************************************************************
//      *                                                                *
//      *                    Motor Hardware Control                      *
//      *                                                                *
//      ******************************************************************

//
// function prototypes
//
void diskVelocitiesInitialize(void);
void diskVelocitiesStartTransition(float outerDiskVelocityInRPM, float innerDiskVelocityInRPM, unsigned long TransitionDurationMS);
bool diskVelocitiesTransitionIsFinished();
void diskVelocitiesTransition();
void diskVelocitiesSet(float outerDiskVelocityInRPM, float innerDiskVelocityInRPM);
void motorInitialise(void);
void motorSetSpeedAndDirection1(int desiredMotorSpeedInRPM, int desiredDirection);
void motorProportionalIntegralControl1();
void motorZeroIntegralTerms();
int motorReadRPM1();
void motorSetPWMPower1(int motorPWM, int motorDirection);
void motorSetSpeedAndDirection2(int desiredMotorSpeedInRPM, int desiredDirection);
void motorProportionalIntegralControl2();
int motorReadRPM2();
void motorSetPWMPower2(int motorPWM, int motorDirection);
void motorTachometer1ISR();
void motorTachometer2ISR();

// ---------------------------------------------------------------------------------
//                         Outer & Inner Rotating Disk Functions
// ---------------------------------------------------------------------------------

//
// global variables by the motors for transition from one speed to another
//
float diskVelocitiesCurrentSpeedOuter;
float diskVelocitiesTransitionInitialSpeedOuter;
float diskVelocitiesTransitionDeltaSpeedOuter;
float diskVelocitiesTransitionFinalSpeedOuter;

float diskVelocitiesCurrentSpeedInner;
float diskVelocitiesTransitionInitialSpeedInner;
float diskVelocitiesTransitionDeltaSpeedInner;
float diskVelocitiesTransitionFinalSpeedInner;

long diskVelocitiesTransitionDurationMS;
unsigned long diskVelocitiesTransitionStartTimeMS;
unsigned long diskVelocitiesTransitionFinishTimeMS;
bool diskVelocitiesTransitionCompleteFlg;


//
// motor constants
//
const int DIRECTION_CW = LOW;
const int DIRECTION_CCW = HIGH;


// ---------------------------------------------------------------------------------

//
// initialize the disk velocty functions
//
void diskVelocitiesInitialize(void)
{
  diskVelocitiesSet(0, 0);
  diskVelocitiesTransitionCompleteFlg = true;
}



//
// start a transition from the current disk velocities to a new ones over a period of time
//   Enter: outerDiskVelocityInRPM = discVelocities to transition to for the outer disk in RPM, negative value goes counter counter-clockwise
//          innerDiskVelocityInRPM = discVelocities to transition to for the inner disk in RPM, negative value goes counter counter-clockwise
//          TransitionDurationMS = number of milliseconds for the transition (1 - 60000)
//
void diskVelocitiesStartTransition(float outerDiskVelocityInRPM, float innerDiskVelocityInRPM, unsigned long TransitionDurationMS)
{
  unsigned long startTimeMS;
  unsigned long finishTimeMS;

  diskVelocitiesTransitionCompleteFlg = true;

  startTimeMS = millis();
  
  finishTimeMS = startTimeMS + (unsigned long) TransitionDurationMS;
  if (finishTimeMS < startTimeMS)
    finishTimeMS = startTimeMS;
  
  
  //
  // make sure the new velocities meet the minimums
  //
  if ((outerDiskVelocityInRPM >= -0.2) && (outerDiskVelocityInRPM <= 0.2))
    outerDiskVelocityInRPM = 0.0;
  else
  {
    if ((outerDiskVelocityInRPM > 0.1) && (outerDiskVelocityInRPM < MINIMUM_VELOCITY_IN_RPM))
      outerDiskVelocityInRPM = MINIMUM_VELOCITY_IN_RPM;

    if ((outerDiskVelocityInRPM < -0.1) && (-outerDiskVelocityInRPM < MINIMUM_VELOCITY_IN_RPM))
      outerDiskVelocityInRPM = -MINIMUM_VELOCITY_IN_RPM;
  }
    
  if ((innerDiskVelocityInRPM >= -0.2) && (innerDiskVelocityInRPM <= 0.2))
    innerDiskVelocityInRPM = 0.0;
  else
  {
    if ((innerDiskVelocityInRPM > 0.1) && (innerDiskVelocityInRPM < MINIMUM_VELOCITY_IN_RPM))
      innerDiskVelocityInRPM = MINIMUM_VELOCITY_IN_RPM;

    if ((innerDiskVelocityInRPM < -0.1) && (-innerDiskVelocityInRPM < MINIMUM_VELOCITY_IN_RPM))
      innerDiskVelocityInRPM = -MINIMUM_VELOCITY_IN_RPM;
  }

  //
  // remember what the final velocities will be
  //
  diskVelocitiesTransitionFinalSpeedOuter = outerDiskVelocityInRPM;
  diskVelocitiesTransitionFinalSpeedInner = innerDiskVelocityInRPM;
  

  //
  // start the transition using the disk's current discVelocities
  //
  diskVelocitiesTransitionInitialSpeedOuter = diskVelocitiesCurrentSpeedOuter;  
  diskVelocitiesTransitionInitialSpeedInner = diskVelocitiesCurrentSpeedInner;


  //
  // if the disk is stopped now, set the initial discVelocities to a minimum for a 
  // smoother startup
  //
  if (diskVelocitiesTransitionInitialSpeedOuter == 0)
  {
    if (outerDiskVelocityInRPM > 0.1)
      diskVelocitiesTransitionInitialSpeedOuter = MINIMUM_VELOCITY_IN_RPM;

    if (outerDiskVelocityInRPM < -0.1)
      diskVelocitiesTransitionInitialSpeedOuter = -MINIMUM_VELOCITY_IN_RPM;
  }

  if (diskVelocitiesTransitionInitialSpeedInner == 0)
  {
    if (innerDiskVelocityInRPM > 0.1)
      diskVelocitiesTransitionInitialSpeedInner = MINIMUM_VELOCITY_IN_RPM;

    if (innerDiskVelocityInRPM < -0.1)
      diskVelocitiesTransitionInitialSpeedInner = -MINIMUM_VELOCITY_IN_RPM;
  }


  //
  // if the final discVelocities of the disk is stopped, but its moving now, set the final 
  // discVelocities to a minimum for a smoother slow down
  //
  if (outerDiskVelocityInRPM == 0)
  {
    if (diskVelocitiesTransitionInitialSpeedOuter > MINIMUM_VELOCITY_IN_RPM)
      outerDiskVelocityInRPM = MINIMUM_VELOCITY_IN_RPM;
      
    if (diskVelocitiesTransitionInitialSpeedOuter < -MINIMUM_VELOCITY_IN_RPM)
      outerDiskVelocityInRPM = -MINIMUM_VELOCITY_IN_RPM;
  }
  
  if (innerDiskVelocityInRPM == 0)
  {
    if (diskVelocitiesTransitionInitialSpeedInner > MINIMUM_VELOCITY_IN_RPM)
      innerDiskVelocityInRPM = MINIMUM_VELOCITY_IN_RPM;
      
    if (diskVelocitiesTransitionInitialSpeedInner < -MINIMUM_VELOCITY_IN_RPM)
      innerDiskVelocityInRPM = -MINIMUM_VELOCITY_IN_RPM;
  }
  

  //
  // determine how much the discVelocities will change over time
  //
  diskVelocitiesTransitionDeltaSpeedOuter = outerDiskVelocityInRPM - diskVelocitiesTransitionInitialSpeedOuter;
  
  diskVelocitiesTransitionDeltaSpeedInner = innerDiskVelocityInRPM - diskVelocitiesTransitionInitialSpeedInner;
 
  cli();
  diskVelocitiesTransitionDurationMS = TransitionDurationMS;
  diskVelocitiesTransitionStartTimeMS = startTimeMS;
  diskVelocitiesTransitionFinishTimeMS = finishTimeMS;
  sei();

  diskVelocitiesTransitionCompleteFlg = false;
}



//
// check if the disk velocities transistion has finished
//  Exit:  true returned if finished, false returned if not finished
//
bool diskVelocitiesTransitionIsFinished()
{
  return(diskVelocitiesTransitionCompleteFlg);
}



//
// transition the disk velocities, this must be called every 20ms or so
//
void diskVelocitiesTransition()
{
  unsigned long currentTime;
  long elapsedTime;
  float outerDiskVelocityInRPM;
  float innerDiskVelocityInRPM;

  //
  // check if doing a transition, if not return
  //
  if (diskVelocitiesTransitionCompleteFlg)
    return;
  
  //
  // doing a transition, now check if the period has ended
  //
  currentTime = millis();
  elapsedTime = (long) (currentTime - diskVelocitiesTransitionStartTimeMS);
  if (currentTime >= diskVelocitiesTransitionFinishTimeMS)
  {
    //
    // transition has finished, set the final velocities
    //
    diskVelocitiesSet(diskVelocitiesTransitionFinalSpeedOuter, diskVelocitiesTransitionFinalSpeedInner);
    diskVelocitiesTransitionCompleteFlg = true;
    return;
  }


 //
 // determine the velocities for this point in time during the transition
 //
 outerDiskVelocityInRPM = diskVelocitiesTransitionInitialSpeedOuter + 
   (diskVelocitiesTransitionDeltaSpeedOuter * (float) elapsedTime) / (float) diskVelocitiesTransitionDurationMS;

 innerDiskVelocityInRPM = diskVelocitiesTransitionInitialSpeedInner + 
   (diskVelocitiesTransitionDeltaSpeedInner * (float) elapsedTime) / (float) diskVelocitiesTransitionDurationMS;
      
 diskVelocitiesSet(outerDiskVelocityInRPM, innerDiskVelocityInRPM);
}



//
// set the inner and outer disk rotation velocities
//   Enter: outerDiskVelocityInRPM = new discVelocities of outer disk in RPM, negative value goes counter counter-clockwise
//          innerDiskVelocityInRPM = new discVelocities of inner disk in RPM, negative value goes counter counter-clockwise
//
void diskVelocitiesSet(float outerDiskVelocityInRPM, float innerDiskVelocityInRPM)
{ 
 int motorVelocity;
  
  //
  // scale the outer disk RPM speed to the motor RPM speed
  //
  diskVelocitiesCurrentSpeedOuter = outerDiskVelocityInRPM;
  motorVelocity = outerDiskVelocityInRPM * GEAR_REDUCTION_TO_FINAL_STAGE;

  //
  // determine the direction and speed for the motor
  //
  if (motorVelocity >= 0)
    motorSetSpeedAndDirection1(motorVelocity, DIRECTION_CCW);
  else
    motorSetSpeedAndDirection1(-motorVelocity, DIRECTION_CW);
  
  //
  // scale the inner disk RPM speed to the motor RPM speed
  //
  diskVelocitiesCurrentSpeedInner = innerDiskVelocityInRPM;
  motorVelocity = innerDiskVelocityInRPM * GEAR_REDUCTION_TO_FINAL_STAGE;

  //
  // determine the direction and speed for the motor
  //
  if (motorVelocity >= 0)
    motorSetSpeedAndDirection2(motorVelocity, DIRECTION_CCW);
  else
    motorSetSpeedAndDirection2(-motorVelocity, DIRECTION_CW);
}


// ---------------------------------------------------------------------------------
//                                  Motor Functions
// ---------------------------------------------------------------------------------

//
// global variables by the motors for servoing
//
int motorDesiredSpeedInRPM1;
int motorDesiredDirection1;
int motorMeasuredRPM1;
int motorIntegratedSpeedError1;

int motorDesiredSpeedInRPM2;
int motorDesiredDirection2;
int motorMeasuredRPM2;
int motorIntegratedSpeedError2;


//
// global variables used by the tachometer ISR
//
unsigned long motorTachMicrosecondsBetweenLines1;
unsigned long motorTachTimeOfLastMeasurement1;
unsigned long motorTachMicrosecondsBetweenLines2;
unsigned long motorTachTimeOfLastMeasurement2;

// ---------------------------------------------------------------------------------

//
// initialize the motors
//
void motorInitialise(void)
{
  //
  // setup the IO pins
  //
  pinMode(MOTOR_DIRECTION_1_PIN, OUTPUT);
  pinMode(MOTOR_PWM_1_PIN, OUTPUT);
  pinMode(MOTOR_TACHOMETER_1_PIN, INPUT);

  pinMode(MOTOR_DIRECTION_2_PIN, OUTPUT); 
  pinMode(MOTOR_PWM_2_PIN, OUTPUT);
  pinMode(MOTOR_TACHOMETER_2_PIN, INPUT);


  //
  // initially turn the motors off
  //
  motorSetPWMPower1(0, DIRECTION_CW);
  motorSetPWMPower2(0, DIRECTION_CW);


  //
  // initialize global vars used by the motors
  //
  motorDesiredSpeedInRPM1 = 0;
  motorDesiredDirection1 = DIRECTION_CW;
  motorIntegratedSpeedError1 = 0L;
  
  motorDesiredSpeedInRPM2 = 0;
  motorDesiredDirection2 = DIRECTION_CW;
  motorIntegratedSpeedError2 = 0L;


  //
  // change the prescaler for timer 1 (used by the motor PWM) to increase the PWM frequency
  // to 31250Hz so that it can not be heard  (1=31250Hz, 2=3906Hz, 3=488Hz, 4=122Hz, 5=30.5Hz)
  //
  TCCR1B = (TCCR1B & B11111000) | 1;                // set Timer1 prescaler so PWM frequency is 31.2 kHz


  //
  // setup the tachometers to interrupt on the rising edge
  //
  attachInterrupt(4, motorTachometer1ISR, RISING);    // interrupt 4 = D19 = tachometer1
  attachInterrupt(5, motorTachometer2ISR, RISING);    // interrupt 5 = D18 = tachometer2
}


  
//
// set desired speed and direction for motor 1 in RPM, the motor will servo to this speed
//   Enter: desiredMotorSpeedInRPM = desired motor speed in RPM to servo to
//          desiredDirection = motor direction: DIRECTION_CW or DIRECTION_CCW
//
void motorSetSpeedAndDirection1(int desiredMotorSpeedInRPM, int desiredDirection)
{
  cli();                             // disable interrupts 
  motorDesiredSpeedInRPM1 = desiredMotorSpeedInRPM;
  motorDesiredDirection1 = desiredDirection;
  sei();                             // enable interrupts
}


//
// motor 1 servo using proportional-integral control
//   Enter: motorDesiredSpeedInRPM1 = desired motor speed in RPM to servo to
//          desiredDirection1 = desired motor direction: DIRECTION_CW or DIRECTION_CCW
//
void motorProportionalIntegralControl1()
{
  int measuredMotorRPM;
  int speedError;
  int motorPower;

  //
  // read the motor's speed and determine the error in speed
  //
  measuredMotorRPM = motorReadRPM1();
  speedError = motorDesiredSpeedInRPM1 - measuredMotorRPM;

  //
  // integrate the speed error
  //
  motorIntegratedSpeedError1 += speedError;
  if (motorIntegratedSpeedError1 > MOTOR_KINT_SPEED_ERROR_MAX)
    motorIntegratedSpeedError1 = MOTOR_KINT_SPEED_ERROR_MAX;
  if (motorIntegratedSpeedError1 < -MOTOR_KINT_SPEED_ERROR_MAX)
    motorIntegratedSpeedError1 = -MOTOR_KINT_SPEED_ERROR_MAX;
  
  //
  // compute power to motor
  //
  motorPower = (speedError * MOTOR_KP_PROP_CONTROL) / 10;
  motorPower += (motorIntegratedSpeedError1 / MOTOR_KI_PROP_INT_CONTROL);

    
  //
  // output power and direction to motor
  //
  motorSetPWMPower1(motorPower, motorDesiredDirection1);
}


void motorZeroIntegralTerms()
{
  motorIntegratedSpeedError1 = 0;
  motorIntegratedSpeedError2 = 0;
}
  


//
// measure the RPM of motor 1 using the tachometer
//
int motorReadRPM1()
{ 
  unsigned long currentTime;
  unsigned long timeSinceLastTachUpdate;
  unsigned long tachTimeBetweenLines;
  
  currentTime = micros();
  
  cli();
  tachTimeBetweenLines = motorTachMicrosecondsBetweenLines1;
  timeSinceLastTachUpdate = currentTime - motorTachTimeOfLastMeasurement1;
  sei();

  //
  // check if the tach is not moving
  //
  if (timeSinceLastTachUpdate > 50000L)
    motorMeasuredRPM1 = 0;
    
  //
  // check if the tack is moving too fast
  //
  else if (tachTimeBetweenLines < 10)
    motorMeasuredRPM1 = 0;
  
  //
  // return the speed in RPM
  //
  else
    motorMeasuredRPM1 = (int)((1000000 * 60L) / (tachTimeBetweenLines * LINES_ON_TACHOMETER_DISK));

  return(motorMeasuredRPM1);
}



//
// set power to motor 1
//  Enter:  motorPWM = power to motor (0 = 0%, 255 = 100%)
//          motorDirection = motor direction: DIRECTION_CW or DIRECTION_CCW
//
void motorSetPWMPower1(int motorPWM, int motorDirection)
{
  //
  // clip PWM value to minimum and maximum values
  //
  if (motorPWM < 0)
    motorPWM = 0;
  
  if (motorPWM > MOTOR_MAX_PWM)
    motorPWM = MOTOR_MAX_PWM;
    
  //
  // output power and direction to motor
  //
  digitalWrite(MOTOR_DIRECTION_1_PIN, motorDirection);
  analogWrite(MOTOR_PWM_1_PIN, 255 - motorPWM);          // PWM signal inverted so subtract from 255
}
  
  

//
// set desired speed and direction for motor 2 in RPM, the motor will servo to this speed
//   Enter: desiredMotorSpeedInRPM = desired motor speed in RPM to servo to
//          desiredDirection = motor direction: DIRECTION_CW or DIRECTION_CCW
//
void motorSetSpeedAndDirection2(int desiredMotorSpeedInRPM, int desiredDirection)
{
  cli();                             // disable interrupts 
  motorDesiredSpeedInRPM2 = desiredMotorSpeedInRPM;
  motorDesiredDirection2 = desiredDirection;
  sei();                             // enable interrupts
}


//
// motor 2 servo using proportional-integral control
//   Enter: motorDesiredSpeedInRPM2 = desired motor speed in RPM to servo to
//          desiredDirection2 = desired motor direction: DIRECTION_CW or DIRECTION_CCW
//
void motorProportionalIntegralControl2()
{
  int measuredMotorRPM;
  int speedError;
  int motorPower;

  //
  // read the motor's speed and determine the error in speed
  //
  measuredMotorRPM = motorReadRPM2();
  speedError = motorDesiredSpeedInRPM2 - measuredMotorRPM;

  //
  // integrate the speed error
  //
  motorIntegratedSpeedError2 += speedError;
  if (motorIntegratedSpeedError2 > MOTOR_KINT_SPEED_ERROR_MAX)
    motorIntegratedSpeedError2 = MOTOR_KINT_SPEED_ERROR_MAX;
  if (motorIntegratedSpeedError2 < -MOTOR_KINT_SPEED_ERROR_MAX)
    motorIntegratedSpeedError2 = -MOTOR_KINT_SPEED_ERROR_MAX;
  
  //
  // compute power to motor
  //
  motorPower = (speedError * MOTOR_KP_PROP_CONTROL) / 10;
  motorPower += (motorIntegratedSpeedError2 / MOTOR_KI_PROP_INT_CONTROL);

    
  //
  // output power and direction to motor
  //
  motorSetPWMPower2(motorPower, motorDesiredDirection2);
}



//
// measure the RPM of motor 2 using the tachometer
//
int motorReadRPM2()
{ 
  unsigned long currentTime;
  unsigned long timeSinceLastTachUpdate;
  unsigned long tachTimeBetweenLines;
  
  currentTime = micros();
  
  cli();
  tachTimeBetweenLines = motorTachMicrosecondsBetweenLines2;
  timeSinceLastTachUpdate = currentTime - motorTachTimeOfLastMeasurement2;
  sei();
  
  //
  // check if the tach is not moving
  //
  if (timeSinceLastTachUpdate > 25000L)
    motorMeasuredRPM2 = 0;
    
  //
  // check if the tack is moving too fast
  //
  else if (tachTimeBetweenLines < 10)
    motorMeasuredRPM2 = 0;
  
  //
  // return the speed in RPM
  //
  else
    motorMeasuredRPM2 = (int)((1000000 * 60L) / (tachTimeBetweenLines * LINES_ON_TACHOMETER_DISK));

  return(motorMeasuredRPM2);
}



//
// set power to motor 2
//  Enter:  motorPWM = power to motor (0 = 0%, 255 = 100%)
//          motorDirection = motor direction: DIRECTION_CW or DIRECTION_CCW
//
void motorSetPWMPower2(int motorPWM, int motorDirection)
{
  //
  // clip PWM value to minimum and maximum values
  //
  if (motorPWM < 0)
    motorPWM = 0;
  
  if (motorPWM > MOTOR_MAX_PWM)
    motorPWM = MOTOR_MAX_PWM;
    
  //
  // output power and direction to motor
  //
  digitalWrite(MOTOR_DIRECTION_2_PIN, motorDirection);
  analogWrite(MOTOR_PWM_2_PIN, 255 - motorPWM);          // PWM signal inverted so subtract from 255
}



//
// interrupt service routine for Tachometer 1
//
void motorTachometer1ISR()
{
  unsigned long newTime;

  newTime = micros();
  motorTachMicrosecondsBetweenLines1 = newTime - motorTachTimeOfLastMeasurement1;
  motorTachTimeOfLastMeasurement1 = newTime;
}



//
// interrupt service routine for Tachometer 2
//
void motorTachometer2ISR()
{
  unsigned long newTime;

  newTime = micros();  
  motorTachMicrosecondsBetweenLines2 = newTime - motorTachTimeOfLastMeasurement2;
  motorTachTimeOfLastMeasurement2 = newTime;
}

