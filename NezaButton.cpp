
#include "NezaButton.h"

// Delegating constructors
NezaButton::NezaButton(uint8_t buttonPin)
  : NezaButton(buttonPin, LOW, false) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType)
  : NezaButton(buttonPin, activeType, false) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType, bool internalPull)
{
  _pin        = buttonPin;
  _activeHigh = activeType ? 1 : 0;

  // Start raw-normalized state as "inactive" (only matters for first edge timing)
  _btnState   = _activeHigh ? 0 : 1; // normalized inactive is 0; initial value is used only for first compare
  _lastState  = _btnState;

  // Public flags
  depressed   = 0;
  changed     = 0;

  // Clicks / counters
  _clickCount = 0;
  clicks      = 0;

  // Timers
  _lastBounceTime  = 0;
  _stateChangeTime = 0;

  // Configure input mode (ESP32 supports both PULLUP and PULLDOWN on many pins; avoid 34..39)
  if (!_activeHigh && internalPull == NEZABTN_PULLUP) {
    pinMode(_pin, INPUT_PULLUP);     // active LOW + pull-up
  } else if (_activeHigh && internalPull == NEZABTN_PULLDOWN) {
    pinMode(_pin, INPUT_PULLDOWN);   // active HIGH + pull-down
  } else {
    pinMode(_pin, INPUT);            // external pull or none
  }
}

void NezaButton::Update()
{
  // One-shot events per Update() for compatibility
  changed = 0;
  clicks  = 0;

  const uint16_t now16 = (uint16_t)millis();

  // Read raw and normalize to "1 = active"
  uint8_t raw = digitalRead(_pin) ? 1 : 0;
  if (!_activeHigh) raw ^= 1; // invert if active LOW

  // Any instantaneous edge? reset bounce timer
  if (raw != _lastState) {
    _lastBounceTime = now16;
  }
  _lastState = raw;
  _btnState  = raw;

  // Debounce: accept the level if stable beyond debounceTime
  if ((uint16_t)(now16 - _lastBounceTime) > debounceTime && (_btnState != depressed)) {
    depressed = _btnState;
    _stateChangeTime = now16;        // debounced change time

    if (depressed) {
      if (_clickCount < 127) ++_clickCount; // avoid overflow
    }
  }

  // Finalize short clicks once released and multi-click window elapsed
  if (!depressed &&
      _clickCount > 0 &&
      (uint16_t)(now16 - _stateChangeTime) > multiclickTime) {
    clicks = _clickCount;   // 1, 2, 3...
    _clickCount = 0;
    changed = 1;
  }

  // Finalize long press once held beyond longClickTime (negative clicks)
  if (depressed &&
      _clickCount > 0 &&
      (uint16_t)(now16 - _stateChangeTime) > longClickTime) {
    clicks = (int8_t)(0 - _clickCount); // e.g., -1 for long press
    _clickCount = 0;
    changed = 1;
 }
}
