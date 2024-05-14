// init the libraries that we may use to run the project
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

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
void sendSpeechToTextRequest();

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

  sendSpeechToTextRequest();
}

void loop() {
  // Main loop
  delay(10000); // Placeholder delay
}

// Function to send a request to the Google Cloud Speech-to-Text API
void sendSpeechToTextRequest(){
  if (!client.connect("speech.googleapis.com", 443)) {
    Serial.println("Connection to Speech-to-Text API failed");
    return;
  }

  // Example request payload (adjust based on your audio data)
  String payload = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"en-US\"},\"audio\":{\"content\":\"<base64_encoded_audio_data>\"}}";

  client.println("POST /v1/speech:recognize HTTP/1.1");
  client.println("Host: speech.googleapis.WiFiClientSecurecom");
  client.println("Authorization: Bearer " + String(accessToken));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(payload.length()));
  client.println();
  client.print(payload);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  String response = client.readString();
  Serial.println("Response from Speech-to-Text API:");
  Serial.println(response);

  client.stop();
}