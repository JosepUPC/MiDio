#ifndef MIDIO_FUNCTION
class MiDio {
public:
    MiDio();
    void begin();
    void setupMic();
    void setupSpeaker();
    void setupSD();
    void setupPMode();

    void PinCheck();
    void Amplify();
    void recordAudio();
    void clearSD();
    void playCurrentFile();
    void playNextFile();
    void Translate();
};

#endif