#include <Arduino.h>
#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

static const unsigned ledPin = 13;

bool ledOn = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.begin(9600);  
  Serial.println("Ready.");
}

void loop() {
  if (MIDI.read()) {
    Serial.println("MIDI read!");
    ledOn = !ledOn;
    if (ledOn) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
}