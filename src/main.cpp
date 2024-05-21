// init the libraries that we may use to run the project
#include <Arduino.h>
#include <translateService.h>
#include <mainData.h>

// class wificlient with some secured functions to protect the Json data
WiFiClientSecure client;

void processAudioTask(void *pvParameters) {
  for(;;) {
    // Replace with actual base64 encoded audio data
    String audioData = "<base64_encoded_audio_data>";

    String transcript = sendSpeechToTextRequest(client, ACCESS_TOKEN, ORIGINAL_LANGUAGE, audioData);
    if (transcript.length() > 0) {
      String translatedText = translateText(client, ACCESS_TOKEN, ORIGINAL_LANGUAGE, TARGET_LANGUAGE, transcript);
      if (translatedText.length() > 0) {
        sendTextToSpeechRequest(client, ACCESS_TOKEN, TARGET_LANGUAGE, translatedText);
      }
    }

    // Delay to avoid spamming the API
    vTaskDelay(60000 / portTICK_PERIOD_MS); // Delay for 60 seconds
  }
}

void setup() {
  Serial.begin(115200);

  setupWiFi(WIFI_SSID, WIFI_PASSWORD);

  xTaskCreate(
    processAudioTask,    // Function that implements the task
    "ProcessAudioTask",  // Task name
    8192,                // Stack size in words
    NULL,                // Task input parameter
    1,                   // Priority of the task
    NULL                 // Task handle
  );
}