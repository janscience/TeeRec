#include <Menu.h>


#define BLACK    0x0000
#define WHITE    0xFFFF


Menu::Menu(Display *screen, PushButtons *buttons) :
  Buttons(buttons),
  UpID(-1),
  DownID(-1),
  SelectID(-1),
  BackID(-1),
  Screen(screen),
  NActions(0),
  Title(""),
  Canvas(0),
  Baseline(0),
  ActionHeight(0),
  Index(0) {
  memset(Texts, 0, sizeof(Texts));
  memset(Actions, 0, sizeof(Actions));
  memset(IDs, 0, sizeof(IDs));
  memset(YPos, 0, sizeof(YPos));
}


void Menu::setButtons(PushButtons *buttons, int up_id, int down_id,
		      int select_id, int back_id) {
  Buttons = buttons;
  UpID = up_id;
  DownID = down_id;
  SelectID = select_id;
  BackID = back_id;
}


void Menu::setButtons(int up_id, int down_id,
		      int select_id, int back_id) {
  UpID = up_id;
  DownID = down_id;
  SelectID = select_id;
  BackID = back_id;
}


void Menu::setDisplay(Display *screen) {
  Screen = screen;
}


void Menu::setTitle(const char *title) {
  strncpy(Title, title, MaxText);
}


int Menu::add(const char *text, Action action, int id) {
  if (id < 0)
    id = NActions;
  strncpy(Texts[NActions], text, MaxText);
  Actions[NActions] = action;
  IDs[NActions] = id;
  NActions++;
  return id;
}


int Menu::add(const char *text, int id) {
  return add(text, 0, id);
}


void Menu::drawAction(int index, bool active) {
  uint16_t xoffs = Screen->defaultFont()->yAdvance;
  Canvas->fillScreen(0x0000);
  Canvas->setCursor(xoffs, Baseline);
  Canvas->print(Texts[index]);
  if (active)
    Screen->screen()->drawBitmap(0, YPos[index], Canvas->getBuffer(),
				 Screen->width(), ActionHeight, BLACK, WHITE);
  else
    Screen->screen()->drawBitmap(0, YPos[index], Canvas->getBuffer(),
				 Screen->width(), ActionHeight, WHITE, BLACK);
}


void Menu::draw() {
  int n = NActions;
  if (strlen(Title) > 0)
    n++;
  Screen->clear();
  uint16_t height = Screen->height();
  uint16_t width = Screen->width();
  ActionHeight = height/n;
  uint16_t font_height = Screen->defaultFont()->yAdvance;
  Baseline = (ActionHeight - font_height)/2 + 4*font_height/5;
  if (Canvas == 0)
    Canvas = new GFXcanvas1(width, ActionHeight);
  Canvas->setRotation(0);
  Canvas->setFont(Screen->defaultFont());
  Canvas->setTextColor(0xffff);
  Canvas->setTextSize(1);
  Canvas->setTextWrap(false);
  uint16_t ypos = 0;
  if (strlen(Title) > 0) {
    Canvas->fillScreen(0x0000);
    Canvas->setCursor(0, Baseline);
    Canvas->print(Title);
    Screen->screen()->drawBitmap(0, ypos, Canvas->getBuffer(),
				 Screen->width(), ActionHeight, WHITE, BLACK);
    ypos += ActionHeight;
  }
  for (int k=0; k<NActions; k++) {
    YPos[k] = ypos;
    drawAction(k, k == Index);
    ypos += ActionHeight;
  }
  Screen->setBacklightOn();
}


int Menu::exec() {
  int index = -1;
  bool enabled = Buttons->enabled();
  Buttons->disable();
  draw();
  Index = 0;
  while (1) {
    Buttons->update();
    if (UpID >= 0 && Buttons->pressed(UpID)) {
      if (Index > 0) {
	drawAction(Index, false);
	Index--;
	drawAction(Index, true);
      }
    }
    else if (DownID >= 0 && Buttons->pressed(DownID)) {
      if (Index < NActions-1) {
	drawAction(Index, false);
	Index++;
	drawAction(Index, true);
      }
    }
    else if (SelectID >= 0 && Buttons->pressed(SelectID)) {
      drawAction(Index, false);
      delay(200);
      drawAction(Index, true);
      delay(200);
      Buttons->waitReleased(SelectID);
      index = IDs[Index];
      if (Actions[Index] != 0) {
	Actions[Index](index);
	draw();
      }
      else
	break;
    }
    else if (BackID >= 0 && Buttons->pressed(BackID)) {
      Buttons->waitReleased(BackID);
      index = -1;
      break;
    }
    yield();
  }
  if (enabled)
    Buttons->enable();
  return index;
}

