#include "NezaButton.h"

// ------------------- Constructors -------------------

NezaButton::NezaButton(uint8_t buttonPin)
  : NezaButton(buttonPin, ActiveLevel::ActiveLow, PullMode::None)
{
}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType)
  : NezaButton(
      buttonPin,
      activeType ? ActiveLevel::ActiveHigh : ActiveLevel::ActiveLow,
      PullMode::None
    )
{
}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType, bool internalPull)
  : NezaButton(
      buttonPin,
      activeType ? ActiveLevel::ActiveHigh : ActiveLevel::ActiveLow,
      (internalPull == NEZABTN_PULLUP) ? PullMode::PullUp : PullMode::PullDown
    )
{
}

NezaButton::NezaButton(uint8_t buttonPin, ActiveLevel active, PullMode pull)
{
  _pin = buttonPin;

  _activeLevel = active;
  _pullMode = pull;
  _activeHigh = (active == ActiveLevel::ActiveHigh) ? 1 : 0;

  debounceTime   = 50;
  multiclickTime = 400;
  longClickTime  = 1500;

  depressed = 0;
  changed   = 0;
  clicks    = 0;

  _begun = 0;

  _lastState = 0;
  _clickCount = 0;
  _longFired = 0;

  _lastBounceTime = 0;
  _stateChangeTime = 0;
}

// ------------------- begin() -------------------

void NezaButton::begin()
{
  configurePin_();

  const uint32_t now = millis();

  uint8_t raw = readNormalized_();

  _lastState = raw;
  depressed = raw;

  changed = 0;
  clicks = 0;

  _clickCount = 0;
  _longFired = 0;

  _lastBounceTime = now;
  _stateChangeTime = now;

  _begun = 1;
}

// ------------------- Private helpers -------------------

void NezaButton::configurePin_()
{
  if (_activeLevel == ActiveLevel::ActiveLow && _pullMode == PullMode::PullUp) {
    pinMode(_pin, INPUT_PULLUP);
  }
  else if (_activeLevel == ActiveLevel::ActiveHigh && _pullMode == PullMode::PullDown) {
    #if defined(INPUT_PULLDOWN)
      pinMode(_pin, INPUT_PULLDOWN);
    #else
      // Some boards do not support internal pulldown.
      // Use an external pulldown resistor on those boards.
      pinMode(_pin, INPUT);
    #endif
  }
  else {
    pinMode(_pin, INPUT);
  }
}

uint8_t NezaButton::readNormalized_() const
{
  uint8_t raw = digitalRead(_pin) ? 1 : 0;

  // Normalize: 1 always means pressed/active.
  if (!_activeHigh) {
    raw ^= 1;
  }

  return raw;
}

// ------------------- Main Update() -------------------

void NezaButton::Update()
{
  // Compatibility fallback:
  // If begin() was not called in setup(), call it automatically here.
  if (!_begun) {
    begin();
  }

  // One-shot event flags.
  changed = 0;
  clicks = 0;

  const uint32_t now = millis();
  uint8_t raw = readNormalized_();

  // Instantaneous edge detected, reset debounce timer.
  if (raw != _lastState) {
    _lastBounceTime = now;
  }

  _lastState = raw;

  // Accept new state only after debounce time.
  if ((now - _lastBounceTime) >= debounceTime && raw != depressed) {
    depressed = raw;
    _stateChangeTime = now;

    // Count only press edges.
    if (depressed) {
      if (_clickCount < 127) {
        ++_clickCount;
      }
    }
  }

  // Long press detection.
  // Examples:
  // _clickCount = 1 -> clicks = -1
  // _clickCount = 2 -> clicks = -2
  // _clickCount = 5 -> clicks = -5
  if (depressed &&
      _clickCount > 0 &&
      !_longFired &&
      (now - _stateChangeTime) >= longClickTime)
  {
    clicks = -_clickCount;
    _clickCount = 0;
    _longFired = 1;
    changed = 1;
  }

  // Short click / multi-click detection.
  // Fires only after button is released and multiclick window has elapsed.
  if (!depressed &&
      _clickCount > 0 &&
      !_longFired &&
      (now - _stateChangeTime) >= multiclickTime)
  {
    clicks = _clickCount;
    _clickCount = 0;
    changed = 1;
  }

  // Reset long-press lock after release.
  if (!depressed && _clickCount == 0) {
    _longFired = 0;
  }
}
