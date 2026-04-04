#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// =============================
// BTS7960B Motor Driver Pinout
// =============================

// LEFT SIDE DRIVER
#define LL_EN 12
#define LR_EN 13
#define L_LPWM 27
#define L_RPWM 14

// RIGHT SIDE DRIVER
#define RL_EN 25
#define RR_EN 26
#define R_LPWM 32
#define R_RPWM 33

// =============================

// Direction correction (IMPORTANT)
#define LEFT_MOTOR_DIR  1
#define RIGHT_MOTOR_DIR -1   // <-- THIS FIXES YOUR ISSUE

String inputString = "";

// Smoothing variables
float smoothLeft = 0;
float smoothRight = 0;

// =============================
void setup() {
  Serial.begin(115200);
  SerialBT.begin("Damselete");

  pinMode(LL_EN, OUTPUT);
  pinMode(LR_EN, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(L_RPWM, OUTPUT);

  pinMode(RL_EN, OUTPUT);
  pinMode(RR_EN, OUTPUT);
  pinMode(R_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);

  digitalWrite(LL_EN, HIGH);
  digitalWrite(LR_EN, HIGH);
  digitalWrite(RL_EN, HIGH);
  digitalWrite(RR_EN, HIGH);
}

// =============================
void loop() {
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      parseCommand(inputString);
      inputString = "";
    } else {
      inputString += c;
    }
  }
}

// =============================
void parseCommand(String data) {
  int forward = 0, backward = 0, left = 0, right = 0;

  int fIndex = data.indexOf('F');
  if (fIndex != -1) forward = data.substring(fIndex + 1, fIndex + 4).toInt();

  int bIndex = data.indexOf('B');
  if (bIndex != -1) backward = data.substring(bIndex + 1, bIndex + 4).toInt();

  int lIndex = data.indexOf('L');
  if (lIndex != -1) left = data.substring(lIndex + 1, lIndex + 4).toInt();

  int rIndex = data.indexOf('R');
  if (rIndex != -1) right = data.substring(rIndex + 1, rIndex + 4).toInt();

  driveMotors(forward, backward, left, right);
}

// =============================
int expoMap(int input) {
  float x = input / 255.0;

  float y;
  if (x < 0.7) {
    y = x * x;
  } else {
    y = 0.49 + (x - 0.7) * 1.7;
  }

  return constrain(y * 255, 0, 255);
}

// =============================
void driveMotors(int forward, int backward, int left, int right) {

  int throttle = forward - backward;
  int steering = right - left;

  // Reduce steering sensitivity
  float speedFactor = abs(throttle) / 255.0;

// more steering at low speed, less at high speed
  float steeringGain = 0.8 - (speedFactor * 0.4);

  steering = steering * steeringGain;

  int leftMotorSpeed  = throttle + steering;
  int rightMotorSpeed = throttle - steering;

  // 🔥 APPLY DIRECTION FIX
  leftMotorSpeed  *= LEFT_MOTOR_DIR;
  rightMotorSpeed *= RIGHT_MOTOR_DIR;


  if (forward == 0 && backward == 0 && left == 0 && right == 0) {
  smoothLeft = 0;
  smoothRight = 0;
}
  // Smooth transitions
  smoothLeft  += (leftMotorSpeed - smoothLeft) * 0.2;
  smoothRight += (rightMotorSpeed - smoothRight) * 0.2;

  leftMotorSpeed  = smoothLeft;
  rightMotorSpeed = smoothRight;

  // Limit range
  leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
  rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);

  // =============================
  // LEFT MOTOR
  int leftPWM = expoMap(abs(leftMotorSpeed));

  if (leftMotorSpeed > 0) {
    analogWrite(L_LPWM, leftPWM);
    analogWrite(L_RPWM, 0);
  } else {
    analogWrite(L_LPWM, 0);
    analogWrite(L_RPWM, leftPWM);
  }

  // =============================
  // RIGHT MOTOR
  int rightPWM = expoMap(abs(rightMotorSpeed));

  if (rightMotorSpeed > 0) {
    analogWrite(R_LPWM, rightPWM);
    analogWrite(R_RPWM, 0);
  } else {
    analogWrite(R_LPWM, 0);
    analogWrite(R_RPWM, rightPWM);
  }
}