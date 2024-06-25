#include <Arduino.h>
#include "MqttClient.h"

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT broker settings
const char* mqttBroker = "f66eae81.ala.dedicated.gcp.emqxcloud.com";
const int mqttPort = 1883; // Default MQTT port
const char* mqttUsername = "MiDio_bridge";
const char* mqttPassword = "Z5MLK2pXxDQfNbc";

// MQTT topics
const char* subscribeTopic = "fristphase";
const char* publishTopic = "Publish";
const char* publishMessage = "Hello from ESP32";

MqttClient mqttClient(mqttBroker, mqttPort, mqttUsername, mqttPassword);

void setup() {
    Serial.begin(115200);
    delay(1000); // Allow time for serial monitor to initialize
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected");
    mqttClient.connect();
    mqttClient.subscribe(subscribeTopic);
}

void loop() {
    mqttClient.loop();
    // Publish a message every 5 seconds
    mqttClient.publish(publishTopic, publishMessage);
    delay(5000);
}
