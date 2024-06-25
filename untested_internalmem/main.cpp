#include <Arduino.h>
#include "driver/i2s.h"
#include <SPIFFS.h> 

#define I2S_WS 15  // Microphone I2S WS (LRCLK)
#define I2S_SCK 14  // Microphone I2S SCK (BCLK)
#define I2S_SD 32  // Microphone I2S Data

// Switches and LEDs
#define SWITCH1_PIN 26
#define SWITCH2_PIN 34
#define SWITCH3_PIN 35
#define SWITCH4_PIN 25

#define I2S_SPEAKER_WS 21  // Speaker I2S WS (LRCLK)
#define I2S_SPEAKER_SCK 22  // Speaker I2S SCK (BCLK)
#define I2S_SPEAKER_DI 17  // Speaker I2S Data
#define GAIN 2  //GAIN OF THE SPEAKER (USUALLY, +2.0)

#define I2S_PORT I2S_NUM_0
#define I2S_SPEAKER_PORT I2S_NUM_1

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 1024

#define MAX_FILES 20 // Max number of files

// Constants
int currentIndex = 0;
int currentReadIndex = 0;

bool swtich1_pastState = LOW;
bool swtich2_pastState = LOW;
bool swtich3_pastState = LOW;
bool swtich4_pastState = LOW;

bool Gravar = 0;
bool Reproducir = 0;
bool Amplificar = 0;
bool Traducir = 0;

// Function Declarations
void setupMic();
void setupSpeaker();
void Amplify();
void setupSPIFFS();
void recordAudio();
void clearSPIFFS();
void playCurrentFile();
void playNextFile();
void Translate();

void setup() {
  Serial.begin(115200);

  pinMode(SWITCH1_PIN, INPUT);
  pinMode(SWITCH2_PIN, INPUT);
  pinMode(SWITCH3_PIN, INPUT);
  pinMode(SWITCH4_PIN, INPUT);

  setupMic();
  setupSpeaker();
  setupSPIFFS();
}

void loop() {
  bool swtich1_currentState = digitalRead(SWITCH1_PIN);
  bool swtich2_currentState = digitalRead(SWITCH2_PIN);
  bool swtich3_currentState = digitalRead(SWITCH3_PIN);
  bool swtich4_currentState = digitalRead(SWITCH4_PIN);

  if(swtich1_currentState != swtich1_pastState){
    if(swtich1_currentState){
      Amplificar = 1;
    }
    else
      Amplificar = 0;
    swtich1_pastState = swtich1_currentState;
  };

  if(swtich2_currentState != swtich2_pastState && Amplificar && !Traducir){
    if (Reproducir){
      playNextFile();
    }
    else{
      if(swtich2_currentState){
        Amplificar = 0;
        Gravar = 1;
        recordAudio();
      }
      else
        Gravar = 0;
    }
    swtich2_pastState = swtich2_currentState;
  };

  if(swtich3_currentState != swtich3_pastState && Amplificar && !Traducir){
    if (Gravar){
      if(swtich3_currentState)
        clearSPIFFS();
    }
    else{
      if(swtich3_currentState){
        playNextFile();
        Amplificar = 0;
        Reproducir = 1;
      }
      else{
        Reproducir = 0;
      }
    }
    swtich3_pastState = swtich3_currentState;
  };
  
  if(swtich4_currentState != swtich4_pastState && Amplificar){
    if(swtich4_currentState){
      Translate();
      Amplificar = 0;
      Traducir = 1;
    }
    else{
      Traducir = 0;
    }
    swtich4_pastState = swtich4_currentState;
  };
  if(Amplificar){
    Amplify();
  }
};

void setupMic() {
    i2s_config_t i2s_mic_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
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
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
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
      .data_out_num = I2S_SPEAKER_DI,
      .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_SPEAKER_PORT, &i2s_speaker_config, 0, NULL);
    i2s_set_pin(I2S_SPEAKER_PORT, &speaker_pin_config);
    i2s_set_clk(I2S_SPEAKER_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
  }

  void Amplify(){
    int16_t buffer[BUFFER_SIZE];
    size_t bytesRead;

    // Read audio data from I2S (microphone)
    i2s_read(I2S_PORT, buffer, BUFFER_SIZE * sizeof(int16_t), &bytesRead, portMAX_DELAY);

    // Verify if any data was read
    if (bytesRead > 0) {
      // Apply gain to the audio samples
      for (size_t i = 0; i < bytesRead / sizeof(int16_t); i++) {
        buffer[i] = int16_t(buffer[i] * GAIN);
        // Clip the value to avoid overflow
        if (buffer[i] > INT16_MAX) buffer[i] = INT16_MAX;
        if (buffer[i] < INT16_MIN) buffer[i] = INT16_MIN;
      }
      // Write audio data to I2S (speaker)
      size_t bytesWritten;
      i2s_write(I2S_SPEAKER_PORT, buffer, bytesRead, &bytesWritten, portMAX_DELAY);

      // Optional: Print the first 10 samples to the serial monitor for debugging
      for (int i = 0; i < 10 && i < (bytesRead / sizeof(int16_t)); i++) {
        Serial.println(buffer[i]);
      }
    }

    delay(100); // Small delay to avoid overloading the loop
  }

  void setupSPIFFS() {
    if (!SPIFFS.begin(true)) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
    Serial.println("SPIFFS mounted successfully");
  }

  void recordAudio() {
  char filename[20];
  snprintf(filename, sizeof(filename), "/audio%d.wav", currentIndex);
  File file = SPIFFS.open(filename, FILE_WRITE);

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

void clearSPIFFS() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    SPIFFS.remove(file.name());
    file = root.openNextFile();
  }
  currentIndex = 0;
  currentReadIndex = 0; // Refresh the read index to 0
  Serial.println("SPIFFS cleared");
}

void playCurrentFile() {
  char filename[20];
  snprintf(filename, sizeof(filename), "/audio%d.wav", currentReadIndex);
  File file = SPIFFS.open(filename);

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
