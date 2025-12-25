#include "key.h"

keys key[2];


void key_proc(void)
{
	if(key[0].single_flag)
	{
		key[0].single_flag=0;
		//view++; view%=6;
		cnt=0;
		LED2_Toggle;
		OLED_Clear();
	}
	if(key[0].long_flag)
	{
		key[0].long_flag=0;

	}
	if(key[0].double_flag)
	{
		key[0].double_flag=0;

	}
}


void key_task(void)
{
	key[0].key_sta = HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2);
	key[1].key_sta = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_11);
	for(int i = 0; i < 2; i++)
	{
		switch(key[i].key_judge)
		{
			case 0:
			{
				if(key[i].key_sta==0)
				{
					key[i].key_judge=1;
					key[i].key_time=0;
				}
				break;
			}
			case 1:
			{
				if(key[i].key_sta==0)
				{
					key[i].key_judge=2;
					key[i].key_time++;
				}
				else
				{
					key[i].key_judge=0;
				}
				break;
			}
			case 2:
			{
				key[i].key_time++;
				if(key[i].key_sta==1)
				{
					key[i].key_judge=0;
					if(key[i].double_pre==0)
					{
						key[i].double_pre=1;
					}
					else
					{
						key[i].double_pre=0;
						key[i].double_flag=1;
					}
				}
				break;
			}
		}
		if(key[i].double_pre && key[i].key_sta)
		{
			key[i].double_time++;
		}
		if(key[i].double_time>1)
		{
			key[i].double_time=0;
			key[i].double_pre=0;
			if(key[i].key_time<70)
			{
				key[i].single_flag=1;
			}
			else
			{
				key[i].long_flag=1;
			}
		}
	}
}


void key_task2(void)
{
	key[0].key_sta = HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2);
	key[1].key_sta = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_11);
	for(int i = 0; i < 2; i++)
	{
		switch(key[i].key_judge)
		{
			case 0:
			{
				if(key[i].key_sta==0)
				{
					key[i].key_judge=1;
					key[i].key_time=0;
				}
				break;
			}
			case 1:
			{
				if(key[i].key_sta==0)
				{
					key[i].key_judge=2;
					key[i].key_time++;
				}
				break;
			}
			case 2:
			{
				key[i].key_time++;
				if(key[i].key_sta==1)
				{
					key[i].key_judge=0;
					if(key[i].key_time<70)
					{
						key[i].single_flag=1;
					}
					else
					{
						key[i].long_flag=1;
					}
				}
				break;
			}
		}
	}
}


//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	if(htim->Instance==TIM16)
//	{
//		key_task();
//	}
//}



