
//========== BBMATRIX54 HARDWARE LIBARY ==========

//Date: June 30, 2019


#ifndef LIBHARDWARE_H
#define LIBHARDWARE_H

#include <Arduino.h>


//---------- DISPLAY CLASS ----------

// Purpose: To abstract the BBMatrix54 LED matrix as a display, 
// with methods/functions to Clear the display, draw a dot,
// draw horizontal or vertical lines and draw text.


class Display
{
public:
  void Setup(byte _numCols, byte _numRows);
  void Clear();

  static byte MakeBit(byte y);
  byte SetPixel(byte x, byte y, byte c = 1);
  void SetByte(byte x, byte bits);
  void vLine(byte x, byte y1 = 255, byte y2 = 255);
  void hLine(byte y, byte x1 = 255, byte x2 = 255);
  void hShift(byte dir = 0, bool wrap = false);
  void vShift(byte dir = 0, bool wrap = false);

  byte numCols;
  byte numRows;

  void SetSlowdown(byte factor);

private:
  void SetupTimedRefresh();
};


//---------- BUTTONS CLASS ----------

// Purpose: To simplify the use of the pushbuttons on the 
// BBMatrix54 board. The BtnPressed() method/function only 
// returns true one time after a buttons is pressed, and it 
// is not reset until the button is released.


#define BTN1 0
#define BTN2 1

class Buttons
{
public:
    void Setup();
    bool BtnPressed(byte i);
    bool BtnDown(byte i);
private:
  bool btnDown[2];
  bool btnWasNotPressed[2];
  void ReadButtons();
};


//---------- TEXTDISPLAY CLASS ----------

// Purpose: To draw alphanumeric characters and punctuation on the
// BBMatrix54 display.


class TextDisplay
{
public:
  TextDisplay(Display& _display);
  
  void DisplayText(String s, int pos = 0);
  void DisplayTextCentered(String s);
  void ScrollText(String s, int counter);
  int GetWidth(String s);
  int getScrollWidth(String s);

private:
  Display& display;
  void GetCharPixelData(char c, byte *np, byte const **p);
};


#endif //LIBHARDWARE_H
