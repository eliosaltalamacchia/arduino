#include <AccelStepper.h>
#include <elapsedMillis.h>
#include <ezButton.h>
#include <RotaryEncoder.h>

// Define stepper motor connections
#define L_DIR_PIN 2
#define L_STEP_PIN 3
#define R_DIR_PIN 4
#define R_STEP_PIN 5

// Define button pins
#define DEBOUNCE_INTERVAL 50
#define START_BUTTON_PIN 6
#define STOP_BUTTON_PIN 7
#define HOME_BUTTON_PIN 8

// Define limit switch pin for homing
#define HOME_LIMIT_L_PIN 9
#define HOME_LIMIT_R_PIN 10
#define HOME_STEP 300

// Define rotary encoder pins for acceleration
#define ROTARY_CLK_PIN 11
#define ROTARY_DT_PIN 12
#define ROTARY_BUTTON_PIN 13
#define ROTARY_STEPS 10

// Common parameters for stepper motors (L-R)
#define SPEED_MIN 100
#define SPEED_MAX 500
#define SPEED 100
#define ACCEL 100

// Define max upper and lower position
#define POS_STOP 350

// Define max pulses for homing
#define HOMING_MAX 6400

// Create a new instance of the AccelStepper class for each stepper
AccelStepper stepperL = AccelStepper(AccelStepper::DRIVER, L_STEP_PIN, L_DIR_PIN);
AccelStepper stepperR = AccelStepper(AccelStepper::DRIVER, R_STEP_PIN, R_DIR_PIN);

// Create control button instances
ezButton buttonStart(START_BUTTON_PIN);
ezButton buttonStop(STOP_BUTTON_PIN);
ezButton buttonHome(HOME_BUTTON_PIN);

// Create rotary encoder instance and rotary button
RotaryEncoder encoder(ROTARY_CLK_PIN, ROTARY_DT_PIN, RotaryEncoder::LatchMode::TWO03);
ezButton encoderButton(ROTARY_BUTTON_PIN);

// Accel initial values from rotary
int lastPos = -1;

// one second info printout timer
elapsedMillis printTime;

// stop signal from button one for each stepper
bool isStoppedL = true;
bool isStoppedR = true;
int currentDirL = 1; // 1 or -1 positive or negative for direction
int currentDirR = 1;

// Homing control one for each stepper
enum HomingStatus {Init, Searching, Touched, Reached}; // Used to check limit switch position
ezButton buttonLimitL(HOME_LIMIT_L_PIN);
ezButton buttonLimitR(HOME_LIMIT_R_PIN);
HomingStatus homingStatusL = Init;
HomingStatus homingStatusR = Init;
long initialHomingPosL = 0;
long initialHomingPosR = 0;
bool isHomingL = false;
bool isHomingR = false;

void setup() {
  // Initialize for loggin purposes
  Serial.begin(115200);

  // Initialize buttons
  buttonStart.setDebounceTime(DEBOUNCE_INTERVAL);
  buttonStop.setDebounceTime(DEBOUNCE_INTERVAL);
  buttonHome.setDebounceTime(DEBOUNCE_INTERVAL);

  // Initialize limit switches
  buttonLimitL.setDebounceTime(DEBOUNCE_INTERVAL);
  buttonLimitR.setDebounceTime(DEBOUNCE_INTERVAL);

  // Initialize acceleration encoder
  encoderButton.setDebounceTime(DEBOUNCE_INTERVAL);
  encoder.setPosition(SPEED * ROTARY_STEPS);

  // Initialize stepper positions
  stepperL.setCurrentPosition(0);
  stepperR.setCurrentPosition(0);
}

void loop() {
  bool homingButtonPressed = false;

  // Call loop to poll each button state
  buttonStart.loop();
  buttonStop.loop();
  buttonHome.loop();
  buttonLimitL.loop();
  buttonLimitR.loop();
  encoderButton.loop();

  // Loggin debug information from both steppers
  logInfo();

  // Poll acceleration changes 
  encoder.tick();
  
  // Set acceleration using encoder position
  setAcceleration();

  // Start button pressed then continue moving to next postition
  if (buttonStart.isPressed()) {
    stepperStart();
  }

  // Stop button pressed then stop inmediately both motors
  if(buttonStop.isPressed()) {
    stepperStopL();
    stepperStopR();
  } 
  
  // Home button pressed or homing is executing
  homingButtonPressed = buttonHome.isPressed();
  if (homingButtonPressed || isHomingL) {
    stepperHomingL();
  }
  if (homingButtonPressed || isHomingR) {
    stepperHomingR();
  }

  // Move to next position until end is reached
  if (!isStoppedL) {
    stepperL.run();
    currentDirL = (stepperL.currentPosition() > 0 ? 1 : -1);
  }
  if (!isStoppedR) {
    stepperR.run();
    currentDirR = (stepperR.currentPosition() > 0 ? 1 : -1);
  }

  // Change direction when end is reached
  if (!isHomingL && !isHomingR) {
    changeRotationDir();
  }
}

void initMovementL() {
  // Initialize stepper motor
  stepperL.setMaxSpeed(SPEED_MAX);
  stepperL.setAcceleration(ACCEL);
  stepperL.setSpeed(SPEED);
}

void initMovementR() {
  // Initialize stepper motor
  stepperR.setMaxSpeed(SPEED_MAX);
  stepperR.setAcceleration(ACCEL);
  stepperR.setSpeed(SPEED);
}

void changeRotationDir() {
  // final position reached then invert direction 
  if (abs(stepperL.currentPosition()) >= POS_STOP) {
    stepperL.moveTo(-stepperL.currentPosition());
  }
  if (abs(stepperR.currentPosition()) >= POS_STOP) {
    stepperR.moveTo(-stepperR.currentPosition());
  }
}

void stepperStart() {
  Serial.println("Start button pressed, start all");
  isStoppedL = false;
  initMovementL();
  stepperL.moveTo(currentDirL * POS_STOP);
  isStoppedR = false;
  initMovementR();
  stepperR.moveTo(currentDirR * POS_STOP);
}

void stepperStopL() {
    Serial.println("Stop button pressed, stop left stepper");
    stepperL.stop();
    isStoppedL = true;
}

void stepperStopR() {
    Serial.println("Stop button pressed, stop rigth stepper");
    stepperR.stop();
    isStoppedR = true;
}

void stepperHoming() {
  // homing both steppers
  stepperHomingL();
  stepperHomingR();
}

void stepperHomingL() {
  int state = LOW;

  switch (homingStatusL) {
    case Init:
      // Set Max Speed and Acceleration of each Steppers at startup for homing
      stepperL.setCurrentPosition(0);
      stepperL.setMaxSpeed(SPEED_MAX);
      stepperL.moveTo(-HOMING_MAX); // always rotate clockwise
      isStoppedL = false;
      isHomingL = true;
      homingStatusL = Searching;
      break;
      
    case Searching:
      state = buttonLimitL.getState();
      if (state == HIGH) {
        // limit switch touched
        homingStatusL = Touched;
      }
      break;

    case Touched:
      stepperStopL();
      stepperL.setMaxSpeed(SPEED_MAX);
      isStoppedL = false;
      homingStatusL = Reached;
      break;

    case Reached:
      state = buttonLimitL.getState();
      if (state == LOW) {
        // homing ended then stop
        homingStatusL = Init;
        isHomingL = false;
        currentDirL = 0;
        stepperStopL();
        stepperL.setCurrentPosition(0);
        initMovementL();
      }
      else {
        initialHomingPosL+=HOME_STEP;
        stepperL.moveTo(initialHomingPosL);
      }
      break;

    default:
      // error
      Serial.println("Left homing status invalid");
  }  
}

void stepperHomingR() {
  int state = LOW;

  switch (homingStatusR) {
    case Init:
      // Set Max Speed and Acceleration of each Steppers at startup for homing
      stepperR.setCurrentPosition(0);
      stepperR.setMaxSpeed(SPEED_MAX);
      stepperR.moveTo(HOMING_MAX); // always rotate counter clockwise
      isStoppedR = false;
      isHomingR = true;
      homingStatusR = Searching;
      break;
      
    case Searching:
      state = buttonLimitR.getState();
      if (state == HIGH) {
        // limit switch touched
        homingStatusR = Touched;
      }
      break;

    case Touched:
      stepperStopR();
      stepperR.setMaxSpeed(SPEED_MAX);
      isStoppedR = false;
      homingStatusR = Reached;
      break;

    case Reached:
      state = buttonLimitR.getState();
      if (state == LOW) {
        // homing ended then stop
        homingStatusR = Init;
        isHomingR = false;
        currentDirR = 0;
        stepperStopR();
        stepperR.setCurrentPosition(0);
        initMovementR();
      }
      else {
        initialHomingPosR+=HOME_STEP;
        stepperR.moveTo(-initialHomingPosR);
      }
      break;

    default:
      // error
      Serial.println("Rigth homing status invalid");
  }  
}

void setAcceleration() {
  // get the current physical position and calc the logical position
  int newPos = encoder.getPosition() * ROTARY_STEPS;

  // change acceleration if rotary moved
  if (lastPos != newPos) {
    // check min value and max values for speed
    if (newPos < SPEED_MIN) {
      encoder.setPosition(SPEED_MIN / ROTARY_STEPS);
      newPos = SPEED_MIN;

    } else if (newPos > SPEED_MAX) {
      encoder.setPosition(SPEED_MAX / ROTARY_STEPS);
      newPos = SPEED_MAX;
    }

    // save new speed
    if (lastPos != newPos) {
      Serial.print("Setting new accel value: ");
      Serial.println(newPos);
      lastPos = newPos;
    }

    // set new speed to both steppers 
    if (newPos > 0) {
      stepperL.setMaxSpeed((float)newPos);
      stepperR.setMaxSpeed((float)newPos);
    }
  }
}

void logInfo() {
  if (printTime >= 1000) {
    printTime = 0;
    Serial.print("Left stepper (speed/max/position) -> ");
    Serial.print(stepperL.speed());
    Serial.print("/");
    Serial.print(stepperL.maxSpeed());
    Serial.print("/");
    Serial.println(stepperL.currentPosition());
    Serial.print("Right stepper (speed/position) -> ");
    Serial.print(stepperR.speed());
    Serial.print("/");
    Serial.println(stepperR.currentPosition());
    Serial.print("Current acceleration: ");
    Serial.println(stepperL.speed());
  }
}
