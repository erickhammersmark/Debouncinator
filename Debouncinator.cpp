#include "Debouncinator.h"
#include "Arduino.h"

Debouncinator::Debouncinator() {
  debounce_timeout = 50;
  _pinStates = 0x00000000;
  _pinTriggerValues = 0x00000000;
  _pinFired = 0x00000000;
  _pinAnalog = 0x00000000;

  uint8_t i;
  for (i=0; i<NUM_DB_PINS; ++i) {
    _pinTimes[i] = 0;
    _pinCallbacks[i] = NULL;
    _pinAnalogStates[i] = 0;
    _pinNewAnalogStates[i] = 0;
  }
}

void Debouncinator::set_timeout(int new_timeout) {
  debounce_timeout = new_timeout;
}

// If you do not specify, pin is set high and triggers low
void Debouncinator::init(int pin) {
  init(pin, 1, 0);
}

/*
 * "state" is the state to which the pin should be initialized
 * "triggerValue" is the value that will trigger "fired" when the pin
 * transitions to it.
 */
void Debouncinator::init(int pin, int state, int triggerValue) {
  pinMode(pin, INPUT);
  digitalWrite(pin, state);
  bitWrite(_pinStates, pin, state);
  bitWrite(_pinFired, pin, 0);
  bitWrite(_pinTriggerValues, pin, triggerValue);
  _pinTimes[pin] = millis();
}

/*
 * The callback will be passed the int read via analogRead
 * and should return the state represented by the value
 * for purposes of debounce state tracking.
 */
void Debouncinator::init_analog(int pin, int (*cb)(int)) {
  init(pin, 0, 0);
  bitWrite(_pinAnalog, pin, 1);
  _pinCallbacks[pin] = cb;
  _pinAnalogStates[pin] = cb(0);
  _pinNewAnalogStates[pin] = cb(0);
}

int Debouncinator::read(int pin) {
  unsigned long ms = millis();
  int newState;

  /*
   * TODO:
   * Should there be an option for digital pins to "fire" on every state transition, like analong pins?
   * Consider keeping a history of debounced transitions and timestamps. Would this be useful?
   * Probably useful to have per-pin debounce timeout, especially analog vs. digital. Who knows
   * WHAT the settle time will be for whatever an analog pin is reading?
   */

  //
  // Analog logic
  //

  if (bitRead(_pinAnalog, pin)) {
    // Track the return value of the callback if we have a callback
    // Otherwise, just track the raw value (this sucks, use a callback).
    newState = _pinCallbacks[pin] ? _pinCallbacks[pin](analogRead(pin)) : analogRead(pin);

    if (newState == _pinAnalogStates[pin]) {
      // pin state has not changed
      _pinTimes[pin] = ms;
    } else if (newState == _pinNewAnalogStates[pin]) {
      // pin has been moving to a new state, and it is still in that state
      // analog pins "fire" every time they change state
      if ((ms - _pinTimes[pin]) > debounce_timeout) {
        bitWrite(_pinFired, pin, 1);
        _pinAnalogStates[pin] = newState;
        _pinTimes[pin] = ms;
      }
    } else {
      // pin is in a brand new state, start debounce timing
      _pinNewAnalogStates[pin] = newState;
      _pinTimes[pin] = ms;
    }
    return _pinAnalogStates[pin];
  }

  //
  // Digital logic
  //

  newState = digitalRead(pin);

  if (newState == bitRead(_pinStates, pin)) {
    _pinTimes[pin] = ms;
  } else {
    if ((ms - _pinTimes[pin]) > debounce_timeout) {
      if (newState == bitRead(_pinTriggerValues, pin)) {
        bitWrite(_pinFired, pin, 1);
      }
      bitWrite(_pinStates, pin, newState);
      _pinTimes[pin] = ms;
    }
  }
  return(bitRead(_pinStates, pin) == bitRead(_pinTriggerValues, pin));
}

void Debouncinator::read_all(void) {
  uint8_t i;
  for (i=0; i<NUM_DB_PINS; ++i)
    if (_pinTimes[i] != 0)
      read(i);
}

int Debouncinator::state(int pin) {
  if (bitRead(_pinAnalog, pin))
    return _pinAnalogStates[pin];
  return(bitRead(_pinStates, pin) == bitRead(_pinTriggerValues, pin));
}

int Debouncinator::fired(int pin) {
  if (bitRead(_pinFired, pin)) {
    bitWrite(_pinFired, pin, 0);
    return 1;
  }
  return 0;
}
