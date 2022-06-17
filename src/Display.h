/*
  Display - displaying acquired data on a monitor.
  Created by Jan Benda, May 25th, 2021.
*/

/* Adafruit1.44" TFT monitor
 * 
 * Overview:
 * https://learn.adafruit.com/adafruit-1-44-color-tft-with-micro-sd-socket
 * 
 * Wiring monitor on Teensy 3.5:
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
 *
 * Usage:
 *
 * #include <Display.h>
 * #include <Adafruit_ST7735.h> 
 *
 * Display screen;
 *
 * void initScreenST7735() {
 *   // Adafruit 1.44" TFT hardware specific initialization:
 *   #define TFT_SCK   13
 *   #define TFT_MISO  12
 *   #define TFT_MOSI  11
 *   #define TFT_CS    10  
 *   #define TFT_RST   9
 *   #define TFT_DC    8 
 *   Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
 *   tft->initR(INITR_144GREENTAB);
 *   screen.init(tft, 1);
 * }
 *
 * void setup() {
 *   initScreenST7735();
 * }
 *
 */


/* Adafruit 2.8" TFT Touch Shield v2 - capacitive
 * 
 * Overview:
 * https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2
 * 
 * Wiring:
 * https://forums.adafruit.com/download/file.php?id=78488&sid=4a474d33f407a7a019f27e58661732e2
 * from thread:
 * https://forums.adafruit.com/viewtopic.php?f=31&t=170088
 * WARNING: on this image, SDA and SCL are switched!
 *
 * Wiring monitor on Teensy 3.5:
 * 
 * Monitor  Description                         Cable   Teensy
 * ------------------------------------------------------------
 * Vin      power pin                           red     3.3V
 * Gnd      ground                              blue    GND
 * SCK      SPI clock input                     black   SCK0  (pin 13)
 * SO       MISO microcontroller in serial out  green   MISO0 (pin 12)
 * SI       MOSI microcontroller out serial in  yellow  MOSI0 (pin 11)
 * TCS      TFT SPI chip select                 orange  (pin 10)  
 * D/C      SPI data or command selector        white   (pin 8)
 *
 * SDA      I2C data pin (touch)                        SDA0 (pin 18)
 * SCL      I2C clock pin (touch)                       SCL0 (pin 19)
 *
 * Usage:
 *
 * Check out alternative library:
 * https://github.com/PaulStoffregen/ILI9341_t3/blob/master/ILI9341_t3.h
 *
 * #include <Display.h>
 * #include <Adafruit_ILI9341.h> 
 *
 * Display screen;
 *
 * void initScreen() {
 *   // Adafruit 2.8" TFT Touch shield V2 hardware specific initialization:
 *   #define TFT_SCK   13
 *   #define TFT_MISO  12
 *   #define TFT_MOSI  11
 *   #define TFT_CS    10  
 *   #define TFT_RST   9
 *   #define TFT_DC    8 
 *   Adafruit_ILI9341 *tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);
 *   tft->begin();
 *   screen.init(tft, 0);
 * }
 *
 * void setup() {
 *   initScreen();
 * }
 *
 */


#ifndef Display_h
#define Display_h


#include <Arduino.h>
#include <Adafruit_GFX.h>        // Core graphics library

template <class T>
class DisplayWrapper : public Adafruit_GFX {

public:

  DisplayWrapper(T *screen) :
    Adafruit_GFX(screen->width(), screen->height()), Screen(screen) {};
  int16_t width() const { return Screen->width(); };
  int16_t height() const { return Screen->height(); };
  void setRotation(uint8_t r) { Screen->setRotation(r); };
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color)
    { Screen->drawPixel( x, y, color); };
  void fillScreen(uint16_t color) { Screen->fillScreen(color); };
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
    { Screen->drawFastVLine(x, y, h, color); };
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
    { Screen->drawFastHLine(x, y, w, color); };
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
    { Screen->fillRect(x, y, w, h, color); };
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
		  int16_t w, int16_t h, uint16_t color)
    { Screen->drawBitmap(x, y, bitmap, w, h, color); };
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
    { Screen->drawLine(x0, y0, x1, y1, color); };

protected:

  T *Screen;
};


class Display {

 public:

  Display();

  // Initialize the display.
  // If flip is true, flip width and heigh for odd rotations.
  void init(Adafruit_GFX *screen, uint8_t rotation=0, bool flip=false);

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
  // By default center the text vertically into the area. If top is set to true,
  // align the text vertically at the top.
  // Returns the actually needed and used text height as fraction of screen height.
  float setTextArea(uint8_t area, float x0, float y0, float x1, float y1,
		    bool top=false);

  // Clear specified text area.
  void clearText(uint8_t area);

  // Clear all text areas.
  void clearText();

  // Set default font for text areas.
  void setDefaultFont(const GFXfont &font);

  // Set font for specified text area.
  void setFont(uint8_t area, const GFXfont &font);

  // Write into text area.
  void writeText(uint8_t area, const char *text);

  // Scroll text in area.
  void scrollText(uint8_t area);

  // Store current text and write new text into text area.
  void pushText(uint8_t area, const char *text);

  // Write previous text into text area.
  void popText(uint8_t area);


 protected:

  // Colors:
  static const uint16_t Background = 0x0000;       // black
  static const uint16_t PlotBackground = 0x0000;   // black
  static const uint16_t PlotGrid = 0x7BEF;         // gray
  static const uint16_t PlotLines[8];
  static const uint16_t TextBackground = 0x0000;   // black
  static const uint16_t TextColor = 0xffff;        // white

  Adafruit_GFX *Screen;

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

  // Fonts:
  const GFXfont *Font;

  // Text area position and extent:
  int16_t TextX[MaxAreas];
  int16_t TextY[MaxAreas];
  int16_t TextW[MaxAreas];
  int16_t TextH[MaxAreas];
  int16_t TextB[MaxAreas];
  uint16_t TextC[MaxAreas];          // max number of characters
  uint16_t TextI[MaxAreas];          // character offset
  bool TextS[MaxAreas];              // scroll it
  // Text stack:
  static const uint8_t MaxTexts = 8;
  static const size_t MaxChars = 100;
  char Text[MaxAreas][MaxTexts][MaxChars];
  int TextHead[MaxAreas];
  PROGMEM GFXcanvas1 *TextCanvas[MaxAreas];

  void drawText(uint8_t area, const char *text);

};

#endif
