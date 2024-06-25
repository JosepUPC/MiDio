#include <Arduino.h>

#define SWITCH1_PIN 34
#define SWITCH2_PIN 35

bool switch1_lastState = LOW;
bool switch2_lastState = LOW;

bool load, reproduce, clean;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Configure the switch pins as input with internal pull-up resistors
    pinMode(SWITCH1_PIN, INPUT);
    pinMode(SWITCH2_PIN, INPUT);
}

void loop() {
    bool switch1_currentState = digitalRead(SWITCH1_PIN);
    bool switch2_currentState = digitalRead(SWITCH2_PIN);

    if (switch1_currentState == LOW && switch2_currentState == LOW){
        bool load = false;
        bool reproduce = false;
        bool clean = false;
    }

    if (switch1_currentState != switch1_lastState){
        if (reproduce){
            Serial.print("readNextFileFromSD()");
        }
        else{
            if(switch1_currentState){
                load = true;
                Serial.print("writeFileToSD()");
            }
            else
                load = false;
                clean = false;
        }
        switch1_lastState = switch1_currentState;
    }
    if (switch2_currentState != switch2_lastState){
        if (load){
            if (switch2_currentState && !clean)
                Serial.print("cleanSD()");
                clean = true;
        }
        else{
            if(switch2_currentState){
                reproduce = true;
                Serial.print("readFileFromSD()");
            }
            else
                reproduce = false;
        }
        switch2_lastState = switch2_currentState;
    }
    delay(100);
}