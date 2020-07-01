#include <stdio.h>
#include <cmsis_os2.h>

int n=0;
void counter(void *arg)
{
	while(1)
	{
		++n;
		osDelay(100);
	}
}

void printer(void *arg)
{
	int last=0;
	while(1)
	{
		if(n != last)
		{
			printf("%s %d\n", (char *) arg, n);
		}
		last =n;
		osThreadYield();
		
	}
}

int main()
{
	osKernelInitialize();
	osThreadNew(counter, NULL,NULL);
	osThreadNew(printer, "Printer says: ", NULL);
	osKernelStart();
}
