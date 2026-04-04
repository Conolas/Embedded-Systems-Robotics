#include "DualWheelControl_JD.h"

// Usaremos los pines para controlar un DRV8833
// Para motor izquierdo
#define IN_A2 5
#define IN_A1 6

// Para motor derecho 
#define IN_B2 11
#define IN_B1 10 

#define SLEEP 7
DualWheelControl_JD robot(IN_A1, IN_A2, SLEEP, IN_B1, IN_B2);

void setup() {
  Serial.begin(9600);
  Serial.println("Prueba de libreria para motor");
  robot.sleepS(1);
  delay(2000);
}

void loop() {
  robot.adelante(50);
  delay(1000);
  robot.detenido();
  delay(1000);

}
