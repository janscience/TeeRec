#include <Arduino.h>
#include "Display.h"

// 16 bit colors:
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define ORANGE   0xFC00
#define WHITE    0xFFFF

const uint16_t Display::PlotLines[8] = {GREEN, YELLOW, BLUE, RED, CYAN,
					ORANGE, MAGENTA, WHITE};


Display::Display() {
  NPlots = 0;
  for (int k=0; k<MaxAreas; k++) {
    PlotX[k] = 0;
    PlotY[k] = 0;
    PlotW[k] = 0;
    PlotH[k] = 0;
    PlotYOffs[k] = 0;
    PlotYScale[k] = 0;
    TextX[k] = 0;
    TextY[k] = 0;
    TextW[k] = 0;
    TextH[k] = 0;
    TextB[k] = 0;
    TextC[k] = 0;
    TextI[k] = 0;
    TextS[k] = false;
    TextHead[k] = 0;
    TextCanvas[k] = 0;
  }
  memset(Text, 0, sizeof(Text));
  Font = 0;
  Screen = 0;
}


void Display::init(Adafruit_SPITFT *screen, uint8_t rotation) {
  Screen = screen;
  Screen->setRotation(rotation);
  Width = Screen->width();
  Height = Screen->height();
  clear();
}


void Display::clear() {
  Screen->fillScreen(Background);
}


void Display::setPlotArea(uint8_t area, float x0, float y0, float x1, float y1) {
  PlotX[area] = uint16_t(x0*Width);
  PlotY[area] = uint16_t((1.0-y1)*Height);
  PlotW[area] = uint16_t((x1-x0)*Width);
  PlotH[area] = uint16_t((y1-y0)*Height);
  PlotYOffs[area] = PlotY[area] + 0.5*PlotH[area];
  PlotYScale[area] = 0.5*PlotH[area];
  NPlots = area + 1;
}


void Display::setPlotAreas(int num, float x0, float y0, float x1, float y1) {
  float h = (y1-y0)/num;
  for (int k=0; k<num; k++)
    setPlotArea(num-k-1, x0, y0+k*h, x1, y0+(k+0.9)*h);
  NPlots = num;
}

  
int Display::numPlots() const {
  return NPlots;
}


void Display::clearPlot(uint8_t area) {
  if (PlotW[area] == 0 )
    return;
  Screen->fillRect(PlotX[area], PlotY[area], PlotW[area], PlotH[area]+1, PlotBackground);
  Screen->drawFastHLine(PlotX[area], dataY(area, 0), PlotW[area], PlotGrid);
}


void Display::clearPlots() {
  for (int k=0; k<MaxAreas; k++)
    clearPlot(k);
}


void Display::plot(uint8_t area, float *buffer, int nbuffer, int color) {
  if (PlotW[area] == 0 )
    return;
  color %= 8;
  uint16_t x0 = dataX(area, 0, nbuffer);
  uint16_t y0 = dataY(area, buffer[0]);
  for (uint16_t k=1; k<nbuffer; k++) {
    uint16_t x = dataX(area, k, nbuffer);
    uint16_t y = dataY(area, buffer[k]);
    Screen->drawLine(x0, y0, x, y, PlotLines[color]);
    x0 = x;
    y0 = y;
  }
}


uint16_t Display::dataX(uint8_t area, float x, float maxx) {
  return PlotX[area] + uint16_t(x/maxx*PlotW[area]);
}


uint16_t Display::dataY(uint8_t area, float y) {
  return uint16_t(PlotYOffs[area] - PlotYScale[area]*y);
}


float Display::setTextArea(uint8_t area, float x0, float y0, float x1, float y1, bool top) {
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
  if ( Font != 0 )
    TextCanvas[area]->setFont(Font);
  TextCanvas[area]->setTextColor(0xffff);
  TextCanvas[area]->setTextSize(1);
  TextCanvas[area]->setTextWrap(false);
  int16_t  x, y;
  uint16_t w, h;
  TextCanvas[area]->getTextBounds("Agy", 0, 4*TextH[area]/5, &x, &y, &w, &h);
  if (top)
    TextB[area] = 4*TextH[area]/5 - y + 1;
  else {
    TextB[area] = TextH[area] - (y + h - 4*TextH[area]/5) - 1;
    if (TextH[area] > h)
      TextB[area] -= (TextH[area] - h - 1)/2;
  }
  TextC[area] = TextW[area]*3/w;
  TextI[area] = 0;
  TextHead[area] = 0;
  Text[area][0][0] = '\0';
  return float(h)/float(Height);
}
  

void Display::clearText(uint8_t area) {
  if (TextW[area] == 0 )
    return;
  Text[area][TextHead[area]][0] = '\0';
  TextCanvas[area]->fillScreen(0x0000);
  Screen->drawBitmap(TextX[area], TextY[area], TextCanvas[area]->getBuffer(),
		     TextW[area], TextH[area], TextColor, TextBackground);
  TextS[area] = 0;
  TextI[area] = 0;
}


void Display::clearText() {
  for (int k=0; k<MaxAreas; k++)
    clearText(k);
}


void Display::setDefaultFont(const GFXfont &font) {
  Font = &font;
}


void Display::setFont(uint8_t area, const GFXfont &font) {
  if ( TextCanvas[area] != 0 )
    TextCanvas[area]->setFont(&font);
}


void Display::drawText(uint8_t area, const char *text) {
  if (TextW[area] == 0 )
    return;
  TextCanvas[area]->fillScreen(0x0000);
  TextCanvas[area]->setCursor(0, TextB[area]);
  TextCanvas[area]->print(text);
  Screen->drawBitmap(TextX[area], TextY[area], TextCanvas[area]->getBuffer(),
		     TextW[area], TextH[area], TextColor, TextBackground);
}


void Display::writeText(uint8_t area, const char *text) {
  if (TextW[area] == 0 )
    return;
  drawText(area, text);
  strncpy(Text[area][TextHead[area]], text, MaxChars);
  TextS[area] = (TextCanvas[area]->getCursorX() > TextW[area]);
  TextI[area] = 0;
}


void Display::scrollText(uint8_t area) {
  if (Text[area][TextHead[area]][0] == '\0')
    return;
  if (! TextS[area])
    return;
  drawText(area, &Text[area][TextHead[area]][TextI[area]]);
  TextI[area]++;
  if (TextI[area] >= strlen(Text[area][TextHead[area]]) - 1)
    TextI[area] = 0;
}


void Display::pushText(uint8_t area, const char *text) {
  if (TextW[area] == 0 )
    return;
  if (TextHead[area] < MaxTexts)
    TextHead[area]++;
  writeText(area, text);
}


void Display::popText(uint8_t area) {
  if (TextW[area] == 0 )
    return;
  if (TextHead[area] > 0)
    TextHead[area]--;
  if (Text[area][TextHead[area]][0] != '\0') {
    drawText(area, Text[area][TextHead[area]]);
    TextS[area] = (TextCanvas[area]->getCursorX() > TextW[area]);
    TextI[area] = 0;
  }
  else
    clearText(area);
}
