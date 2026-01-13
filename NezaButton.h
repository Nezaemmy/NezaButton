
#ifndef NEZABUTTON_H
#define NEZABUTTON_H

#include <Arduino.h>

#define NEZABTN_PULLUP   HIGH
#define NEZABTN_PULLDOWN LOW

class NezaButton
{
public:
  // ---- New strongly-typed configuration ----
  enum class ActiveLevel : uint8_t {
    ActiveLow  = 0,
    ActiveHigh = 1
  };

  enum class PullMode : uint8_t {
    None   = 0,
    PullUp = 1,
    PullDown = 2
  };

  // ----  constructor ----
  NezaButton(uint8_t buttonPin,
             ActiveLevel active = ActiveLevel::ActiveLow,
             PullMode pull = PullMode::None);

  NezaButton(uint8_t buttonPin);                              // defaults
  NezaButton(uint8_t buttonPin, bool activeType);             // HIGH/LOW
  NezaButton(uint8_t buttonPin, bool activeType, bool internalPull); // NEZABTN_PULLUP / NEZABTN_PULLDOWN

  // Call this frequently in loop()
  void Update();

  // --------- Public status (fast uint8_t flags) ----------
  uint8_t depressed;   // Debounced state (1 = active)
  uint8_t changed;     // 1 for the Update() when clicks is latched

  // --------- Tunable timings (defaults set in ctor) -------
  uint8_t  debounceTime;     // ms
  uint16_t multiclickTime;   // ms
  uint16_t longClickTime;    // ms

  // Short/long click results (negative => long press). One-shot per Update().
  int8_t clicks;

private:
  // Helper to apply input mode and read normalized state
  void configurePin_(ActiveLevel active, PullMode pull);
  uint8_t readNormalized_() const;

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


