#include "GCPClient.h"

GCPClient::GCPClient(const char* topicName)
    : topicName(topicName), client(netClient) {}

void GCPClient::begin(const char* ssid, const char* password, const char* jsonKeyPath) {
    setupWiFi(ssid, password);
    loadJsonKey(jsonKeyPath);
    setupMQTT();
}

void GCPClient::setupWiFi(const char* ssid, const char* password) {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected!");
}

void GCPClient::setupMQTT() {
    client.setServer("mqtt.googleapis.com", 8883);
    client.setCallback([](char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (unsigned int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    });
}

bool GCPClient::connect() {
    
    String jwt = getJwt();
    // Construct clientId as the topic path for Pub/Sub
    String clientId = String("projects/") + projectId + "/topics/" + topicName;

    if (client.connect(clientId.c_str(), "unused", jwt.c_str())) {
        Serial.println("Connected to GCP Pub/Sub!");
        return true;
    } else {
        Serial.print("Failed to connect, state: ");
        Serial.println(client.state());
        return false;
    }
}

void GCPClient::loop() {
    if (!client.connected()) {
        connect();
    }
    client.loop();
}

void GCPClient::publishMessage(const char* message) {
    String topic = String("/devices/YOUR_DEVICE_ID/events") + topicName;
    client.publish(topic.c_str(), message);
}

void GCPClient::loadJsonKey(const char* jsonKeyPath) {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount file system");
        return;
    }
    
    File file = SPIFFS.open(jsonKeyPath, "r");
    if (!file) {
        Serial.println("Failed to open key file");
        return;
    }
    
    size_t size = file.size();
    std::unique_ptr<char[]> buf(new char[size]);
    file.readBytes(buf.get(), size);
    
    JSONVar doc = JSON.parse(buf.get());
    if (JSON.typeof(doc) != "undefined") {
        projectId = String((const char*)doc["project_id"]);
        privateKey = String((const char*)doc["private_key"]);
        clientEmail = String((const char*)doc["client_email"]);
    } else {
        Serial.println("Failed to parse key file");
        return;
    }
    
    file.close();
}


String GCPClient::base64UrlEncode(const unsigned char* input, size_t length) {
    // Calculate the base64 encoded size
    size_t encodedLen = 4 * ((length + 2) / 3);
    char encoded[encodedLen];
    
    // Perform base64 encoding
    mbedtls_base64_encode((unsigned char*)encoded, encodedLen, &encodedLen, input, length);

    // Replace '+' with '-', '/' with '_', and remove '='
    String encodedStr = String(encoded, encodedLen);
    encodedStr.replace('+', '-');
    encodedStr.replace('/', '_');
    encodedStr.replace("=", "");

    return encodedStr;
}

String GCPClient::getJwt() {
    // Create JWT header
    JSONVar header;
    header["alg"] = "RS256";
    header["typ"] = "JWT";
    String headerStr = JSON.stringify(header);

    // Create JWT payload
    JSONVar payload;
    payload["iat"] = time(nullptr);
    payload["exp"] = time(nullptr) + 3600; // Token expiration time set to 1 hour
    payload["aud"] = projectId;
    String payloadStr = JSON.stringify(payload);

    // Base64Url encode header and payload
    String encodedHeader = base64UrlEncode((unsigned char*)headerStr.c_str(), headerStr.length());
    String encodedPayload = base64UrlEncode((unsigned char*)payloadStr.c_str(), payloadStr.length());

    // Create the signature input (header.payload)
    String signatureInput = encodedHeader + "." + encodedPayload;

    // Load the private key
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    mbedtls_pk_parse_key(&pk, (const unsigned char*)privateKey.c_str(), privateKey.length() + 1, nullptr, 0);

    // Create the signature (RS256)
    unsigned char hash[32];
    mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char*)signatureInput.c_str(), signatureInput.length(), hash);

    unsigned char sig[512];
    size_t sigLen;
    mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA256, hash, 0, sig, &sigLen, nullptr, nullptr);

    // Base64Url encode the signature
    String encodedSig = base64UrlEncode(sig, sigLen);

    // Construct the final JWT (header.payload.signature)
    String jwt = signatureInput + "." + encodedSig;

    // Free the private key context
    mbedtls_pk_free(&pk);

    return jwt;
}