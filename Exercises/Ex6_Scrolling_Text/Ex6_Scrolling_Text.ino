//========== BBMatrix54 Exercise 6 ==========

// Purpose: Scrolling text on 5x4 LED matrix
// Date: July 4, 2019

#include "BBMatrix54.h"
Display display;
TextDisplay textDisplay(display);


//---------- Setup routine to run once ----------

void setup() 
{
  //Initialize LED matrix display
  display.Setup(5, 4);
}


//--- Global variables ---
int counter = 0;
String message = "HELLO 12345";


//---------- Loop routine that runs repeatedly ----------

void loop() 
{
  //Draw text using scrolling text function and counter for position
  textDisplay.ScrollText(message, counter);
  counter++;
  delay(150);
}
