#pragma once                                                                                // Asegura que este archivo se incluya solo una vez
#include <Arduino.h>

class DualWheelControl_JD {
    public: 
    DualWheelControl_JD(int in_a1, int in_a2, int sleep, int in_b1, int in_b2);             // Constructor
    
    // Metodos para movimiento del robot
    void adelante(int pwm);
    void atras(int pwm);
    void derecha(int pwm);
    void izquierda(int pwm);
    void adelanteD(int pwm);
    void adelanteI(int pwm);
    void atrasD(int pwm);
    void atrasI(int pwm);
    void detenido();
    void sleepS(uint8_t estado);

    private:
    // Pines para los motores
    int IN_A1;                                                                              // Motor Izquierdo 
    int IN_A2;
    int IN_B1;                                                                              // Motor Derecho
    int IN_B2;
    int SLEEP;                                                                              // sleep (activa o desactiva el driver drv8833)
};