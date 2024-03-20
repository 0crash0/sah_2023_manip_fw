
#define REQUIRE_MEGA2560
#include "macros.h"
#include "pins_MKS_GEN_13.h"

#define DRIVER_STEP_TIME 10  // меняем задержку на 10 мкс

// работу можно посмотреть в плоттере, а лучше в приложенном stepperPlot для Processing

int path2[][3] = {
  {100, 250, -250},
  {200, 30, -30},
  {300, 250, -250},
  {400, 100, -100},
  {500, 100, -100},
};


#define MAX_X 62000
#define MAX_Y 22500


#define MAX_SPEED 5000
#define ACCEL 5000
#define HomingSPEED -5000

long YHOME_PATH[3] =  { 0, -23000, 23000};

#include "GyverPlanner.h"
//#include "GyverStepper.h"

Stepper<STEPPER2WIRE> stepperX(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN);
Stepper<STEPPER2WIRE> stepperY(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN);
Stepper<STEPPER2WIRE> stepperY2(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN);
GPlanner<STEPPER2WIRE, 3> planner;


const int pointAm = 30;     // количество точек в круге
int radius = 10000;           // радиус круга
long pathCircle[pointAm + 2][3];     // буфер круга
// +1 на стартовую точку +1 на замыкание круга
int32_t circle_x_offset = 26000;
int32_t circle_y_offset = 0;



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

  Serial.begin(115200);
  // добавляем шаговики на оси
  planner.addStepper(0, stepperX);  // ось 0
  planner.addStepper(1, stepperY);  // ось 1
  planner.addStepper(2, stepperY2);  // ось 1

  // устанавливаем ускорение и скорость
  planner.setAcceleration(ACCEL);
  planner.setMaxSpeed(MAX_SPEED);
  //  planner.setRunMode(FOLLOW_POS);  //KEEP_SPEED

  // начальная точка системы должна совпадать с первой точкой маршрута
  //planner.setCurrent(path[0]);

  // заполняем буфер
  for (int i = 0; i <= pointAm; i++) {
    pathCircle[i + 1][0] = radius + radius * cos(TWO_PI * i / pointAm) + circle_x_offset;
    pathCircle[i + 1][1] = radius + radius * sin(TWO_PI * i / pointAm) + circle_y_offset;
    pathCircle[i + 1][2] = (radius + radius * sin(TWO_PI * i / pointAm) + circle_y_offset) * (-1);
  }
  // 0 - координата 0,0
  // 1 - первая координата круга
  // итд
  homing();
  /*planner.setTarget(path2);
    planner.start();*/
}


bool random_run = true;
bool circle_run = false;
bool pah_run = false;

int count = 0;  // счётчик точек маршрута
int circle_times = 0; //счетчик раз

static uint32_t tmr, tmr_circle;

void loop() {
  // здесь происходит движение моторов, вызывать как можно чаще
  planner.tick();


if (planner.ready()) {
      if (random_run) {
        int rndX = random(0, MAX_X);
        int rndY = random(0, MAX_Y);
        long rndGO[3] =  { rndX, rndY, -rndY};
        planner.setTarget(rndGO);
        Serial.println("RANDOM GO!" );
      }

      if (circle_run) {
        planner.setTarget(pathCircle[count]);
        if (++count > pointAm){ count = 1;
        circle_times++;
        }

      }
    }
  

  if(circle_times>=3){
    random_run = true;
    circle_run = false;
    pah_run = false;
  }
  


  // асинхронно вывожу в порт графики
  if (millis() - tmr_circle >= 200000) {
    random_run = false;
    circle_run = true;
    pah_run = false;
  }
  if (millis() - tmr >= 2000) {
    tmr = millis();
    
    Serial.print(planner.getTarget(0));
      Serial.print(',');
      Serial.print(planner.getTarget(1));
      Serial.print(',');
      Serial.print(planner.getTarget(2));
      Serial.print(',');
      Serial.print(stepperX.pos);
      Serial.print(',');
      Serial.println(stepperY.pos);
      Serial.print(',');
      Serial.println(stepperY2.pos);
  }
}





void homing() {
  Serial.println("Homing.....");
  Serial.println("Homing X");
  if (digitalRead(X_MIN_PIN)) {       // если концевик X не нажат
    planner.setSpeed(0, HomingSPEED);      // ось Х, -10 шаг/сек
    //stepperX.setTarget(800000);
    while (digitalRead(X_MIN_PIN)) {  // пока кнопка не нажата
      planner.tick();
    }
    // кнопка нажалась - покидаем цикл
    planner.brake();
  }

  planner.reset();


  Serial.println("X Home!");
  Serial.println("Homing Y");

  if (digitalRead(Y_MIN_PIN)) {       // если концевик X не нажат
    /*planner.setSpeed(1,HomingSPEED);       // ось Х, -10 шаг/сек
      planner.setSpeed(2,HomingSPEED);       // ось Х, -10 шаг/сек
    */
    planner.setTarget(YHOME_PATH);
    //stepperX.setTarget(800000);
    while (digitalRead(Y_MIN_PIN)) {  // пока кнопка не нажата
      planner.tickManual();// крутим
    }
    // кнопка нажалась - покидаем цикл
    planner.brake(); // тормозим, приехали
  }

  planner.reset();
  Serial.println("Y Home!");

  Serial.println("All axes at home");
}
