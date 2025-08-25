#include "NezaButton.h"

// Delegating constructors
NezaButton::NezaButton(uint8_t buttonPin)
  : NezaButton(buttonPin, LOW, false) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType)
  : NezaButton(buttonPin, activeType, false) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType, bool internalPull)
{
  _pin        = buttonPin;
  _activeHigh = activeType;

  // Start raw-normalized state as "inactive"
  _btnState   = !_activeHigh;
  _lastState  = _btnState;

  // Public flags
  depressed   = false;
  changed     = false;

  // Clicks / counters
  _clickCount = 0;
  clicks      = 0;

  // 16-bit timebase (we use modular subtraction)
  _lastBounceTime = 0;

  // Configure input mode
  if (_activeHigh == LOW && internalPull == NEZABTN_PULLUP) {
    pinMode(_pin, INPUT_PULLUP);
  } else if (_activeHigh == HIGH && internalPull == NEZABTN_PULLDOWN) {
    pinMode(_pin, INPUT_PULLDOWN);
  } else {
    pinMode(_pin, INPUT);
  }
}

void NezaButton::Update()
{
  // Snapshot time (lower 16 bits are enough for our intervals)
  const uint16_t now16 = (uint16_t)millis();

  // Read raw and normalize to "true = active"
  bool raw = (digitalRead(_pin) != 0);
  if (!_activeHigh) raw = !raw;

  // Any edge? reset bounce timer (modular arithmetic with uint16_t)
  if (raw != _lastState) {
    _lastBounceTime = now16;
  }

  _btnState = raw;

  // If stable beyond debounceTime, accept the level
  if ((uint16_t)(now16 - _lastBounceTime) > debounceTime && (_btnState != depressed)) {
    depressed = _btnState;
    if (depressed) {
      // New press within current multi-click window
      if (_clickCount < INT8_MAX) ++_clickCount;
    }
  }

  // Keep "changed" only for latched results (click group complete or long press)
  changed = (_lastState != _btnState) ? false : changed;
  _lastState = _btnState;

  // Finalize short clicks once released and multi-click window elapsed
  if (!depressed && (uint16_t)(now16 - _lastBounceTime) > multiclickTime) {
    clicks = _clickCount;   // 1, 2, 3...
    _clickCount = 0;
    if (clicks != 0) changed = true;
  }

  // Finalize long press once held beyond longClickTime (negative clicks)
  if (depressed && (uint16_t)(now16 - _lastBounceTime) > longClickTime) {
    clicks = (int8_t)(0 - _clickCount); // e.g., -1 for long press
    _clickCount = 0;
    if (clicks != 0) changed = true;
  }
}
