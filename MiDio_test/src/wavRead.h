#ifndef WAVREAD_H
#define WAVREAD_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Base64.h>

class wavRead {
public:
    wavRead();
    bool begin();
    String readWavFile(const char* filename);

private:
    String convertToBase64(const uint8_t* data, size_t length);
};

#endif // WAVREAD_H