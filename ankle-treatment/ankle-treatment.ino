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
#define HOME_STEP 50

// Define rotary encoder pins for acceleration
#define ROTARY_CLK_PIN 11
#define ROTARY_DT_PIN 12
#define ROTARY_BUTTON_PIN 13
#define ROTARY_STEPS 10

// Common parameters for stepper motors (L-R)
#define SPEED_MIN 10
#define SPEED_MAX 500
#define SPEED 100
#define ACCEL 100

// Define max upper and lower position
#define POS_STOP 200

// Homing config
#define HOMING_MAX 6400 // Define max pulses for homing
#define HOMING_OFFSET 350 // Define steps after reach limit switch

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
int maxSpeed = SPEED_MAX;

// one second info printout timer
elapsedMillis printTime;
long lastPosL = 0;
long lastPosR = 0;

// stop signal from button one for each stepper
bool isStoppedL = true;
bool isStoppedR = true;
int currentDirL = 1; // 1 or -1 positive or negative for direction
int currentDirR = 1;

// target pos found
bool isMovingToTargetL = false;
bool isMovingToTargetR = false;

// Homing control one for each stepper
enum HomingStatus {Init, Searching, Touched, Reached, Home}; // Used to check limit switch position
ezButton buttonLimitL(HOME_LIMIT_L_PIN);
ezButton buttonLimitR(HOME_LIMIT_R_PIN);
HomingStatus homingStatusL = Init;
HomingStatus homingStatusR = Init;
bool isHomingL = true;
bool isHomingR = true;

void setup() {
  // Initialize for loggin purposes
  Serial.begin(115200);
  Serial.println("Starting setup...");

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
  Serial.println("Setup complete");
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
  if (buttonStart.isPressed() && (!isHomingL || !isHomingR)) {
    Serial.println("Start button pressed, start all");
    steppersStart();
  }

  // Stop button pressed then stop inmediately both motors
  if(buttonStop.isPressed() && (!isHomingL || !isHomingR)) {
    Serial.println("Stop button pressed, stopping both steppers");
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
    isMovingToTargetL = stepperL.run();
    currentDirL = (stepperL.currentPosition() > 0 ? 1 : -1);
    if (!isMovingToTargetL) {
      Serial.println("Changing left stepper direction");
      stepperL.moveTo(-stepperL.currentPosition());
    }
  }
  if (!isStoppedR) {
    isMovingToTargetR = stepperR.run();
    currentDirR = (stepperR.currentPosition() > 0 ? 1 : -1);
    if (!isMovingToTargetR) {
      Serial.println("Changing right stepper direction");
      stepperR.moveTo(-stepperR.currentPosition());
    }
  }
}

void initMovementL() {
  // Initialize stepper motor
  stepperL.setMaxSpeed(maxSpeed);
  stepperL.setAcceleration(ACCEL);
  stepperL.setSpeed(SPEED);
}

void initMovementR() {
  // Initialize stepper motor
  stepperR.setMaxSpeed(maxSpeed);
  stepperR.setAcceleration(ACCEL);
  stepperR.setSpeed(SPEED);
}

void steppersStart() {
  // Init and start movement
  isStoppedL = false;
  initMovementL();
  stepperL.moveTo(currentDirL * POS_STOP);

  isStoppedR = false;
  initMovementR();
  stepperR.moveTo(currentDirR * POS_STOP);
}

void stepperStopL() {
    Serial.println("Stopping left stepper");
    stepperL.stop();
    isStoppedL = true;
}

void stepperStopR() {
    Serial.println("Stopping rigth stepper");
    stepperR.stop();
    isStoppedR = true;
}

void stepperHomingL() {
  switch (homingStatusL) {
    case Init:
      Serial.println("Homing left stepper");
      Serial.println("Moving searching for limit switch");
      stepperL.setCurrentPosition(0);
      initMovementL();
      stepperL.moveTo(-HOMING_MAX); // always rotate clockwise
      isStoppedL = false;
      isHomingL = true;
      homingStatusL = Searching;
      break;
      
    case Searching:
      if (buttonLimitL.getState() == HIGH) {
        // limit switch touched
        Serial.println("Limit found, changing direction");
        stepperL.stop();
        stepperL.setCurrentPosition(0);
        initMovementL();
        stepperL.moveTo(HOMING_MAX); // counterclockwise
        homingStatusL = Touched;
      }
      break;

    case Touched:
      if (buttonLimitL.getState() == LOW) {
        Serial.println("Moving offset");
        stepperL.setCurrentPosition(0);
        initMovementL();
        stepperL.moveTo(HOMING_OFFSET);
        homingStatusL = Home;
      }
      break;

    case Home:
      // run() returns false when target pos found
      if (!isMovingToTargetL) {
        // homing ended then stop
        Serial.println("Homing procedure ended");
        stepperL.stop();
        stepperL.setCurrentPosition(0);
        homingStatusL = Init;
        isHomingL = false;
        isStoppedL = true;
        currentDirL = 1;
      }
      break;
      
    default:
      // error
      Serial.println("Left homing status invalid");
  }  
}

void stepperHomingR() {
  switch (homingStatusR) {
    case Init:
      Serial.println("Homing right stepper");
      Serial.println("Moving searching for limit switch");
      stepperR.setCurrentPosition(0);
      initMovementR();
      stepperR.moveTo(HOMING_MAX); // always rotate counterclockwise
      isStoppedR = false;
      isHomingR = true;
      homingStatusR = Searching;
      break;
      
    case Searching:
      if (buttonLimitR.getState() == HIGH) {
        // limit switch touched
        Serial.println("Limit found, changing direction");
        stepperR.stop();
        stepperR.setCurrentPosition(0);
        initMovementR();
        stepperR.moveTo(-HOMING_MAX); 
        homingStatusR = Touched;
      }
      break;

    case Touched:
      if (buttonLimitR.getState() == LOW) {
        Serial.println("Moving offset");
        stepperR.setCurrentPosition(0);
        initMovementR();
        stepperR.moveTo(-HOMING_OFFSET);
        homingStatusR = Home;
      }
      break;

    case Home:
      // run() returns false when target pos found
      if (!isMovingToTargetR) {
        // homing ended then stop
        Serial.println("Homing procedure ended");
        stepperR.stop();
        stepperR.setCurrentPosition(0);
        homingStatusR = Init;
        isHomingR = false;
        isStoppedR = true;
        currentDirR = 1;
      }
      break;
      
    default:
      // error
      Serial.println("Right homing status invalid");
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
      lastPos = newPos;
    }

    // set new speed to both steppers 
    if (newPos > 0) {
      Serial.print("Setting new acceleration (current/new): ");
      Serial.print(stepperL.acceleration());
      Serial.print("/");
      Serial.println(newPos);
      maxSpeed = newPos;
      stepperL.setMaxSpeed((float)maxSpeed);
      stepperR.setMaxSpeed((float)maxSpeed);
    }
  }
}

void logInfo() {
  if (printTime >= 1000) {
    printTime = 0;

    // print only if stepper is moving
    if (stepperL.currentPosition() != lastPosL) {
      lastPosL = stepperL.currentPosition();
      Serial.print("Left stepper (speed/max/position/target) -> ");
      Serial.print(stepperL.speed());
      Serial.print("/");
      Serial.print(stepperL.maxSpeed());
      Serial.print("/");
      Serial.print(stepperL.currentPosition());
      Serial.print("/");
      Serial.println(stepperL.targetPosition());
    }

    if (stepperR.currentPosition() != lastPosR) {
      lastPosR = stepperR.currentPosition();
      Serial.print("Right stepper (speed/max/position/target) -> ");
      Serial.print(stepperR.speed());
      Serial.print("/");
      Serial.print(stepperR.maxSpeed());
      Serial.print("/");
      Serial.print(stepperR.currentPosition());
      Serial.print("/");
      Serial.println(stepperR.targetPosition());
    }
  }
}
