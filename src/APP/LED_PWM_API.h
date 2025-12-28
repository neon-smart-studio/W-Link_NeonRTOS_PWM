
#ifndef LED_PWM_API_H
#define LED_PWM_API_H

#include "GPIO/GPIO.h"
#include "PWM/PWM.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_NUM_OF_LED_PWM_SW      4

typedef struct
{
	uint8_t index;
	bool on_off;
	uint16_t level;
}LED_PWM_Status;

extern LED_PWM_Status All_LED_Status[];

extern void LED_PWM_Init(void);
int LED_PWM_Get_Num_Of_LED(uint8_t* Num_Of_LED);
int LED_PWM_Get_Specific_LED_PWM_Status(uint8_t index, LED_PWM_Status* LED_Status);
int LED_PWM_Get_All_LED_PWM_Status(LED_PWM_Status* LED_Status []);
void LED_PWM_Set_LED_On_Off_Status(uint8_t led_index, bool on_off);
void LED_PWM_Set_LED_Level_Status(uint8_t led_index, uint16_t level);
void LED_PWM_Toggle_LED_On_Off_Status(uint8_t led_index);
void LED_PWM_Set_Selected_LED_On_Off_Status(uint16_t on_off_bit_map, bool on_off);
void LED_PWM_Set_Selected_LED_Level_Status(uint16_t on_off_bit_map, uint16_t level);
void LED_PWM_Toggle_Selected_LED_On_Off_Status(uint16_t toggle_bit_map);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //LED_PWM_API_H