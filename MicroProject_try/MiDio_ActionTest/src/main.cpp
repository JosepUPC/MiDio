#include <Arduino.h>
#include <MiDio.cpp>

MiDio Device_1;

void setup() {
  Serial.begin(115200);
  Device_1.begin();
}

void loop() {
  Device_1.PinCheck();
  if(switch1_currentState != swtich1_pastState){
    if(switch1_currentState){
      Amplificar = 1;
    }
    else
      Amplificar = 0;
    swtich1_pastState = switch1_currentState;
  };

  if(switch2_currentState != swtich2_pastState && Amplificar && !Traducir){
    if (Reproducir){
      Device_1.playNextFile();
    }
    else{
      if(switch2_currentState){
        Amplificar = 0;
        Gravar = 1;
        Device_1.recordAudio();
      }
      else
        Gravar = 0;
    }
    swtich2_pastState = switch2_currentState;
  }

  if(switch3_currentState != swtich3_pastState && !Amplificar && !Traducir){
    if (Gravar){
      if(switch3_currentState)
        Device_1.clearSD();
    }
    else{
      if(switch3_currentState){
        Device_1.playNextFile();
        Amplificar = 0;
        Reproducir = 1;
      }
      else{
        Reproducir = 0;
      }
    }
    swtich3_pastState = switch3_currentState;
  }

  if(switch4_currentState != swtich4_pastState && Amplificar){
    if(switch4_currentState){
      Device_1.Translate();
      Amplificar = 0;
      Traducir = 1;
    }
    else{
      Traducir = 0;
    }
    swtich4_pastState = switch4_currentState;
  }

  if(Amplificar){
    Device_1.Amplify();
  }
};

