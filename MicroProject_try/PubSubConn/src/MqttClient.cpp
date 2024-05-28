#include <WiFi.h>
#include <PubSubClient.h>

class MqttClient {
public:
    MqttClient(const char* broker, int port, const char* username, const char* password);
    void connect();
    void subscribe(const char* topic);
    void publish(const char* topic, const char* payload);
    void loop();

private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    const char* broker;
    int port;
    const char* username;
    const char* password;
    void callback(char* topic, byte* payload, unsigned int length);
};

MqttClient::MqttClient(const char* broker, int port, const char* username, const char* password)
    : mqttClient(wifiClient) {
    this->broker = broker;
    this->port = port;
    this->username = username;
    this->password = password;
    mqttClient.setServer(broker, port);
    mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });
}

void MqttClient::connect() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect("ESP32Client", username, password)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void MqttClient::subscribe(const char* topic) {
    mqttClient.subscribe(topic);
}

void MqttClient::publish(const char* topic, const char* payload) {
    mqttClient.publish(topic, payload);
}

void MqttClient::loop() {
    if (!mqttClient.connected()) {
        connect();
    }
    mqttClient.loop();
}

void MqttClient::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}
