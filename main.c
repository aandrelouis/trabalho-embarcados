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
// #include "Ultrasonic.h"

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

//{
#define SYSTICKDIVIDER 1000
#define SOFTDIVIDER 1000
#define PRETO 1500
#define BRANCO 3000

#define SENSOR_ESQUERDA 3000
#define SENSOR_DIREITA 3000

#define DISTANCIA_PAREDE 15

#define SERVO_MINIMO 0
#define SERVO_MAXIMO 1023
#define SERVO_CENTRO (SERVO_MINIMO + SERVO_MAXIMO) / 2

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

int ehCruzamento(int sensorEsquerda, int sensorDireita)
{
    if (sensorEsquerda < SENSOR_ESQUERDA && sensorDireita < SENSOR_DIREITA)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ehDesvio(int sensorEsquerda, int sensorDireita)
{
    if (sensorEsquerda > SENSOR_ESQUERDA && sensorDireita < SENSOR_DIREITA)
    {
        return 1;
    }
    else if (sensorEsquerda < SENSOR_ESQUERDA && sensorDireita > SENSOR_DIREITA)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// int escolheCaminho()
// {
//     // olha para frente e ve se tem parede
//     // se nao tiver parede, vai para frente
//     if (Ranging(1) > DISTANCIA_PAREDE)
//     {
//         return 1;
//     }

//     // se tiver parede, olha para direita
//     // se nao tiver parede, vai para direita
//     if (Ranging(1) > DISTANCIA_PAREDE)
//     {
//         return 2;
//     }

//     // se tiver parede, olha para a esquerda
//     // se nao tiver parede, vai pra esquerda
//     if (Ranging(1) > DISTANCIA_PAREDE)
//     {
//         return 3;
//     }
// }

int main(void)
{
    uint32_t v;
    char s[10];
    char r[10];
    int t;

    // init inputs
    GPIO_Init(GPIOD, BIT(0), 0);
    GPIO_Init(GPIOD, BIT(1), 0);
    GPIO_Init(GPIOD, BIT(2), 0);

    // init dos leds
    GPIO_Init(GPIOD, 0, BIT(7));
    GPIO_Init(GPIOC, 0, BIT(0));

    // init do servo
    GPIO_Init(GPIOA, 0, BIT(12));

    // Set clock source to external crystal: 48 MHz
    (void)SystemCoreClockSet(CLOCK_HFXO, 1, 1);
    SysTick_Config(SystemCoreClock / SYSTICKDIVIDER);

    /* Turn on LEDs */
    // LED_Write(LED1, 0);

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

    PWM_Init(TIMER3, PWM_LOC1, PWM_PARAMS_ENABLECHANNEL2); // potenciometro
    PWM_Init(TIMER0, PWM_LOC4, PWM_PARAMS_ENABLECHANNEL1); // pc0
    PWM_Init(TIMER1, PWM_LOC4, PWM_PARAMS_ENABLECHANNEL1); // pd7
    PWM_Init(TIMER2, PWM_LOC1, PWM_PARAMS_ENABLECHANNEL0); // servo

    // Enable IRQs
    __enable_irq();
    int max = 0xFFFF;
    int raz = max / 4096;

    int servo = 0;
    // PWM_Write(TIMER2, 0, -SERVO_MAXIMO);
    // Delay(500);
    // PWM_Write(TIMER2, 0, SERVO_CENTRO);
    int i = 0;
    while (1)
    {
        // v = ADC_Read(ADC_CH0);
        int valueReadDir = ADC_Read(ADC_CH0);
        int valueReadEsq = ADC_Read(ADC_CH1);
        // int val = v * raz;

        if (ehCruzamento(valueReadEsq, valueReadDir))
        {
            servo++;
            if (servo == 1)
            {
                PWM_Write(TIMER1, 1, 0); // apaga o LED 1
                PWM_Write(TIMER0, 1, 0); // apaga o LED 2

                // move o servo para a direita, aguarda um tempo e move para o centro
                PWM_Write(TIMER2, 0, 256);
                Delay(1000);
                PWM_Write(TIMER2, 0, -256);

                // liga o LED 1 por um tempo
                PWM_Write(TIMER1, 1, 30000);
                Delay(1000);

                // liga o LED 2
                PWM_Write(TIMER0, 1, 30000);
            }

            else if (servo == 2)
            {
                PWM_Write(TIMER1, 1, 0); // apaga o LED 1
                PWM_Write(TIMER0, 1, 0); // apaga o LED 2

                // move o servo para a direita, aguarda um tempo
                PWM_Write(TIMER2, 0, 256);
                Delay(1000);
                // move o servo para a esquerda, aguarda um tempo e move para o centro
                PWM_Write(TIMER2, 0, -512);

                Delay(1000);
                PWM_Write(TIMER2, 0, 256);

                // liga o LED 2 por um tempo
                PWM_Write(TIMER0, 1, 30000);
                Delay(1000);

                // liga o LED 1
                PWM_Write(TIMER1, 1, 30000);
            }

            else if (servo == 3) // vai pra frente
            {

                PWM_Write(TIMER1, 1, 0); // apaga o LED 1
                PWM_Write(TIMER0, 1, 0); // apaga o LED 2

                Delay(1000);

                // liga o LED 2 por um tempo
                PWM_Write(TIMER1, 1, 30000);

                // liga o LED 1
                PWM_Write(TIMER0, 1, 30000);
            }
            else if (servo == 4) // volta
            {
                Delay(1000);
                // liga o LED 2 por um tempo
                PWM_Write(TIMER1, 1, 0);

                // liga o LED 1
                PWM_Write(TIMER0, 1, 0);
                servo = 0;
                Delay(1000);
                // liga o LED 2 por um tempo
                PWM_Write(TIMER1, 1, 30000);

                // liga o LED 1
                PWM_Write(TIMER0, 1, 30000);
                Delay(1000);
                // liga o LED 2 por um tempo
                PWM_Write(TIMER1, 1, 0);

                // liga o LED 1
                PWM_Write(TIMER0, 1, 0);
                servo = 0;
                Delay(1000);
                // liga o LED 2 por um tempo
                PWM_Write(TIMER1, 1, 30000);

                // liga o LED 1
                PWM_Write(TIMER0, 1, 30000);

                servo = 0;

                Delay(1000);
                // liga o LED 2 por um tempo
                PWM_Write(TIMER1, 1, 30000);

                // liga o LED 1
                PWM_Write(TIMER0, 1, 30000);

                servo = 0;
            }
        }
        else if (ehDesvio(valueReadEsq, valueReadDir))
        {
            if (valueReadEsq < SENSOR_ESQUERDA)
            {
                PWM_Write(TIMER1, 1, 100);
                PWM_Write(TIMER0, 1, 30000);
            }
            else if (valueReadDir < SENSOR_DIREITA)
            {
                PWM_Write(TIMER1, 1, 30000);
                PWM_Write(TIMER0, 1, 100);
            }
            Delay(1000);
        }
        PWM_Write(TIMER0, 1, 30000);
        PWM_Write(TIMER1, 1, 30000);

        // PWM_Write(TIMER3, 2, val);
        // // PWM_Write(TIMER1, 1, val);
        // // PWM_Write(TIMER0, 1, val);
        sprintf(s, "%5d", valueReadDir);
        // sprintf(r, "%5d", valueReadEsq);
        LCD_WriteString(s);
        //  Delay(500);
        //  LCD_WriteString(r);
        Delay(200);
    }
}

// void labirinto(int valueReadDir, int valueReadEsq)
// {
//     // VAI RETO
//     if (valueReadDir > BRANCO && valueReadEsq > BRANCO)
//     {
//         PWM_Write(TIMER1, 1, val);
//         PWM_Write(TIMER0, 1, val);
//     }

//     // ENCONTRA UM CRUZAMENTO ENTAO GIRA PARA DIREITA
//     else if (valueReadDir < PRETO && valueReadEsq < PRETO)
//     {
//         while (valueReadEsq > BRANCO)
//         {
//             PWM_Write(TIMER1, 1, val);
//             PWM_Write(TIMER0, 0, val);
//         }
//         PWM_Write(TIMER0, 1, val);
//     }
// }
