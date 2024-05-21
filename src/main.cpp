// init the libraries that we may use to run the project
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// name and password of the internet conection you wanna made
const char* ssid = "";
const char* password = "";

// configurate the connection with the google cloud Services
const char* prID = "project_id";
const char* prKid = "private_key_id";
const char* pvK = "private_key";
const char* clGM = "client_email";
const char* tkUr = "token_uri";

//Variable to manually control the access with your token
const char* accessToken = "your_access_token";

// class wificlient with some secured functions to protect the Json data
WiFiClientSecure client;

// init the function
String sendSpeechToTextRequest();
String translateText(const String& text);
void sendTextToSpeechRequest(const String& text);
void playAudio(const char* audioContent);

void setup() {
  // Start communication with the serial
  Serial.begin(115200);
  
  // Connect to the WiFi network
  WiFi.begin(ssid, password);

  // Check if the connection to establish
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");

  }

  // Show trough Serial monitor if the wifi connection is made or not. Show the IP adress too.
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  String transcript = sendSpeechToTextRequest();
  if (transcript.length() > 0) {
    String translatedText = translateText(transcript);
    if (translatedText.length() > 0) {
      sendTextToSpeechRequest(translatedText);
    }
  }
}

void loop() {
  // Main loop
  delay(10000); // Placeholder delay
}

// Function to send a request to the Google Cloud Speech-to-Text API
String sendSpeechToTextRequest() {
  if (!client.connect("speech.googleapis.com", 443)) {
    Serial.println("Connection to Speech-to-Text API failed");
    return "";
  }

  // Example request payload (adjust based on your audio data)
  String payload = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"en-US\"},\"audio\":{\"content\":\"<base64_encoded_audio_data>\"}}";

  client.println("POST /v1/speech:recognize HTTP/1.1");
  client.println("Host: speech.googleapis.com");
  client.println("Authorization: Bearer " + String(accessToken));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(payload.length()));
  client.println();
  client.print(payload);

  // Read response from the server
  String response;
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // End of headers
  }
  while (client.available()) {
    response += client.readStringUntil('\n');
  }
  
  client.stop();

  // Parse the JSON response
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "";
  }

  // Extract the transcript
  const char* transcript = doc["results"][0]["alternatives"][0]["transcript"];
  return String(transcript);
}

// Function to translate text using Google Cloud Translation API
String translateText(const String& text) {
  if (!client.connect("translation.googleapis.com", 443)) {
    Serial.println("Connection to Translation API failed");
    return "";
  }

  // Example request payload (adjust based on your target language)
  String payload = "{\"q\":\"" + text + "\",\"target\":\"es\"}";

  client.println("POST /language/translate/v2 HTTP/1.1");
  client.println("Host: translation.googleapis.com");
  client.println("Authorization: Bearer " + String(accessToken));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(payload.length()));
  client.println();
  client.print(payload);

  // Read response from the server
  String response;
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // End of headers
  }
  while (client.available()) {
    response += client.readStringUntil('\n');
  }
  
  client.stop();

  // Parse the JSON response
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "";
  }

  // Extract the translated text
  const char* translatedText = doc["data"]["translations"][0]["translatedText"];
  return String(translatedText);
}

// Function to send a request to the Google Cloud Text-to-Speech API
void sendTextToSpeechRequest(const String& text) {
  if (!client.connect("texttospeech.googleapis.com", 443)) {
    Serial.println("Connection to Text-to-Speech API failed");
    return;
  }

  // Example request payload (adjust based on your requirements)
  String payload = "{\"input\":{\"text\":\"" + text + "\"},\"voice\":{\"languageCode\":\"en-US\",\"name\":\"en-US-Wavenet-D\"},\"audioConfig\":{\"audioEncoding\":\"MP3\"}}";

  client.println("POST /v1/text:synthesize HTTP/1.1");
  client.println("Host: texttospeech.googleapis.com");
  client.println("Authorization: Bearer " + String(accessToken));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(payload.length()));
  client.println();
  client.print(payload);

  // Read response from the server
  String response;
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // End of headers
  }
  while (client.available()) {
    response += client.readStringUntil('\n');
  }
  
  client.stop();

  // Parse the JSON response
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract the audio content
  const char* audioContent = doc["audioContent"];
  if (audioContent) {
    // Decode the base64 audio content and play it
    playAudio(audioContent);
  }
}

// Function to decode and play the base64 encoded audio
void playAudio(const char* audioContent) {
  // TODO: Implement audio playback
  // You will need to decode the base64 audio content and send it to the DAC or audio output of the ESP32-S3
  Serial.println("Received audio content. Implement playback here.");
}