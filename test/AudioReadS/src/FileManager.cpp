#include "FileManager.h"
#include <Arduino.h>

FileManager::FileManager() {
    if (!SPIFFS.begin(true)) {  // true parameter forces format if mount fails
        Serial.println("SPIFFS Mount Failed");
    }
}

FileManager::~FileManager() {
    SPIFFS.end();
}

bool FileManager::writeFile(const char* path, const uint8_t* data, size_t length) {
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    if (file.write(data, length) != length) {
        Serial.println("Failed to write all data");
        return false;
    }
    file.close();
    return true;
}

String FileManager::readFile(const char* path) {
    File file = SPIFFS.open(path, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return String();
    }
    size_t size = file.size();
    uint8_t* buffer = new uint8_t[size];
    file.read(buffer, size);
    file.close();
    String data = String((char*)buffer, size);
    delete[] buffer;
    return data;
}

String FileManager::encodeBase64(const uint8_t* data, size_t length) {
    return base64::encode(data, length);
}