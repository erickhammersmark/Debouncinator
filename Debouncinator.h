#define NUM_DB_PINS 24

class Debouncinator {
  public:
    Debouncinator();
    void set_timeout(int new_timeout);
    void init(int pin);
    void init(int pin, int state, int triggerValue);
    void init_analog(int pin, int (*cb)(int));
    int read(int pin);
    void read_all(void);
    int state(int pin);
    int fired(int pin);
    int debounce_timeout;
  private:
    unsigned long _pinStates;
    unsigned long _pinFired;
    unsigned long _pinTriggerValues;
    unsigned long _pinAnalog;
    int _pinAnalogStates[NUM_DB_PINS];
    int _pinNewAnalogStates[NUM_DB_PINS];
    int (* _pinCallbacks[NUM_DB_PINS])(int);
    unsigned long _pinTimes[NUM_DB_PINS];
};

