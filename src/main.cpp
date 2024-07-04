#include <Arduino.h>
#include "driver/i2s.h"
#include "SD.h"
#include "FS.h"

#define I2S_WS 15  // Microphone I2S WS (LRCLK)
#define I2S_SCK 14  // Microphone I2S SCK (BCLK)
#define I2S_SD 32  // Microphone I2S Data

#define SWITCH1_PIN 26
#define SWITCH2_PIN 34
#define SWITCH3_PIN 35
#define SWITCH4_PIN 25
#define LED1_PIN 13
#define LED2_PIN 12
#define LED3_PIN 16
#define LED4_PIN 4

#define I2S_SPEAKER_WS 21  // Speaker I2S WS (LRCLK)
#define I2S_SPEAKER_SCK 22  // Speaker I2S SCK (BCLK)
#define I2S_SPEAKER_SD 17  // Speaker I2S Data
#define GAIN 2

#define I2S_PORT I2S_NUM_0
#define I2S_SPEAKER_PORT I2S_NUM_1

#define SAMPLE_RATE 32000
#define BUFFER_SIZE 1024

#define SD_CS 5
#define MAX_FILES 20

int currentIndex = 0;
int currentReadIndex = 0;

static bool switch1_pastState = false;
static bool switch2_pastState = false;
static bool switch3_pastState = false;
static bool switch4_pastState = false;

bool switch1_currentState;
bool switch2_currentState;
bool switch3_currentState;
bool switch4_currentState;

static bool Gravar = false;
static bool Reproducir = false;
static bool Amplificar = false;
static bool Traducir = false;

class MiDio {
public:
  MiDio() {}

  void begin() {
    Serial.begin(115200);
    setupMic();
    setupSpeaker();
    setupSD();
    setupPMode();
  }

  void PinCheck(){
    switch1_currentState = digitalRead(SWITCH1_PIN);
    switch2_currentState = digitalRead(SWITCH2_PIN);
    switch3_currentState = digitalRead(SWITCH3_PIN);
    switch4_currentState = digitalRead(SWITCH4_PIN);

    digitalWrite(LED1_PIN,switch1_currentState);
    digitalWrite(LED2_PIN,switch2_currentState);
    digitalWrite(LED3_PIN,switch3_currentState);
    digitalWrite(LED4_PIN,switch4_currentState);
  }

  void Amplify() {
    int16_t buffer[BUFFER_SIZE];
    size_t bytesRead;

    // Read audio data from I2S (microphone)
    i2s_read(I2S_PORT, buffer, BUFFER_SIZE * sizeof(int16_t), &bytesRead, portMAX_DELAY);

    // Verify if any data was read
    if (bytesRead > 0) {
      // Write audio data to I2S (speaker)
      size_t bytesWritten;
      i2s_write(I2S_SPEAKER_PORT, buffer, bytesRead, &bytesWritten, portMAX_DELAY);

      // Optional: Print the first 10 samples to the serial monitor for debugging
      for (int i = 0; i < 10 && i < (bytesRead / sizeof(int16_t)); i++) {
        Serial.println(buffer[i]);
      }
    }

    delay(10); // Small delay to avoid overloading the loop
  }

  void recordAudio() {
    char filename[20];
    snprintf(filename, sizeof(filename), "/audio%d.wav", currentIndex);
    File file = SD.open(filename, FILE_WRITE);

    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }

    int16_t buffer[BUFFER_SIZE];
    size_t bytesRead;
    unsigned long startTime = millis();

    while (millis() - startTime < 10000) { // Record for 10 seconds
      i2s_read(I2S_PORT, buffer, BUFFER_SIZE * sizeof(int16_t), &bytesRead, portMAX_DELAY);
      file.write((uint8_t *)buffer, bytesRead);
    }

    file.close();
    Serial.println("Recording complete");

    currentIndex = (currentIndex + 1) % MAX_FILES;
  }

  void clearSD() {
    File root = SD.open("/");
    File file = root.openNextFile();
    while (file) {
      SD.remove(file.name());
      file = root.openNextFile();
    }
    currentIndex = 0;
    currentReadIndex = 0; // Refresh the read index to 0
    Serial.println("SD card cleared");
  }

  void playCurrentFile() {
    char filename[20];
    snprintf(filename, sizeof(filename), "/audio%d.wav", currentReadIndex);
    File file = SD.open(filename);

    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }

    int16_t buffer[BUFFER_SIZE];
    while (file.available()) {
      size_t bytesRead = file.read((uint8_t *)buffer, BUFFER_SIZE * sizeof(int16_t));
      i2s_write(I2S_SPEAKER_PORT, buffer, bytesRead, &bytesRead, portMAX_DELAY);
    }

    file.close();
    Serial.println("Playback complete");
  }

  void playNextFile() {
    currentReadIndex = (currentReadIndex + 1) % MAX_FILES;
    playCurrentFile();
  }
  void Translate(){
    Serial.println("translate function");
  }

private:
  void setupMic() {
    i2s_config_t i2s_mic_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0
    };

    i2s_pin_config_t mic_pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = I2S_SD
    };

    i2s_driver_install(I2S_PORT, &i2s_mic_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &mic_pin_config);
    i2s_set_clk(I2S_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
  }

  void setupSpeaker() {
    i2s_config_t i2s_speaker_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0
    };

    i2s_pin_config_t speaker_pin_config = {
      .bck_io_num = I2S_SPEAKER_SCK,
      .ws_io_num = I2S_SPEAKER_WS,
      .data_out_num = I2S_SPEAKER_SD,
      .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_SPEAKER_PORT, &i2s_speaker_config, 0, NULL);
    i2s_set_pin(I2S_SPEAKER_PORT, &speaker_pin_config);
    i2s_set_clk(I2S_SPEAKER_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
  }
  void setupPMode(){
    pinMode(SWITCH1_PIN, INPUT);
    pinMode(SWITCH2_PIN, INPUT);
    pinMode(SWITCH3_PIN, INPUT);
    pinMode(SWITCH4_PIN, INPUT);

    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED4_PIN, OUTPUT);
  }
  void setupSD() {
    if (!SD.begin(SD_CS)) {
      Serial.println("Card Mount Failed");
      return;
    }
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
      Serial.println("MMC");
    } else if (cardType == CARD_SD) {
      Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
      Serial.println("SDHC");
    } else {
      Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
  }
};

MiDio audio;

void setup() {
  audio.begin();
}

void loop() {
  audio.PinCheck();
  if(switch1_currentState != switch1_pastState && Gravar == 0 && Reproducir == 0 && Traducir == 0){
    if(switch1_currentState){
      Amplificar = 1;
      audio.Amplify();
    }
    else
      Amplificar = 0;
    switch1_pastState = switch1_currentState;
  };

  if(switch2_currentState != switch2_pastState && Amplificar == 0 && Traducir == 0){
    if (Reproducir){
      audio.playNextFile();
    }
    else{
      if(switch2_currentState){
        Gravar = 1;
        audio.recordAudio();
      }
      else
        Gravar = 0;
    }
    switch2_pastState = switch2_currentState;
  }

  if(switch3_currentState != switch3_pastState && Amplificar == 0 && Traducir == 0){
    if (Gravar){
      if(switch3_currentState)
        audio.clearSD();
    }
    else{
      if(switch3_currentState){
        audio.playNextFile();
        Reproducir = 1;
      }
      else{
        Reproducir = 0;
      }
    }
    switch3_pastState = switch3_currentState;
  }

  if(switch4_currentState != switch4_pastState && Amplificar == 0 && Reproducir == 0 && Gravar == 0){
    if(switch4_currentState){
      audio.Translate();
      Traducir = 1;
    }
    else{
      Traducir = 0;
    }
    switch4_pastState = switch4_currentState;
  }
}


