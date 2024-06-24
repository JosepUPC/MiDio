#ifndef MIDIO_CONSTANTS_H
#define MIDIO_CONSTANTS_H

#include <Arduino.h>

#define I2S_WS 15
#define I2S_SCK 14
#define I2S_SD 32

#define SWITCH1_PIN 26
#define SWITCH2_PIN 34
#define SWITCH3_PIN 35
#define SWITCH4_PIN 25
#define LED1_PIN 13
#define LED2_PIN 12
#define LED3_PIN 16
#define LED4_PIN 4

#define I2S_SPEAKER_WS 21
#define I2S_SPEAKER_SCK 22
#define I2S_SPEAKER_DI 17
#define GAIN 2

#define I2S_PORT I2S_NUM_0
#define I2S_SPEAKER_PORT I2S_NUM_1

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 1024

#define SD_CS 5
#define MAX_FILES 20

int currentIndex = 0;
int currentReadIndex = 0;

bool swtich1_pastState = LOW;
bool swtich2_pastState = LOW;
bool swtich3_pastState = LOW;
bool swtich4_pastState = LOW;

bool switch1_currentState;
bool switch2_currentState;
bool switch3_currentState;
bool switch4_currentState;

bool Gravar = 0;
bool Reproducir = 0;
bool Amplificar = 0;
bool Traducir = 0;

#endif