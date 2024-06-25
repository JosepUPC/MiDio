#include <Arduino.h>
#include "serviceManager.h"

serviceManager manager;

void setup() {
    manager.begin();
    manager.SetupWavFileTask(); // Add this line to activate the WAV file processing task
    manager.SetupTaskCreat();
}

void loop() {
    // Main loop can remain empty or handle other tasks
}