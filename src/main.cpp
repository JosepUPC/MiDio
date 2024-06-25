#include <MiDio.cpp>

MiDio Device_1;

void setup() {
  Serial.begin(115200);
  Device_1.begin();
}

void loop() {
  Device_1.PinCheck();
  if(switch1_currentState != switch1_pastState){
    if(switch1_currentState){
      Amplificar = 1;
    }
    else
      Amplificar = 0;
    switch1_pastState = switch1_currentState;
  };

  if(switch2_currentState != switch2_pastState && Amplificar && !Traducir){
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
    switch2_pastState = switch2_currentState;
  }

  if(switch3_currentState != switch3_pastState && !Amplificar && !Traducir){
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
    switch3_pastState = switch3_currentState;
  }

  if(switch4_currentState != switch4_pastState && Amplificar){
    if(switch4_currentState){
      Device_1.Translate();
      Amplificar = 0;
      Traducir = 1;
    }
    else{
      Traducir = 0;
    }
    switch4_pastState = switch4_currentState;
  }

  if(Amplificar){
    Device_1.Amplify();
  }
};

