/*interrupts for communication
  Mega, Mega2560, MegaADK
  2, 3, 18, 19, 20, 21 (pins 20 & 21 are not available to use for interrupts while they are used for I2C communication)

  Mega
  INT.0   INT.1   INT.2   INT.3   INT.4   INT.5
  2       3       21      20      19      18

*/
//////////////////////////////////////////////////////////////////////////// drivers


//#define DRIVER_STEP_TIME 10  // меняем задержку на 10 мкс

//#define GS_FAST_PROFILE 10
#include "GyverStepper2.h"

#define MAX_X 62000
#define MAX_Y 22500

#define minRAND_X 62000/5
#define minRAND_Y 22500/3

#define HomingSPEED -5000

#define power_OFF_while_STANDING true

#define REQUIRE_MEGA2560
#include "macros.h"
#include "pins_MKS_GEN_13.h"

#define MAX_SPEED_X 10000
#define ACCEL_X 10000 

#define MAX_SPEED_Y 5000
#define ACCEL_Y 5000

long currPOS[2] = {0, 0}; // X,Y
long nextPOS[2] = {0, 0}; // X,Y
bool posDONE = false;

#define TIME_TO_WAIT  10000


const int pointAm = 30;     // количество точек в круге
int radius = 10000;           // радиус круга
int32_t pathCircle[pointAm + 2][2];     // буфер круга
// +1 на стартовую точку +1 на замыкание круга
int32_t circle_x_offset = 26000;
int32_t circle_y_offset = 0;

GStepper2<STEPPER2WIRE> stepperX(256, X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN);
GStepper2<STEPPER2WIRE> stepperY(256, Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN);
GStepper2<STEPPER2WIRE> stepperY2(256, Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN);

void setup() {

  pinMode(X_MIN_PIN, INPUT);
  pinMode(X_MAX_PIN, INPUT);
  pinMode(Y_MIN_PIN, INPUT);
  pinMode(Y_MAX_PIN, INPUT);
  pinMode(Z_MIN_PIN, INPUT);
  pinMode(Z_MAX_PIN, INPUT);


  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(X_CS_PIN, OUTPUT);
  pinMode(Y_CS_PIN, OUTPUT);
  pinMode(Z_CS_PIN, OUTPUT);
  pinMode(E0_CS_PIN, OUTPUT);
  pinMode(E1_CS_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);

  Serial.begin(9600);

  stepperX.setMaxSpeed(MAX_SPEED_X);     // скорость движения к цели
  stepperX.setAcceleration(ACCEL_X); // ускорение

  stepperY.setMaxSpeed(MAX_SPEED_Y);     // скорость движения к цели
  stepperY.setAcceleration(ACCEL_Y); // ускорение


  stepperY2.setMaxSpeed(MAX_SPEED_Y);     // скорость движения к цели
  stepperY2.setAcceleration(ACCEL_Y); // ускорение
  homing();

  for (int i = 0; i <= pointAm; i++) {
    pathCircle[i + 1][0] = radius + radius * cos(TWO_PI * i / pointAm) + circle_x_offset;
    pathCircle[i + 1][1] = radius + radius * sin(TWO_PI * i / pointAm) + circle_y_offset;
    //pathCircle[i + 1][2] = radius + radius * sin(TWO_PI * i / pointAm) *(-1);
  }
}

static uint32_t tmr;



int randX = 0;
int randY = 0;
bool onDemand = false;

int count = 0;  // счётчик точек маршрута




bool circle_run = false;
bool random_run = true;

static uint32_t tmr_to_circle, tmr_to_random;


void loop() {

  stepperX.tick();   // мотор асинхронно крутится тут
  stepperY.tick();   // мотор асинхронно крутится тут
  stepperY2.tick();

  if (onDemand) {
   
    if (stepperX.pos == nextPOS[0]  && stepperY.pos == nextPOS[1] && stepperY2.pos == -nextPOS[1]) {
      onDemand = false;
      if (power_OFF_while_STANDING) {
        stepperX.disable();
        stepperY.disable();
        stepperY2.disable();
      }
    }
  }

  if (millis() - tmr >= TIME_TO_WAIT) {

    tmr = millis();
    Serial.print(stepperX.pos);
    Serial.print(';');
    Serial.print(stepperY.pos);
    Serial.print(',');
    Serial.println(stepperY2.pos);



    if (onDemand == false) {

      if (random_run) {
        Serial.print("RANDOM GO TO : " );
        do{
        nextPOS[0] = random(0, MAX_X);
        }while(nextPOS[0]<minRAND_X);
        do{
        nextPOS[1] = random(0, MAX_Y);
        }while(nextPOS[1]<minRAND_Y);
      }

      /*Serial.print(nextPOS[0]);
        Serial.print(",");
        Serial.println(nextPOS[1]);*/
      /*stepperX.setTarget(MAX_X);
        stepperY.setTarget(MAX_Y);
        stepperY2.setTarget(-MAX_Y);*/
      if (circle_run) {
        nextPOS[0] = pathCircle[count][0];
        nextPOS[1] = pathCircle[count][1];
        if (++count > pointAm) count = 1;
      }
      stepperX.setTarget(nextPOS[0]);
      stepperY.setTarget(nextPOS[1]);
      stepperY2.setTarget(-nextPOS[1]);
      onDemand = true;
      if (power_OFF_while_STANDING) {
        stepperX.enable();
        stepperY.enable();
        stepperY2.enable();
      }
      //}
    }

  }
}


void homing() {
 Serial.println("Homing.....");
  Serial.println("Homing X");
  if (digitalRead(X_MIN_PIN)) {       // если концевик X не нажат
    stepperX.setSpeed(HomingSPEED);       // ось Х, -10 шаг/сек
    //stepperX.setTarget(800000);
    while (digitalRead(X_MIN_PIN)) {  // пока кнопка не нажата
      stepperX.tick();
    }
    // кнопка нажалась - покидаем цикл
    stepperX.brake();
  }

  stepperX.reset();


  Serial.println("X Home!");
  Serial.println("Homing Y");

  if (digitalRead(Y_MIN_PIN)) {       // если концевик X не нажат
    stepperY.setSpeed(HomingSPEED);       // ось Х, -10 шаг/сек
    stepperY2.setSpeed(-HomingSPEED);       // ось Х, -10 шаг/сек
    //stepperX.setTarget(800000);
    while (digitalRead(Y_MIN_PIN)) {  // пока кнопка не нажата
      stepperY.tick();// крутим
      stepperY2.tick();// крутим
    }
    // кнопка нажалась - покидаем цикл
    stepperY.brake(); // тормозим, приехали
    stepperY2.brake();
  }

  stepperY.reset();
  stepperY2.reset();
  Serial.println("Y Home!");

  Serial.println("All axes at home");
}
