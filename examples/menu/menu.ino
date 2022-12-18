// select a library for the TFT display:
//#define ST7735_T3
#define ST7789_T3
//#define ILI9341_T3  // XXX does not compile yet
//#define ILI9488_T3
//#define ST7735_ADAFRUIT
//#define ST7789_ADAFRUIT
//#define ILI9341_ADAFRUIT

// define pins to control TFT display:
/*
#define TFT_ROTATION 0
#define TFT_SCK   13
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_CS    10  
#define TFT_RST    8 // 9
#define TFT_DC     7 // 8 
#define TFT_BL    30 // backlight PWM, -1 to not use it
*/

#define TFT_ROTATION      3
#define TFT_SCK      32   // SPI1 bus
#define TFT_CS       31  
#define TFT_MOSI      0   // SPI1 bus
#define TFT_RST       1
#define TFT_DC       15
#define TFT_BL       30   // backlight PWM

// define pins for push buttons:
#define DOWN_PIN         26
#define UP_PIN           27
#define SELECT_PIN       28
#define BACK_PIN         29

#include <Display.h>
#include <AllDisplays.h>       // edit this file for your TFT monitor
#include <PushButtons.h>
#include <Menu.h>


Display screen;
PushButtons buttons;
Menu menu;
Menu radiomenu;
Menu submenu;


void initButtons() {
  int select = buttons.add(SELECT_PIN, INPUT_PULLUP);
  int back = buttons.add(BACK_PIN, INPUT_PULLUP);
  int up = buttons.add(UP_PIN, INPUT_PULLUP);
  int down = buttons.add(DOWN_PIN, INPUT_PULLUP);
  menu.setButtons(&buttons, up, down, select, back);
  menu.setDisplay(&screen);
}


void menuAction(int id) {
  Serial.printf("Selected menu %d\n", id);
}


void submenuAction(int id) {
  Serial.printf("Selected submenu %d\n", id);
}


void initMenu() {
  submenu.setTitle("Sub menu");
  submenu.addAction("Subaction 0", submenuAction);
  submenu.addCheckable("Subaction 1", true);
  submenu.addAction("Subaction 2", submenuAction);
  radiomenu.setTitle("Select one");
  radiomenu.addRadioButton("Subaction 00", true);
  radiomenu.addRadioButton("Subaction 11");
  radiomenu.addRadioButton("Subaction 22");
  menu.setTitle("Main menu");
  menu.add("Action A");
  menu.addCheckable("Action B", false);
  menu.addAction("Action C", menuAction);
  menu.addMenu("Select", radiomenu);
  menu.addMenu("Submenu", submenu);
}


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 200) {};
  initScreen(screen);
  screen.screen()->setRotation(TFT_ROTATION);
  initMenu();
  initButtons();
  screen.setBacklightOn();
  Serial.println("Menu");
  int selected = menu.exec();
  Serial.printf("Menu selected %d\n", selected);
  Serial.println("STOP");
}


void loop() {
}
