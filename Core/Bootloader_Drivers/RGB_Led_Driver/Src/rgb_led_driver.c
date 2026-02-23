/*
 * rgb_led_driver.c
 *
 *  Created on: Apr 7, 2025
 *      Author: Fatih
 */


#include "rgb_led_driver.h"

uint8_t colorHexValue[5][4] =
{
	{RGB_Color_None, 	  0,   0,   0},		// 0 - 255 -> 0 - 100
	{RGB_Color_White, 	255, 255, 255},
	{RGB_Color_Red, 	255,   0,   0},
	{RGB_Color_Sun,     255,  75,   0}
};

/**
 * @brief  Initializes the LED Red control structure with timer and channel parameters.
 *
 * This function assigns the specified timer instance and timer channel to the
 * Led_Red_Info_t structure and sets the initial red intensity value to zero.
 *
 * @param[in]  htim        Pointer to the timer handle used for PWM generation.
 * @param[in]  timChannel  Timer channel associated with the red LED (e.g., TIM_CHANNEL_1).
 * @param[out] redInfo     Pointer to the LED red info structure to initialize.
 *
 * @retval None
 */
void LED_Red_Init(TIM_HandleTypeDef *htim,
                  uint32_t timChannel,
                  Led_Red_Info_t *redInfo)
{
    if ((htim != NULL) && (redInfo != NULL))
    {
        redInfo->ledRed.TIMx       = htim;
        redInfo->ledRed.TIM_Channel = timChannel;
        redInfo->redValue          = 0U;
    }
}

/**
 * @brief  Initializes the LED Green control structure with timer and channel parameters.
 *
 * This function configures the green LED control structure with the specified
 * timer handle and timer channel. It also sets the initial green intensity to zero.
 *
 * @param[in]  htim         Pointer to the timer handle used for PWM generation.
 * @param[in]  timChannel   Timer channel associated with the green LED (e.g., TIM_CHANNEL_2).
 * @param[out] greenInfo    Pointer to the LED green info structure to initialize.
 *
 * @retval None
 */
void LED_Green_Init(TIM_HandleTypeDef *htim,
                    uint32_t timChannel,
                    Led_Green_Info_t *greenInfo)
{
    if ((htim != NULL) && (greenInfo != NULL))
    {
        greenInfo->ledGreen.TIMx        = htim;
        greenInfo->ledGreen.TIM_Channel = timChannel;
        greenInfo->greenValue           = 0U;
    }
}

/**
 * @brief  Initializes the LED Blue control structure with timer and channel parameters.
 *
 * This function sets up the timer instance and channel for the blue LED control
 * and initializes the blue value to zero. It prepares the structure for PWM-based
 * brightness control.
 *
 * @param[in]  htim         Pointer to the timer handle used for PWM control.
 * @param[in]  timChannel   Timer channel used to drive the blue LED.
 * @param[out] blueInfo     Pointer to the LED blue information structure to be initialized.
 *
 * @retval None
 */
void LED_Blue_Init(TIM_HandleTypeDef *htim,
                   uint32_t timChannel,
                   Led_Blue_Info_t *blueInfo)
{
    if ((htim != NULL) && (blueInfo != NULL))
    {
        blueInfo->ledBlue.TIMx        = htim;
        blueInfo->ledBlue.TIM_Channel = timChannel;
        blueInfo->blueValue           = 0U;
    }
}

/**
 * @brief  Starts PWM output for all RGB LED channels.
 *
 * This function enables the PWM outputs for red, green, and blue LEDs
 * using the timer instances and channels stored in the provided LEDs state structure.
 * Each channel must have been previously configured with correct timer settings.
 *
 * @param[in] ledsState  Pointer to the structure containing red, green, and blue LED configurations.
 *
 * @retval None
 */
void LEDs_Initialization(Leds_State_t *ledsState)
{
    if (ledsState != NULL)
    {
        (void)HAL_TIM_PWM_Start(ledsState->ledRedInfo.ledRed.TIMx,
                                ledsState->ledRedInfo.ledRed.TIM_Channel);

        (void)HAL_TIM_PWM_Start(ledsState->ledGreenInfo.ledGreen.TIMx,
                                ledsState->ledGreenInfo.ledGreen.TIM_Channel);

        (void)HAL_TIM_PWM_Start(ledsState->ledBlueInfo.ledBlue.TIMx,
                                ledsState->ledBlueInfo.ledBlue.TIM_Channel);
    }
}

/**
 * @brief  Initializes the motor control configuration.
 *
 * This function assigns the given timer instance and channel to the motor structure,
 * and sets the initial PWM level to zero. It is intended to be called once during
 * the motor peripheral setup phase.
 *
 * @param[in]  htim        Pointer to the timer handle used for motor PWM output.
 * @param[in]  timChannel  Timer channel to which the motor is connected (e.g., TIM_CHANNEL_1).
 * @param[out] motorInfo   Pointer to the structure that holds motor configuration data.
 *
 * @retval None
 */
void Motor_Init(TIM_HandleTypeDef *htim, uint32_t timChannel, Motor1_Info_t *motorInfo)
{
    if (motorInfo != NULL)
    {
        motorInfo->singleMotor.TIMx        = htim;
        motorInfo->singleMotor.TIM_Channel = timChannel;
        motorInfo->motorPWMLevel           = 0U;
    }
}

/**
 * @brief  Starts the PWM signal for the motor.
 *
 * This function initializes the motor by starting the PWM output
 * using the timer and channel information stored in the motor state structure.
 * It should be called after configuring the motor timer instance via Motor_Init().
 *
 * @param[in] motorsState  Pointer to the structure containing motor state and configuration.
 *
 * @retval None
 */
void Motor_Intialization(Motor_State_t *motorsState)
{
    if (motorsState != NULL)
    {
        (void)HAL_TIM_PWM_Start(motorsState->motor1Info.singleMotor.TIMx,
                                motorsState->motor1Info.singleMotor.TIM_Channel);
    }
}

/**
 * @brief  Sets the RGB LED color using PWM duty cycles.
 *
 * This function updates the PWM compare values for red, green, and blue LEDs
 * based on the brightness levels specified in the ledsState structure.
 * Each color intensity is determined by its respective value (0 to max).
 *
 * @param[in] ledsState Pointer to the structure containing LED state and configuration.
 *
 * @retval None
 */
void RGB_Set_Color(Leds_State_t *ledsState)
{
    if (ledsState != NULL)
    {
        __HAL_TIM_SET_COMPARE(ledsState->ledRedInfo.ledRed.TIMx,
                              ledsState->ledRedInfo.ledRed.TIM_Channel,
                              ledsState->ledRedInfo.redValue);

        __HAL_TIM_SET_COMPARE(ledsState->ledGreenInfo.ledGreen.TIMx,
                              ledsState->ledGreenInfo.ledGreen.TIM_Channel,
                              ledsState->ledGreenInfo.greenValue);

        __HAL_TIM_SET_COMPARE(ledsState->ledBlueInfo.ledBlue.TIMx,
                              ledsState->ledBlueInfo.ledBlue.TIM_Channel,
                              ledsState->ledBlueInfo.blueValue);
    }
}

/**
 * @brief  Creates a breathing (heartbeat) effect on the RGB LED.
 *
 * This function modulates the brightness of the RGB LED to simulate
 * a heartbeat effect. The RGB values are scaled based on a dimming
 * level which gradually increases and decreases in a loop.
 *
 * @param[in,out] ledsState Pointer to the LED state structure containing RGB values and dim level.
 *
 * @retval None
 */
void RGB_HeartBeat(Leds_State_t *ledsState)
{
    static uint32_t dimStep = 1;
    static uint32_t factor  = 10U;

    if (ledsState != NULL)
    {
        ledsState->rgbDimLevel += dimStep;

        if (ledsState->rgbDimLevel >= factor)
        {
            ledsState->rgbDimLevel = factor;
            dimStep = -1;
        }
        else if (ledsState->rgbDimLevel <= 0)
        {
            ledsState->rgbDimLevel = 0;
            dimStep = 1;
        }
        else
        {
            /* Do nothing */
        }

        ledsState->ledRedInfo.redValue =
            (uint16_t)(((uint32_t)ledsState->ledRedInfo.redValue * (uint32_t)ledsState->rgbDimLevel) / factor);
        ledsState->ledGreenInfo.greenValue =
            (uint16_t)(((uint32_t)ledsState->ledGreenInfo.greenValue * (uint32_t)ledsState->rgbDimLevel) / factor);
        ledsState->ledBlueInfo.blueValue =
            (uint16_t)(((uint32_t)ledsState->ledBlueInfo.blueValue * (uint32_t)ledsState->rgbDimLevel) / factor);

        RGB_Set_Color(ledsState);
    }
}

/* ==== Görsel eşikler ==== */
#define BRIGHT_MAX        (255U)
#define BRIGHT_X          (48U)    /* görünürlük eşiği */
#define BRIGHT_Y          (180U)   /* pratik doygunluk (tepe) */

/* ==== Zaman dağılımı (50 ms tick) ==== */
/* Yükseliş: 0→X yavaş, X→Y yavaş (tepe kısa/0) */
#define RISE_SLOW_0X      (10U)
#define RISE_SLOW_XY      (18U)
/* tepeye çıkınca bekleme sayısı; 0 = hiç bekleme */
#define HOLD_TOP_STEPS    (0U)

/* Düşüş: tamamı yavaş (smooth) ve uzun */
#define FALL_SLOW_YX      (36U)
#define FALL_SLOW_X0      (22U)

/* Türetilenler */
#define RISE_STEPS   (RISE_SLOW_0X + RISE_SLOW_XY)                 /* 28 */
#define FALL_STEPS   (FALL_SLOW_YX + FALL_SLOW_X0)                 /* 58 */
#define HEART_STEPS  (RISE_STEPS + FALL_STEPS)                     /* 86 ≈ 4.3 s */

#define U16_ONE (65535U)

/* S-eğrisi: f(u)=u*u*(3-2u)  (başta ve sonda yavaş, ortada orta hızlı) */
static uint16_t ease_scurve(uint16_t u_q16)
{
    const uint32_t u  = u_q16;
    const uint32_t u2 = (u * u) / U16_ONE;
    const uint32_t k  = (3U * U16_ONE) - (2U * u);
    const uint32_t f  = (u2 * k) / U16_ONE;  /* Q16 */
    return (uint16_t)f;
}

/* 0..255 arası interpolasyon (Q16 faktör) */
static uint16_t lerp_u16(uint16_t a, uint16_t b, uint16_t f_q16)
{
    const int32_t d  = (int32_t)b - (int32_t)a;
    const int32_t dv = (int32_t)(((int64_t)d * (int64_t)f_q16) / (int64_t)U16_ONE);
    int32_t r = (int32_t)a + dv;
    if (r < 0) { r = 0; }
    if (r > (int32_t)BRIGHT_MAX) { r = BRIGHT_MAX; }
    return (uint16_t)r;
}

static uint16_t seg_eval_slow(uint16_t bStart, uint16_t bEnd, uint8_t k, uint8_t n)
{
    if (n <= 1U) { return bEnd; }
    const uint16_t u = (uint16_t)(((uint32_t)k * U16_ONE) / (uint32_t)(n - 1U));
    const uint16_t f = ease_scurve(u);
    return lerp_u16(bStart, bEnd, f);
}

void RGB_HeartBeat_Green(Leds_State_t *ledsState)
{
    static uint16_t idx = 0U;
    static uint8_t  initialized = 0U;
    static uint16_t baseR = 0U, baseG = 0U, baseB = 0U;
    static RGB_Color_Status_t lastSel = (RGB_Color_Status_t)255;
    static uint8_t  holdCnt = 0U;

    if (ledsState == NULL) { return; }

    if ((initialized == 0U) || (ledsState->rgbColorSelection != lastSel))
    {
        baseR   = (uint16_t)ledsState->ledRedInfo.redValue;
        baseG   = (uint16_t)ledsState->ledGreenInfo.greenValue;
        baseB   = (uint16_t)ledsState->ledBlueInfo.blueValue;
        lastSel = ledsState->rgbColorSelection;
        idx     = 0U;
        holdCnt = 0U;
        initialized = 1U;
    }

    const uint16_t b0 = 0U, b1 = BRIGHT_X, b2 = BRIGHT_Y;
    uint16_t bright; /* 0..255 */
    uint8_t  pause = 0U;

    if (idx < RISE_STEPS)
    {
        /* ---- YÜKSELİŞ: tamamı S-eğrisi (0→X, X→Y) ---- */
        uint16_t k = idx;
        if (k < RISE_SLOW_0X)
        {
            bright = seg_eval_slow(b0, b1, (uint8_t)k, (uint8_t)RISE_SLOW_0X);
        }
        else
        {
            k -= RISE_SLOW_0X;
            bright = seg_eval_slow(b1, b2, (uint8_t)k, (uint8_t)RISE_SLOW_XY);
        }

        /* Tepe bekleme: HOLD_TOP_STEPS kadar Y seviyesinde kal (varsayılan 0) */
        if (idx == (RISE_STEPS - 1U))
        {
            if (holdCnt < HOLD_TOP_STEPS)
            {
                bright  = b2;
                holdCnt++;
                pause   = 1U;      /* idx ilerlemesin */
            }
            else
            {
                holdCnt = 0U;      /* düşüşe geçilecek */
            }
        }
    }
    else
    {
        /* ---- DÜŞÜŞ: tamamı S-eğrisi, yavaş ---- */
        uint16_t j = (uint16_t)(idx - RISE_STEPS);
        if (j < FALL_SLOW_YX)
        {
            bright = seg_eval_slow(b2, b1, (uint8_t)j, (uint8_t)FALL_SLOW_YX);
        }
        else
        {
            j -= FALL_SLOW_YX;
            bright = seg_eval_slow(b1, b0, (uint8_t)j, (uint8_t)FALL_SLOW_X0);
        }
    }

    ledsState->rgbDimLevel = (uint8_t)bright;
    ledsState->ledRedInfo.redValue     = (uint16_t)((baseR * (uint32_t)bright) / BRIGHT_MAX);
    ledsState->ledGreenInfo.greenValue = (uint16_t)((baseG * (uint32_t)bright) / BRIGHT_MAX);
    ledsState->ledBlueInfo.blueValue   = (uint16_t)((baseB * (uint32_t)bright) / BRIGHT_MAX);

    RGB_Set_Color(ledsState);

    if (pause == 0U)
    {
        idx++;
        if (idx >= HEART_STEPS) { idx = 0U; }
    }
}



/**
 * @brief  Sets the PWM output value for the motor driver.
 *
 * This function configures the duty cycle of the motor's PWM signal using the
 * stored PWM level in the Motor_State_t structure. It writes the PWM value
 * to the appropriate timer and channel using HAL macro.
 *
 * @param[in] motorState Pointer to the motor state structure containing motor configuration and PWM level.
 *
 * @retval None
 */
void Motor_Set_Value(Motor_State_t *motorState)
{
    if (motorState != NULL)
    {
        __HAL_TIM_SET_COMPARE(
            motorState->motor1Info.singleMotor.TIMx,
            motorState->motor1Info.singleMotor.TIM_Channel,
            motorState->motor1Info.motorPWMLevel
        );
    }
    else
    {
        /* Nothing to do if motorState is NULL */
    }
}

