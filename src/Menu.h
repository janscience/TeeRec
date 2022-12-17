/*
  Menu - Select some action from a menu.
  Created by Jan Benda, December 17th, 2022.
*/

#ifndef Menu_h
#define Menu_h


#include <Arduino.h>
#include <Display.h>
#include <PushButtons.h>


class Menu {

public:

  // Maximum number of menu actions.
  static const int MaxActions = 8;
  
  // Maximum characters of a menu entry.
  static const int MaxText = 20;

  // Function type for menu actions.
  typedef void (*Action)(int);

  // Constructor.
  Menu(Display *screen=0, PushButtons *buttons=0);

  // Provide display for showing menu.
  void setDisplay(Display *screen);

  // Provide push buttons for menu navigation.
  void setButtons(PushButtons *buttons, int up_id, int down_id,
		  int select_id, int back_id=-1);

  // Provide push buttons for menu navigation.
  void setButtons(int up_id, int down_id,
		  int select_id, int back_id=-1);

  // Add manu entry with text.
  // If it is selected, action is called.
  int add(const char *text, Action action=0);

  // Draw and execute the menu.
  // Returns index of selected menu entry.
  int exec();

  // Draw the menu.
  void draw();

  
protected:

  void drawAction(int index, bool active);
  

  PushButtons *Buttons;
  int UpID;
  int DownID;
  int SelectID;
  int BackID;
  Display *Screen;
  int NActions;
  char Texts[MaxActions][MaxText];
  Action Actions[MaxActions];
  uint16_t YPos[MaxActions];
  GFXcanvas1 *Canvas;
  uint16_t Baseline;
  uint16_t ActionHeight;
  int Index;

};


#endif
