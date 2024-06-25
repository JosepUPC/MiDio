#ifndef WAV_READ_H
#define WAV_READ_H

#include <FS.h>
#include <SPIFFS.h>
#include <Arduino.h>  // Include Arduino header for Serial

class wavRead {
public:
    wavRead(const char* filename) : _filename(filename) {}

    bool begin() {
        if (!SPIFFS.begin(true)) {
            Serial.println("An error has occurred while mounting SPIFFS");
            return false;
        }
        return true;
    }

    uint8_t* readAndEncode(size_t& encodedSize) {
        File file = SPIFFS.open(_filename, "r");  // Use the provided filename
        if (!file) {
            Serial.println("Failed to open file for reading");
            return nullptr;
        }

        size_t fileSize = file.size();
        if (fileSize == 0) {
            Serial.println("File is empty");
            file.close();
            return nullptr;
        }

        Serial.print("File size: ");
        Serial.println(fileSize);

        // Allocate memory for the buffer
        uint8_t* buffer = new uint8_t[fileSize];
        if (!buffer) {
            Serial.println("Failed to allocate memory");
            file.close();
            return nullptr;
        }

        // Read audio data into the buffer
        size_t bytesRead = file.readBytes(reinterpret_cast<char*>(buffer), fileSize);
        file.close();

        if (bytesRead != fileSize) {
            Serial.print("Read ");
            Serial.print(bytesRead);
            Serial.println(" bytes, but expected to read full file");
            delete[] buffer;
            return nullptr;
        }

        // Calculate the size of the encoded buffer manually
        size_t encodedBufferSize = ((fileSize + 2) / 3) * 4;

        // Allocate memory for the encoded buffer
        uint8_t* encodedBuffer = new uint8_t[encodedBufferSize];
        if (!encodedBuffer) {
            Serial.println("Failed to allocate memory for encoded buffer");
            delete[] buffer;
            return nullptr;
        }

        // Encode the buffer using the Arduino base64 encoding algorithm
        encodeBase64(buffer, fileSize, encodedBuffer);

        // Free the original buffer
        delete[] buffer;

        // Set the size of the encoded buffer
        encodedSize = encodedBufferSize;
        
        // Return the encoded buffer
        return encodedBuffer;
    }

private:
    const char* _filename;

    // Custom Base64 encoding function
    void encodeBase64(const uint8_t* input, size_t inputLength, uint8_t* output) {
        static const char encodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        size_t i = 0, j = 0;
        uint8_t byte1, byte2, byte3;
        while (i < inputLength) {
            byte1 = input[i++];
            byte2 = (i < inputLength) ? input[i++] : 0;
            byte3 = (i < inputLength) ? input[i++] : 0;
            output[j++] = encodingTable[byte1 >> 2];
            output[j++] = encodingTable[((byte1 & 0x03) << 4) | ((byte2 & 0xF0) >> 4)];
            output[j++] = (i < inputLength + 2) ? encodingTable[((byte2 & 0x0F) << 2) | ((byte3 & 0xC0) >> 6)] : '=';
            output[j++] = (i < inputLength + 1) ? encodingTable[byte3 & 0x3F] : '=';
        }
    }
};

#endif // WAV_READ_