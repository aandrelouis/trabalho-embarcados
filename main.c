/** ***************************************************************************
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.0
 ******************************************************************************/

#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include <stdint.h>
#include <stdio.h>

#include "em_device.h"
#include "gpio.h"
#include "adc.h"
#include "clock_efm32gg2.h"
#include "led.h"
#include "lcd.h"
#include "pwm.h"

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

//{
#define SYSTICKDIVIDER 1000
#define SOFTDIVIDER 1000

static uint32_t ticks = 0;

void SysTick_Handler(void)
{
    static int counter = 0; // must be static

    ticks++;

    if (counter != 0)
    {
        counter--;
    }
    else
    {
        counter = SOFTDIVIDER - 1;
    }
}

//}

/*****************************************************************************
 * @brief  Delay function based on SysTick
 *****************************************************************************/

void Delay(uint32_t v)
{
    uint32_t lim = ticks + v; // Missing processing of overflow here

    while (ticks < lim)
    {
    }
}

/*****************************************************************************
 * @brief  Main function
 *
 * @note   Using default clock configuration
 *         HFCLK = HFRCO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

#include "uart2.h"

#define DELAYVAL 2

int main(void)
{
    uint32_t v;
    char s[10];
    char r[10];
    int t;

    // init inputs
    GPIO_Init(GPIOD, BIT(0), 0);
    GPIO_Init(GPIOD, BIT(1), 0);

    // init dos leds
    GPIO_Init(GPIOD, 0, BIT(7));
    GPIO_Init(GPIOC, 0, BIT(0));
    GPIO_Init(GPIOA,0, BIT(12));

    // Set clock source to external crystal: 48 MHz
    (void)SystemCoreClockSet(CLOCK_HFXO, 1, 1);
    SysTick_Config(SystemCoreClock / SYSTICKDIVIDER);

    /* Turn on LEDs */
    LED_Write(LED1, 0);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock / SYSTICKDIVIDER);

    /* Configure LCD */
    LCD_Init();

    LCD_SetAll();
    Delay(DELAYVAL);

    LCD_ClearAll();
    Delay(DELAYVAL);

    // Configure ADC
    ADC_Init(500000);
    ADC_ConfigChannel(ADC_CH0, 0);
    ADC_ConfigChannel(ADC_CH1, 0); // ADC_SINGLECTRL_REF_
    // ADC_ConfigChannel(ADC_CH2, 0);

    PWM_Init(TIMER0, PWM_LOC4, PWM_PARAMS_ENABLECHANNEL1);
    PWM_Init(TIMER1, PWM_LOC4, PWM_PARAMS_ENABLECHANNEL1);
    PWM_Init(TIMER2, PWM_LOC1, PWM_PARAMS_ENABLECHANNEL0);

    // Enable IRQs
    __enable_irq();
    int max = 0xFFFF;
    int raz = max / 4096;

    while (1)
    {
        v = ADC_Read(ADC_CH0);
       int valueReadDir = ADC_Read(ADC_CH1);
        int val = v * raz;
        int val2 = valueReadDir * raz;

        PWM_Write(TIMER1, 1, val);
        PWM_Write(TIMER0, 2, val);
        PWM_Write(TIMER2, 0, val2/4); //servo

        sprintf(s, "%5d", v);

        LCD_WriteString(s);
        // Delay(100);

    }
}
