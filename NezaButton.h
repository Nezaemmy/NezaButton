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

    // --------- Public status (packed to save RAM) ----------
    bool depressed : 1;  // Debounced button state (true = active)
    bool changed  : 1;   // State change flag (set when clicks is finalized)

    // --------- Tunable timings (downsized types) -----------
    // Matches your declared maxima: 50 ms, 500 ms, 10000 ms
    uint8_t  debounceTime;    // 0..255 ms (you use <= 50 ms)
    uint16_t multiclickTime;  // 0..65535 ms (you use <= 500 ms)
    uint16_t longClickTime;   // 0..65535 ms (you use <= 10000 ms)

    // Short/long click results (negative => long press)
    int8_t clicks;

  private:
    // Keep layout tight to minimize padding
    uint8_t _pin;             // GPIO pin

    // State bits (packed)
    bool _activeHigh : 1;     // true: active HIGH, false: active LOW
    bool _btnState   : 1;     // instantaneous (normalized) raw state
    bool _lastState  : 1;     // previous instantaneous state

    int8_t   _clickCount;     // accumulates clicks while depressed
    uint16_t _lastBounceTime; // lower 16 bits of millis()

    // NOTE: We rely on modular arithmetic with uint16_t:
    // (uint16_t)(now - then) works across wrap, up to ~65.536s.
};

#endif
