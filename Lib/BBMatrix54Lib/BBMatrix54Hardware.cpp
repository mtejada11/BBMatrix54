
//========== BBMATRIX54 HARDWARE LIBARY ==========

//Date: June 30, 2019

#include "BBMatrix54.h"


//---------- DISPLAY CLASS ----------

// Purpose: To abstract the BBMatrix54 LED matrix as a display, 
// with methods/functions to clear the display, draw a dot,
// draw horizontal or vertical lines and draw text.


#define COLUMNS 5
byte displayCols[COLUMNS] = { 2, 3, 4, 5, 6 };
byte displayRows[4] = { 9, 8, A4, A3 };

byte displayBuffer[COLUMNS];
int  displayCounter = 0;
byte displaySlowdownFactor = 1;
byte displayScanRow = 0;



void Display::Setup(byte _numCols, byte _numRows)
{
  numCols = _numCols;
  numRows = _numRows;
  if(numCols > COLUMNS) numCols = COLUMNS;
  if(numRows > 4)  numRows = 4;

  int i,j;
  for(i = 0; i < COLUMNS; i++) pinMode(displayCols[i], OUTPUT);
  for(j = 0; j < 4; j++)  pinMode(displayRows[j], OUTPUT);
  Clear();

  SetupTimedRefresh();
}


void Display::Clear()
{
  byte *p = displayBuffer;
  for(byte i = 0; i < numCols; i++)
    (*(p++)) = 0;
}


byte Display::MakeBit(byte y)
{
  return (1 << (3 - y));
}


byte Display::SetPixel(byte x, byte y, byte c)
{
  if ((x<0) || (x>=numCols)) return 0;
  
  byte mask = ~(8 >> y);
  byte bits = (c << (3 - y));
  noInterrupts();
  displayBuffer[x] &= mask;
  displayBuffer[x] |= bits;
  interrupts();

  return bits;
}


void Display::SetByte(byte x, byte bits)
{
  if ((x<0) || (x>=numCols)) return;
  
  noInterrupts();
  displayBuffer[x] |= bits;
  interrupts();
}


void Display::vLine(byte x, byte y1, byte y2)
{
  if (y1 == 255) y1 = 0;
  if (y2 == 255) y2 = numRows;
  for(byte i = y1; i <= y2; i++)
    SetPixel(x, i, 1);
}


void Display::hLine(byte y, byte x1, byte x2)
{
  if (x1 == 255) x1 = 0;
  if (x2 == 255) x2 = numCols;
  for(byte i = x1; i <= x2; i++)
    SetPixel(i, y, 1);
}


void Display::hShift(byte dir, bool wrap)
{
  signed char incr = (dir == 0) ? +1 : -1;
  byte *p1 = (dir == 0) ? (displayBuffer+1) : (displayBuffer+numCols-2);
  byte *p2 = p1 - incr;
  byte *p0 = p2;
  byte v0 = (wrap == false) ? 0 : (*p2);

  for(byte i = 0; i < (numCols-1); i++)
  {
    (*p2) = (*p1);
    p1 += incr;
    p2 += incr;
  }
  (*p2) = v0;
}


void Display::vShift(byte dir, bool wrap)
{
  byte shiftedOutBits;
  
  for(byte i = 0; i < numCols; i++)
  {
    if (dir == 0) 
    {
      shiftedOutBits = (displayBuffer[i] & 0x8) >> 3;
      displayBuffer[i] <<= 1;
}
    else
    {
      shiftedOutBits = (displayBuffer[i] & 0x1) << 3;
      displayBuffer[i] >>= 1;
    }

    if (wrap == true)
      displayBuffer[i] |= shiftedOutBits;  
  } 
}

void Display::SetSlowdown(byte factor)
{
  if (factor < 1) factor = 1;
  displaySlowdownFactor = factor;
}


void Display::SetupTimedRefresh()
{
  cli();//stop interrupts

  //set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0

  // set compare match register for 8khz increments
  OCR2A = 60;// = n / (8000*8) - 1 (must be <256)

  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS20); //100 - 64 scaler, 1 ms
  TCCR2B |= (1 << CS21); //110 - 256 scaler, 4 ms
  TCCR2B |= (1 << CS22); //111 - 1024 scaler, 16 ms

  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  sei();//allow interrupts
}


SIGNAL(TIMER2_COMPA_vect) 
{
  byte j = displayScanRow;
  byte on_value;

  displayCounter++;
  if ((displayCounter % displaySlowdownFactor) != 0) return;

  digitalWrite(displayRows[0], 1);
  digitalWrite(displayRows[1], 1);
  digitalWrite(displayRows[2], 1);
  digitalWrite(displayRows[3], 1);
  for(byte i = 0; i < COLUMNS; i++)
    digitalWrite(displayCols[i], 0);

  digitalWrite(displayRows[j], 0);
  byte *p = displayBuffer;
  byte mask = 8 >> j; 
  for(byte i = 0; i < COLUMNS; i++)
    digitalWrite(displayCols[i], ((*(p++)) & mask) ? 1 : 0);

  noInterrupts();
  displayScanRow++;
  displayScanRow %= 4;
  interrupts();
}


//---------- BUTTONS CLASS ----------

// Purpose: To simplify the use of the pushbuttons on the 
// BBMatrix54 board. The BtnPressed() method/function only 
// returns true one time after a buttons is pressed, and it 
// is not reset until the button is released.


static const byte buttonPins[2] = { A0, A5 };


void Buttons::Setup()
{
  pinMode(buttonPins[0], INPUT);
  pinMode(buttonPins[1], INPUT_PULLUP);

  for (byte i = 0; i < 2; i++)
    btnWasNotPressed[i] = true;
}


void Buttons::ReadButtons()
{
  btnDown[0] = (digitalRead(buttonPins[0]) == 1);
  btnDown[1] = (digitalRead(buttonPins[1]) == 0);
}


bool Buttons::BtnPressed(byte i)
{
  bool BtnPressedNow = BtnDown(i);

  if (btnWasNotPressed[i] && BtnPressedNow)
  {
    btnWasNotPressed[i] = false;
    return true;
  }
  else
  {
    if (!BtnPressedNow)
      btnWasNotPressed[i] = true;
    return false;
  }
}


bool Buttons::BtnDown(byte i)
{
  ReadButtons();
  return btnDown[i];
}


//---------- TEXTDISPLAY CLASS ----------

// Purpose: To draw alphanumeric characters and punctuation on the
// BBMatrix54 display.


TextDisplay::TextDisplay(Display& _display):
  display(_display)
{
}


void TextDisplay::DisplayText(String s, int pos)
{
  byte n = s.length();
  byte np = GetWidth(s);
  byte const *p;

  for(byte i=0; i< n; i++)
  {
    GetCharPixelData(s[i],&np,&p);
    for(byte j=0; j<np; j++)
    {
      display.SetByte(pos+j, pgm_read_byte_near(p+j));
    }
    pos += np;
    if (i < (n-1)) pos += 1;
  }
}


void TextDisplay::DisplayTextCentered(String s)
{
  byte np = GetWidth(s);
  int pos = (display.numCols - np) / 2;
  if (pos < 0) pos = 0;
  DisplayText(s, pos);
}


int TextDisplay::GetWidth(String s)
{
  byte n = s.length();
  byte np;
  byte const *p;

  int pos = 0;
  for(int i = 0; i < n; i++)
  {
    GetCharPixelData(s[i],&np,&p);
    pos += np;
    if (i < (n-1)) pos += 1;
  }

  return pos;
}


void TextDisplay::ScrollText(String s, int counter)
{
  int pos = display.numCols - (counter % getScrollWidth(s));
  display.Clear();
  DisplayText(s, pos);
}


int TextDisplay::getScrollWidth(String s)
{
  int width = display.numCols + GetWidth(s) + display.numCols;
  return width;
}


const byte PROGMEM font3x4[] = 
{
//        !    "    #      $    %    &    '    (    )    *    +    ,    -    .    /
//  ...  #..  ##.  .#.#   .#.  #.#  ##.  #..  .#.  #..  ...  ...  ...  ...  ...  ...#
//  ...  #..  ##.  #####  .##  .#.  ##.  #..  #..  .#.  #.#  .#.  ...  ...  ...  ..#.
//  ...  ...  ...  #####  ##.  #..  ###  ...  #..  .#.  .#.  ###  .#.  ###  ...  .#..
//  ...  #..  ...  .#.#.  .#.  #.#  ###  ...  .#.  #..  #.#  .#.  #..  ...  #..  #...

3, 0x0, 0x0, 0x0, 0x0, 0x0, // (space)
1, 0xD, 0x0, 0x0, 0x0, 0x0, // !
2, 0xC, 0xC, 0x0, 0x0, 0x0, // "
5, 0x6, 0xF, 0x6, 0xF, 0x6, // #
3, 0x2, 0xF, 0x4, 0x0, 0x0, // $
3, 0xB, 0x4, 0x9, 0x0, 0x0, // %
3, 0xF, 0xF, 0x3, 0x0, 0x0, // &
1, 0xC, 0x0, 0x0, 0x0, 0x0, // '
2, 0x6, 0x9, 0x0, 0x0, 0x0, // (
2, 0x9, 0x6, 0x0, 0x0, 0x0, // )
3, 0x5, 0x2, 0x5, 0x0, 0x0, // *
3, 0x2, 0x7, 0x2, 0x0, 0x0, // +
2, 0x1, 0x2, 0x0, 0x0, 0x0, // ,
3, 0x2, 0x2, 0x2, 0x0, 0x0, // -
1, 0x1, 0x0, 0x0, 0x0, 0x0, // .
4, 0x1, 0x2, 0x4, 0x8, 0x0, // /

//  .X.  .X.  XX.  XX.  X.X  XXX  .XX  XXX  XXX  XXX
//  X.X  XX.  .XX  .XX  X.X  XX.  XX.  ..X  XXX  X.X
//  X.X  .X.  X..  ..X  XXX  ..X  X.X  .X.  X.X  XXX
//  .X.  .X.  XXX  XX.  ..X  XX.  XXX  .X.  XXX  ..X

3, 0x6, 0x9, 0x6, 0x0, 0x0,
2, 0x4, 0xF, 0x0, 0x0, 0x0,
3, 0xB, 0xD, 0x5, 0x0, 0x0,
3, 0x9, 0xD, 0x6, 0x0, 0x0,
3, 0xE, 0x2, 0xF, 0x0, 0x0,
3, 0xD, 0xD, 0xA, 0x0, 0x0,
3, 0x7, 0xD, 0xB, 0x0, 0x0,
3, 0x8, 0xB, 0xC, 0x0, 0x0,
3, 0xF, 0xD, 0xF, 0x0, 0x0,
3, 0xE, 0xA, 0xF, 0x0, 0x0,

//  ...  .#.  ...  ...  ...  ###  .##.
//  #..  ...  .#.  ###  #..  ..#  #.##
//  ...  .#.  #..  ...  .#.  ...  #...
//  #..  #..  .#.  ###  #..  .#.  .##.

1, 0x5, 0x0, 0x0, 0x0, 0x0, // :
2, 0x1, 0xA, 0x0, 0x0, 0x0, // ;
2, 0x2, 0x5, 0x0, 0x0, 0x0, // <
3, 0x5, 0x5, 0x5, 0x0, 0x0, // =
2, 0x5, 0x2, 0x0, 0x0, 0x0, // >
3, 0x8, 0x9, 0xC, 0x0, 0x0, // ?
4, 0x6, 0x9, 0xD, 0x4, 0x0, // @

//  .X.  XX.  .XX  XX.  XXX  XXX  .XX  X.X  XXX  . X  X.X  X..  X...X
//  X.X  XX.  X..  X.X  XX.  XX.  X..  XXX  .X.  ..X  XX.  X..  XX.XX
//  XXX  X.X  X..  X.X  X..  X..  X.X  X.X  .X.  ..X  X.X  X..  X.X.X
//  X.X  XXX  .XX  XX.  XXX  X..  .XX  X.X  XXX  XX.  X.X  XXX  X...X

3, 0x7, 0xA, 0x7, 0x0, 0x0,
3, 0xF, 0xD, 0x3, 0x0, 0x0,
3, 0x6, 0x9, 0x9, 0x0, 0x0,
3, 0xF, 0x9, 0x6, 0x0, 0x0,
3, 0xF, 0xD, 0x9, 0x0, 0x0,
3, 0xF, 0xC, 0x8, 0x0, 0x0,
3, 0x6, 0x9, 0xB, 0x0, 0x0,
3, 0xF, 0x4, 0xF, 0x0, 0x0,
1, 0xF, 0x0, 0x0, 0x0, 0x0,
3, 0x1, 0x1, 0xE, 0x0, 0x0,
3, 0xF, 0x4, 0xB, 0x0, 0x0,
3, 0xF, 0x1, 0x1, 0x0, 0x0,
3, 0xF, 0xC, 0xF, 0x0, 0x0,
 
//  XX.  XXX  XX.  XXX.  XX.  .XX  XXX  X.X  X.X  X...X  X.X  X.X  XXX
//  X.X  X.X  X.X  X.X.  X.X  XX.  .X.  X.X  X.X  X.X.X  .X.  X.X  ..X
//  X.X  X.X  XX.  X.X.  XX.  ..X  .X.  X.X  X.X  X.X.X  .X.  .X.  XX.
//  X.X  XXX  X..  XX.X  X.X  XX.  .X.  XXX  .X.  .X X.  X.X  .X.  XXX

3, 0xF, 0x8, 0x7, 0x0, 0x0,
3, 0xF, 0x9, 0xF, 0x0, 0x0,
3, 0xF, 0xA, 0x4, 0x0, 0x0,
4, 0xF, 0x9, 0xE, 0x1, 0x0,
3, 0xF, 0xA, 0x5, 0x0, 0x0,
3, 0x5, 0xD, 0xA, 0x0, 0x0,
3, 0x8, 0xF, 0x8, 0x0, 0x0,
3, 0xF, 0x1, 0xF, 0x0, 0x0,
3, 0xE, 0x1, 0xE, 0x0, 0x0,
3, 0xF, 0x2, 0xF, 0x0, 0x0,
3, 0x9, 0x6, 0x9, 0x0, 0x0,
3, 0xC, 0x3, 0xC, 0x0, 0x0,
3, 0xB, 0xD, 0x9, 0x0, 0x0,
 
//  ##.  #..  ##.  .#.  ...
//  #..  #..  .#.  #.#  ...
//  #..  #..  .#.  ...  ...
//  ##.  #..  ##.  ...  ###

2, 0xF, 0x9, 0x0, 0x0, 0x0, // [
1, 0xF, 0x0, 0x0, 0x0, 0x0, // |
2, 0x9, 0xF, 0x0, 0x0, 0x0, // ]
3, 0x4, 0x8, 0x4, 0x0, 0x0, // ^
3, 0x1, 0x1, 0x1, 0x0, 0x0, // _
};


void TextDisplay::GetCharPixelData(char c, byte *np, byte const **p)
{
  int index = 0;
  if((c >= ' ') && (c <= '_'))
    index = c - ' ';
  else if((c >= 'a') && (c <= 'z'))
    index = c - 'a' + 33;
  else
    index = c - ' ';
  index *= 6;
  (*np) = pgm_read_byte_near(font3x4+index);
  (*p) = font3x4 + index + 1;
}
