#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// BTS7960B Motor Driver Pinout
// =============================

// LEFT SIDE DRIVER
#define LL_EN 12   // Left Enable 1
#define LR_EN 13   // Left Enable 2
#define L_LPWM 27  // Left motor forward PWM
#define L_RPWM 14  // Left motor reverse PWM

// RIGHT SIDE DRIVER
#define RL_EN 25   // Right Enable 1
#define RR_EN 26   // Right Enable 2
#define R_LPWM 32  // Right motor forward PWM
#define R_RPWM 33  // Right motor reverse PWM

// =============================
String inputString = "";

// =============================
void setup() {
  Serial.begin(115200);
  SerialBT.begin("Damselete");
  Serial.println("Waiting for data...");

  // Set pin modes
  pinMode(LL_EN, OUTPUT);
  pinMode(LR_EN, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(L_RPWM, OUTPUT);

  pinMode(RL_EN, OUTPUT);
  pinMode(RR_EN, OUTPUT);
  pinMode(R_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);

  // Enable all drivers
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
  Serial.print("Raw Data: ");
  Serial.println(data);

  int forward = 0, backward = 0, left = 0, right = 0;

  int fIndex = data.indexOf('F');
  if (fIndex != -1) forward = data.substring(fIndex + 1, fIndex + 4).toInt();

  int bIndex = data.indexOf('B');
  if (bIndex != -1) backward = data.substring(bIndex + 1, bIndex + 4).toInt();

  int lIndex = data.indexOf('L');
  if (lIndex != -1) left = data.substring(lIndex + 1, lIndex + 4).toInt();

  int rIndex = data.indexOf('R');
  if (rIndex != -1) right = data.substring(rIndex + 1, rIndex + 4).toInt();

  Serial.print("FWD: "); Serial.print(forward);
  Serial.print(" | BWD: "); Serial.print(backward);
  Serial.print(" | LEFT: "); Serial.print(left);
  Serial.print(" | RIGHT: "); Serial.println(right);

  driveMotors(forward, backward, left, right);
}

int expoMap(int input) {
  float x = input / 255.0;  // normalize (0 to 1)

  float y;
  if (x < 0.7) {
    // gentle curve
    y = x * x;  
  } else {
    // aggressive boost after 70%
    y = 0.49 + (x - 0.7) * 1.7;  
  }

  return constrain(y * 255, 0, 255);
}

// =============================
void driveMotors(int forward, int backward, int left, int right) {
  int leftMotorSpeed = 0;
  int rightMotorSpeed = 0;

  int move = left - right;  // Positive = forward, Negative = backward
  int steer = forward - backward;       // Positive = turn right, Negative = turn left

  leftMotorSpeed = move - steer;
  rightMotorSpeed = move + steer;
  

  leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
  rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);

  // LEFT MOTOR CONTROL
  int leftPWM = abs(leftMotorSpeed);
  leftPWM = expoMap(leftPWM);
  
  if (leftMotorSpeed > 0) {
    analogWrite(L_LPWM, 0);
    analogWrite(L_RPWM, leftPWM);
  } else {
    analogWrite(L_LPWM, leftPWM);
    analogWrite(L_RPWM, 0);
  }
  
  // RIGHT MOTOR CONTROL
  int rightPWM = abs(rightMotorSpeed);
  rightPWM = expoMap(rightPWM);
  
  if (rightMotorSpeed > 0) {
    analogWrite(R_LPWM, 0);
    analogWrite(R_RPWM, rightPWM);
  } else {
    analogWrite(R_LPWM, rightPWM);
    analogWrite(R_RPWM, 0);
  }

  Serial.print("LeftMotor: "); Serial.print(leftMotorSpeed);
  Serial.print(" | RightMotor: "); Serial.println(rightMotorSpeed);
}
