#include <Arduino.h>
#include <driver/i2s.h>
#include <SD.h>
#include <FS.h>

// Pin definitions for I2S
#define I2S_WS 15    // Microphone I2S WS (LRCLK)
#define I2S_SCK 14   // Microphone I2S SCK (BCLK)
#define I2S_SD 32    // Microphone I2S Data

// Pin definitions for SD card
#define SD_CS_PIN 5   // SD card Chip Select pin
#define SD_MISO_PIN 19 // SD card MISO pin
#define SD_MOSI_PIN 23 // SD card MOSI pin
#define SD_CLK_PIN 18  // SD card Clock pin

// Pin definitions for switches
#define SWITCH_RECORD_PIN 34  // GPIO pin for recording switch
#define SWITCH_NEXT_PIN 35     // GPIO pin for next file switch
#define SWITCH_PLAY_PAUSE_PIN 36 // GPIO pin for play/pause switch

// I2S configuration
#define SAMPLE_RATE 16000
#define BUFFER_SIZE 1024
#define MAX_FILE_SIZE 10485760  // Maximum file size in bytes (10 MB)
#define MAX_FILES 99            // Maximum number of files allowed

// I2S configuration for speaker
#define I2S_SPEAKER_WS 21  // Speaker I2S WS (LRCLK)
#define I2S_SPEAKER_SCK 22 // Speaker I2S SCK (BCLK)
#define I2S_SPEAKER_SD 23  // Speaker I2S Data

// I2S port
#define I2S_PORT I2S_NUM_0
#define I2S_SPEAKER_PORT I2S_NUM_1

// Buffer for audio data
int16_t i2sBuffer[BUFFER_SIZE];

// File to save the audio data
File audioFile;

int fileIndex = 0;

// Playback state
bool isPlaying = false;

// Function declarations
void startPlayback();
void stopPlayback();

void setup() {
    Serial.begin(115200);
    
    // Configure the switch pin
    pinMode(SWITCH_RECORD_PIN, INPUT_PULLUP);
    pinMode(SWITCH_NEXT_PIN, INPUT_PULLUP);
    pinMode(SWITCH_PLAY_PAUSE_PIN, INPUT_PULLUP);
    
    // Initialize I2S
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

    // Initialize SD card
    SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("Card Mount Failed");
        return;
    }
    
    // Initialize I2S speaker
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
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }
    
    // Check number of files in the root directory
    fileIndex = 0;
    File root = SD.open("/");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break; // no more files
        }
        if (!entry.isDirectory()) {
            fileIndex++;
        }
        entry.close();
    }
    root.close();
    if (fileIndex >= MAX_FILES) {
        Serial.println("Maximum number of files reached. Cannot start recording.");
    }
}

void loop() {
    static bool isRecording = false;
    static bool switchState = false;
    static bool lastSwitchState = false;
    static unsigned long fileSize = 0;

    if (digitalRead(SWITCH_RECORD_PIN) == LOW) {
        if (!isPlaying) {
            startPlayback();
        } else {
            stopPlayback();
        }
        delay(100); // Debounce delay
    }

    // Check if the next file switch is pressed to move to the next file
    if (digitalRead(SWITCH_NEXT_PIN) == LOW) {
        fileIndex++;
        // TODO: Add logic to handle when reaching the end of files
        delay(100); // Debounce delay
    }

    // Check if the play/pause switch is pressed to toggle playback
    if (digitalRead(SWITCH_PLAY_PAUSE_PIN) == LOW) {
        if (isPlaying) {
            stopPlayback();
        } else {
            startPlayback();
        }
        delay(100); // Debounce delay
    }
    
    // Read the switch state
    switchState = digitalRead(SWITCH_RECORD_PIN) == LOW;
    
    // Start recording when the switch goes from HIGH to LOW
    if (switchState && !lastSwitchState && fileIndex < MAX_FILES) {
        isRecording = true;
        Serial.println("Recording started");
        String fileName = "/audio" + String(fileIndex++) + ".pcm";
        audioFile = SD.open(fileName.c_str(), FILE_WRITE);
        if (!audioFile) {
            Serial.println("Failed to open file for writing");
            isRecording = false;
        }
        fileSize = 0;
    }
    
    // Stop recording when the switch goes from LOW to HIGH
    if (!switchState && lastSwitchState) {
        isRecording = false;
        Serial.println("Recording stopped");
        if (audioFile) {
            audioFile.close();
        }
    }
    
    // Update last switch state
    lastSwitchState = switchState;
    
    // If recording, read data from I2S and write to the SD card
    if (isRecording) {
        size_t bytesRead = 0;
        i2s_read(I2S_PORT, (void*)i2sBuffer, BUFFER_SIZE * sizeof(int16_t), &bytesRead, portMAX_DELAY);
        audioFile.write((uint8_t*)i2sBuffer, bytesRead);
        fileSize += bytesRead;

        // Stop recording if the file size exceeds the maximum limit
        if (fileSize >= MAX_FILE_SIZE) {
            isRecording = false;
            Serial.println("Recording stopped due to file size limit");
            if (audioFile) {
                audioFile.close();
            }
        }
    }
    
    delay(10); // Shorter delay for better responsiveness
}

void startPlayback() {
    String fileName = "/audio" + String(fileIndex) + ".pcm";
    File audioFile = SD.open(fileName.c_str(), FILE_READ);
    if (!audioFile) {
        Serial.println("Failed to open file for playback");
        return;
    }

    Serial.println("Playback started");
    isPlaying = true;

    size_t bytesRead;
    while (isPlaying && (bytesRead = audioFile.read((uint8_t*)i2sBuffer, sizeof(i2sBuffer))) > 0) {
        size_t bytesWritten;
        i2s_write(I2S_SPEAKER_PORT, i2sBuffer, bytesRead, &bytesWritten, portMAX_DELAY);
    }

    // Playback finished
    audioFile.close();
    Serial.println("Playback finished");
    isPlaying = false;
}

void stopPlayback() {
    Serial.println("Playback stopped");
    isPlaying = false;
}