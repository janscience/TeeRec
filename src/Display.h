/*
  EFishMonitor - library for displaying electric fish recordings on a monitor.
  Created by Jan Benda, May 25th, 2021.
*/

/* Adafruit TFT monitor
 * 
 * Overview and wiring:
 * https://learn.adafruit.com/adafruit-1-44-color-tft-with-micro-sd-socket
 * 
 * Adafruit GFX library tutorial:
 * https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
 * 
 * Wiring adafruit monitor on Teensy 3.5:
 * 
 * Monitor  Description                         Cable   Teensy
 * ------------------------------------------------------------
 * Vin      power pin                           red     3.3V
 * 3V3      voltage output
 * Gnd      ground                              blue    GND
 * SCK      SPI clock input                     black   SCK0  (pin 13)
 * SO       MISO microcontroller in serial out          MISO0 (pin 12)
 * SI       MOSI microcontroller out serial in  yellow  MOSI0 (pin 11)
 * TCS      TFT SPI chip select                 orange  (pin 10)  
 * RST      reset pin                           green   (pin 9) 
 * D/C      SPI data or command selector        white   (pin 8)
 * CCS      SD card SPI chip select             brown
 * Lite     PWM backlight control 
 */


#ifndef Display_h
#define Display_h


#include <Arduino.h>
#include <Adafruit_GFX.h>        // Core graphics library
#include <Adafruit_ST7735.h>     // Hardware-specific library
#include "fonts/FreeSans7pt7b.h" // Nice font
#include <SPI.h> 


#define TFT_SCK   13
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_CS    10  
#define TFT_RST   9
#define TFT_DC    8 


class Display {

 public:

  Display(uint8_t rotation=0);

  // Clear the entire screen with the background color.
  void clear();

  // Define area where to plot traces.
  void setPlotArea(uint8_t area, float x0, float y0, float x1, float y1);

  // Define multiple areas where to plot traces.
  void setPlotAreas(int num, float x0, float y0, float x1, float y1);

  // Number of defined plot areas.
  int numPlots() const;

  // Clear specified plot area.
  void clearPlot(uint8_t area);

  // Clear all plot areas.
  void clearPlots();

  // Plot data trace from buffer in plot area with some color (index into PlotLines).
  void plot(uint8_t area, float *buffer, int nbuffer, int color=0);

  // Define text area. 
  // Returns the actually needed and used text height as fraction of screen height.
  float setTextArea(uint8_t area, float x0, float y0, float x1, float y1);

  // Clear specified text area.
  void clearText(uint8_t area);

  // Clear all text areas.
  void clearText();

  // Write into text area.
  void writeText(uint8_t area, const char *text);

  // Scroll text in area.
  void scrollText(uint8_t area);

  // Store current text and write new text into text area.
  void pushText(uint8_t area, const char *text);

  // Write previous text into text area.
  void popText(uint8_t area);

  Adafruit_ST7735 *Screen;


 protected:

  // Colors:
  static const uint16_t Background = ST7735_BLACK;
  static const uint16_t PlotBackground = ST7735_BLACK;
  static const uint16_t PlotGrid = 0x7BEF;
  static const uint16_t PlotLines[8];
  static const uint16_t TextBackground = ST7735_BLACK;
  static const uint16_t TextColor = ST7735_WHITE;

  // Fonts:
  static constexpr GFXfont const &Font = FreeSans7pt7b;

  // Monitor dimensions:
  uint16_t Width;
  uint16_t Height;

  // Plot area position and extent:
  static const uint8_t MaxAreas = 8;
  int NPlots = 0;
  int16_t PlotX[MaxAreas];
  int16_t PlotY[MaxAreas];
  int16_t PlotW[MaxAreas];
  int16_t PlotH[MaxAreas];
  float PlotYOffs[MaxAreas];
  float PlotYScale[MaxAreas];

  // Translate x value to x-coordinate.
  uint16_t dataX(uint8_t area, float x, float maxx);
  // Translate y value (between -1 and 1) to y-coordinate.
  uint16_t dataY(uint8_t area, float y);

  // Text area position and extent:
  int16_t TextX[MaxAreas];
  int16_t TextY[MaxAreas];
  int16_t TextW[MaxAreas];
  int16_t TextH[MaxAreas];
  int16_t TextB[MaxAreas];
  uint16_t TextC[MaxAreas];   // max number of characters
  uint16_t TextI[MaxAreas];   // character offset
  bool TextS[MaxAreas];       // scroll it
  char *TextT[MaxAreas];      // text
  PROGMEM GFXcanvas1 *TextCanvas[MaxAreas];
  // Text stack:
  static const uint8_t MaxTexts = 8;
  char *Text[MaxAreas][MaxTexts];
  int TextHead[MaxAreas];

  void drawText(uint8_t area, const char *text);

};


#endif
