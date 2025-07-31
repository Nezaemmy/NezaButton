#ifndef NezaButton_H
#define NezaButton_H

#include <Arduino.h>

#define NEZABTN_PULLUP HIGH
#define NEZABTN_PULLDOWN LOW

class NezaButton
{
  public:
    NezaButton(uint8_t buttonPin);
    NezaButton(uint8_t buttonPin, bool active);
    NezaButton(uint8_t buttonPin, bool active, bool internalPull);
    void Update();

    int clicks;                        // Number of button clicks
    bool depressed;                    // Debounced button state
    bool changed;                      // State change flag

    uint32_t debounceTime;             // Debounce duration in ms
    uint32_t multiclickTime;          // Max time between clicks
    uint32_t longClickTime;           // Time to register long click

  private:
    uint8_t _pin;
    bool _activeHigh;
    bool _btnState;
    bool _lastState;
    int _clickCount;
    uint32_t _lastBounceTime;
};

#endif
