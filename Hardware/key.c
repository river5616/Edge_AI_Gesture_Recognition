#include "key.h"
#include "disp.h"

keys key[2];


void key_proc(void)
{
	if(key[0].single_flag)
	{
		key[0].single_flag=0;

		// 切换工作模式：IDLE -> COLLECTING -> INFERENCE -> IDLE
		WorkMode_t current_mode = Task_GetMode();
		if(current_mode == MODE_IDLE)
		{
			Task_SetMode(MODE_COLLECTING);
			g_gesture_id = 0;
			g_target_images = 100;  // 默认采集100张
			g_current_images = 0;
		}
		else if(current_mode == MODE_COLLECTING)
		{
			Task_SetMode(MODE_INFERENCE);
		}
		else
		{
			Task_SetMode(MODE_IDLE);
		}

		LED2_Toggle;
		OLED_Clear();
	}
	if(key[0].long_flag)
	{
		key[0].long_flag=0;
		// 长按：重置计数器（保留原功能）
		cnt1=0; cnt2=0;
	}
	if(key[0].double_flag)
	{
		key[0].double_flag=0;

	}

	// KEY1：切换显示视图
	if(key[1].single_flag)
	{
		key[1].single_flag=0;

		// 循环切换视图：STATUS -> IMAGE -> UART_STATS -> SENSORS -> ALL_CONF -> STATUS
		view++;
		if(view > VIEW_ALL_CONF)
		{
			view = VIEW_STATUS;
		}

		LED1_Toggle;
		OLED_Clear();
	}
	if(key[1].long_flag)
	{
		key[1].long_flag=0;
		// KEY1长按：直接跳转到图像视图
		view = VIEW_IMAGE;
		OLED_Clear();
	}
	if(key[1].double_flag)
	{
		key[1].double_flag=0;
		// KEY1双击：跳转到UART统计视图
		view = VIEW_UART_STATS;
		OLED_Clear();
	}
}


void key_task(void)
{
	key[0].key_sta = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);  // 修正为 KEY0_Pin (PA0)
	key[1].key_sta = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_11); // KEY1_Pin (PD11) 正确
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
	key[0].key_sta = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);  // 修正为 KEY0_Pin (PA0)
	key[1].key_sta = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_11); // KEY1_Pin (PD11) 正确
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



