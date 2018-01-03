/*  Radio signal clock - DCF77 Module

    Computerarchitektur 3
    (C) 2017-2018 J. Friedrich, W. Zimmermann Hochschule Esslingen

    Author:   W.Zimmermann, Jun  10, 2016
    Modified: -
*/

/*
; A C H T U N G:  D I E S E  S O F T W A R E  I S T  U N V O L L S T Ä N D I G
; Dieses Modul enthält nur Funktionsrahmen, die von Ihnen ausprogrammiert werden
; sollen.
*/


#include <hidef.h>                                      // Common defines
#include <mc9s12dp256.h>                                // CPU specific defines
#include <stdio.h>

#include "dcf77.h"
#include "led.h"
#include "clock.h"
#include "lcd.h"

// Global variable holding the last DCF77 event
DCF77EVENT dcf77Event = NODCF77EVENT;

// Modul internal global variables
static int  dcf77Year=2017, dcf77Month=1, dcf77Day=1, dcf77Hour=0, dcf77Minute=0;       //dcf77 Date and time as integer values

// Prototypes of functions simulation DCF77 signals, when testing without
// a DCF77 radio signal receiver
void initializePortSim(void);                   // Use instead of initializePort() for testing
char readPortSim(void);                         // Use instead of readPort() for testing

// ****************************************************************************
// Initalize the hardware port on which the DCF77 signal is connected as input
// Parameter:   -
// Returns:     -
void initializePort(void)
{
  asm MOVB #$00, DDRH
}

// ****************************************************************************
// Read the hardware port on which the DCF77 signal is connected as input
// Parameter:   -
// Returns:     0 if signal is Low, >0 if signal is High
char readPort(void)
{
// --- Add your code here ----------------------------------------------------
// --- ??? --- ??? --- ??? --- ??? --- ??? --- ??? --- ??? --- ??? --- ??? ---
    return -1;
}

// ****************************************************************************
//  Initialize DCF77 module
//  Called once before using the module
void initDCF77(void)
{   setClock((char) dcf77Hour, (char) dcf77Minute, 0);
    displayDateDcf77();

    initializePortSim();
}

// ****************************************************************************
// Display the date derived from the DCF77 signal on the LCD display, line 1
// Parameter:   -
// Returns:     -
void displayDateDcf77(void)
{   char datum[32];

    (void) sprintf(datum, "%02d.%02d.%04d", dcf77Day, dcf77Month, dcf77Year);
    writeLine(datum, 1);
}

// ****************************************************************************
//  Read and evaluate DCF77 signal and detect events
//  Must be called by user every 10ms
//  Parameter:  Current CPU time base in milliseconds
//  Returns:    DCF77 event, i.e. second pulse, 0 or 1 data bit or minute marker
DCF77EVENT sampleSignalDCF77(int currentTime)
{   
  DCF77EVENT event = NODCF77EVENT;
  
  static char lastEdge = 0;
  static char presentEdge = 0;
  static int minuteMark = 0;
  static int T_high = 0;
  static int T_low = 0;
  static int T_raisingMark = 0;
  static int T_fallingMark = 0;
  
  lastEdge = presentEdge;
  presentEdge = readPortSim();
  
  //Raising Edge
  if ((lastEdge == 0) && (presentEdge == 1)) 
  {
    T_raisingMark = currentTime;
    T_low = currentTime - T_fallingMark;
    
    if ((T_low >= 70) && ( T_low <= 130)) 
    {
      event = VALIDZERO;  
    }
    else if ((T_low >= 170) && ( T_low <= 230)) 
    {
      event = VALIDONE;  
    } 
    else 
    {
      event = INVALID; 
    }
  }

  //Falling Edge
  if ((lastEdge == 1) && (presentEdge == 0)) 
  {
    T_fallingMark = currentTime;
    T_high = currentTime - T_raisingMark;
    
    if (((T_high + T_low) >= 900) && ((T_high + T_low) <= 1100)) 
    {
      event = VALIDSECOND;  
    }
    else if (((T_high + T_low) >= 1900) && ((T_high + T_low) <= 2100)) 
    {
      event = VALIDMINUTE;  
    }
    else 
    {
      event = INVALID; 
    }      
  }
  
  //Signal did not change
  if (lastEdge == presentEdge)
  {
    event = NODCF77EVENT;  
  }

  return event;
}

// ****************************************************************************
// Process the DCF77 events
// Contains the DCF77 state machine
// Parameter:   Result of sampleSignalDCF77 as parameter
// Returns:     -
void processEventsDCF77(DCF77EVENT event)
{
  static int index = 0;
  static int error = 0;
  static char signal[59] = {0};
  
  //Filling signal array
  if (event == VALIDMINUTE) 
  {
    index = 0;
    error = 0;
  } 
  
  else if (event == INVALID) 
  {
    error = 1;
    index++;
  }
  
  else if (event == VALIDZERO) 
  {
    signal[index] = 0;
    index++;
  }
  
  else if (event == VALIDONE) 
  {
    signal[index] = 1;
    index++;
  } 

  //Read data from complete signal
  if ((index == 58) && (error == 0))
  {
    //Time
    dcf77Minute = 1 * signal[21] + 2 * signal[22] + 4 * signal[23] + 8 * signal[24] + 10 * signal[25] + 20 * signal[26] + 80 * signal[27];  
    dcf77Hour = 1 * signal[29] + 2 * signal[30] + 3 * signal[31] + 4 * signal[32] + 10 * signal[33] + 20 * signal[34];
    
    
    //Date
    dcf77Day = 1 * signal[36] + 2 * signal[37] + 4 * signal[38] + 8 * signal[39] + 10 * signal[40] + 20 * signal[41];
    dcf77Month = 1 * signal[45] + 2 * signal[46] + 4 * signal[47] + 8 * signal[48] + 10 * signal[49];
    dcf77Year = 1 * signal[50] + 2 * signal[51] + 4 * signal[52] + 8 * signal[53] + 10 * signal[54] + 20 * signal[55] + 40 * signal[56] + 80 * signal[57];
    
    setClock((char) dcf77Hour, (char) dcf77Minute, 0);
  }
}

