#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_MIN 150
#define SERVO_MAX 600

int angleToPWM(int angle) { // converts the angle to the servo value will be good for easier syntax
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  Serial.begin(115200); 
  Wire.begin(18, 19);
  pwm.begin(); // start up the servo driver
  pwm.setPWMFreq(50); // needs to be 50hz remember for the main code before flashing
  delay(1000);
}

void testFLHip(){
  pwm.setPWM(FL_Hip, 0, angleToPWM(60));
  delay(1000);
  pwm.setPWM(FL_Hip, 0, angle to PWM(90));
  delay(1000);
  pwm.setPWM(FL_Hip, 0, angleToPWM(60));
  delay(1000);
}
void loop() {
  for (int ch = 0; ch < 8; ch++) { // tests individual servos
    Serial.print("Testing servo ");
    Serial.println(ch);
    pwm.setPWM(ch, 0, angleToPWM(0)); //set to zero so i dont break the servo
    delay(500);
    pwm.setPWM(ch, 0, angleToPWM(90)); // set to 90 
    delay(500);
    pwm.setPWM(ch, 0, angleToPWM(180)); // set to 180
    delay(500);
  }

  delay(1000);
  for (int angle = 60; angle <= 120; angle += 5) { // go from 60 to 120
    for (int ch = 0; ch < 8; ch++) {
      pwm.setPWM(ch, 0, angleToPWM(angle));
    }
    delay(50);
  }
  for (int angle = 120; angle >= 60; angle -= 5) { // go from 120 to 60
    for (int ch = 0; ch < 8; ch++) {
      pwm.setPWM(ch, 0, angleToPWM(angle));
    }
    delay(50);
  }
}