#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <translateService.h>
#include <WiFiClientSecure.h>

class serviceManager {
public:
    serviceManager ();
    void begin();
    void SetupTaskCreat();

private:
    TranslateService translateService;
    WiFiClientSecure client;

    static void processAudioTask(void *pvParameters);
};

#endif // SERVICEMANAGER_H
