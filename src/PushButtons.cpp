#include <PushButtons.h>


PushButtons::PushButtons() {
  Interval = 20;
  memset(OnPress, 0, sizeof(OnPress));
  memset(OnRelease, 0, sizeof(OnRelease));
  memset(Pressed, 0, sizeof(Pressed));
  NButtons = 0;
}


PushButtons::~PushButtons() {
  NButtons = 0;
}


int PushButtons::add(int pin, int mode, Callback onpress, Callback onrelease) {
  if (pin < 0)
    return -1;
  int id = NButtons;
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
