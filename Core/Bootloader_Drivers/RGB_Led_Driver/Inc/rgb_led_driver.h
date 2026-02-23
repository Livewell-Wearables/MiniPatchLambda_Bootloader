/*
 * rgb_led_driver.h
 *
 *  Created on: Apr 7, 2025
 *      Author: Fatih
 */

#ifndef LW_DRIVERS_RGB_LED_DRIVER_INC_RGB_LED_DRIVER_H_
#define LW_DRIVERS_RGB_LED_DRIVER_INC_RGB_LED_DRIVER_H_

#include "main.h"

typedef enum
{
	RGB_Color_None		= 0,
	RGB_Color_White		= 1,
	RGB_Color_Red		= 2,
	RGB_Color_Sun		= 3,
	RGB_Color_Deneme    = 4,
}RGB_Color_Status_t;

typedef struct
{
	TIM_HandleTypeDef	*TIMx;
	uint32_t 			 TIM_Channel;
}PWMx_t;

typedef struct{
	PWMx_t				ledRed;
	uint8_t 			redValue;
}Led_Red_Info_t;

typedef struct{
	PWMx_t				ledGreen;
	uint8_t 			greenValue;
}Led_Green_Info_t;

typedef struct{
	PWMx_t				ledBlue;
	uint8_t 			blueValue;
}Led_Blue_Info_t;

typedef struct
{
	Led_Red_Info_t		ledRedInfo;
	Led_Green_Info_t	ledGreenInfo;
	Led_Blue_Info_t		ledBlueInfo;

	uint8_t 		  	rgbDimLevel;
	RGB_Color_Status_t  rgbColorSelection;
}Leds_State_t;

typedef struct
{
	PWMx_t				singleMotor;
	uint8_t 		  	motorPWMLevel;
}Motor1_Info_t;

typedef struct
{
	Motor1_Info_t		motor1Info;
}Motor_State_t;

void LEDs_Initialization(Leds_State_t *ledsState);
void LED_Red_Init(TIM_HandleTypeDef *htim, uint32_t timChannel, Led_Red_Info_t *redInfo);
void LED_Green_Init(TIM_HandleTypeDef *htim, uint32_t timChannel, Led_Green_Info_t *greenInfo);
void LED_Blue_Init(TIM_HandleTypeDef *htim, uint32_t timChannel, Led_Blue_Info_t *blueInfo);
void Motor_Intialization(Motor_State_t *motorsState);
void Motor_Init(TIM_HandleTypeDef *htim, uint32_t timChannel, Motor1_Info_t *motorInfo);

void RGB_Control(Leds_State_t *ledState);
void RGB_Set_Color(Leds_State_t *rgbState);
void Motor_Set_Value(Motor_State_t *motorState);

void RGB_HeartBeat(Leds_State_t *ledsState);
void RGB_HeartBeat_Green(Leds_State_t *leds);

#endif /* LW_DRIVERS_RGB_LED_DRIVER_INC_RGB_LED_DRIVER_H_ */
