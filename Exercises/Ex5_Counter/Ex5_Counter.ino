//========== BBMatrix54 Exercise 5 ==========

// Purpose: Numeric counter on 5x4 LED matrix
// Date: July 4, 2019

#include "BBMatrix54.h"
Display display;
TextDisplay textDisplay(display);


//---------- Setup routine to run once ----------

void setup() 
{
  //Initialize LED matrix display
  display.Setup(5,4);
}


//--- Global variable ---
int counter = 0;


//---------- Loop routine that runs repeatedly ----------

void loop() 
{
  //Create text string from counter value
  String text(counter);

  //Draw text string to display
  display.Clear();
  textDisplay.DisplayTextCentered(text);
  delay(350);

  //Increment counter
  counter++;
  if (counter > 9) counter = 0;
}

