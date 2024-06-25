#include <Arduino.h>
#include <SD.h>
#include <I2S.h>

// Define ESP32-S3 pins
#define MICROSD_CS_PIN 5
#define SWITCH_PIN 4

// Define I2S pins for ESP32-S3
#define I2S_BCLK_PIN 14
#define I2S_LRC_PIN 15
#define I2S_DATA_PIN 32

// Initialize SD library and file
File audioFile;

void setup() {
    Serial.begin(115200);

    // Initialize SD card
    if (!SD.begin(MICROSD_CS_PIN)) {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized.");

    // Initialize I2S microphone
    I2S.begin(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DATA_PIN);

    // Set up switch pin
    pinMode(SWITCH_PIN, INPUT_PULLUP);
}

void loop() {
    // Check the state of the switch
    if (digitalRead(SWITCH_PIN) == LOW) { // Switch is pulled down
        // Start recording
        startRecording();
    } else { // Switch is pulled up
        // Stop recording and save to SD card
        stopRecording();
    }
}

void startRecording() {
    // Open a new file for writing
    audioFile = SD.open("recording.wav", FILE_WRITE);

    if (!audioFile) {
        Serial.println("Error opening file for recording.");
        return;
    }

    Serial.println("Recording started.");

    // Start reading from the I2S microphone and write to SD card
    while (digitalRead(SWITCH_PIN) == LOW) { // Continue recording until switch is released
        audioFile.write(I2S.read()); // Write audio data to file
    }

    // Close the file when recording is finished
    audioFile.close();
    Serial.println("Recording stopped.");
}

void stopRecording() {
    // If the file is open, close it
    if (audioFile) {
        audioFile.close();
        Serial.println("Recording stopped.");
    }
}