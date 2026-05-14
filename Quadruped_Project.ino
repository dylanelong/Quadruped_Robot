#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PWMServoDriver.h>
#include <iostream>
#include <list>
#include <stdexcept>

//constants for the face
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int BLUE_START_Y = 16;
const int CENTER_Y = 40; 

// EXTREME SERVO VALS
#define SERVO_MIN 150
#define SERVO_MAX 600

//leg action angles, subject to change
#define HIP_REST 90
#define HIP_FORWARD 110
#define HIP_BACK 70

// Left side hips (Standard)
#define L_HIP_FORWARD 110
#define L_HIP_BACK 70

// Right side hips (Inverted!)
#define R_HIP_FORWARD 60
#define R_HIP_BACK 110

// Keep your existing knee constants
#define L_KNEE_UP   60   // If 60 makes it swing "in", try 120
#define L_KNEE_DOWN 100  // Neutral standing position

#define R_KNEE_UP   120  // Mirrored side often needs the opposite value
#define R_KNEE_DOWN 80
/* refer to this diagram for the leg designations
 FL     FR
  \__f__/
   |__b_|
  /     \
  BL    BR
*/

//servo numbers for the driver
const int FR_hip = 8;
const int FR_knee = 0;
const int FL_hip = 3;
const int FL_knee = 2;
const int BR_hip = 5;
const int BR_knee = 4;
const int BL_hip = 7;
const int BL_knee = 6;

//combinations of the legs for easier syntax later
const int legs  = 4;
std::array<std::array<int, 2>, legs> motors = {{
    {{FR_hip, FR_knee}},
    {{FL_hip, FL_knee}},
    {{BR_hip, BR_knee}},
    {{BL_hip, BL_knee}}
}};
int angleToPWM(int angle){ // converts the angle to a servo val
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void moveLegForward(int hipServo, int kneeServo, int hipAngle, int kneeAngle){
  pwm.setPWM(hipServo, 0, angleToPWM(hipAngle)); //move hip to angle first 
  pwm.setPWM(kneeServo, 0, angleToPWM(kneeAngle)); // then move knee to angle
}

//void testBLHip

void testLeg(int hipChannel, int kneeChannel) {
  // Lift leg
  pwm.setPWM(hipChannel, 0, angleToPWM(90));
  pwm.setPWM(kneeChannel, 0, angleToPWM(60));
  delay(500);

  // Move forward
  pwm.setPWM(hipChannel, 0, angleToPWM(110));
  delay(500);

  // Move backward
  pwm.setPWM(hipChannel, 0, angleToPWM(70));
  delay(500);

  // Return to center
  pwm.setPWM(hipChannel, 0, angleToPWM(90));
  pwm.setPWM(kneeChannel, 0, angleToPWM(90));
  delay(500);
}

void resetLegs(){
  for (int i =0; i < 8; i++){
    pwm.setPWM(i, 0, angleToPWM(90));
  }
}

void testFLKnee(){
  //go from obtuse to acute for walking fl leg
  pwm.setPWM(FL_knee, 0, angleToPWM(130));
  delay(1000);
  pwm.setPWM(FL_knee, 0, angleToPWM(90));
  delay(1000);
  pwm.setPWM(FL_knee, 0, angleToPWM(130));
  delay(1000);
}

void setServoStaggered(int channel, int angle) {
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  
  // Stagger the START of the pulse (0-4095 range)
  // Channel 0 starts at 0, Channel 1 starts at 512, etc.
  int phaseShift = channel * 512; 
  if (phaseShift >= 4096) phaseShift = 0; // Keep it within bounds

  int onTime = phaseShift;
  int offTime = (onTime + pulse) % 4096;
  
  pwm.setPWM(channel, onTime, offTime);
}
void turnRight() {
  int turnAngle = 15; 
  
  for (int i = 0; i < 2; i++) { 
    setServoStaggered(FL_knee, L_KNEE_UP - 10); 
    setServoStaggered(BR_knee, R_KNEE_UP - 10);
    delay(150);
    setServoStaggered(FL_hip, 90 + turnAngle); 
    setServoStaggered(BR_hip, 90 + turnAngle); 
    setServoStaggered(FR_hip, 90 + turnAngle); 
    setServoStaggered(BL_hip, 90 + turnAngle); 
    delay(200);
    setServoStaggered(FL_knee, L_KNEE_DOWN);
    setServoStaggered(BR_knee, R_KNEE_DOWN);
    delay(150);
    setServoStaggered(FR_knee, R_KNEE_UP - 10);
    setServoStaggered(BL_knee, L_KNEE_UP - 10);
    delay(150);
    setServoStaggered(FL_hip, 90); 
    setServoStaggered(BR_hip, 90); 
    setServoStaggered(FR_hip, 90); 
    setServoStaggered(BL_hip, 90); 
    delay(200);
    setServoStaggered(FR_knee, R_KNEE_DOWN);
    setServoStaggered(BL_knee, L_KNEE_DOWN);
    delay(150);
  }
}

void turnLeft() {
  // Mirror of TurnRight logic
  setServoStaggered(FL_knee, L_KNEE_UP);
  setServoStaggered(BR_knee, R_KNEE_UP);
  delay(150);

  // Swing all hips to 70 for Counter-Clockwise
  setServoStaggered(FL_hip, 70); 
  setServoStaggered(BR_hip, 70); 
  setServoStaggered(FR_hip, 70); 
  setServoStaggered(BL_hip, 70); 
  delay(300);

  setServoStaggered(FL_knee, L_KNEE_DOWN);
  setServoStaggered(BR_knee, R_KNEE_DOWN);
  delay(150);

  setServoStaggered(FR_knee, R_KNEE_UP);
  setServoStaggered(BL_knee, L_KNEE_UP);
  delay(150);

  setServoStaggered(FL_hip, 110); 
  setServoStaggered(BR_hip, 110); 
  setServoStaggered(FR_hip, 110); 
  setServoStaggered(BL_hip, 110); 
  delay(300);

  setServoStaggered(FR_knee, R_KNEE_DOWN);
  setServoStaggered(BL_knee, L_KNEE_DOWN);
  delay(150);
}
void walkForward() {
  
  pwm.setPWM(FL_knee, 0, angleToPWM(L_KNEE_UP));
  pwm.setPWM(BR_knee, 0, angleToPWM(R_KNEE_UP));
  delay(250); 

 
  pwm.setPWM(FL_hip, 0, angleToPWM(L_HIP_FORWARD)); 
  pwm.setPWM(BR_hip, 0, angleToPWM(R_HIP_FORWARD)); 
  
 
  pwm.setPWM(FR_hip, 0, angleToPWM(90 + 10)); 
  pwm.setPWM(BL_hip, 0, angleToPWM(L_HIP_BACK));    
  //watch weak hip moves here 
  delay(400); 
  
  pwm.setPWM(FL_knee, 0, angleToPWM(L_KNEE_DOWN));
  pwm.setPWM(BR_knee, 0, angleToPWM(R_KNEE_DOWN));
  delay(150);

  pwm.setPWM(FR_knee, 0, angleToPWM(R_KNEE_UP));
  pwm.setPWM(BL_knee, 0, angleToPWM(L_KNEE_UP));
  delay(150);


  pwm.setPWM(FR_hip, 0, angleToPWM(R_HIP_FORWARD)); 
  pwm.setPWM(BL_hip, 0, angleToPWM(L_HIP_FORWARD)); 

  pwm.setPWM(FL_hip, 0, angleToPWM(L_HIP_BACK));    
  pwm.setPWM(BR_hip, 0, angleToPWM(R_HIP_BACK));    
  
  delay(250);

  pwm.setPWM(FR_knee, 0, angleToPWM(R_KNEE_DOWN));
  pwm.setPWM(BL_knee, 0, angleToPWM(L_KNEE_DOWN));
  delay(150);
}
void walkBackward() {
  // --- STEP 1: Lift Diagonal Pair 1 (Front-Left & Back-Right) ---
  setServoStaggered(FL_knee, L_KNEE_UP);
  setServoStaggered(BR_knee, R_KNEE_UP);
  delay(200);

  // --- STEP 2: Swing Pair 1 BACK (In Air), Push Pair 2 FORWARD (On Ground) ---
  // Legs moving toward the tail
  setServoStaggered(FL_hip, L_HIP_BACK); 
  setServoStaggered(BR_hip, R_HIP_BACK); 
  
  // Legs pushing against the floor toward the head
  setServoStaggered(FR_hip, R_HIP_FORWARD);    
  setServoStaggered(BL_hip, L_HIP_FORWARD);    
  
  Serial.println("Backward: Pair 1 Swinging Back | Pair 2 Pushing Forward");
  delay(300);

  // --- STEP 3: Lower Pair 1 ---
  setServoStaggered(FL_knee, L_KNEE_DOWN);
  setServoStaggered(BR_knee, R_KNEE_DOWN);
  delay(200);

  // --- STEP 4: Lift Diagonal Pair 2 (Front-Right & Back-Left) ---
  setServoStaggered(FR_knee, R_KNEE_UP);
  setServoStaggered(BL_knee, L_KNEE_UP);
  delay(200);

  // --- STEP 5: Swing Pair 2 BACK (In Air), Push Pair 1 FORWARD (On Ground) ---
  // Legs moving toward the tail
  setServoStaggered(FR_hip, R_HIP_BACK); 
  setServoStaggered(BL_hip, L_HIP_BACK); 
  
  // Legs pushing against the floor toward the head
  setServoStaggered(FL_hip, L_HIP_FORWARD);    
  setServoStaggered(BR_hip, R_HIP_FORWARD);    
  
  Serial.println("Backward: Pair 2 Swinging Back | Pair 1 Pushing Forward");
  delay(300);

  // --- STEP 6: Lower Pair 2 ---
  setServoStaggered(FR_knee, R_KNEE_DOWN);
  setServoStaggered(BL_knee, L_KNEE_DOWN);
  delay(200);
}
void setup() {
  Serial.begin(115200); 
  Wire.begin(18, 19); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.display(); 
  pwm.begin();
  pwm.setPWMFreq(50);
  resetLegs();
  delay(1000);
  Serial.println("Setup Complete");
}

void loop() {
  //resetLegs();
  //testLeg(FR_hip, FR_knee);
  //testLeg(FR_hip, FR_knee);
  //for(int i = 0; i < 10; i++) walkForward();
  for(int i = 0; i < 10; i++) walkBackward();
  //delay(500); 
  //drawface(0); 
}

void drawPochita(int state) {
  display.clearDisplay();

  int leftEyeX = 35;
  int rightEyeX = 93;
  int eyeY = CENTER_Y - 2; 

  // 1. EYES (Normal/Happy or Blinking)
  if (state == 0 || state == 1) { 
    display.fillCircle(leftEyeX, eyeY, 15, SSD1306_WHITE);
    display.fillCircle(rightEyeX, eyeY, 15, SSD1306_WHITE);
    
    display.fillCircle(leftEyeX, eyeY, 6, SSD1306_BLACK);
    display.fillCircle(rightEyeX, eyeY, 6, SSD1306_BLACK);
    
    // Tiny white highlight
    display.fillCircle(leftEyeX - 3, eyeY - 3, 2, SSD1306_WHITE);
    display.fillCircle(rightEyeX - 3, eyeY - 3, 2, SSD1306_WHITE);
  } 
  else if (state == 2) { 
    display.fillRoundRect(leftEyeX - 15, eyeY - 2, 30, 5, 2, SSD1306_WHITE);
    display.fillRoundRect(rightEyeX - 15, eyeY - 2, 30, 5, 2, SSD1306_WHITE);
  }
  display.fillTriangle(64, eyeY + 8, 61, eyeY + 4, 67, eyeY + 4, SSD1306_WHITE);
  int mouthY = eyeY + 11;
    display.drawCircle(60, mouthY, 4, SSD1306_WHITE);
    display.drawCircle(68, mouthY, 4, SSD1306_WHITE);
    display.fillRect(54, mouthY - 5, 22, 5, SSD1306_BLACK); 
  display.display();
}

void identifyMotors() {
  int testChannels[] = {FR_hip, FR_knee, FL_hip, FL_knee, BR_hip, BR_knee, BL_hip, BL_knee};
  String names[] = {"FR Hip", "FR Knee", "FL Hip", "FL Knee", "BR Hip", "BR Knee", "BL Hip", "BL Knee"};

  for (int i = 0; i < 8; i++) {
    Serial.print("Testing Channel ");
    Serial.print(testChannels[i]);
    Serial.print(": ");
    Serial.println(names[i]);

    // Wiggle the motor
    pwm.setPWM(testChannels[i], 0, angleToPWM(110));
    delay(500);
    pwm.setPWM(testChannels[i], 0, angleToPWM(70));
    delay(500);
    pwm.setPWM(testChannels[i], 0, angleToPWM(90)); // Return to center
    delay(1000); // Pause so you can write down which leg moved
  }
}
/*
Scanner for memory address
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial); 
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found\n");
  delay(5000);
}
*/