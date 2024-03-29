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

  // The Adafruit_GFX screen object.
  Adafruit_GFX *screen() { return Screen; };

  // Width of the display in pixels.
  uint16_t width() const { return Width; };

  // Height of the display in pixels.
  uint16_t height() const { return Height; };

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

  // Plot data trace from buffer in plot area with some color
  // (index into PlotLines).
  void plot(uint8_t area, const int16_t *buffer, int nbuffer, int color=0);

  // Plot data trace from buffer (-1 to 1) in plot area with some
  // color (index into PlotLines).
  void plot(uint8_t area, const float *buffer, int nbuffer, int color=0);

  // Set amplitude zoom factor of plot area to fac.
  void setPlotZoom(uint8_t area, float fac);

  // The zoom factor for the amplitude of plot area.
  float plotZoom(uint8_t area);

  // Define text area. 
  // By default center the text vertically into the area. If top is set to true,
  // align the text vertically at the top.
  // Returns the actually needed and used text height as fraction of screen height.
  float setTextArea(uint8_t area, float x0, float y0, float x1, float y1,
		    bool top=false);

  // Set foreground and background color for text in specified area.
  void setTextColors(uint8_t area, uint16_t color, uint16_t background);

  // Set foreground and background color for text in specified area to
  // default colors.
  void setDefaultTextColors(uint8_t area);

  // Swap foreground and background colors for specified text area.
  void swapTextColors(uint8_t area);

  // Clear specified text area.
  void clearText(uint8_t area);

  // Clear all text areas.
  void clearText();

  // Return default font used for text areas.
  const GFXfont *defaultFont() const { return Font; };

  // Set default font for text areas.
  void setDefaultFont(const GFXfont &font);
  
  // Return title font used for text areas.
  const GFXfont *titleFont() const { return TitleFont; };

  // Set font for titles.
  void setTitleFont(const GFXfont &titlefont);
  
  // Return small font used for text areas.
  const GFXfont *smallFont() const { return SmallFont; };

  // Set font smaller than the default font.
  void setSmallFont(const GFXfont &smallfont);

  // Set font for specified text area.
  void setFont(uint8_t area, const GFXfont &font);

  // Set font for specified text area to the title font.
  void setTitleFont(uint8_t area);

  // Set font for specified text area to the small font.
  void setSmallFont(uint8_t area);

  // Write into text area.
  void writeText(uint8_t area, const char *text);

  // Rewrite previously written text into specified text area.
  void rewriteText(uint8_t area);

  // Scroll text in area.
  void scrollText(uint8_t area);

  // Store current text and write new text into text area.
  void pushText(uint8_t area, const char *text);

  // Write previous text into text area.
  void popText(uint8_t area);

  // Set the PWN pin used for controlling backlight
  // and switch backlight off.
  void setBacklightPin(int backlightpin);

  // Set backlight to 0 (off) - 1 (on).
  void setBacklight(float backlight);

  // Set backlight on.
  void setBacklightOn();

  // Set backlight off.
  void setBacklightOff();

  // Fade backlight on.
  // Speed is the delay between increasing backlight values in ms.
  void fadeBacklightOn(int speed=2);

  // Fade backlight off.
  // Speed is the delay between decreasing backlight values in ms.
  void fadeBacklightOff(int speed=2);


 protected:

  // Colors:
  static const uint16_t Background = 0x0000;       // black
  static const uint16_t PlotBackground = 0x0000;   // black
  static const uint16_t PlotGrid = 0x7BEF;         // gray
  static const uint16_t PlotLines[8];
  static const uint16_t DefaultTextBackground = 0x0000; // black
  static const uint16_t DefaultTextColor = 0xffff;      // white

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
  float PlotYZoom[MaxAreas];

  // Translate x value to x-coordinate.
  uint16_t dataX(uint8_t area, float x, float maxx);
  // Translate y value (between -1 and 1) to y-coordinate.
  uint16_t dataY(uint8_t area, float y);
  // Translate y value to y-coordinate.
  uint16_t dataY(uint8_t area, int16_t y);

  // Fonts:
  const GFXfont *Font;
  const GFXfont *TitleFont;
  const GFXfont *SmallFont;

  // Text area position and extent:
  int16_t TextX[MaxAreas];
  int16_t TextY[MaxAreas];
  int16_t TextW[MaxAreas];
  int16_t TextH[MaxAreas];
  int16_t TextB[MaxAreas];
  uint16_t TextC[MaxAreas];          // max number of characters
  uint16_t TextI[MaxAreas];          // character offset
  bool TextS[MaxAreas];              // scroll it
  bool TextColorsSwapped[MaxAreas];
  uint16_t TextBackground[MaxAreas];
  uint16_t TextColor[MaxAreas];
  // Text stack:
  static const uint8_t MaxTexts = 4;
  static const size_t MaxChars = 64;
  char Text[MaxAreas][MaxTexts][MaxChars];
  int TextHead[MaxAreas];
  GFXcanvas1 *TextCanvas[MaxAreas];

  void drawText(uint8_t area, const char *text);

  // backlight:
  int BacklightPin;
  const int MaxBacklight = 256;

};

#endif
