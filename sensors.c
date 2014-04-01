/***********************/
// Sensors driver
/***********************/

#include <avr/io.h>
#include <stdlib.h>

/***********************/
// AC TO DC(ADC) Init
/***********************/
int ADC_init (int channel)
{ switch (channel)
     { case 0:
       ADMUX &= ~(0b00000001);    // Initialize ADMUX, to select input and reference voltage
       break;
       case 1:
	   ADMUX |= 0b00000001;
	   break;
	  }

  
     ADCSRA = 0b10010111;    // Initialize the control and status register
     ADCSRA |=0b01000000;    // Start the conversion   
        
     while ((ADCSRA & 0x10) == 0);

    return (ADC);
}

/***********************/
//TEMP_value
/***********************/
// Edited ->  Return value as int instead of string
int TEMP_value (void)
{
	int adc_result;
	int DEG_value = 84;
	int dummy;
	int DEG;
	int ADC_steps = 4;

	ADC_init(0);		//temp = 0
	adc_result = ADC;          
	dummy = adc_result - DEG_value;
	DEG = dummy / ADC_steps;

	// itoa(DEG, temp_string, 10);
	return DEG;
}

/***********************/
//LIGHT_value
/***********************/
int LIGHT_value (void)
{
	int adc_result;

	ADC_init(1);		//light = 1
	adc_result = ADC;
	
	// itoa(adc_result, light_string, 10);	
	return adc_result;
}
