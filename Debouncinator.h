class Debouncinator {
  public:
    Debouncinator();
    void set_timeout(int new_timeout);
    void init(int pin);
    void init(int pin, int state, int triggerValue);
    int read(int pin);
    int fired(int pin);
    int debounce_timeout;
  private:
    unsigned long _pinStates;
    unsigned long _pinFired;
    unsigned long _pinTriggerValues;
    unsigned long _pinTimes[19];
};

