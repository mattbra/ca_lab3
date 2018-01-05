/*  Header for Button module

    Computerarchitektur 3
    Hochschule Esslingen

    Author:   M.Brandel, J.Ruess, 04.01.2018
    Modified: -
*/

#include <hidef.h>                                      // Common defines
#include <mc9s12dp256.h>                                // CPU specific defines
#include <stdio.h>

extern int USmode;

int readButtonPH2()
{
    char input;
    input = !PTH_PTH2;
    if (input >0)
      return 1;
    else
      return 0;  
}

void processButtons() 
{
  static int counter = 0;
  if (readButtonPH2() == 1) 
  {
    counter++;
    if (counter >= 100) 
    {
      counter = 0;
      if (USmode == 0)
        USmode = 1;
      else if (USmode == 1)
        USmode = 0;  
    }
    
  }  
}