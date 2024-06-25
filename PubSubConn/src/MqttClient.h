#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

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

#endif
