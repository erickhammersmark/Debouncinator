#include "Debouncinator.h"
#include "Arduino.h"

Debouncinator::Debouncinator() {
  debounce_timeout = 50;
  _pinStates = 0x00000000;
  _pinTriggerValues = 0x00000000;
  _pinFired = 0x00000000;
}

void Debouncinator::set_timeout(int new_timeout) {
  debounce_timeout = new_timeout;
}

void Debouncinator::init(int pin) {
  init(pin, 1, 0);
}

void Debouncinator::init(int pin, int state, int triggerValue) {
  pinMode(pin, INPUT);
  digitalWrite(pin, state);
  bitWrite(_pinStates, pin, state);
  bitWrite(_pinFired, pin, 0);
  bitWrite(_pinTriggerValues, pin, triggerValue);
  _pinTimes[pin] = millis();
}

int Debouncinator::read(int pin) {
  unsigned long ms = millis();
  int newState = digitalRead(pin);
  if (newState == bitRead(_pinStates, pin)) {
    _pinTimes[pin] = ms;
  } else if ((ms - _pinTimes[pin]) > debounce_timeout) {
    if (newState == bitRead(_pinTriggerValues, pin)) {
      bitWrite(_pinFired, pin, 1);
    }
    bitWrite(_pinStates, pin, newState);
    _pinTimes[pin] = ms;
  }
  return(bitRead(_pinStates, pin) == bitRead(_pinTriggerValues, pin));
}

int Debouncinator::fired(int pin) {
  if (bitRead(_pinFired, pin)) {
    bitWrite(_pinFired, pin, 0);
    return(1);
  }
  return(0);
}
