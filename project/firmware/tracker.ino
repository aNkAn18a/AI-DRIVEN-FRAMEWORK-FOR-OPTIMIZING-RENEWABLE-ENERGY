// === BLYNK CREDENTIALS ===
#define BLYNK_TEMPLATE_ID "TMPL3NWLrIci8"
#define BLYNK_TEMPLATE_NAME "Solar Tracker"
#define BLYNK_AUTH_TOKEN "WI9QdiqYCdM3WaqHzPP7M8XgCLT4H0lM"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <ESP32Servo.h>

// === WiFi Credentials ===
char ssid[] = "YOUR_WIFI_NAME";      
char pass[] = "YOUR_WIFI_PASSWORD";  

// === Pin Definitions ===
#define LDR_LEFT 34
#define LDR_RIGHT 35
#define SERVO_PIN 13
#define ACS_PIN 32 

// === Settings ===
#define SERVO_INTERVAL 60    
#define DISPLAY_INTERVAL 250 
#define CLOUD_INTERVAL 1000  

// === OLED Setup ===
#define i2c_Address 0x3C 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1   
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo tracker;

// === Variables ===
int balanceOffset = 0;       
int servoPos = 90;   
int stepSize = 1;    
int threshold = 150; 

unsigned long lastServoTime = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastCloudTime = 0;

int leftVal = 0;
int rightVal = 0; 
int rawRight = 0; 
float currentA = 0.0;

void setup() {
  Serial.begin(115200);

  // Init Servo
  tracker.attach(SERVO_PIN);
  tracker.write(servoPos);

  // Init OLED
  delay(250);
  if(!display.begin(i2c_Address, true)) {
    Serial.println(F("SH1106 allocation failed"));
    for(;;);
  }
  
  // --- LDR AUTO CALIBRATION PHASE ---
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("CALIBRATING LDRs...");
  display.println("Shine equal light!");
  display.display();
  Serial.println("CALIBRATING LDRs... Please shine equal light on both LDRs for 3 seconds.");
  
  delay(3000); 
  
  long totalLeft = 0;
  long totalRight = 0;
  int samples = 20;

  for(int i = 0; i < samples; i++) {
    totalLeft += analogRead(LDR_LEFT);
    totalRight += analogRead(LDR_RIGHT);
    delay(100); 
  }
  
  int avgLeft = totalLeft / samples;
  int avgRight = totalRight / samples;
  balanceOffset = avgLeft - avgRight; 

  Serial.print("LDR Calibration Complete! Offset: ");
  Serial.println(balanceOffset);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting to WiFi...");
  display.display();

  // Init Blynk & WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Connected!");
  display.display();
  delay(1000);
}

// --- REAL ACS712 SENSOR READING (3.3V POWER) ---
float readCurrent() {
  int samples = 150;
  float totalAcsValue = 0;
  
  // 1. Take rapid multiple readings to average out sensor noise
  for (int i = 0; i < samples; i++) {
    totalAcsValue += analogRead(ACS_PIN);
  }
  
  float avgAcsValue = totalAcsValue / (float)samples;
  
  // 2. Convert the 12-bit ADC reading (0-4095) to Voltage (0 - 3.3V)
  float measuredVoltage = (avgAcsValue / 4095.0) * 3.3;
  
  // 3. Convert Voltage to Current (3.3V Math)
  // When VCC is 3.3V, the ACS712 rests at exactly 1.65V
  float zeroPointVoltage = 1.65; 
  
  // Sensitivity scales down when underpowered: 0.185 * (3.3 / 5.0) = 0.122
  float sensitivity = 0.122; 
  
  float realCurrent = (measuredVoltage - zeroPointVoltage) / sensitivity;
  
  // 4. Deadband filter to remove ambient electromagnetic noise
  if (abs(realCurrent) < 0.10) {
    realCurrent = 0.0;
  }
  
  return realCurrent;
}

void loop() {
  Blynk.run(); 
  
  unsigned long currentMillis = millis();

  // --- Task 1: Read & Balance Sensors ---
  leftVal  = analogRead(LDR_LEFT);
  rawRight = analogRead(LDR_RIGHT);
  rightVal = rawRight + balanceOffset; 

  // --- Task 2: Servo Logic (45 - 135 degrees) ---
  if (currentMillis - lastServoTime >= SERVO_INTERVAL) {
    lastServoTime = currentMillis;
    int diff = leftVal - rightVal;
    
    if (abs(diff) > threshold) {
      if (diff > 0) {
        if (servoPos > 45) servoPos -= stepSize; 
      } 
      else if (diff < 0) {
        if (servoPos < 135) servoPos += stepSize; 
      }
      tracker.write(servoPos);
    }
  }

  // --- Task 3: Update OLED & Sensors ---
  if (currentMillis - lastDisplayTime >= DISPLAY_INTERVAL) {
    lastDisplayTime = currentMillis;
    currentA = readCurrent();

    display.clearDisplay();
    display.setCursor(0,0);
    display.print("PV TRACKER - IOT");
    display.setCursor(0, 15);
    display.print("L:"); display.print(leftVal);
    display.print(" R:"); display.print(rightVal); 
    display.setCursor(0, 30);
    display.print("Ang: "); display.print(servoPos);
    display.setCursor(0, 45);
    display.setTextSize(2); 
    display.print(currentA, 2); display.print(" A");
    display.setTextSize(1);
    display.display();
  }

  // --- Task 4: Send to Blynk Cloud & Serial ---
  if (currentMillis - lastCloudTime >= CLOUD_INTERVAL) {
    lastCloudTime = currentMillis;
    
    Blynk.virtualWrite(V0, servoPos); 
    Blynk.virtualWrite(V1, currentA); 
    Blynk.virtualWrite(V2, leftVal);  
    Blynk.virtualWrite(V3, rightVal); 

    Serial.print("Left LDR: "); Serial.print(leftVal);
    Serial.print(" | Balanced Right: "); Serial.print(rightVal);
    Serial.print(" | Servo Angle: "); Serial.print(servoPos);
    Serial.print(" | Current: "); Serial.print(currentA);
    Serial.println(" A");
  }
}