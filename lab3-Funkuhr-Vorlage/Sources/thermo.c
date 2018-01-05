/*  Header for Thermometer module

    Computerarchitektur 3
    Hochschule Esslingen

    Author:   M.Brandel, J.Ruess, 04.01.2018
    Modified: -
*/

#include "ad.h"

extern int ad_value;
static int temp_value;
static char sign = ' ';
static int upperLimit = 70;
static int lowerLimit = -30;
static int maxResolution = 1023;
static int minResolution = 0;

void updateThermo(int ad_val)
{
  ad_value = ad_val;
  temp_value = (int)(ad_value/10) + lowerLimit;//= (long)((ad_value * (upperLimit-lowerLimit)) / (maxResolution-minResolution) + lowerLimit); 
  //temp_value = (int)temp_val;
  if (temp_value < 0){   
    temp_value *= -1 ;
    sign = '-';
  } else {
   sign = '+'; 
  }  
}

char getTempChar() 
{
  return sign;
}

int getTempValue() 
{
  return (char)temp_value;
}