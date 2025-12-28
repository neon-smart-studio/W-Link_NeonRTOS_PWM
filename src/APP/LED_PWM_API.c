
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "NeonRTOS.h"

#include "LED_PWM_API.h"

LED_PWM_Status All_LED_Status[MAX_NUM_OF_LED_PWM_SW];

const hwPWM_Channel PWM_Channels[MAX_NUM_OF_LED_PWM_SW] = { hwPWM_Channel_1, hwPWM_Channel_2, hwPWM_Channel_3, hwPWM_Channel_4};

void LED_PWM_Init()
{
	for(uint8_t i = 0; i<MAX_NUM_OF_LED_PWM_SW; i++)
	{
		PWM_Channel_Init(PWM_Channels[i], false);
		memset(&All_LED_Status[i], 0, sizeof(LED_PWM_Status));
	}
}

//---------------------------------------------------------------------------------------------------------

int LED_PWM_Get_Num_Of_LED(uint8_t* Num_Of_LED)
{
	if (Num_Of_LED == NULL)
	{
		return -1;
	}
	*Num_Of_LED = MAX_NUM_OF_LED_PWM_SW;
	return 0;
}

void LED_PWM_Set_LED_On_Off_Status(uint8_t led_index, bool on_off)
{
	if (led_index >= MAX_NUM_OF_LED_PWM_SW){return;}

	if (on_off == true) {
		All_LED_Status[led_index].on_off = true;
		PWM_Turn_On(PWM_Channels[led_index]);
	}
	else {
		All_LED_Status[led_index].on_off = false;
		PWM_Turn_Off(PWM_Channels[led_index]);
	}
}

void LED_PWM_Set_LED_Level_Status(uint8_t led_index, uint16_t level)
{
	if (led_index >= MAX_NUM_OF_LED_PWM_SW){return;}
	
	if(All_LED_Status[led_index].on_off)
	{
		All_LED_Status[led_index].level = level;
		PWM_Set_Duty(PWM_Channels[led_index], level);
	}
}

void LED_PWM_Toggle_LED_On_Off_Status(uint8_t led_index)
{
	if (led_index >= MAX_NUM_OF_LED_PWM_SW){return;}

	LED_PWM_Set_LED_On_Off_Status(led_index, !All_LED_Status[led_index].on_off);
}

void LED_PWM_Set_Selected_LED_On_Off_Status(uint16_t led_bit_map, bool on_off)
{
	for (uint8_t i = 0; i < MAX_NUM_OF_LED_PWM_SW; i++) {
		if (led_bit_map&(1 << i)) {
			if (on_off == true) {
				All_LED_Status[i].on_off = true;
				PWM_Turn_On(PWM_Channels[i]);
			}
			else {
				All_LED_Status[i].on_off = false;
				PWM_Turn_Off(PWM_Channels[i]);
			}
		}
	}
}

void LED_PWM_Set_Selected_LED_Level_Status(uint16_t led_bit_map, uint16_t level)
{
	for (uint8_t i = 0; i < MAX_NUM_OF_LED_PWM_SW; i++) {
		if (led_bit_map&(1 << i)) {
			if(All_LED_Status[i].on_off){
				All_LED_Status[i].level = level;
				PWM_Set_Duty(PWM_Channels[i], level);
			}
		}
	}
}

void LED_PWM_Toggle_Selected_LED_On_Off_Status(uint16_t toggle_bit_map)
{
	for (uint8_t i = 0; i < MAX_NUM_OF_LED_PWM_SW; i++) {
		if (toggle_bit_map&(1 << i)) {
			if (All_LED_Status[i].on_off == false) {
				All_LED_Status[i].on_off = true;
				PWM_Turn_On(PWM_Channels[i]);
			}
			else {
				All_LED_Status[i].on_off = false;
				PWM_Turn_Off(PWM_Channels[i]);
			}
		}
	}
}
