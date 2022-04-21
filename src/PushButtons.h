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
  static const int MaxButtons = 20;


  // Function type for on-press and on-release callbacks.
  typedef void (*Callback)(int);


  PushButtons();
  ~PushButtons();

  // Add a button at a pin (if pin >= 0) and mode (INPUT or INPUT_PULLUP.
  // Optionally install callbacks that are called on press and release events. 
  // Returns the id of the button.
  int add(int pin, int mode=INPUT, Callback onpress=0, Callback onrelease=0);

  // Update state of all buttons. Call this as frequently as possible in loop().
  void update();

  // True if the specified button is currently pressed down.
  bool isPressed(int id);

  // True if the specified button has been pressed.
  // Do not use this function if you have a callback installed.
  bool pressed(int id);

  // True if the specified button has been released.
  // Do not use this function if you have a callback installed.
  bool released(int id);

  // Set the debouncing interval. Default is 20ms.
  void setInterval(uint16_t interval);


protected:

  uint16_t Interval;

  int NButtons = 0;                     // Number of buttons.
  Button Buttons[MaxButtons];  // The buttons.
  Callback OnPress[MaxButtons];
  Callback OnRelease[MaxButtons];

};


#endif
