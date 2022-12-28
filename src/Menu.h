/*
  Menu - Select actions from a menu.
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
  static const int MaxText = 30;

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

  // Set a title for the menu.
  void setTitle(const char *title);

  // Add menu entry with text.
  // If it is selected, action is called.
  // The optional identifier id is returned and passed on to the action.
  // If the id is negative it is set to the index of the new menu action. 
  int addAction(const char *text, Action action, int id=-1);
  
  // Add menu entry with text.
  // The optional identifier id is returned and passed on to the action.
  // If the id is negative it is set to the index of the new menu action. 
  int add(const char *text, int id=-1);

  // Add a checkable menu entry with text.
  // If it is selected, its checked state is toggled.
  // The optional identifier id is returned and passed on to the action.
  // If the id is negative it is set to the index of the new menu action. 
  int addCheckable(const char *text, bool checked, int id=-1);

  // Add a radio button with text.
  // If it is selected, its checked state is set
  // and all other radio buttons are unchecked.
  // The optional identifier id is returned and passed on to the action.
  // If the id is negative it is set to the index of the new menu action. 
  int addRadioButton(const char *text, bool checked=false, int id=-1);

  // Add submenu entry with text.
  // If it is selected, the menu is executed.
  void addMenu(const char *text, Menu &menu);

  // Return the number of actions in the menu.
  int nActions() const { return NActions; };

  // Check whether menu entry id is checked.
  bool checked(int id) const;

  // Return id of checked radio button.
  int checked() const;

  // Provide a callback function that is called when a menu entry is checked.
  void setCheckedAction(Action action);

  // If a menu entry is checked, return from the menu immediately.
  void setCheckedReturns(bool returns=true);

  // Draw and execute the menu.
  // Returns ID of selected menu entry.
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
  Action CheckedAction;
  bool CheckedReturns;
  int NActions;
  char Texts[MaxActions][MaxText];
  Action Actions[MaxActions];
  int8_t Checked[MaxActions];
  bool RadioButton[MaxActions];
  Menu *Menus[MaxActions];
  int IDs[MaxActions];
  uint16_t YPos[MaxActions];
  char Title[MaxText];
  GFXcanvas1 *Canvas;
  uint16_t Baseline;
  uint16_t ActionHeight;
  int Index;

};


#endif
