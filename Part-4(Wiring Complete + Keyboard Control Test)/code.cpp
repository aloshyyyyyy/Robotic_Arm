// Rusted Relay - 4-DOF arm, Phase 1: wired keyboard control
// Reads single-char commands over USB Serial and sweeps one servo per keypress.
// Requires ESP32Servo library (declared in platformio.ini lib_deps).

#include <Arduino.h>
#include <ESP32Servo.h>

// ---- Pin map (left-side GPIO header, avoiding input-only 34/35 and strapping pin 12) ----
const int PIN_BASE     = 13;
const int PIN_SHOULDER = 14;
const int PIN_ELBOW    = 27;
const int PIN_GRIPPER  = 26;

// ---- Tuning ----
const int STEP_DEG      = 5;   // total degrees moved per keypress
const int STEP_DELAY_MS = 25;  // delay per 1-degree increment - raise this for slower motion
const int MIN_ANGLE     = 0;
const int MAX_ANGLE     = 180;

Servo base, shoulder, elbow, gripper;

// current angle trackers - these track intended position, not necessarily the servo's
// actual physical position at boot, since nothing is written until the first keypress
int angBase = 90, angShoulder = 90, angElbow = 90, angGripper = 90;

// Sweeps smoothly toward the target, 1 degree at a time, instead of jumping instantly.
// direction: -1 or +1
void moveServoSmooth(Servo &servo, int &currentAngle, int direction) {
  int target = constrain(currentAngle + (direction * STEP_DEG), MIN_ANGLE, MAX_ANGLE);
  int step = (target > currentAngle) ? 1 : -1;

  while (currentAngle != target) {
    currentAngle += step;
    servo.write(currentAngle);
    delay(STEP_DELAY_MS);
  }

  Serial.print("angle -> ");
  Serial.println(currentAngle);
}

void setup() {
  Serial.begin(115200);

  // Allow allocation of all timers - needed for 4 simultaneous servo channels on ESP32
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  base.setPeriodHertz(50);
  shoulder.setPeriodHertz(50);
  elbow.setPeriodHertz(50);
  gripper.setPeriodHertz(50);

  base.attach(PIN_BASE, 500, 2400);
  shoulder.attach(PIN_SHOULDER, 500, 2400);
  elbow.attach(PIN_ELBOW, 500, 2400);
  gripper.attach(PIN_GRIPPER, 500, 2400);

  // No write() here on purpose - servos stay wherever they physically are until
  // the first keypress. Nothing moves on power-up.

  Serial.println("Arm ready. Controls:");
  Serial.println("  w/e = base left/right");
  Serial.println("  s/d = shoulder left/right");
  Serial.println("  x/c = elbow left/right");
  Serial.println("  r/t = gripper left/right");
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    switch (c) {
      case 'w': Serial.print("[base] ");     moveServoSmooth(base, angBase, -1); break;
      case 'e': Serial.print("[base] ");     moveServoSmooth(base, angBase, 1); break;

      case 's': Serial.print("[shoulder] "); moveServoSmooth(shoulder, angShoulder, -1); break;
      case 'd': Serial.print("[shoulder] "); moveServoSmooth(shoulder, angShoulder, 1); break;

      case 'x': Serial.print("[elbow] ");    moveServoSmooth(elbow, angElbow, -1); break;
      case 'c': Serial.print("[elbow] ");    moveServoSmooth(elbow, angElbow, 1); break;

      case 'r': Serial.print("[gripper] ");  moveServoSmooth(gripper, angGripper, -1); break;
      case 't': Serial.print("[gripper] ");  moveServoSmooth(gripper, angGripper, 1); break;

      default: break; // ignore newlines, unrelated keys
    }
  }
}
