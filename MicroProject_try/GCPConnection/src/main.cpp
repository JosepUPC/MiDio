#include "GCPClient.h"
#include <Arduino.h>

const char* ssid = "MIWIFI_5G_sfhx";
const char* password = "gJmjjuNe";

const char* topicName = "iot-topic";
const char* jsonKeyPath = "/gcp-key.json";  // Path to the JSON key file in SPIFFS
const char* projectid = "midio-16011998";                               // Your project id


GCPClient gcpClient(topicName);

void setup() {
    Serial.begin(115200);
    gcpClient.begin(ssid, password, jsonKeyPath);
    if (gcpClient.connect()) {
        Serial.println("Successfully connected to GCP Pub/Sub!");
    } else {
        Serial.println("Failed to connect to GCP Pub/Sub.");
    }
}

void loop() {
    gcpClient.loop();
    static unsigned long lastMillis = 0;
    if (millis() - lastMillis > 10000) {
        lastMillis = millis();
        gcpClient.publishMessage("Hello from ESP32-S3!");
    }
}