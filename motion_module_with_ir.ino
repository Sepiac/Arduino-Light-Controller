#include <IRLib.h>
#define MOTION_MODE 0
#define OFF_MODE 1
#define ON_MODE 2

const int externalDevicePin = 3;
const int remotePin = 4;
const int motionPin = 5;
const int ledIndicatorPin = 13;

unsigned long remoteCode = 0;
unsigned long lastMotionTime;
unsigned long shutoffDuration = 60000 * 10;

int detectionMode = MOTION_MODE;

IRrecv receiver(remotePin);
IRdecode decoder;

void setup() {
  pinMode(externalDevicePin, OUTPUT);
  pinMode(motionPin, INPUT);
  receiver.enableIRIn();

  //scan for initial remote code
  while (remoteCode == 0) {
    if(receiver.GetResults(&decoder)) {
      decoder.decode();
      remoteCode = decoder.value;
      receiver.resume();
    }
    
    digitalWrite(ledIndicatorPin, millis()%1000 < 500);
  }

  digitalWrite(ledIndicatorPin, LOW);
}

void loop() {

  if(receiver.GetResults(&decoder)) {
    decoder.decode();
    //toggle between motion detection and off
    if(decoder.value == remoteCode ) {
      if(detectionMode == MOTION_MODE) {
        detectionMode = OFF_MODE;
      } else if(detectionMode == OFF_MODE) {
        detectionMode = MOTION_MODE;
        digitalWrite(externalDevicePin, HIGH);
      }
    }

    receiver.resume();
  }

  switch(detectionMode) {
    case MOTION_MODE:
      if(digitalRead(motionPin)) {
        lastMotionTime = millis();
        digitalWrite(externalDevicePin, HIGH);
      } else if(millis() > lastMotionTime + shutoffDuration){
        digitalWrite(externalDevicePin, LOW);
      }
      break;
    case OFF_MODE:
      digitalWrite(externalDevicePin, LOW);
      break;
    case ON_MODE:
    digitalWrite(externalDevicePin, HIGH);
      break;
  }
}
