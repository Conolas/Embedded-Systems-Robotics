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

// Direction correction
#define LEFT_MOTOR_DIR  1
#define RIGHT_MOTOR_DIR -1

String inputString = "";

// Smoothing variables (UNCHANGED)
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
  int steeringInput = right - left;

  // Detect curve condition
  bool isCurve = (throttle != 0 && (left > 0 || right > 0));

  // =============================
  // 🔥 FIX 1: Instant steering control (NO smoothing lag)
  int steering = steeringInput;

  // Kill residual steering instantly
  if (left == 0 && right == 0) {
    steering = 0;
  }

  // =============================
  // 🔥 FIX 2: Precision curve control (ONLY during curves)
  if (isCurve) {
    float s = steering / 255.0;

    // cubic expo → very precise at small input, strong at full
    s = s * s * s;

    steering = s * 255;
  }

  // =============================
  // Original dynamic steering (unchanged feel)
  float speedFactor = abs(throttle) / 255.0;
  float steeringGain = 0.8 - (speedFactor * 0.4);
  steering *= steeringGain;

  // =============================
  // Motor mixing
  int leftMotorTarget  = throttle + steering;
  int rightMotorTarget = throttle - steering;

  // Direction correction
  leftMotorTarget  *= LEFT_MOTOR_DIR;
  rightMotorTarget *= RIGHT_MOTOR_DIR;

  // =============================
  // 🔥 ORIGINAL SMOOTHING (UNCHANGED - only affects throttle behavior feel)
  float accelRate = 0.2;
  float decelRate = 0.5;

  smoothLeft += (leftMotorTarget - smoothLeft) * 
               (abs(leftMotorTarget) > abs(smoothLeft) ? accelRate : decelRate);

  smoothRight += (rightMotorTarget - smoothRight) * 
                (abs(rightMotorTarget) > abs(smoothRight) ? accelRate : decelRate);

  int leftMotorSpeed  = smoothLeft;
  int rightMotorSpeed = smoothRight;

  // =============================
  // 🔥 FIX 3: FORCE straight when steering released (CRITICAL)
  if (throttle != 0 && left == 0 && right == 0) {
    leftMotorSpeed  = throttle * LEFT_MOTOR_DIR;
    rightMotorSpeed = throttle * RIGHT_MOTOR_DIR;

    smoothLeft  = leftMotorSpeed;
    smoothRight = rightMotorSpeed;
  }

  if(left == 0 && right == 0)
  {
    steering = 0;
  }

  if(left == 0 && right == 0 && forward == 0 && backward == 0)
  {
    smoothLeft = 0;
    smoothRight = 0;

    leftMotorSpeed = 0;
    rightMotorSpeed = 0;
  }

  // =============================
  // 🔥 Reduce peak ONLY during curves (extra 15–20%)
  if (isCurve) {
    float turnIntensity = abs(steering) / 255.0;

    // earlier was 0.30 → now ~0.45 (stronger reduction)
    float turnScale = 1.0 - (turnIntensity * 0.30);

    leftMotorSpeed  *= turnScale;
    rightMotorSpeed *= turnScale;
  }

  // =============================
  // Limit range
  leftMotorSpeed  = constrain(leftMotorSpeed, -255, 255);
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