#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <AceButton.h>
using namespace ace_button;

const char* ssid = "f.xng_gxr";
const char* password = "gorfang100553";
#define API_KEY "AIzaSyADo5wyzjDa4-EPrltkqKAl5XkaE61b2U0"
#define DATABASE_URL "https://miniproject0217-9d777-default-rtdb.firebaseio.com/"
#define USER_EMAIL "fxnggorr2009@gmail.com"
#define USER_PASSWORD "miniproject0217"

const int buttonPin1 = 25;
const int led_room1 = 32;
const int ldrPin = 34;
const int led_outhouse1 = 26;
const int led_outhouse2 = 27;
const int irPin_top = 15;
const int irPin_bottom = 4;
const int led_stairs1 = 16, led_stairs2 = 17, led_stairs3 = 5, led_stairs4 = 18, led_stairs5 = 19;

int threshold = 2000;
bool ledState1 = LOW;
unsigned long lastLdrPrint = 0; 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
AceButton button1(buttonPin1);

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  if (button->getPin() == buttonPin1 && eventType == AceButton::kEventPressed) {
    ledState1 = !ledState1;
    digitalWrite(led_room1, ledState1);
    if (Firebase.ready()) Firebase.RTDB.setBool(&fbdo, "/led1", ledState1);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(led_room1, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(led_outhouse1, OUTPUT);
  pinMode(led_outhouse2, OUTPUT);
  pinMode(irPin_top, INPUT);
  pinMode(irPin_bottom, INPUT);
  pinMode(led_stairs1, OUTPUT); pinMode(led_stairs2, OUTPUT);
  pinMode(led_stairs3, OUTPUT); pinMode(led_stairs4, OUTPUT);
  pinMode(led_stairs5, OUTPUT);

  ButtonConfig* buttonConfig1 = button1.getButtonConfig();
  buttonConfig1->setEventHandler(handleEvent);
  buttonConfig1->setFeature(ButtonConfig::kFeatureClick);

  Serial.println("System starting... (Sensors are active)");

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi in background...");
  
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  button1.check();

  int ldrValue = analogRead(ldrPin);
  if (millis() - lastLdrPrint > 1000) {
    Serial.print("LDR Value: ");
    Serial.println(ldrValue);
    lastLdrPrint = millis();
  }

  if (ldrValue > threshold) {
    digitalWrite(led_outhouse1, HIGH);
    digitalWrite(led_outhouse2, HIGH);
  } else {
    digitalWrite(led_outhouse1, LOW);
    digitalWrite(led_outhouse2, LOW);
  }

  int irValueTop = digitalRead(irPin_top);
  int irValueBottom = digitalRead(irPin_bottom);

  if (irValueBottom == LOW) { 
    digitalWrite(led_stairs1, HIGH); delay(300);
    digitalWrite(led_stairs2, HIGH); delay(300);
    digitalWrite(led_stairs3, HIGH); delay(300);
    digitalWrite(led_stairs1, LOW);  delay(300);
    digitalWrite(led_stairs4, HIGH); delay(300);
    digitalWrite(led_stairs2, LOW);  delay(300);
    digitalWrite(led_stairs5, HIGH); delay(300);
    digitalWrite(led_stairs3, LOW);  delay(300);
    digitalWrite(led_stairs4, LOW);  delay(300);
    digitalWrite(led_stairs5, LOW);
  } 

  if (irValueTop == LOW) {
    digitalWrite(led_stairs5, HIGH); delay(300);
    digitalWrite(led_stairs4, HIGH); delay(300);
    digitalWrite(led_stairs3, HIGH); delay(300);
    digitalWrite(led_stairs5, LOW);  delay(300);
    digitalWrite(led_stairs2, HIGH); delay(300);
    digitalWrite(led_stairs4, LOW);  delay(300);
    digitalWrite(led_stairs1, HIGH); delay(300);
    digitalWrite(led_stairs3, LOW);  delay(300);
    digitalWrite(led_stairs2, LOW);  delay(300);
    digitalWrite(led_stairs1, LOW);
  }

  if (irValueTop == HIGH && irValueBottom == HIGH) {
    digitalWrite(led_stairs1, LOW); digitalWrite(led_stairs2, LOW);
    digitalWrite(led_stairs3, LOW); digitalWrite(led_stairs4, LOW);
    digitalWrite(led_stairs5, LOW);
  }

  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    static unsigned long lastFbCheck = 0;
    if (millis() - lastFbCheck > 2000) { 
      if (Firebase.RTDB.getBool(&fbdo, "/led1")) {
        ledState1 = fbdo.boolData();
        digitalWrite(led_room1, ledState1);
      }
      lastFbCheck = millis();
    }
  }

  delay(10); 
}