

#include <AccelStepper.h>
#define REQUIRE_MEGA2560
#include "macros.h"
#include "pins_MKS_GEN_13.h"


#define STEP_PER_MM 50000

#define MOVEMENT_DELAY 10000

#define MAX_X 750
#define MAX_Y 450
#define MAX_Z 370

#define MAX_SPEED_X 10000 * STEP_PER_MM
#define MAX_ACCEL_X 50 * STEP_PER_MM
#define MIN_SPEED_X 10 * STEP_PER_MM
#define MIN_ACCEL_X 2 * STEP_PER_MM
#define HOMING_SPEED_X 10000 * STEP_PER_MM
#define HOMING_ACCEL_X 50 * STEP_PER_MM

#define MAX_SPEED_Y 1000 * STEP_PER_MM
#define MAX_ACCEL_Y 50 * STEP_PER_MM
#define MIN_SPEED_Y 20 * STEP_PER_MM
#define MIN_ACCEL_Y 5 * STEP_PER_MM
#define HOMING_SPEED_Y 20 * STEP_PER_MM
#define HOMING_ACCEL_Y 5 * STEP_PER_MM

#define HOMING_SLOW_DISTANCE 5000


unsigned long timing;

int CurrPosX = 0;
int CurrPosY = 0;
int CurrPosZ = 0;

int MOVtoPosX = 0;
int MOVtoPosY = 0;
int MOVtoPosZ = 0;



AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

void setup() {
  pinMode(X_MIN_PIN, INPUT);
  pinMode(Y_MIN_PIN, INPUT);

  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(X_CS_PIN, OUTPUT);


  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Y_CS_PIN, OUTPUT);

  homing();

  /*
  stepperX.setMaxSpeed(MAX_SPEED_X);
  stepperX.setAcceleration(MAX_ACCEL_X);
  stepperX.moveTo(MAX_X*STEP_PER_MM);

  stepperY.setMaxSpeed(MAX_SPEED_Y);
  stepperY.setAcceleration(MAX_ACCEL_Y);
  stepperY.moveTo(MAX_Y*STEP_PER_MM);*/
}

void loop() {
  /*int rand_x = random(MAX_X + 1);
    int rand_y = random(MAX_Y + 1);

    // Change direction at the limits

    stepperY.moveTo(rand_x);
    stepperY.moveTo(rand_y);
    */

  /* if (stepperX.distanceToGo() == 0)
    stepperZ.moveTo(-stepperZ.currentPosition());*/
  /*
  if (millis() - timing > MOVEMENT_DELAY) {
    if ((stepperX.distanceToGo() == 0) && (stepperY.distanceToGo() == 0)) {
      MOVtoPosX = rand() % (MAX_X - CurrPosX) + 1;
      MOVtoPosY = rand() % (MAX_Y - CurrPosY) + 1;

      stepperX.moveTo(MOVtoPosX);
      CurrPosX = CurrPosX + MOVtoPosX;
      stepperY.moveTo(MOVtoPosY);
      CurrPosY = CurrPosY + MOVtoPosY;
    }
    timing = millis();
  }
  stepperX.run();
  stepperY.run();*/
}


void homing() {

  stepperX.setMaxSpeed(HOMING_SPEED_X);
  stepperX.setAcceleration(HOMING_ACCEL_X);
  //stepperX.moveTo(-MAX_X*STEP_PER_MM);
  stepperX.moveTo(1000000000);
  while (!digitalRead(X_MIN_PIN)) {
    stepperX.run();
  }
  stepperX.stop();

  //stepperX.runToPosition();
  //stepperX.runToNewPosition(0);

  stepperX.setCurrentPosition(0);

  stepperX.setMaxSpeed(MIN_SPEED_X);
  stepperX.setAcceleration(MIN_ACCEL_X);
  stepperX.moveTo(-HOMING_SLOW_DISTANCE);
  do {
    stepperX.run();
  } while (stepperX.distanceToGo() != 0);
  stepperX.stop();
  //stepperX.runToPosition();
  stepperX.setCurrentPosition(0);

  stepperX.setMaxSpeed(MIN_SPEED_X);
  stepperX.setAcceleration(MIN_ACCEL_X);
  stepperX.moveTo(HOMING_SLOW_DISTANCE);
  do {
    stepperX.run();
  } while (stepperX.distanceToGo() != 0);
  stepperX.stop();
  // stepperX.runToPosition();
  stepperX.setCurrentPosition(0);




  stepperY.setMaxSpeed(HOMING_SPEED_Y);
  stepperY.setAcceleration(HOMING_ACCEL_Y);
  stepperY.moveTo(MAX_Y * STEP_PER_MM * (-1));

  do {
    stepperY.run();
  } while (digitalRead(Y_MIN_PIN));
  stepperY.stop();
  // stepperX.runToPosition();
  stepperY.setCurrentPosition(0);

  stepperY.setMaxSpeed(MIN_SPEED_Y);
  stepperY.setAcceleration(MIN_ACCEL_Y);
  stepperY.moveTo(HOMING_SLOW_DISTANCE);
  do {
    stepperY.run();
  } while (stepperY.distanceToGo() == 0);
  stepperY.stop();
  // stepperX.runToPosition();
  stepperY.setCurrentPosition(0);

  stepperY.moveTo(HOMING_SLOW_DISTANCE * (-1));
  do {
    stepperY.run();
  } while (digitalRead(Y_MIN_PIN));
  stepperY.stop();
  // stepperX.runToPosition();
  stepperY.setCurrentPosition(0);
}



//
/*
#define X_STEP_PIN                            54
#define X_DIR_PIN                             55
#define X_ENABLE_PIN                          38
#ifndef X_CS_PIN
  #define X_CS_PIN                            53
#endif

#define Y_STEP_PIN                            60
#define Y_DIR_PIN                             61
#define Y_ENABLE_PIN                          56
#ifndef Y_CS_PIN
  #define Y_CS_PIN                            49
#endif

#define X_MIN_PIN 

#define Y_MIN_PIN   
*/
