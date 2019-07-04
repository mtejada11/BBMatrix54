//========== BBMatrix54 Exercise 2 ==========

// Purpose: Blink LEDs in sequence
// Date: July 4, 2019

#include "BBMatrix54.h"
Display display;


//---------- Setup routine to run once ----------

void setup()  
{ 
  //Initialize LED matrix display
  display.Setup(5, 4);
}


//--- Global variable ---
int counter = 1;


//---------- Loop routine that runs repeatedly ----------

void loop()  
{
  //Turn on one LED according to counter
  if (counter==1) display.SetPixel(0, 1);
  if (counter==2) display.SetPixel(1, 1);
  if (counter==3) display.SetPixel(2, 1);
  if (counter==4) display.SetPixel(3, 1);
  if (counter==5) display.SetPixel(4, 1);
  delay(200);

  //Increment counter variable
  counter = counter + 1;
  if(counter > 5) counter = 1;

  //Turn off all LEDs for an instant (no delay after)
  display.Clear();
}
