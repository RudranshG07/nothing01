// @author Parth Kale
// Realtime inventory and stock management system

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "HX711.h"

HX711 scale;
uint8_t dataPin = 12;
uint8_t clockPin = 13;

#define WIFI_SSID "Nothing"
#define WIFI_PASSWORD "Something"
#define API_KEY "AIzaSyADNyd2opjxAIopiP4tn7el8AvuJoWNfBU"
#define DATABASE_URL "https://nothing-3d861-default-rtdb.asia-southeast1.firebasedatabase.app/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int BUTTON_PIN = D0;
int prev_button_state = HIGH;
int button_state;

int weight = 0;

void setup() {
  Serial.begin(115200);

  scale.begin(dataPin, clockPin);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(32, 10);
  display.println("Hello, world!");
  display.display();
  delay(800);

  /*Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
  Serial.println("\nEmpty the scale, press a key to continue");
  while (!Serial.available())
    ;
  while (Serial.available()) Serial.read();
  scale.tare();
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
  Serial.println("\nPut 1000 gram in the scale, press a key to continue");
  while (!Serial.available())
    ;
  while (Serial.available()) Serial.read();
  scale.calibrate_scale(132, 5);
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
  Serial.println("\nScale is calibrated, press a key to continue");
  while (!Serial.available())
    ;
  while (Serial.available()) Serial.read();*/

  scale.set_offset(4294910802);
  scale.set_scale(26.210611);
  scale.tare();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(44, 10);
    display.println("Connecting");
    display.display();
  }
  Serial.println(WiFi.localIP());
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
  weight = scale.get_units(10);
  if (weight < 0) {
    weight = 0;
  }
  Firebase.RTDB.setInt(&fbdo, "Device/PK17RG22", weight);

  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(24, 32-);
  display.println(weight);
  display.display();

  button_state = digitalRead(BUTTON_PIN);
  if (button_state == LOW) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(24, 24);
    display.println("Tare");
    display.display();
    scale.tare();
    Serial.println("Tare");
    delay(500);
  }
}
