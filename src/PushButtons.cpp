#include <PushButtons.h>


PushButtons::PushButtons() {
  Interval = 20;
  memset(Pins, 0, sizeof(Pins));
  memset(OnPress, 0, sizeof(OnPress));
  memset(OnRelease, 0, sizeof(OnRelease));
  memset(Pressed, 0, sizeof(Pressed));
  NButtons = 0;
}


PushButtons::~PushButtons() {
  NButtons = 0;
}


int PushButtons::add(int pin, int mode,
		     Callback onpress, Callback onrelease) {
  if (pin < 0)
    return -1;
  int id = NButtons;
  Pins[id] = pin;
  Buttons[id].attach(pin, mode);
  Buttons[id].setPressedState(mode==INPUT_PULLUP?LOW:HIGH);
  Buttons[id].interval(Interval);
  OnPress[id] = onpress;
  OnRelease[id] = onrelease;
  Pressed[id] = false;
  NButtons++;
  return id;
}


void PushButtons::update() {
  for (int k=0; k<NButtons; k++) {
    Buttons[k].update();
    if (Buttons[k].pressed()) {
      Pressed[k] = true;
      if (OnPress[k] != 0)
	OnPress[k](k);
    }
    if (Pressed[k] && Buttons[k].released()) {
      Pressed[k] = false;
      if (OnRelease[k] != 0)
	OnRelease[k](k);
    }
  }
}


int PushButtons::id(int pin) {
  for (int k=0; k<NButtons; k++)
    if (Pins[k] == pin)
      return k;
  return -1;
}

    
void PushButtons::clear(int id) {
  OnPress[id] = 0;
  OnRelease[id] = 0;
}


void PushButtons::set(int id, Callback onpress, Callback onrelease) {
  OnPress[id] = onpress;
  OnRelease[id] = onrelease;
}


bool PushButtons::isPressed(int id) {
  return Buttons[id].isPressed();
}


bool PushButtons::pressed(int id) {
  return Buttons[id].pressed();
}


bool PushButtons::released(int id) {
  return Buttons[id].released();
}


void PushButtons::setInterval(uint16_t interval) {
  Interval = interval;
}


Button *PushButtons::button(int id) {
  if (id >= 0 && id < NButtons)
    return &Buttons[id];
  return NULL;
}
