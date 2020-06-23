/****************************************************************************
 * This code is copyright (c) 2017 by the University of Waterloo and may    *
 * not be redistributed without explicit written permission.                *
 ****************************************************************************/
/*
MTE 241 LAB 2
June 14, 2020

Harsimran Darhan
*Partner info removed for privacy reasons*
*/

#define PROGRAM 1

#include "lpc17xx.h"
#include "stdio.h"
#include "uart.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"


#if PROGRAM ==1
int main()
{
	//initialize
	LPC_GPIO2->FIODIR |= 0x0000007C;
	LPC_GPIO1->FIODIR |= 0xB0000000;
	//turn off
	LPC_GPIO1->FIOCLR |= 0xB0000000;
	LPC_GPIO2->FIOCLR |= 0x0000007C;
	while(TRUE)
	{
		
		int buttonPressed=(LPC_GPIO2->FIOPIN &= (1<<10));
	
		
		if (buttonPressed!=0)
		{
			LPC_GPIO2->FIOCLR |= (1<<6); //turn off
		}	
		else
		{
			LPC_GPIO2->FIOSET |= (1<<6); //0x40, turn on
			while((LPC_GPIO2->FIOPIN & (1<<10))==0); //0x400, wait to turn on again to avoid delay
		}
	}
}
#endif

#if PROGRAM ==2
int main()
{
	while(TRUE)
	{
	int joystickNORTH=(LPC_GPIO1->FIOPIN &= (1<<23)); //read pin23
	int joystickSOUTH=(LPC_GPIO1->FIOPIN &= (1<<25)); //read pin 25
	int joystickWEST=(LPC_GPIO1->FIOPIN &= (1<<26)); //read pin 26
	int joystickEAST=(LPC_GPIO1->FIOPIN &= (1<<24)); //read pin 24
	int joystickPRESSED=(LPC_GPIO1->FIOPIN &= (1<<20)); //read pin 20
		
	
	if (joystickNORTH==0) //if this pin is on
		printf("North, ");
	else if (joystickSOUTH==0)
		printf("South, ");
	else if (joystickWEST==0)
		printf("West, ");
	else if (joystickEAST==0)
		printf("East, ");
	else
		printf("No Direction,  ");
	
	if (joystickPRESSED==0)
		printf("Pressed\n");
	else 
		printf("Not Pressed\n");
	}
}
#endif

#if PROGRAM==3
int main()
{
	//initialize
	LPC_GPIO2->FIODIR |= 0x0000007C;  
	LPC_GPIO1->FIODIR |= 0xB0000000;
	
	//turn off
	LPC_GPIO1->FIOCLR |= 0xB0000000;
	LPC_GPIO2->FIOCLR |= 0x0000007C;
	while(TRUE)
	{
		int num = 0;
		while(TRUE){
			printf("Please enter a number between 0-255: ");
			char testing [20];
			scanf ("%s", testing);
			// error trapping
			for (int i=0;i<strlen(testing); i++) //string check with negative sign 
			{
        if (!isdigit(testing[i])){
					if (i == 0 && testing[0] == '-') continue;
					printf("ERROR: Invalid input - string inputs not supported\n");
          break;
        }
			}
			num=atoi(testing);
			if (num <= 255 && num >= 0)	break; //bounds check
			printf("ERROR: Invalid input - out of specified bounds\n");
		}
	
	//turn off
	LPC_GPIO1->FIOCLR |= 0xB0000000;
	LPC_GPIO2->FIOCLR |= 0x0000007C;
	
	if((num&1)!=0)
		LPC_GPIO1->FIOSET|=(1<<28); //0x10000000

	if((num&2)!=0)
		LPC_GPIO1->FIOSET |=(1<<29);//0x20000000

	if((num&4)!=0)
		LPC_GPIO1->FIOSET |=0x80000000; //(1<<31)

	if((num&8)!=0)
		LPC_GPIO2->FIOSET |=(1<<2); //0x4

	if((num&16)!=0)
		LPC_GPIO2->FIOSET |=(1<<3); //0x8
	
	if((num&32)!=0)
		LPC_GPIO2->FIOSET |=(1<<4);//0x10
	
	if((num&64)!=0)
		LPC_GPIO2->FIOSET |=(1<<5);//0x20
	
	if((num&128)!=0)
		LPC_GPIO2->FIOSET |=(1<<6);//0x40
	
	}
}
#endif

#if PROGRAM==4
int main()
{
	
	LPC_PINCON->PINSEL1 &= ~(0x03<<18); //clear bits 18 and 19
	LPC_PINCON->PINSEL1 |= (0x01<<18); //set bits 18 and 19 to 01 = AD0.2
	
	LPC_SC->PCONP |= (1<<12); //power on adc
	
	LPC_ADC->ADCR =(1<<2); // analog input
	LPC_ADC->ADCR |=(1<<21); // enable adc circuitry
	LPC_ADC->ADCR |=(1<<11); //clock rate (5Mhz)
	
	while(TRUE)
	{
		LPC_ADC->ADCR |= (1<<24); //start conversion
		
		while((LPC_ADC->ADGDR & 0x80000000)==0);// wait for pin 31 (indicates conversion is complete)
		uint16_t ADC_VALUE= ((LPC_ADC->ADGDR)>>4);	 //get pins 4-15
		printf("%.1f\n", (ADC_VALUE/(4095/3.3))); //divide by (resolution of ADC / System voltage)
		
	}
}
#endif

















