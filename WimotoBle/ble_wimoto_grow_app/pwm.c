#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"

#define PWM_OUTPUT_PIN_NUMBER    0    /**< Pin number for PWM output. Now it is configured as LED0 */

#define TICKS (4UL)                   /**< Ticks */
#define TIMER_PRESCALERS 1U           /**< Prescaler setting for timer, as 1. */


/** @brief Function for initializing the Timer 2 peripheral.
*/
static void timer2_init(void)
{
    // Start 16 MHz crystal oscillator .
    NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;
    NRF_CLOCK->TASKS_HFCLKSTART     = 1;

    // Wait for the external oscillator to start up.
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
    {
        //Do nothing.
    }

    NRF_TIMER2->MODE        = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE     = TIMER_BITMODE_BITMODE_08Bit << TIMER_BITMODE_BITMODE_Pos;
    NRF_TIMER2->PRESCALER   = TIMER_PRESCALERS;

    //Enable Shortcut between CC[0] event and the CLEAR task. 
    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;

    // Clears the timer, sets it to 0.
    NRF_TIMER2->TASKS_CLEAR = 1;

    // Load the initial values to TIMER2 CC register.
    NRF_TIMER2->CC[0] = TICKS ;

    NRF_TIMER2->EVENTS_COMPARE[0] = 0;

}


/** @brief Function for initializing the GPIO Tasks/Events peripheral.
*/
static void gpiote_init(void)
{
    // Configure PWM_OUTPUT_PIN_NUMBER as an output.
    nrf_gpio_cfg_output(PWM_OUTPUT_PIN_NUMBER);

    NRF_GPIO->OUT = 0x00000000UL;

    // Configure GPIOTE channel 0 to toggle the PWM pin state
    // @note Only one GPIOTE task can be connected to an output pin.
    nrf_gpiote_task_config(0, PWM_OUTPUT_PIN_NUMBER, \
    NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
}


/** @brief Function for initializing the Programmable Peripheral Interconnect peripheral.
*/
static void ppi_init(void)
{
    // Configure PPI channel 0 to toggle PWM_OUTPUT_PIN on every TIMER2 COMPARE[0] match.
    NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[0];
    NRF_PPI->CH[0].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0];

    // Enable PPI channel 0.
    NRF_PPI->CHEN = (PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos);
}


/**
* @brief Function for application main entry.
*/
int one_mhz_start(void)
{
    gpiote_init();
    ppi_init();
    timer2_init();

    // Enable interrupt on Timer 2.
    NVIC_EnableIRQ(TIMER2_IRQn);
    __enable_irq();

    // Start the timer.
    NRF_TIMER2->TASKS_START = 1;
    return 0;		
}

/** @} */
