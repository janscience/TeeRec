/*
  AllDisplays - include selected TFT library for the examples.
  Created by Jan Benda, June 18th, 2022.
*/

#ifndef AllDisplays_h
#define AllDisplays_h


#include <Arduino.h>
#include <Display.h>


#if defined(ST7735_T3)
  #include <ST7735_t3.h>
  #include "fonts/FreeSans7pt7b.h"
  #include "fonts/FreeSans8pt7b.h"
  #include "fonts/FreeSans6pt7b.h"
#elif defined(ST7789_T3)
  #include <ST7789_t3.h>
  #include "fonts/FreeSans10pt7b.h"
  #include "fonts/FreeSans12pt7b.h"
#elif defined(ILI9341_T3)
  #include <ILI9341_t3.h>
  #include "fonts/FreeSans7pt7b.h"
  #include "fonts/FreeSans8pt7b.h"
  #include "fonts/FreeSans6pt7b.h"
#elif defined(ILI9488_T3)
  #include <ILI9488_t3.h>
  #include "fonts/FreeSans7pt7b.h"
  #include "fonts/FreeSans8pt7b.h"
  #include "fonts/FreeSans6pt7b.h"
#elif defined(ST7735_ADAFRUIT)
  #include <Adafruit_ST7735.h>
  #include "fonts/FreeSans7pt7b.h"
  #include "fonts/FreeSans8pt7b.h"
  #include "fonts/FreeSans6pt7b.h"
#elif defined(ST7789_ADAFRUIT)
  #include <Adafruit_ST7789.h>
  #include "fonts/FreeSans10pt7b.h"
  #include "fonts/FreeSans12pt7b.h"
#elif defined(ILI9341_ADAFRUIT)
  #include "Adafruit_ILI9341.h"
  #include "fonts/FreeSans8pt7b.h"
  #include "fonts/FreeSans10pt7b.h"
  #include "fonts/FreeSans7pt7b.h"
#endif
#if defined(FT6206)
  #include <Wire.h>
  #include <Adafruit_FT6206.h>
#endif


void initScreen(Display &screen) {
#if defined(ST7735_T3)
  ST7735_t3 *tft = new ST7735_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->initR(INITR_144GREENTAB);
  DisplayWrapper<ST7735_t3> *tftscreen = new DisplayWrapper<ST7735_t3>(tft);
  screen.init(tftscreen, 1);
  Serial.println();
  screen.setDefaultFont(FreeSans7pt7b);
  screen.setTitleFont(FreeSans8pt7b);
  screen.setSmallFont(FreeSans6pt7b);
#elif defined(ST7789_T3)
  ST7789_t3 *tft = new ST7789_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->init(240, 320);
  DisplayWrapper<ST7789_t3> *tftscreen = new DisplayWrapper<ST7789_t3>(tft);
  screen.init(tftscreen, 1, true);
  Serial.println();
  screen.setDefaultFont(FreeSans10pt7b);
  screen.setTitleFont(FreeSans12pt7b);
  screen.setSmallFont(FreeSans10pt7b);
#elif defined(ILI9341_T3)
  ILI9341_t3 *tft = new ILI9341_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->begin();
  DisplayWrapper<ILI9341_t3> *tftscreen = new DisplayWrapper<ILI9341_t3>(tft);
  screen.init(tftscreen, 3);
  screen.setDefaultFont(FreeSans7pt7b);
  screen.setTitleFont(FreeSans8pt7b);
  screen.setSmallFont(FreeSans6pt7b);
#elif defined(ILI9488_T3)
  ILI9488_t3 *tft = new ILI9488_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->begin();
  DisplayWrapper<ILI9488_t3> *tftscreen = new DisplayWrapper<ILI9488_t3>(tft);
  screen.init(tftscreen, 3);
  screen.setDefaultFont(FreeSans7pt7b);
  screen.setTitleFont(FreeSans8pt7b);
  screen.setSmallFont(FreeSans6pt7b);
#elif defined(ST7735_ADAFRUIT)
  Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->initR(INITR_144GREENTAB);
  screen.init(tft, 3);
  screen.setDefaultFont(FreeSans7pt7b);
  screen.setTitleFont(FreeSans8pt7b);
  screen.setSmallFont(FreeSans6pt7b);
#elif defined(ST7789_ADAFRUIT)
  Adafruit_ST7789 *tft = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->init(240, 320);
  DisplayWrapper<Adafruit_ST7789> *tftscreen = new DisplayWrapper<Adafruit_ST7789>(tft);
  screen.init(tftscreen, 1, true);
  screen.setDefaultFont(FreeSans10pt7b);
  screen.setTitleFont(FreeSans12pt7b);
  screen.setSmallFont(FreeSans10pt7b);
#elif defined(ILI9341_ADAFRUIT)
  Adafruit_ILI9341 *tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);
  tft->begin();
  screen.init(tft, 1);
  screen.setDefaultFont(FreeSans8pt7b);
  screen.setTitleFont(FreeSans10pt7b);
  screen.setSmallFont(FreeSans7pt7b);
#endif
#if defined(FT6206)
  if (! touch.begin(128)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }
#endif
  screen.setBacklightPin(TFT_BL);
  screen.clear();
}


#ifdef AI_SPLASH_SCREEN
void AIsplashScreen(Display &screen,
		    const InputADC &aidata, const char *title) {
  char msg[100];
  String convspeed = aidata.conversionSpeedShortStr();
  String samplspeed = aidata.samplingSpeedShortStr();
  char chans0[50];
  char chans1[50];
  aidata.channels(0, chans0);
  aidata.channels(1, chans1);
  if (chans0[0] == '\0')
    strcpy(chans0, "-");
  if (chans1[0] == '\0')
    strcpy(chans1, "-");
  float bt = aidata.bufferTime();
  char bts[20];
  if (bt < 1.0)
    sprintf(bts, "%.0fms\n", 1000.0*bt);
  else
    sprintf(bts, "%.2fs\n", bt);
  sprintf(msg, "%.0fkHz\n%dbit\n%d,%s,%s\n%s\n%s\n%s",
          0.001*aidata.rate(), aidata.resolution(), aidata.averaging(),
          convspeed.c_str(), samplspeed.c_str(), chans0, chans1, bts);
  screen.setTextArea(0, 0.0, 0.75, 1.0, 0.95);
  screen.setTitleFont(0);
  screen.setTextArea(1, 0.0, 0.0, 0.4, 0.7, true);
  screen.setSmallFont(1);
  screen.setTextArea(2, 0.4, 0.0, 1.0, 0.7, true);
  screen.setSmallFont(2);
  screen.writeText(0, title);
  screen.writeText(1, "rate:\nres.:\nspeed:\nADC0:\nADC1\nbuffer:");
  screen.writeText(2, msg);
  screen.fadeBacklightOn();
  delay(3000);
  screen.fadeBacklightOff();
  screen.clear();
  screen.clearText();
}
#endif

#endif
