
//========== SEED Electronics/Coding Workshop Exercise ==========

//Purpose: Test 5x4 LED matrix
//Date: June 22, 2019


#include "BBMatrix54.h"
Display display;
int d = 100; //ms



//---------- Setup routine to run once ----------

void setup() 
{
  //Initialize LED matrix display
  display.Setup(5,4);
}


//---------- Loop routine that runs repeatedly ----------

void loop() 
{
  //Flash sequence of horizontal lines
  for(int y = 0; y < 4; y++)
  {
    display.Clear();
    display.hLine(y);
    delay(d);
  }

  //Flash sequence of vertical lines
  for(int x = 0; x < 5; x++)
  {
    display.Clear();
    display.vLine(x);
    delay(d);
  }
}

