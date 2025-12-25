#include "led.h"


u32 led_tick;
u8 state;

void led_proc(void)
{
	if(state==0)
	{
		LED1_ON;
		
		if(uwTick - led_tick >= 500)
		{
			state=1;
			led_tick = uwTick;
		}
	}
	else if(state==1)
	{
		LED1_OFF;
		if(uwTick - led_tick >= 500)
		{
			state=0;
			led_tick = uwTick;
		}
	}
}



