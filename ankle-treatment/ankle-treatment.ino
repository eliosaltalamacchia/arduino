#include <AccelStepper.h>
#include <MultiStepper.h>
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
#define ROTARY_STEPS 5

// Common parameters for stepper motors (L-R)
#define SPEED_MIN 400
#define SPEED_MAX 600

// Define max upper and lower position
#define POS_STOP 250

// Homing config
#define HOMING_MAX 6400 // Define max steps for homing
#define HOMING_OFFSET 500 // Define steps after reach limit switch

// Create a new instance of the AccelStepper class for each stepper
MultiStepper steppers;
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
int speed = (SPEED_MAX - SPEED_MIN) / 2; // initial pos in the middle

// one second info printout timer
elapsedMillis printTime;
long lastPosL = 0;
long lastPosR = 0;

// stop signal from button one for each stepper
bool isStopped = true;
bool isMoving = false;
long newPos[] = {POS_STOP, POS_STOP};

// Homing control one for each stepper
enum HomingStatus {Init, Searching, Touched, Reached, Home}; // Used to check limit switch position
ezButton buttonLimitL(HOME_LIMIT_L_PIN);
ezButton buttonLimitR(HOME_LIMIT_R_PIN);
HomingStatus homingStatusL = Init;
HomingStatus homingStatusR = Init;
bool isHomingL = false;
bool isHomingR = false;

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

  // Initialize speed encoder
  encoderButton.setDebounceTime(DEBOUNCE_INTERVAL);
  encoder.setPosition(speed);

  // Initialize stepper positions & max speeds
  stepperL.setMaxSpeed(SPEED_MAX);
  stepperL.setCurrentPosition(0);
  stepperR.setMaxSpeed(SPEED_MAX);
  stepperR.setCurrentPosition(0);

  // Manage both steppers coordinated
  steppers.addStepper(stepperL);
  steppers.addStepper(stepperR);

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
  
  // Set speed using encoder position
  setSpeed();

  // Start button pressed then continue moving to next postition
  if (buttonStart.isPressed() && (!isHomingL || !isHomingR)) {
    Serial.println("Start button pressed, start all");
    steppersStart();
  }

  // Stop button pressed then stop inmediately both motors
  if(buttonStop.isPressed()) {
    Serial.println("Stop button pressed, stopping both steppers");
    steppersStop();
  } 
  
  // Home button pressed or homing is executing
  homingButtonPressed = buttonHome.isPressed();
  if (homingButtonPressed || isHomingL) {
    stepperHomingL();
  }
  if (homingButtonPressed || isHomingR) {
    stepperHomingR();
  }

  // Move left stepper to next position until stop or home unless homing
  if (!isStopped && (!isHomingL || !isHomingR)) {
    // move to next position
    isMoving = steppers.run();

    if (isMoving) {
      // check limit switches
      if (buttonLimitL.getState() == HIGH || buttonLimitR.getState() == HIGH) {
        Serial.println("Limit switch found - stop all");
        steppersStop();
      } 
    }
    else {
      // target position - change direction
      Serial.println("Target position - change direction");
      newPos[0] = stepperL.currentPosition() * -1;
      newPos[1] = stepperR.currentPosition() * -1;
      steppers.moveTo(newPos);
    }
  }
}

void steppersStart() {
  Serial.println("Starting steppers");
  isStopped = false;
  newPos[0] = POS_STOP;
  newPos[1] = POS_STOP;
  steppers.moveTo(newPos);
}

void steppersStop() {
  Serial.println("Stopping steppers");
  stepperL.stop();
  stepperR.stop();
  isStopped = true;
}

void stepperHomingL() {
  switch (homingStatusL) {
    case Init:
      Serial.println("Homing left stepper");
      Serial.println("Moving searching for limit switch");
      stepperL.move(-HOMING_MAX); // always rotate clockwise
      stepperL.setSpeed(speed);
      isStopped = false;
      isHomingL = true;
      homingStatusL = Searching;
      break;
      
    case Searching:
      if (buttonLimitL.getState() == HIGH) {
        // limit switch touched
        Serial.println("Limit found, changing direction");
        stepperL.move(HOMING_MAX); // counterclockwise
        stepperL.setSpeed(speed);
        homingStatusL = Touched;
      }
      break;

    case Touched:
      if (buttonLimitL.getState() == LOW) {
        Serial.println("Moving offset");
        stepperL.move(HOMING_OFFSET);
        stepperL.setSpeed(speed);
        homingStatusL = Home;
      }
      break;

    case Home:
      if (stepperL.distanceToGo() == 0) {
        // homing ended then stop
        Serial.println("Homing procedure ended");
        stepperL.setCurrentPosition(0);
        stepperL.setMaxSpeed(speed);
        homingStatusL = Init;
        isHomingL = false;
        isStopped = true;
      }
      break;
      
    default:
      // error
      Serial.println("Left homing status invalid");
  }

  // move to target
  if (isHomingL) {
    stepperL.runSpeedToPosition();
  }
}

void stepperHomingR() {
  switch (homingStatusR) {
    case Init:
      Serial.println("Homing right stepper");
      Serial.println("Moving searching for limit switch");
      stepperR.move(HOMING_MAX); // always rotate counterclockwise
      stepperR.setSpeed(speed);
      isStopped = false;
      isHomingR = true;
      homingStatusR = Searching;
      break;
      
    case Searching:
      if (buttonLimitR.getState() == HIGH) {
        // limit switch touched
        Serial.println("Limit found, changing direction");
        stepperR.move(-HOMING_MAX); 
        stepperR.setSpeed(speed);
        homingStatusR = Touched;
      }
      break;

    case Touched:
      if (buttonLimitR.getState() == LOW) {
        Serial.println("Moving offset");
        stepperR.move(-HOMING_OFFSET);
        stepperR.setSpeed(speed);
        homingStatusR = Home;
      }
      break;

    case Home:
      if (stepperR.distanceToGo() == 0) {
        // homing ended then stop
        Serial.println("Homing procedure ended");
        stepperR.stop();
        stepperR.setCurrentPosition(0);
        stepperR.setMaxSpeed(speed);
        homingStatusR = Init;
        isHomingR = false;
        isStopped = true;
      }
      break;
      
    default:
      // error
      Serial.println("Right homing status invalid");
  }  

  // move to target
  if (isHomingR) {
    stepperR.runSpeedToPosition();
  }
}

void setSpeed() {
  int newSpeed = SPEED_MIN;
  RotaryEncoder::Direction dir = RotaryEncoder::Direction::NOROTATION;

  // change speed if rotary moved
  dir = encoder.getDirection();
  if (dir != RotaryEncoder::Direction::NOROTATION) {
    if (dir == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
      newSpeed = speed + ROTARY_STEPS;
      if (newSpeed > SPEED_MAX) {
        newSpeed = SPEED_MAX;
      }
    } else if (dir == RotaryEncoder::Direction::CLOCKWISE) {
      newSpeed = speed - ROTARY_STEPS;
      if (newSpeed < SPEED_MIN) {
        newSpeed = SPEED_MIN;
      }
    }
    speed = newSpeed;

    Serial.print("Setting new speed (current/new): ");
    Serial.print(stepperL.speed());
    Serial.print("/");
    Serial.println(speed);
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
