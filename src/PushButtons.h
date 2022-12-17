/*
  PushButtons - manage and query push buttons with callback functions.
  Wrapper for the Bounce2 library (https://github.com/thomasfredericks/Bounce2).
  Created by Jan Benda, June 5th, 2021.
*/

#ifndef PushButtons_h
#define PushButtons_h


#include <Arduino.h>
#include <Bounce2.h>


class PushButtons {

public:

  // Maximum number of buttons that can be managed.
  static const int MaxButtons = 8;


  // Function type for on-press and on-release callbacks.
  typedef void (*Callback)(int);


  PushButtons();
  ~PushButtons();

  // Add a button at a pin (if pin >= 0) and mode (INPUT or INPUT_PULLUP).
  // Optionally install callbacks that are called on press and release events. 
  // Returns the id of the button.
  int add(int pin, int mode=INPUT, Callback onpress=0, Callback onrelease=0);

  // Update state of all buttons. Call this as frequently as possible in loop().
  void update();

  // Get id of button on pin.
  int id(int pin) const;

  // Uninstall the callback functions for button of specified id.
  void clear(int id);

  // Install callbacks that are called on press and release events for
  // button id.
  void set(int id, Callback onpress=0, Callback onrelease=0);

  // Enable callbacks of all buttons.
  void enable();

  // Disable callbacks of all buttons.
  void disable();

  // True if callbacks are enabled.
  bool enabled() const;
  
  // True if the specified button is currently pressed down.
  bool isPressed(int id);

  // True if the specified button has been pressed.
  // Do not use this function if you have a callback installed.
  bool pressed(int id);

  // True if the specified button has been released.
  // Do not use this function if you have a callback installed.
  bool released(int id);

  // Loop until the released specified button is pressed.
  void waitPressed(int id);

  // Loop until the pressed specified button is released.
  void waitReleased(int id);

  // Set the debouncing interval. Default is 20ms.
  void setInterval(uint16_t interval);

  // Returns the Button object of button id.
  Button *button(int id);


protected:

  uint16_t Interval;

  int NButtons;                // number of buttons.
  Button Buttons[MaxButtons];  // buttons ...
  int Pins[MaxButtons];        // and their pins.
  bool Enabled;
  Callback OnPress[MaxButtons];
  Callback OnRelease[MaxButtons];
  bool Pressed[MaxButtons];

};


#endif
