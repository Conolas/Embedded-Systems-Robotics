#include "DualWheelControl_JD.h"

// Constructor
DualWheelControl_JD::DualWheelControl_JD(int in_a1, int in_a2, int sleep, int in_b1, int in_b2){
    //Serial.begin(9600);
    Serial.println("Libreria iniciada");
    IN_A1 = in_a1;
    IN_A2 = in_a2;
    SLEEP = sleep;
    IN_B1 = in_b1;
    IN_B2 = in_b2;

    // Configurar Timer 0 (pines 5 y 6) a 490 Hz en modo Phase Correct PWM
    TCCR0A = (TCCR0A & B11111100) | B00000001;                          // Configura Timer 0 en Phase Correct PWM
    TCCR0B = (TCCR0B & B11111000) | B00000011;                          // Prescaler de 64 para Timer 0
    // Configurar Timer 1 (pines 9 y 10) a 490 Hz en modo Phase Correct PWM
    TCCR1B = (TCCR1B & B11111000) | B00000011;                          // Prescaler de 64 para Timer 1

    pinMode(IN_A1, OUTPUT);
    pinMode(IN_A2, OUTPUT);
    pinMode(IN_B1, OUTPUT);
    pinMode(IN_B2, OUTPUT);
    pinMode(SLEEP, OUTPUT);
}

void DualWheelControl_JD::sleepS(uint8_t estado){
    digitalWrite(SLEEP, estado);
}
void DualWheelControl_JD::detenido(){
    analogWrite(IN_B1, 0);            
    analogWrite(IN_B2, 0);
    analogWrite(IN_A1, 0);         
    analogWrite(IN_A2, 0);
}

void DualWheelControl_JD::adelante(int pwm){
    Serial.println("Adelante");
    analogWrite(IN_B1, 0);            
    analogWrite(IN_B2, pwm);
    analogWrite(IN_A1, pwm);         
    analogWrite(IN_A2, 0);
}


void DualWheelControl_JD::adelanteI(int pwm){
    /* El motor izquierdo gira con menos potencia para que el robot avance y gire a ese lado*/
    analogWrite(IN_B1, 0);            
    analogWrite(IN_B2, pwm);
    analogWrite(IN_A1, pwm/3);
    analogWrite(IN_A2, 0);
}

void DualWheelControl_JD::adelanteD(int pwm){
    /*El motor derecho gira con menos potencia para que el robot avance y gire a ese lado*/
    analogWrite(IN_B1, 0);
    analogWrite(IN_B2, pwm/3);
    analogWrite(IN_A1, pwm);         
    analogWrite(IN_A2, 0);
}

void DualWheelControl_JD::derecha(int pwm){
    analogWrite(IN_B1, pwm);         
    analogWrite(IN_B2, 0);
    analogWrite(IN_A1, pwm);          
    analogWrite(IN_A2, 0); 
}

void DualWheelControl_JD::izquierda(int pwm){
    analogWrite(IN_B1, 0);         
    analogWrite(IN_B2, pwm);
    analogWrite(IN_A1, 0);     
    analogWrite(IN_A2, pwm);
}

void DualWheelControl_JD::atras(int pwm)
{
    analogWrite(IN_B1, pwm);       
    analogWrite(IN_B2, 0);
    analogWrite(IN_A1, 0);      
    analogWrite(IN_A2, pwm);
}

void DualWheelControl_JD::atrasI(int pwm)
{
    analogWrite(IN_B1, pwm);       
    analogWrite(IN_B2, 0);
    analogWrite(IN_A1, 0);      
    analogWrite(IN_A2, pwm/3);
}

void DualWheelControl_JD::atrasD(int pwm)
{
    analogWrite(IN_B1, pwm/3);       
    analogWrite(IN_B2, 0);
    analogWrite(IN_A1, 0);      
    analogWrite(IN_A2, pwm);
}