#include <PushButtons.h>


PushButtons::PushButtons() {
  Interval = 20;
  memset(Buttons, 0, sizeof(Buttons));
  memset(OnPress, 0, sizeof(Buttons));
  memset(OnRelease, 0, sizeof(Buttons));
  NButtons = 0;
}


PushButtons::~PushButtons() {
  /*
  for (int k=0; k<NButtons; k++) {
    if (Buttons[k] != 0)
      delete Buttons[k];
  }
  */
  NButtons = 0;
}


int PushButtons::add(int pin, int mode, Callback onpress, Callback onrelease) {
  if (pin < 0)
    return -1;
  int id = NButtons;
  Buttons[id] = new Bounce2::Button;
  Buttons[id]->attach(pin, mode);
  Buttons[id]->setPressedState(mode==INPUT_PULLUP?LOW:HIGH);
  Buttons[id]->interval(Interval);
  OnPress[id] = onpress;
  OnRelease[id] = onrelease;
  NButtons++;
  return id;
}


void PushButtons::update() {
  for (int k=0; k<NButtons; k++) {
    Buttons[k]->update();
    if (OnPress[k] != 0 && Buttons[k]->pressed())
      OnPress[k](k);
    if (OnRelease[k] != 0 && Buttons[k]->released())
      OnRelease[k](k);
  }
}


bool PushButtons::isPressed(int id) {
  return Buttons[id]->isPressed();
}


bool PushButtons::pressed(int id) {
  return Buttons[id]->pressed();
}


bool PushButtons::released(int id) {
  return Buttons[id]->released();
}


void PushButtons::setInterval(uint16_t interval) {
  Interval = interval;
}

