#ifndef GCPCLIENT_H
#define GCPCLIENT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <time.h>
#include "FS.h"
#include "SPIFFS.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"

class GCPClient {
public:
    GCPClient(const char* topicName);
    void begin(const char* ssid, const char* password, const char* jsonKeyPath);
    bool connect();
    void loop();
    void publishMessage(const char* message);

private:
    const char* topicName;
    WiFiClientSecure netClient;
    PubSubClient client;
    String projectId;
    String privateKey;
    String clientEmail;
    String getJwt();
    void setupWiFi(const char* ssid, const char* password);
    void setupMQTT();
    void loadJsonKey(const char* jsonKeyPath);
    String base64UrlEncode(const unsigned char* input, size_t length);
};

#endif // GCPCLIENT_H

