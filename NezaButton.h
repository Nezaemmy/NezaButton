#ifndef NEZABUTTON_H
#define NEZABUTTON_H

#include <Arduino.h>

#define NEZABTN_PULLUP   HIGH
#define NEZABTN_PULLDOWN LOW

class NezaButton
{
public:
  enum class ActiveLevel : uint8_t {
    ActiveLow  = 0,
    ActiveHigh = 1
  };

  enum class PullMode : uint8_t {
    None     = 0,
    PullUp   = 1,
    PullDown = 2
  };

  // ---- Constructors ----
  NezaButton(uint8_t buttonPin);
  NezaButton(uint8_t buttonPin, bool activeType);
  NezaButton(uint8_t buttonPin, bool activeType, bool internalPull);
  NezaButton(uint8_t buttonPin, ActiveLevel active, PullMode pull);

  // Call once in setup().
  // If forgotten, Update() will call it automatically.
  void begin();

  // Call frequently in loop().
  void Update();

  // --------- Public status flags ----------
  uint8_t depressed;   // Debounced state: 1 = pressed/active, 0 = released
  uint8_t changed;     // 1 only when clicks is latched during this Update()

  // --------- Tunable timings ----------
  uint8_t  debounceTime;     // milliseconds
  uint16_t multiclickTime;   // milliseconds
  uint16_t longClickTime;    // milliseconds

  // Click result:
  //  0  = no event
  //  1  = single click
  //  2  = double click
  //  3  = triple click
  //  5  = five clicks, etc.
  // -1  = single long press
  // -2  = double click where second press is held
  // -5  = five clicks where fifth press is held
  int8_t clicks;

private:
  void configurePin_();
  uint8_t readNormalized_() const;

  uint8_t _pin;

  ActiveLevel _activeLevel;
  PullMode _pullMode;

  uint8_t _activeHigh;
  uint8_t _begun;

  uint8_t _lastState;
  int8_t  _clickCount;
  uint8_t _longFired;

  uint32_t _lastBounceTime;
  uint32_t _stateChangeTime;
};

#endif
