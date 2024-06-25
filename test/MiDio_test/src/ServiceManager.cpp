#include "ServiceManager.h"
#include <Arduino.h>
#include "mainData.h"

serviceManager::serviceManager() : translateService(), WavRead(), client() {}

void serviceManager::begin() {
    Serial.begin(115200);
    translateService.setupWiFi(WIFI_SSID, WIFI_PASSWORD);
    if (!WavRead.begin()) {
        Serial.println("Failed to initialize wavRead");
    }
}

void serviceManager::SetupTaskCreat() {
    xTaskCreate(
        processAudioTask,    // Function that implements the task
        "ProcessAudioTask",  // Task name
        8192,                // Stack size in words
        this,                // Task input parameter
        1,                   // Priority of the task
        NULL                 // Task handle
    );
}

void serviceManager::SetupWavFileTask() {
    xTaskCreate(
        processWavFileTask,  // Function that implements the task
        "ProcessWavFileTask",// Task name
        8192,                // Stack size in words
        this,                // Task input parameter
        1,                   // Priority of the task
        NULL                 // Task handle
    );
}

void serviceManager::printReceivedAudio(const String& audio) {
    Serial.println("Received audio:");
    Serial.println(audio);
}

void serviceManager::processAudioTask(void *pvParameters) {
    serviceManager* manager = static_cast<serviceManager*>(pvParameters);
    for(;;) {
        // Replace with actual base64 encoded audio data
        String audioData = "<base64_encoded_audio_data>";

        // Use TranslateService instance to perform operations
        String transcript = manager->translateService.sendSpeechToTextRequest(manager->client, ACCESS_TOKEN, ORIGINAL_LANGUAGE, audioData);
        if (transcript.length() > 0) {
            String translatedText = manager->translateService.translateText(manager->client, ACCESS_TOKEN, ORIGINAL_LANGUAGE, TARGET_LANGUAGE, transcript);
            if (translatedText.length() > 0) {
                manager->translateService.sendTextToSpeechRequest(manager->client, ACCESS_TOKEN, TARGET_LANGUAGE, translatedText);
            }
        }

        // Delay to avoid spamming the API
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Delay for 60 seconds
    }
}

void serviceManager::processWavFileTask(void *pvParameters) {
    serviceManager* manager = static_cast<serviceManager*>(pvParameters);
    for(;;) {
        // Read the WAV file and convert to base64
        String base64AudioData = manager->WavRead.readWavFile("/audio.wav"); // Change to your actual filename

        if (base64AudioData.length() > 0) {
            // Use TranslateService instance to perform operations
            String transcript = manager->translateService.sendSpeechToTextRequest(manager->client, ACCESS_TOKEN, ORIGINAL_LANGUAGE, base64AudioData);
            if (transcript.length() > 0) {
                String translatedText = manager->translateService.translateText(manager->client, ACCESS_TOKEN, ORIGINAL_LANGUAGE, TARGET_LANGUAGE, transcript);
                if (translatedText.length() > 0) {
                    manager->translateService.sendTextToSpeechRequest(manager->client, ACCESS_TOKEN, TARGET_LANGUAGE, translatedText);
                }
            }
        }

        // Delay to avoid spamming the API
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Delay for 60 seconds
    }
}