#include <Arduino.h>
#include "driver/i2s.h"

#define I2S_WS 15
#define I2S_SCK 14
#define I2S_SD 32

#define I2S_PORT I2S_NUM_0

#define SAMPLE_RATE 16000

void setup() {
  Serial.begin(115200);

  // Configure I2S
  i2s_config_t i2s_config = {
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

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  // Install and start I2S driver
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_set_clk(I2S_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void loop() {
  const int bufferSize = 1024;
  int16_t buffer[bufferSize];

  // Read audio data from I2S
  size_t bytesRead;
  i2s_read(I2S_PORT, buffer, bufferSize * sizeof(int16_t), &bytesRead, portMAX_DELAY);

  // Process or store the audio data
  // We print the first 10 samples to the serial monitor
  for (int i = 0; i < 10; i++) {
    Serial.println(buffer[i]);
  }

  delay(1000); // Adjust
}
