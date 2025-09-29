
#ifndef NezaButton_H
#define NezaButton_H

#include <Arduino.h>

#define NEZABTN_PULLUP   HIGH
#define NEZABTN_PULLDOWN LOW

class NezaButton
{
  public:
    // Constructors
    NezaButton(uint8_t buttonPin);
    NezaButton(uint8_t buttonPin, bool active);
    NezaButton(uint8_t buttonPin, bool active, bool internalPull);

    // Call this frequently in loop()
    void Update();

    // --------- Public status (fast uint8_t flags) ----------
    uint8_t depressed;   // Debounced state (1 = active)
    uint8_t changed;     // 1 for the Update() when clicks is latched

    // --------- Tunable timings (with defaults) -------------
    uint8_t  debounceTime;    
    uint16_t multiclickTime;  
    uint16_t longClickTime;   

    // Short/long click results (negative => long press). One-shot per Update().
    int8_t clicks;

  private:
    uint8_t _pin;

    // State bytes (fast)
    uint8_t _activeHigh;      // 1: active HIGH, 0: active LOW
    uint8_t _btnState;        // instantaneous normalized raw (1=active)
    uint8_t _lastState;       // last instantaneous normalized raw

    int8_t   _clickCount;       // counts presses within a group
    uint16_t _lastBounceTime;   // lower 16 bits of millis()
    uint16_t _stateChangeTime;  // debounced change time (for long/multi)
};

#endif
