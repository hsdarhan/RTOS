/****************************************************************************
 * This code is copyright (c) 2017 by the University of Waterloo and may    *
 * not be redistributed without explicit written permission.                *
 ****************************************************************************/
/*
MTE 241 LAB 3
July 6, 2020

Harsimran Darhan
*Partner info removed for privacy reasons*
*/

#include <stdio.h>
#include <cmsis_os2.h>
#include "lpc17xx.h"
#include <stdbool.h>

void joystick(void *arg)
{
	//initialize
	LPC_GPIO2->FIODIR |= 0x0000007C;  
	LPC_GPIO1->FIODIR |= 0xB0000000;
	
	//turn off
	LPC_GPIO1->FIOCLR |= 0xB0000000;
	LPC_GPIO2->FIOCLR |= 0x0000007C;
	
	
	while(1)
	{
		LPC_GPIO1->FIOCLR |= (1 << 28);
		LPC_GPIO1->FIOCLR |= (1 << 29);
		LPC_GPIO1->FIOCLR |= 0x80000000;
		
		
		
		int joystickNORTH=(LPC_GPIO1->FIOPIN &(1<<23)); //read pin23
		int joystickSOUTH=(LPC_GPIO1->FIOPIN &(1<<25)); //read pin 25
		int joystickWEST=(LPC_GPIO1->FIOPIN &(1<<26)); //read pin 26
		int joystickEAST=(LPC_GPIO1->FIOPIN &(1<<24)); //read pin 24
		int joystickPRESSED=(LPC_GPIO1->FIOPIN &(1<<20)); //read pin 20
	
		if (joystickNORTH==0) //if this pin is on
			LPC_GPIO1->FIOSET|=(1<<28);
	
		else if (joystickSOUTH==0)
		{
			LPC_GPIO1->FIOSET|=(1<<28);
			LPC_GPIO1->FIOSET |=(1<<29);
		}
	
		else if (joystickWEST==0)
			LPC_GPIO1->FIOSET |=0x80000000;
	
		else if (joystickEAST==0)
			LPC_GPIO1->FIOSET |=(1<<29);
		else
			LPC_GPIO1->FIOCLR |= 0xB0000000;
	
		if (joystickPRESSED!=0)
			LPC_GPIO2->FIOCLR|=(1<<3);
		else
			LPC_GPIO2->FIOSET |= (1<<3);
	
		osThreadYield();
	}
	

}



void pushButton(void *arg)
{
	
	//initialize
	LPC_GPIO2->FIODIR |= 0x0000007C;
	LPC_GPIO1->FIODIR |= 0xB0000000;
	//turn off
	LPC_GPIO1->FIOCLR |= 0xB0000000;
	LPC_GPIO2->FIOCLR |= 0x0000007C;

	bool ledON= false; //status of led
	bool prevPressed= false; //previously pressed

	
		while(1) 
		{
			int buttonPressed = (LPC_GPIO2->FIOPIN &(1<<10)); 

			if(buttonPressed!=0) //if released
			{
				if((prevPressed)&&(ledON)) 
				{
					LPC_GPIO2->FIOCLR |=(1<<6);
					ledON = false;
					prevPressed=false;
				}
				if((prevPressed)&&(!ledON)) 
				{
					LPC_GPIO2->FIOSET|=(1<<6);
					ledON = true;
					prevPressed=false;
				}					
			}
		
			else //if pressed
					prevPressed=true;
			
			osThreadYield();
		}
	
}

void readADC(void *arg)
{
	
	LPC_PINCON->PINSEL1 &= ~(0x03<<18); //clear bits 18 and 19
	LPC_PINCON->PINSEL1 |= (0x01<<18); //set bits 18 and 19 to 01 = AD0.2
	
	LPC_SC->PCONP |= (1<<12); //power on adc
	
	LPC_ADC->ADCR =(1<<2); // analog input
	LPC_ADC->ADCR |=(1<<21); // enable adc circuitry
	LPC_ADC->ADCR |=(1<<11); //clock rate (5Mhz)
	
	while(1)
	{
		LPC_ADC->ADCR |= (1<<24); //start conversion
		
		while((LPC_ADC->ADGDR & 0x80000000)==0);// wait for pin 31 (indicates conversion is complete)
		uint16_t ADC_VALUE= ((LPC_ADC->ADGDR)>>4);	 //get pins 4-15
		printf("%.1f\n", (ADC_VALUE/(4095/3.3))); //divide by (resolution of ADC / System voltage)
		osThreadYield();
	}
}


int main()
{
	osKernelInitialize(); 
	
	osThreadNew(readADC, NULL,NULL);
	osThreadNew(pushButton, NULL,NULL);
	osThreadNew(joystick, NULL, NULL);

	osKernelStart();
	while(1)
	{}
}
