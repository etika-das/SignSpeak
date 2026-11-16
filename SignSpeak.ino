#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// DFPlayer Mini
SoftwareSerial mp3Serial(10, 11); 
DFRobotDFPlayerMini player;

bool playerReady = false;

// Flex sensor pins
const int thumbPin  = A0;
const int indexPin  = A1;
const int middlePin = A2;
const int littlePin   = A3;

// Calibrated thresholds
const int thumbThreshold  = 276;
const int indexThreshold  = 145;
const int middleThreshold = 280;
const int littleThreshold   = 304;

// Previous detected sign
String previousSign = "";

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SignSpeak Ready");
  delay(1500);
  lcd.clear();

  // Start DFPlayer
  mp3Serial.begin(9600);
  delay(1500);

  lcd.setCursor(0, 0);
  lcd.print("Checking Audio");

  if (player.begin(mp3Serial)) {
    playerReady = true;
    player.volume(50);  // Volume: 0 to 30

    Serial.println("DFPlayer Connected!");
    lcd.setCursor(0, 1);
    lcd.print("Audio Ready");
  }
  else {
    Serial.println("DFPlayer Not Found!");
    lcd.setCursor(0, 1);
    lcd.print("Audio Error");
  }

  delay(1500);
  lcd.clear();
}

void loop() {

  int thumbValue  = analogRead(thumbPin);
  int indexValue  = analogRead(indexPin);
  int middleValue = analogRead(middlePin);
  int littleValue = analogRead(littlePin);

  // Your readings decrease when the fingers bend.
  bool thumbBent  = thumbValue < thumbThreshold;
  bool indexBent  = indexValue < indexThreshold;
  bool middleBent = middleValue < middleThreshold;
  bool littleBent   = littleValue < littleThreshold;

  // Display readings in Serial Monitor
  Serial.print("Thumb: ");
  Serial.print(thumbValue);
  Serial.print(" | Index: ");
  Serial.print(indexValue);
  Serial.print(" | Middle: ");
  Serial.print(middleValue);
  Serial.print(" | Little: ");
  Serial.println(littleValue);

  String detectedSign = "Unknown Sign";
  int audioNumber = 0;

  // Detect sign
  if (!thumbBent && !indexBent &&
      !middleBent && !littleBent) {

    detectedSign = "HELLO";
    audioNumber = 1;
  }
  else if (thumbBent && indexBent &&
           middleBent && littleBent) {

    detectedSign = "YES";
    audioNumber = 2;
  }
  else if (!thumbBent && indexBent &&
           middleBent && !littleBent) {

    detectedSign = "NO";
    audioNumber = 3;
  }
  else if (thumbBent && !indexBent &&
           !middleBent && !littleBent) {

    detectedSign = "THANK YOU";
    audioNumber = 4;
  }
 else if (thumbBent && indexBent &&
           !middleBent && !littleBent) {

    detectedSign = "Ok";
    audioNumber = 5;
  }
   else if (!thumbBent && indexBent &&
           !middleBent && !littleBent) {

    detectedSign = "I need food";
    audioNumber = 6;
  }
  // Display detected sign on LCD
  lcd.setCursor(0, 0);
  lcd.print("Detected Sign: ");

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(detectedSign);

  // Play voice only when the sign changes
  if (detectedSign != previousSign) {

    if (playerReady && audioNumber > 0) {
      player.playMp3Folder(audioNumber);
      Serial.print("Playing Audio: ");
      Serial.println(detectedSign);
    }

    previousSign = detectedSign;
  }

  delay(200);
}