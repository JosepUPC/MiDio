#include <Arduino.h>
#include "FileManager.h"

FileManager fileM;

void setup() {
    Serial.begin(115200);

    // Example data to write
    const char* filePath = "/src/audio_buffer.bin";
    uint8_t data[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello" in ASCII

    // Write data to file
    if (!fileM.writeFile(filePath, data, sizeof(data))) {
        Serial.println("Failed to write file");
        return;
    }

    // Read data from file
    String readData = fileM.readFile(filePath);
    if (readData.length() == 0) {
        Serial.println("Failed to read file");
        return;
    }

    // Encode data to Base64
    String encodedData = fileM.encodeBase64((uint8_t*)readData.c_str(), readData.length());
    Serial.println("Encoded Data: " + encodedData);
}

void loop() {
    // put your main code here, to run repeatedly:
}
