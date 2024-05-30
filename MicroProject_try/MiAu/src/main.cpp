#include <Arduino.h>
#include "driver/i2s.h"

#define I2S_WS 15  // Microphone I2S WS (LRCLK)
#define I2S_SCK 14  // Microphone I2S SCK (BCLK)
#define I2S_SD 32  // Microphone I2S Data

#define I2S_SPEAKER_WS 21  // Speaker I2S WS (LRCLK)
#define I2S_SPEAKER_SCK 22  // Speaker I2S SCK (BCLK)
#define I2S_SPEAKER_SD 23  // Speaker I2S Data

#define I2S_PORT I2S_NUM_0
#define I2S_SPEAKER_PORT I2S_NUM_1

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 1024

class I2SAudio {
public:
  I2SAudio() {}

  void begin() {
    Serial.begin(115200);
    setupMic();
    setupSpeaker();
  }

  void loop() {
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
};

I2SAudio audio;

void setup() {
  audio.begin();
}

void loop() {
  audio.loop();
}