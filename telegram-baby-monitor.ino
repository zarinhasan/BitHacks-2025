#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

const char* ssid = "BitHacks";
const char* password = "BitHacks2025!";

const int motionSensorPin = 5;  // GPIO pin for PIR sensor

WiFiClientSecure client;

const int motionThreshold = 3;            // How many detections to trigger alert
const unsigned long detectionWindow = 10000; // 10 seconds in milliseconds

int motionCount = 0;
unsigned long windowStartTime = 0;
bool windowActive = false;

const char* url = "https://maker.ifttt.com/trigger/baby_alert/with/key/4TUWnEL5R4bKs__Hlicbo";

void setup() {
  Serial.begin(115200);
  pinMode(motionSensorPin, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...");
  }

  client.setInsecure(); // Skip SSL certificate validation
  Serial.println("\nConnected to WiFi");
}

void loop() {
  int sensorState = digitalRead(motionSensorPin);

  if (sensorState == HIGH) {
    if (!windowActive) {
      windowActive = true;
      windowStartTime = millis();
      motionCount = 1;
    } else {
      motionCount++;
    }

    Serial.printf("Motion detected! Count: %d\n", motionCount);
    delay(100); 
  }

  // Check if time window has passed
  if (windowActive && (millis() - windowStartTime >= detectionWindow)) {
    if (motionCount >= motionThreshold) {
      Serial.println("Multiple motions detected — sending alert!");

      url_open();
      delay(10000); //10 sec buffer between alerts
    } else {
      Serial.println("Motion didn't meet threshold — no alert sent.");
    }

    // Reset tracking
    motionCount = 0;
    windowActive = false;
  }

  delay(50); // Avoid flooding the loop
}

void url_open() {
  HTTPClient http;
  http.begin(url);  // Can be HTTPS with BearSSL config if needed
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } 
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
