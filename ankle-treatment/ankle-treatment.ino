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
#define HOME_LIMIT_PIN 9
#define HOME_STEP 300

// Define rotary encoder pins for acceleration
#define ROTARY_CLK_PIN 10
#define ROTARY_DT_PIN 11
#define ROTARY_BUTTON_PIN 12
#define ROTARY_STEPS 10

// Common parameters for stepper motors (L-R)
#define SPEED_MIN 100
#define SPEED_MAX 500
#define SPEED 100
#define ACCEL 100

// Define max upper and lower position
#define POS_STOP 700

// Create a new instance of the AccelStepper class for each stepper
AccelStepper stepperL = AccelStepper(AccelStepper::DRIVER, L_STEP_PIN, L_DIR_PIN);
AccelStepper stepperR = AccelStepper(AccelStepper::DRIVER, R_STEP_PIN, R_DIR_PIN);

// Create button instances
ezButton buttonStart(START_BUTTON_PIN);
ezButton buttonStop(STOP_BUTTON_PIN);
ezButton buttonHome(HOME_BUTTON_PIN);
ezButton buttonLimit(HOME_LIMIT_PIN);

// Create rotary encoder instance and rotary button
RotaryEncoder encoder(ROTARY_CLK_PIN, ROTARY_DT_PIN, RotaryEncoder::LatchMode::TWO03);
ezButton encoderButton(ROTARY_BUTTON_PIN);

// one second info printout timer
elapsedMillis printTime;

// stop signal from button
bool isStopped = true;

// Accel initial values from rotary
int lastPos = -1;

// Homing control
enum HomingStatus {Init, Searching, Touched, Reached}; // Used to check limit switch position
HomingStatus homingStatus = Init;
long initialHomingPos = 0;
bool isHoming = false;

void setup() {
  // Initialize for loggin purposes
  Serial.begin(115200);

  // Initialize buttons
  buttonStart.setDebounceTime(DEBOUNCE_INTERVAL);
  buttonStop.setDebounceTime(DEBOUNCE_INTERVAL);
  buttonHome.setDebounceTime(DEBOUNCE_INTERVAL);
  buttonLimit.setDebounceTime(DEBOUNCE_INTERVAL);

  // Initialize acceleration encoder
  encoderButton.setDebounceTime(DEBOUNCE_INTERVAL);
  encoder.setPosition(SPEED * ROTARY_STEPS);
}

void loop() {
  // Call loop to poll each button state
  buttonStart.loop();
  buttonStop.loop();
  buttonHome.loop();
  buttonLimit.loop();
  encoderButton.loop();

  // Loggin debug information from both steppers
  logInfo();

  // Poll changes for acceleration
  encoder.tick();
  
  // Set acceleration using encoder position
  setAcceleration();

  // Start button pressed then continue moving to next postition
  if (buttonStart.isPressed()) {
    stepperStart();
  }

  // Stop button pressed then stop inmediately both motors
  if(buttonStop.isPressed()) {
    stepperStop();
  } 
  
  // Home button pressed or homing is executing
  if (buttonHome.isPressed() || isHoming) {
    stepperHoming();
  }

  // Move to next position until end is reached
  if (!isStopped) {
    stepperL.run();
    stepperR.run();
  }

  // Change direction when end is reached
  if (!isHoming) {
    changeRotationDir();
  }
}

void initMovement() {
  // Initialize left stepper motor
  stepperL.setMaxSpeed(SPEED_MAX);
  stepperL.setAcceleration(ACCEL);
  stepperL.moveTo(POS_STOP);
  stepperL.setSpeed(SPEED);

  // Initialize right stepper motor
  stepperR.setMaxSpeed(SPEED_MAX);
  stepperR.setAcceleration(ACCEL);
  stepperR.moveTo(POS_STOP);
  stepperR.setSpeed(SPEED);
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
  isStopped = false;
  initMovement();
}

void stepperStop() {
    Serial.println("Stop button pressed, stop all");
    stepperL.stop();
    stepperR.stop();
    isStopped = true;
}

void stepperHoming() {
  int state = LOW;

  switch (homingStatus) {
    case Init:
      // Set Max Speed and Acceleration of each Steppers at startup for homing
      stepperL.setCurrentPosition(0);
      stepperL.setMaxSpeed(SPEED_MAX);
      //stepperL.setAcceleration(ACCEL);
      isStopped = false;
      isHoming = true;
      homingStatus = Searching;
      initialHomingPos = stepperL.currentPosition();
      break;
      
    case Searching:
      state = buttonLimit.getState();
      if (state == HIGH) {
        // limit switch touched
        homingStatus = Touched;
      }
      else {
        initialHomingPos+=HOME_STEP;
        stepperL.moveTo(initialHomingPos);
      }
      break;

    case Touched:
      stepperStop();
      stepperL.setMaxSpeed(SPEED_MAX);
      //stepperL.setAcceleration(ACCEL);
      isStopped = false;
      homingStatus = Reached;
      break;

    case Reached:
      state = buttonLimit.getState();
      if (state == LOW) {
        // homing ended then stop
        homingStatus = Init;
        isHoming = false;
        stepperStop();
        initMovement();
      }
      else {
        initialHomingPos+=HOME_STEP;
        stepperL.moveTo(-initialHomingPos);
      }
      break;

    default:
      // error
      Serial.println("Homing status invalid");
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
