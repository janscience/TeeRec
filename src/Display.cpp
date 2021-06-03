#include <Arduino.h>
#include "Display.h"


const uint16_t Display::DataLines[8] = {ST7735_GREEN, ST7735_YELLOW,
					     ST7735_BLUE, ST7735_RED,
					     ST7735_CYAN, ST7735_ORANGE,
					     ST7735_MAGENTA, ST7735_WHITE};


Display::Display(uint8_t rotation) {
  for (int k=0; k<MaxAreas; k++) {
    DataX[k] = 0;
    DataY[k] = 0;
    DataW[k] = 0;
    DataH[k] = 0;
    DataYOffs[k] = 0;
    DataYScale[k] = 0;
    TextX[k] = 0;
    TextY[k] = 0;
    TextW[k] = 0;
    TextH[k] = 0;
    TextB[k] = 0;
    TextC[k] = 0;
    TextI[k] = 0;
    TextT[k] = 0;
    TextS[k] = false;
    TextCanvas[k] = 0;
    TextHead[k] = 0;
  }
  memset(Text, 0, sizeof(Text));
  Screen = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
  Screen->initR(INITR_144GREENTAB);
  Screen->setRotation(rotation);
  Width = Screen->width();
  Height = Screen->height();
  clear();
}


void Display::clear() {
  Screen->fillScreen(Background);
}


void Display::setDataArea(uint8_t area, float x0, float y0, float x1, float y1) {
  DataX[area] = uint16_t(x0*Width);
  DataY[area] = uint16_t((1.0-y1)*Height);
  DataW[area] = uint16_t((x1-x0)*Width);
  DataH[area] = uint16_t((y1-y0)*Height);
  DataYOffs[area] = DataY[area] + 0.5*DataH[area];
  DataYScale[area] = 0.5*DataH[area];
}
  

void Display::setDataArea(float x0, float y0, float x1, float y1) {
  setDataArea(0, x0, y0, x1, y1);
}
  

void Display::clearData(uint8_t area) {
  if (DataW[area] == 0 )
    return;
  Screen->fillRect(DataX[area], DataY[area], DataW[area], DataH[area]+1, DataBackground);
  Screen->drawFastHLine(DataX[area], dataY(area, 0), DataW[area], DataGrid);
}


void Display::clearData() {
  for (int k=0; k<MaxAreas; k++)
    clearData(k);
}


void Display::plotData(uint8_t area, float *buffer, int nbuffer, int color) {
  if (DataW[area] == 0 )
    return;
  color %= 8;
  uint16_t x0 = dataX(area, 0, nbuffer);
  uint16_t y0 = dataY(area, buffer[0]);
  for (uint16_t k=1; k<nbuffer; k++) {
    uint16_t x = dataX(area, k, nbuffer);
    uint16_t y = dataY(area, buffer[k]);
    Screen->drawLine(x0, y0, x, y, DataLines[color]);
    x0 = x;
    y0 = y;
  }
}


void Display::plotData(float *buffer, int nbuffer, int channel) {
  plotData(0, buffer, nbuffer, channel);
}


uint16_t Display::dataX(uint8_t area, float x, float maxx) {
  return DataX[area] + uint16_t(x/maxx*DataW[area]);
}


uint16_t Display::dataY(uint8_t area, float y) {
  return uint16_t(DataYOffs[area] - DataYScale[area]*y);
}


float Display::setTextArea(uint8_t area, float x0, float y0, float x1, float y1) {
  TextX[area] = uint16_t(x0*Width);
  TextY[area] = uint16_t((1.0-y1)*Height);
  TextW[area] = uint16_t((x1-x0)*Width);
  TextH[area] = uint16_t((y1-y0)*Height);
  /*
  if (TextCanvas[area] != 0)
    delete TextCanvas[area];
  */
  TextCanvas[area] = new GFXcanvas1(TextW[area], TextH[area]);
  TextCanvas[area]->setRotation(0);
  TextCanvas[area]->setFont(&Font);
  TextCanvas[area]->setTextColor(0xffff);
  TextCanvas[area]->setTextSize(1);
  TextCanvas[area]->setTextWrap(false);
  int16_t  x, y;
  uint16_t w, h;
  TextCanvas[area]->getTextBounds("Agy", 0, 4*TextH[area]/5, &x, &y, &w, &h);
  TextB[area] = TextH[area] - (y + h - 4*TextH[area]/5) - 1;
  if (TextH[area] > h)
    TextB[area] -= (TextH[area] - h - 1)/2;
  TextC[area] = TextW[area]*3/w;
  TextI[area] = 0;
  TextT[area] = 0;
  return float(h)/float(Height);
}
  

void Display::clearText(uint8_t area) {
  if (TextW[area] == 0 )
    return;
  if (TextT != 0) {
    delete [] TextT[area];
    TextT[area] = 0;
  }
  TextCanvas[area]->fillScreen(0x0000);
  Screen->drawBitmap(TextX[area], TextY[area], TextCanvas[area]->getBuffer(),
		     TextW[area], TextH[area], TextColor, TextBackground);
}


void Display::clearText() {
  for (int k=0; k<MaxAreas; k++)
    clearText(k);
}


void Display::drawText(uint8_t area, const char *text) {
  TextCanvas[area]->fillScreen(0x0000);
  TextCanvas[area]->setCursor(0, TextB[area]);
  TextCanvas[area]->print(text);
  Screen->drawBitmap(TextX[area], TextY[area], TextCanvas[area]->getBuffer(),
		     TextW[area], TextH[area], TextColor, TextBackground);
}


void Display::writeText(uint8_t area, const char *text) {
  drawText(area, text);
  if (TextT != 0)
    delete [] TextT[area];
  TextT[area] = new char[strlen(text)+1];
  strcpy(TextT[area], text);
  TextS[area] = (TextCanvas[area]->getCursorX() > TextW[area]);
}


void Display::scrollText(uint8_t area) {
  if (TextT[area] == 0)
    return;
  if (! TextS[area])
    return;
  TextI[area]++;
  if (TextI[area] >= strlen(TextT[area]) - TextC[area])
    TextI[area] = 0;
  drawText(area, &TextT[area][TextI[area]]);
}


void Display::pushText(uint8_t area, const char *text) {
  if (TextHead[area] < MaxTexts && TextT != 0) {
    Text[area][TextHead[area]] = TextT[area];
    TextT[area] = 0;
    TextHead[area]++;
  }
  writeText(area, text);
}


void Display::popText(uint8_t area) {
  if (TextT != 0) {
    delete [] TextT[area];
    TextT[area] = 0;
  }
  if (TextHead[area] > 0) {
    TextHead[area]--;
    TextT[area] = Text[area][TextHead[area]];
  }
  if (TextT[area] != 0) {
    drawText(area, TextT[area]);
    TextS[area] = (TextCanvas[area]->getCursorX() > TextW[area]);
  }
  else
    clearText(area);
}
