/*
 * cal_amp.c
 *
 *  Created on: Sep 25, 2015
 *      Author: akozmins
 */
#include <stdio.h>

double calc_amp(double waveform[], int length)
{
   double amp = 0.0;
   double max = waveform[0];
   double min = waveform[0];
   int i;

   // Iterate through array elements
   // calculate maximum and minimum value
   for (i=1 ; i<length ; i++)
   {
	    if (waveform[i] > max)
	       max  = waveform[i];

	    if (waveform[i] < min)
	    	min = waveform[i];
   }
   amp = max - min;
   return amp;
}

