#include <Arduino.h>
#include <wavRead.h>

void setup() {
    Serial.begin(115200);
    delay(1000); // Give time for Serial Monitor to open

    wavRead audioReader("/Audio.wav"); // Update this line to use a String object

    if (!audioReader.begin()) {
        Serial.println("Failed to mount SPIFFS");
        return;
    }

    size_t encodedSize;
    uint8_t* encodedBuffer = audioReader.readAndEncode(encodedSize);
    if (encodedBuffer) {
        Serial.println("Audio file encoded successfully");

        // Print encoded data (for demonstration purposes)
        Serial.print("Encoded audio size: ");
        Serial.println(encodedSize);
        Serial.println("Encoded audio data:");
        for (size_t i = 0; i < encodedSize; i++) {
            Serial.print(encodedBuffer[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        // Free the memory allocated for the encoded buffer
        delete[] encodedBuffer;
    } else {
        Serial.println("Failed to encode audio file");
    }
}

void loop() {
    // Your main code here
}