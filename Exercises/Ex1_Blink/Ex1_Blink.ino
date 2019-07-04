//========== BBMatrix54 Exercise 1 ==========

// Purpose: Blink one LED
// Date: July 4, 2019

#include "BBMatrix54.h"
Display display;


//---------- Setup routine to run once ----------

void setup() 
{
  //Initialize LED matrix display
  display.Setup(5, 4);
}


//---------- Loop routine that runs repeatedly ----------

void loop() 
{
  display.SetPixel(0, 0, 1);
  delay(500);

  display.SetPixel(0, 0, 0);
  delay(500);
}
