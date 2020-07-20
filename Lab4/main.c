/****************************************************************************
 * This code is copyright (c) 2017 by the University of Waterloo and may    *
 * not be redistributed without explicit written permission.                *
 ****************************************************************************/
/*
MTE 241 LAB 4
July 20, 2020

Harsimran Darhan
*Partner info removed for privacy reasons*
*/

#include <stdio.h>
#include <cmsis_os2.h>
#include "random.h"
#include "lpc17xx.h"
#include <stdbool.h>
#include <string.h>

//queues
osMessageQueueId_t q1;
osMessageQueueId_t q2;

//data being queued
int num;

//used to optimizing printf
int seconds = 0, previous_seconds = 0; 


//number of messages sent for each queue
int sent_q1=0;
int sent_q2=0;

//number of messages received for each queue
int received_q1=0;
int received_q2=0;

//number of messages overflowed for each queue
int overflow_q1=0;
int overflow_q2=0;


//client function used for sending messages to each queue(alternating) with an avg of Hz per queue
void client(void *arg)
{
	//variable for alternating client sends
	bool switching=true;

	while (true)
	{

		uint32_t z= ((osKernelGetTickFreq()*next_event())/(9*2))>>16;      //divide by 9*2 for average of 9Hz per queue
		osDelay(z);
	
		//alternation
		if(switching)
		{
			if(osMessageQueuePut(q1,&num,0,0) != osOK)
				overflow_q1++;
			else
				sent_q1++;
			switching=false;
		}
		else
		{
			if(osMessageQueuePut(q2,&num,0,0) != osOK)
				overflow_q2++;
			else
				sent_q2++;
			switching=true;		
		}
	}
}


//server function used for receiving messages from a queue at an average of 10Hz per queue
void server(void *arg)
{
	bool queue1=true; //boolean used for keeping track of which queue is being used
	while(true)
	{
		uint32_t z= ((osKernelGetTickFreq()*next_event())/10)>>16; //divide by 10 for average of 10Hz
		osDelay(z);
		
		
		//determine which queue is being used
		if(q1==(osMessageQueueId_t *) arg)
			queue1=true;
		else 
			queue1=false;

		if(osMessageQueueGet((osMessageQueueId_t *) arg,&num,NULL,osWaitForever)==osOK)
		{
			//increment variable depending on which queue is being used
			if(queue1)
				received_q1++;
			else
				received_q2++;
		}
	}
}
	
//monitor function for updating the time
void monitor_update_time(void *arg)
{
	while(true)
	{
		seconds++;
		osDelay(osKernelGetTickFreq()); //delay by 1 second (simulated)
	}
}

//monitor function for printing variables
void monitor_print(void *arg)
{

	while(true)
	{
		if(seconds>previous_seconds) //will run every 1 simulated second
		{
			previous_seconds=seconds;
			
			printf("Time: %d s q1 sent: %d q1 received: %d q1 overflow: %d q2 sent: %d q2 received: %d q2 overflow: %d\n",osKernelGetTickCount()/osKernelGetTickFreq(),sent_q1,received_q1,overflow_q1,sent_q2,received_q2,overflow_q2);

		}
		osThreadYield();
	}
}


int main()
{
	
	osKernelInitialize(); //initialize kernel
	
	q1 = osMessageQueueNew(10, sizeof(int), NULL); //create queue 1
	q2 = osMessageQueueNew(10, sizeof(int), NULL); // create queue 2
	

	osThreadNew(client, NULL,NULL); //create threads
	osThreadNew(server, q1,NULL);
	osThreadNew(server, q2, NULL);
	osThreadNew(monitor_print,NULL,NULL);
	osThreadNew(monitor_update_time,NULL,NULL);


	osKernelStart(); //start kernel
	while(1)
	{}

}
