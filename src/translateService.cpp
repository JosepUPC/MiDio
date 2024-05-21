#include "translateService.h"
#include <WiFi.h>

// Function to connect to WiFi
void setupWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// Function to send a request to the Google Cloud Speech-to-Text API
String sendSpeechToTextRequest(WiFiClientSecure &client, const char* accessToken, const char* originalLanguage, const String& audioData) {
  if (!client.connect("speech.googleapis.com", 443)) {
    Serial.println("Connection to Speech-to-Text API failed");
    return "";
  }

  String payload = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"" + String(originalLanguage) + "\"},\"audio\":{\"content\":\"" + audioData + "\"}}";

  client.println("POST /v1/speech:recognize HTTP/1.1");
  client.println("Host: speech.googleapis.com");
  client.println("Authorization: Bearer " + String(accessToken));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(payload.length()));
  client.println();
  client.print(payload);

  String response;
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }
  while (client.available()) {
    response += client.readStringUntil('\n');
  }
  
  client.stop();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "";
  }

  const char* transcript = doc["results"][0]["alternatives"][0]["transcript"];
  return String(transcript);
}

// Function to translate text using Google Cloud Translation API
String translateText(WiFiClientSecure &client, const char* accessToken, const char* originalLanguage, const char* targetLanguage, const String& text) {
  if (!client.connect("translation.googleapis.com", 443)) {
    Serial.println("Connection to Translation API failed");
    return "";
  }

  String payload = "{\"q\":\"" + text + "\",\"source\":\"" + String(originalLanguage) + "\",\"target\":\"" + String(targetLanguage) + "\"}";

  client.println("POST /language/translate/v2 HTTP/1.1");
  client.println("Host: translation.googleapis.com");
  client.println("Authorization: Bearer " + String(accessToken));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(payload.length()));
  client.println();
  client.print(payload);

  String response;
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }
  while (client.available()) {
    response += client.readStringUntil('\n');
  }
  
  client.stop();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "";
  }

  const char* translatedText = doc["data"]["translations"][0]["translatedText"];
  return String(translatedText);
}

// Function to send a request to the Google Cloud Text-to-Speech API
void sendTextToSpeechRequest(WiFiClientSecure &client, const char* accessToken, const char* targetLanguage, const String& text) {
  if (!client.connect("texttospeech.googleapis.com", 443)) {
    Serial.println("Connection to Text-to-Speech API failed");
    return;
  }

  String payload = "{\"input\":{\"text\":\"" + text + "\"},\"voice\":{\"languageCode\":\"" + String(targetLanguage) + "\",\"name\":\"es-ES-Wavenet-A\"},\"audioConfig\":{\"audioEncoding\":\"MP3\"}}";

  client.println("POST /v1/text:synthesize HTTP/1.1");
  client.println("Host: texttospeech.googleapis.com");
  client.println("Authorization: Bearer " + String(accessToken));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(payload.length()));
  client.println();
  client.print(payload);

  String response;
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }
  while (client.available()) {
    response += client.readStringUntil('\n');
  }
  
  client.stop();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* audioContent = doc["audioContent"];
  if (audioContent) {
    playAudio(audioContent);
  }
}

// Function to decode and play the base64 encoded audio
void playAudio(const char* audioContent) {
  // TODO: Implement audio playback
  // You will need to decode the base64 audio content and send it to the DAC or audio output of the ESP32-S3
  Serial.println("Received audio content. Implement playback here.");
}