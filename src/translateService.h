#ifndef TRANSLATESERVICE_H
#define TRANSLATESERVICE_H

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Function declarations
class TranslateService {
public:
    TranslateService();
    void setupWiFi(const char* ssid, const char* password);
    String sendSpeechToTextRequest(WiFiClientSecure &client, const char* accessToken, const char* originalLanguage, const String& audioData);
    String translateText(WiFiClientSecure &client, const char* accessToken, const char* originalLanguage, const char* targetLanguage, const String& text);
    void sendTextToSpeechRequest(WiFiClientSecure &client, const char* accessToken, const char* targetLanguage, const String& text);
    void playAudio(const char* audioContent);

};
#endif // GTRANSLATESERVICE_H