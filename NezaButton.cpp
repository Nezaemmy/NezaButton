
#include "NezaButton.h"

// -------------------  constructors  -------------------
NezaButton::NezaButton(uint8_t buttonPin)
  : NezaButton(buttonPin, ActiveLevel::ActiveLow, PullMode::None) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType)
  : NezaButton(buttonPin,
               activeType ? ActiveLevel::ActiveHigh : ActiveLevel::ActiveLow,
               PullMode::None) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType, bool internalPull)
  : NezaButton(buttonPin,
               activeType ? ActiveLevel::ActiveHigh : ActiveLevel::ActiveLow,
               // internalPull == HIGH => PullUp, LOW => PullDown
               (internalPull == NEZABTN_PULLUP) ? PullMode::PullUp : PullMode::PullDown) {}


NezaButton::NezaButton(uint8_t buttonPin, ActiveLevel active, PullMode pull)
{
  _pin        = buttonPin;
  _activeHigh = (active == ActiveLevel::ActiveHigh) ? 1 : 0;

  debounceTime   = 50;    // ms
  multiclickTime = 400;   // ms
  longClickTime  = 1500;  // ms

  // Public one-shot / status flags
  depressed = 0;
  changed   = 0;
  clicks    = 0;

  // Internal counters/timers
  _clickCount = 0;
  _lastBounceTime  = 0;
  _stateChangeTime = 0;

  // Configure input mode (pull-up/pull-down/none)
  configurePin_(active, pull);

  // Read *actual* initial pin state after pinMode()
  // Normalize: 1 = pressed/active
  const uint16_t now16 = (uint16_t)millis();

  uint8_t raw = readNormalized_();
  _btnState = raw;
  _lastState = raw;
  depressed = raw;

  // Avoid “phantom” timing on first Update()
  _lastBounceTime  = now16;
  _stateChangeTime = now16;

  // IMPORTANT: do not auto-count a click if button is held at boot
  _clickCount = 0;
}


// ------------------- Helpers -------------------
void NezaButton::configurePin_(ActiveLevel active, PullMode pull)
{
  // ESP32 supports INPUT_PULLUP and INPUT_PULLDOWN on many GPIOs.
  // If pull mode doesn't match active logic, we leave INPUT (external pull expected).
  if (active == ActiveLevel::ActiveLow && pull == PullMode::PullUp) {
    pinMode(_pin, INPUT_PULLUP);
  } else if (active == ActiveLevel::ActiveHigh && pull == PullMode::PullDown) {
    pinMode(_pin, INPUT_PULLDOWN);
  } else {
    pinMode(_pin, INPUT);
  }
}

uint8_t NezaButton::readNormalized_() const
{
  // Read raw and normalize to "1 = active"
  uint8_t raw = digitalRead(_pin) ? 1 : 0;
  if (!_activeHigh) raw ^= 1; // invert if active LOW
  return raw;
}


// ------------------- Main Update() -------------------
void NezaButton::Update()
{
  // One-shot events per Update() for compatibility
  changed = 0;
  clicks  = 0;

  const uint16_t now16 = (uint16_t)millis();

  uint8_t raw = readNormalized_();

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

