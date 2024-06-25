#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Preferences.h>

#define SD_CS 5
#define SWITCH1_PIN 34
#define SWITCH2_PIN 35

const int maxFiles = 20;
int currentFileIndex = 0;

Preferences preferences;

void writeFileTask(void *pvParameters);
void cleanSDTask(void *pvParameters);
void readFileTask(void *pvParameters);
void readNextFileTask(void *pvParameters);

TaskHandle_t writeTaskHandle = NULL;
TaskHandle_t cleanTaskHandle = NULL;
TaskHandle_t readTaskHandle = NULL;
TaskHandle_t readNextTaskHandle = NULL;

void setup() {
    Serial.begin(115200);

    preferences.begin("sd-test", false);
    currentFileIndex = preferences.getInt("fileIndex", 0);

    if (!SD.begin(SD_CS)) {
        Serial.println("Card Mount Failed");
        return;
    }

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }

    Serial.println("SD card initialized.");

    pinMode(SWITCH1_PIN, INPUT);
    pinMode(SWITCH2_PIN, INPUT);

    xTaskCreatePinnedToCore(
        writeFileTask,      // Function to run on this task
        "WriteFileTask",    // Task name for debugging
        4096,               // Stack size (words)
        NULL,               // Parameters to pass to function
        1,                  // Priority (0 = lowest, 1 = normal, 2 = high)
        &writeTaskHandle,   // Task handle
        1);                 // Core to run task on (0 or 1)

    xTaskCreatePinnedToCore(
        cleanSDTask,        // Function to run on this task
        "CleanSDTask",      // Task name for debugging
        4096,               // Stack size (words)
        NULL,               // Parameters to pass to function
        1,                  // Priority (0 = lowest, 1 = normal, 2 = high)
        &cleanTaskHandle,   // Task handle
        1);                 // Core to run task on (0 or 1)

    xTaskCreatePinnedToCore(
        readFileTask,       // Function to run on this task
        "ReadFileTask",     // Task name for debugging
        4096,               // Stack size (words)
        NULL,               // Parameters to pass to function
        1,                  // Priority (0 = lowest, 1 = normal, 2 = high)
        &readTaskHandle,    // Task handle
        1);                 // Core to run task on (0 or 1)

    xTaskCreatePinnedToCore(
        readNextFileTask,   // Function to run on this task
        "ReadNextFileTask", // Task name for debugging
        4096,               // Stack size (words)
        NULL,               // Parameters to pass to function
        1,                  // Priority (0 = lowest, 1 = normal, 2 = high)
        &readNextTaskHandle,// Task handle
        1);                 // Core to run task on (0 or 1)
}

void loop() {
    vTaskDelay(1000); // Just to keep the main loop running smoothly
}

void writeFileTask(void *pvParameters) {
    while (1) {
        if (digitalRead(SWITCH1_PIN) == HIGH && digitalRead(SWITCH2_PIN) == LOW) {
            char filename[20];
            snprintf(filename, sizeof(filename), "/file%d.txt", currentFileIndex);
            File file = SD.open(filename, FILE_WRITE);
            if (file) {
                file.print("Message from file ");
                file.println(currentFileIndex);
                file.close();
                Serial.print("Written to file: ");
                Serial.println(filename);
            } else {
                Serial.println("Failed to open file for writing.");
            }

            currentFileIndex = (currentFileIndex + 1) % maxFiles;
            preferences.putInt("fileIndex", currentFileIndex);

            // Wait until switch 1 goes LOW
            while (digitalRead(SWITCH1_PIN) == HIGH) {
                vTaskDelay(50);
            }
        }

        vTaskDelay(10); // Task delay to allow other tasks to run
    }
}

void cleanSDTask(void *pvParameters) {
    while (1) {
        if (digitalRead(SWITCH1_PIN) == HIGH && digitalRead(SWITCH2_PIN) == HIGH) {
            for (int i = 0; i < maxFiles; i++) {
                char filename[20];
                snprintf(filename, sizeof(filename), "/file%d.txt", i);
                SD.remove(filename);
            }
            Serial.println("SD card cleaned.");
            currentFileIndex = 0;
            preferences.putInt("fileIndex", currentFileIndex);

            // Wait until both switches go LOW
            while (digitalRead(SWITCH1_PIN) == HIGH || digitalRead(SWITCH2_PIN) == HIGH) {
                vTaskDelay(50);
            }
        }

        vTaskDelay(10); // Task delay to allow other tasks to run
    }
}

void readFileTask(void *pvParameters) {
    while (1) {
        if (digitalRead(SWITCH2_PIN) == HIGH && digitalRead(SWITCH1_PIN) == LOW) {
            char filename[20];
            snprintf(filename, sizeof(filename), "/file%d.txt", currentFileIndex);
            File file = SD.open(filename);
            if (file) {
                Serial.print("Reading from file: ");
                Serial.println(filename);
                while (file.available()) {
                    Serial.write(file.read());
                }
                file.close();
            } else {
                Serial.println("Failed to open file for reading.");
            }

            currentFileIndex = (currentFileIndex + 1) % maxFiles;
            preferences.putInt("fileIndex", currentFileIndex);

            // Wait until switch 2 goes LOW
            while (digitalRead(SWITCH2_PIN) == HIGH) {
                vTaskDelay(50);
            }
        }

        vTaskDelay(10); // Task delay to allow other tasks to run
    }
}

void readNextFileTask(void *pvParameters) {
    while (1) {
        if (digitalRead(SWITCH1_PIN) == HIGH && digitalRead(SWITCH2_PIN) == HIGH) {
            int nextFileIndex = (currentFileIndex + 1) % maxFiles;
            char filename[20];
            snprintf(filename, sizeof(filename), "/file%d.txt", nextFileIndex);
            File file = SD.open(filename);
            if (file) {
                Serial.print("Reading next file: ");
                Serial.println(filename);
                while (file.available()) {
                    Serial.write(file.read());
                }
                file.close();
            } else {
                Serial.println("Failed to open next file for reading.");
            }

            currentFileIndex = nextFileIndex;
            preferences.putInt("fileIndex", currentFileIndex);

            // Wait until both switches go LOW
            while (digitalRead(SWITCH1_PIN) == HIGH || digitalRead(SWITCH2_PIN) == HIGH) {
                vTaskDelay(50);
            }
        }

        vTaskDelay(10); // Task delay to allow other tasks to run
    }
}
