#include "driver/i2s.h"
#include <FS.h>
#include <SD.h>
#include <MiDio.h>
#include <MiDio_constants.h>

MiDio::MiDio(){}

void MiDio::begin(){
    setupMic();
    setupSpeaker();
    setupSD();
    setupPMode();
}

void MiDio::PinCheck(){
  switch1_currentState = digitalRead(SWITCH1_PIN);
  switch2_currentState = digitalRead(SWITCH2_PIN);
  switch3_currentState = digitalRead(SWITCH3_PIN);
  switch4_currentState = digitalRead(SWITCH4_PIN);

  digitalWrite(LED1_PIN,switch1_currentState);
  digitalWrite(LED2_PIN,switch2_currentState);
  digitalWrite(LED3_PIN,switch3_currentState);
  digitalWrite(LED4_PIN,switch4_currentState);
}

void MiDio::setupPMode(){
  pinMode(SWITCH1_PIN, INPUT);
  pinMode(SWITCH2_PIN, INPUT);
  pinMode(SWITCH3_PIN, INPUT);
  pinMode(SWITCH4_PIN, INPUT);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
}

void MiDio::setupMic() {
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

  void MiDio::setupSpeaker() {
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

void MiDio::setupSD() {
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


  void MiDio::Amplify(){
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

  void MiDio::recordAudio() {
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

void MiDio::clearSD() {
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

void MiDio::playCurrentFile() {
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

void MiDio::playNextFile() {
  currentReadIndex = (currentReadIndex + 1) % MAX_FILES;
  playCurrentFile();
}
void MiDio::Translate(){
  Serial.println("translate function");
}