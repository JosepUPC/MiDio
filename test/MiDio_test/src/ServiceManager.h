#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "TranslateService.h"
#include "wavRead.h"
#include <WiFiClientSecure.h>

class serviceManager {
public:
    serviceManager();
    void begin();
    void SetupTaskCreat();
    void SetupWavFileTask();
    void printReceivedAudio(const String& audio);

private:
    TranslateService translateService;
    wavRead WavRead;
    WiFiClientSecure client;

    static void processAudioTask(void *pvParameters);
    static void processWavFileTask(void *pvParameters);
};

#endif // SERVICEMANAGER_H