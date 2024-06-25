#include "wavRead.h"

wavRead::wavRead() {}

bool wavRead::begin() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }
    return true;
}

String wavRead::readWavFile(const char* filename) {
    File file = SPIFFS.open(filename);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "";
    }

    size_t fileSize = file.size();
    uint8_t* buffer = new uint8_t[fileSize];
    file.read(buffer, fileSize);
    file.close();

    String base64Data = convertToBase64(buffer, fileSize);
    delete[] buffer;

    return base64Data;
}

String wavRead::convertToBase64(const uint8_t* data, size_t length) {
    String encoded = base64::encode(data, length);
    return encoded;
}