#include <Servo.h>
#include <MPU6050_light.h>
#include <Wire.h>
#include "HX711.h"

#define LOADCELL_DOUT_PING 39
#define LOADCELL_SCK_PING 37
#define LOADCELL_DOUT_PINB 33
#define LOADCELL_SCK_PINB 35
#define ButtonPin 8
#define EmergencyEndSwitchPin 2 // 

HX711 scaleG;
HX711 scaleB;

MPU6050 mpu(Wire);
Servo myServo;

int pos = 98;
unsigned long previousMillis_Servo = 0;
const long interval = 2000;
int angle = 98;

long timer = 0;
float calibration_factor_G = -107;
float calibration_factor_B = 257; //one needs to be changed
unsigned long lastWeightReadTime = 0; 
const long weightReadInterval = 2000;

bool emergencyStop = false;

void setup() {
  Serial.begin(9600); // Initialize serial communication

  pinMode(ButtonPin, INPUT_PULLUP);
  pinMode(EmergencyEndSwitchPin, INPUT_PULLUP); // Enable internal pull-up resistor

//servo setup
  myServo.attach(9);
  myServo.write(angle); 

   //Load cell setup
  Serial.println("HX711 scale demo");
  scaleG.begin(LOADCELL_DOUT_PING, LOADCELL_SCK_PING);
  scaleB.begin(LOADCELL_DOUT_PINB, LOADCELL_SCK_PINB);
  scaleG.set_scale(calibration_factor_G);
  scaleB.set_scale(calibration_factor_B);
  scaleG.tare();
  scaleB.tare();
  delay(1000);

//IMU setup
 Wire.begin();

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) { } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true, true); // gyro and accelero
  Serial.println("Done!\n"); 
}

void loop() {
  int ButtonState = digitalRead(ButtonPin); //reads pushbutton
    if (ButtonState == LOW) {
   if (digitalRead(EmergencyEndSwitchPin) == HIGH) { // reads emergency switch
    emergencyStop = true;
    myServo.detach(); // Stop the servo motor
    Serial.println("Emergency stop activated! Servo and readings stopped.");
  } else {
    if (emergencyStop) {
      myServo.attach(9); // Reattach the servo motor
      emergencyStop = false;
      Serial.println("Emergency stop released! Servo and readings resumed.");
    }
  }

  if (!emergencyStop) {*/
    unsigned long currentMillis_Loadcell = millis(); //Load cell readinsg
    if (currentMillis_Loadcell - lastWeightReadTime >= weightReadInterval) {
      float weightG = scaleG.get_units();
      float weightB = scaleB.get_units();
      Serial.print("Weight G= ");
      Serial.println(weightG);
      Serial.print("Weight B =");
      Serial.println(weightB);

      lastWeightReadTime = currentMillis_Loadcell;
    }

      unsigned long currentMillis_Servo = millis(); // Move servo
      if (currentMillis_Servo - previousMillis_Servo >= interval) {
        previousMillis_Servo = currentMillis_Servo;
        myServo.write(pos);
        pos -= 5;
        if (pos < 0) {
          pos = 98; // 
        }
      }
    }
//reads angle
    mpu.update();
    if (millis() - timer > 500) { 
      Serial.print("\tZ: ");
      Serial.println(mpu.getAngleZ());
      Serial.println(F("=====================================================\n"));
      timer = millis();
    }
 }
}
