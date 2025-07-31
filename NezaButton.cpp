#include "NezaButton.h"

NezaButton::NezaButton(uint8_t buttonPin)
  : NezaButton(buttonPin, LOW, false) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType)
  : NezaButton(buttonPin, activeType, false) {}

NezaButton::NezaButton(uint8_t buttonPin, bool activeType, bool internalPull)
{
  _pin = buttonPin;
  _activeHigh = activeType;
  _btnState = !_activeHigh;
  _lastState = _btnState;
  _clickCount = 0;
  clicks = 0;
  depressed = false;
  changed = false;
  _lastBounceTime = 0;

  if (_activeHigh == LOW && internalPull == NEZABTN_PULLUP)
    pinMode(_pin, INPUT_PULLUP);
  else if (_activeHigh == HIGH && internalPull == NEZABTN_PULLDOWN)
    pinMode(_pin, INPUT_PULLDOWN);
  else
    pinMode(_pin, INPUT);
}

void NezaButton::Update()
{
  uint32_t now = millis();
  _btnState = digitalRead(_pin);

  if (!_activeHigh) _btnState = !_btnState;

  if (_btnState != _lastState)
    _lastBounceTime = now;

  if ((now - _lastBounceTime > debounceTime) && (_btnState != depressed))
  {
    depressed = _btnState;
    if (depressed) _clickCount++;
  }

  changed = (_lastState != _btnState) ? false : changed;
  _lastState = _btnState;

  if (!depressed && (now - _lastBounceTime > multiclickTime))
  {
    clicks = _clickCount;
    _clickCount = 0;
    if (clicks != 0) changed = true;
  }

  if (depressed && (now - _lastBounceTime > longClickTime))
  {
    clicks = 0 - _clickCount;
    _clickCount = 0;
    if (clicks != 0) changed = true;
  }
}
