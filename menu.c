/***********************/
// LCD DISPLAY DRIVER
/***********************/

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "defines.h"

	//Graph
	if ((--countGraph) == 0)
	{
		countGraph = INTERVAL_GRAPH;
		runGraph = true;
	}


/***********************/
//GRAPH_Value
/***********************/
void GRAPH_value (void)
{
	int percent;
	int adc_result;

	
	ADC_init(1); // uses ADC for Light

	adc_result = ADC; // converts ADC value to percent

	percent = adc_result*0.097;
	itoa(percent, graph_string, 8);
}






bool runGraph = false;



char countGraph = 20;






#define INTERVAL_GRAPH 20;







char graph_string[8];