#include <IRLib.h>
const int externalDevicePin = 3;
const int remotePin = 4;
const int motionPin = 5;

unsigned long lastMotionTime;
unsigned long shutoffDuration = 60000 * 5;
/*
*0: default behavior (turn light on with motion for 5 minutes aka automatic mode)
*1: leave light off
*2: leave light on
*/
//the indicator pin is low when automatic mode is ON and high when automatic mode is OFF
const int automaticModeIndicatorPin = 13;
int detectionMode = 0;
IRrecv receiver(remotePin);
IRdecode decoder;

void setup() {
  // put your setup code here, to run once:
  pinMode(externalDevicePin, OUTPUT);
  pinMode(automaticModeIndicatorPin, OUTPUT);
  pinMode(motionPin, INPUT);
  receiver.enableIRIn();
}

void loop() {

  if(receiver.GetResults(&decoder)) {
    decoder.decode();
    switch(decoder.value) {
      case 0x20DF8877:
        detectionMode = 0;
        digitalWrite(automaticModeIndicatorPin, LOW);
        break;
      case 0x20DF48B7:
        detectionMode = 1;
         digitalWrite(automaticModeIndicatorPin, HIGH);
        break;
      case 0x20DFC837:
        detectionMode = 2;
         digitalWrite(automaticModeIndicatorPin, HIGH);
        break;
    }

    receiver.resume();
  }

  switch(detectionMode) {
    case 0:
      if(digitalRead(motionPin)) {
        lastMotionTime = millis();
        digitalWrite(externalDevicePin, HIGH);
      } else if(millis() > lastMotionTime + shutoffDuration){
        digitalWrite(externalDevicePin, LOW);
      }
      break;
    case 1:
      digitalWrite(externalDevicePin, LOW);
      break;
    case 2:
    digitalWrite(externalDevicePin, HIGH);
      break;
  }
}
