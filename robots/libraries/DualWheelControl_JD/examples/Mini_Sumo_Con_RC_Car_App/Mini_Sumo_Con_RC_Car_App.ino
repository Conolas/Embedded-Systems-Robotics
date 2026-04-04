/*
Codigo de ejemplo con uso de libreria DualWheelControl_JD, para control de los motores de un Mini sumo RC
controlado mediante bluetooth con una aplicacion desde el telefono RC_Car_App: https://drive.google.com/file/d/1NouDJMdqoghxc70ruhO8jQEZMHpRFjkC/view?usp=drive_link
dicho codigo es utilizado en la placa dreamfyre 2.0, pero puede adaptarse y utlizarse para otras placas y/o 
circuitos en protoboard.

Si deseas armar el circuito para mini sumo completo, necesitaras:
  * 1 Arduino Nano
  * 1 Driver de motor drv8833
  * 1 Modulo bluetooth hc-05 o hc-06
  * 2 motorreductores de corriente directa(dc)

creado por Juan Diego Cortes Facio
Ultima Actualizacion: 23/11/2024
correo electronico: juandiegocortes120802@gmail.com
*/ 


#include <Arduino.h>
#include <SoftwareSerial.h>                               // Incluimos la librería  SoftwareSerial  
#include <DualWheelControl_JD>
//#include "DualWheelControl_JD.h"                          // Libreria para control de motores


/* ------------------------------------Definicion de Pines -----------------------*/
// Para Bluetooth
#define RX 2
#define TX 3

// Para motor izquierdo
#define IN_A2 5
#define IN_A1 6

// Para motor derecho 
#define IN_B2 11
#define IN_B1 10 

#define SLEEP 7

SoftwareSerial BT(RX, TX);                                // Definimos los pines RX y TX del Arduino conectados al Bluetooth
DualWheelControl_JD MiniSumo(IN_A1, IN_A2, SLEEP, IN_B1, IN_B2);

// Variables
int velMax=130, velocidad=0, porcentaje;
char lectura;

void setup() {
  Serial.begin(9600);

  BT.begin(9600);                                         // Inicializamos el puerto serie BT
  // Filtro para modulos que meten ruido al arrancar
  if(BT.available())                                      // Si llega un dato por el puerto BT se envía al monitor serial
  {
    lectura = BT.read();                                  //leemos la opcion enviada

    if (isPrintable(lectura)) {
      Serial.print("Dato recibido: ");
      Serial.println(lectura);                            // Muestra el dato limpio en el monitor serial
    }
    else{
      Serial.print("Dato no imprimible");
      lectura = 'S';
    }
  }

  Serial.println("Prueba de lectura de Bluetooth...");

  MiniSumo.sleepS(HIGH);
  delay(3000);
}

void loop() {
  if(BT.available())                                      // Si llega un dato por el puerto BT se envía al monitor serial
  {
    lectura = BT.read();                                  //leemos la opcion enviada

    if (isPrintable(lectura)) {
      Serial.print("Dato recibido: ");
      Serial.println(lectura);                            // Muestra el dato limpio en el monitor serial
    }
    else{
      lectura = 'S';
    }
    delay(10);   
  }

  if(isDigit(lectura)){
    porcentaje = lectura - '0';                           // Convierte de char a int
    velocidad = map(porcentaje, 0, 9, 0, velMax);
  }
  else{
    switch (lectura) {
    case 'F':
      MiniSumo.adelante(velocidad);
      break;

    case 'B':
      MiniSumo.atras(velocidad);
      break;

    case 'L':
      MiniSumo.izquierda(velocidad);
      break;

    case 'R':
      MiniSumo.derecha(velocidad);
      break;

    case 'G':
      MiniSumo.adelanteI(velocidad);
      break;

    case 'I':
      MiniSumo.adelanteD(velocidad);
      break;

    case 'H':
      MiniSumo.atrasI(velocidad);
      break;

    case 'J':
      MiniSumo.atrasD(velocidad);
      break;

    case 'X':
      velMax = 255;
      break;

    case 'x':
      velMax = 130;
      break;

    default:
      MiniSumo.detenido();
      break;
    }
  }
}