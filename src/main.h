#ifndef MAIN_H
#define MAIN_H

#include <translateService.h>
#include <WiFiClientSecure.h>

class MainClass {
public:
    MainClass ();
    void begin();
    void SetupTaskCreat();

private:
    TranslateService translateService;
    WiFiClientSecure client;

    static void processAudioTask(void *pvParameters);
};

#endif // TRANSLATEMANAGER_H
