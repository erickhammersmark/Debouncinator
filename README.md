# Debouncinator
Debounce library for Arduino (and others)

Stick this in a directory called "Debouncinator" and put that in your Arduino libraries directory.  Call init() for each pin you want to debounce, optionally with an initial state and a trigger state (default is write the pin HIGH and trigger on LOW).  Call read() every time you want to check a pin.  read() will return the state of the pin, and it will mark the pin as "fired" if it transitions from the non-trigger state to the trigger state.  fired() will return whether or not the pin has fired since the last time you called fired().

Example usage:

    #include <Debouncinator.h>
    
    #define SWITCH_PIN 4
    #define BUTTON_PIN 5
    #define THIRD_PIN 6
    
    Debouncinator db;
    
    void setup() {
      db.init(SWITCH_PIN);
      db.init(BUTTON_PIN);
      db.init(THIRD_PIN, LOW, HIGH); // pin, state to write to the pin, trigger state
    }
    
    void loop() {
      if (db.read(BUTTON_PIN)) {
        // BUTTON_PIN is LOW right now
      }
      db.read(SWITCH_PIN);
      db.read(THIRD_PIN);
      if (db.fired(SWITCH_PIN)) {
        // SWITCH_PIN has transitioned to LOW
      }
      if (db.fired(THIRD_PIN)) {
        // THIRD_PIN has transitioned to HIGH
      }
    }
