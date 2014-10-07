#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "softdevice_handler.h"
#include "ble_flash.h"
#include "ble_debug_assert_handler.h"
#include "nrf_temp.h"
#include "ble_gap.h"
#include "wimoto_sensors.h"




/** @brief Function for initializing the Timer 2 peripheral.
*/
void timer2_init(void)
{
    uint8_t  softdevice_enabled,err_code;
    uint32_t ret_val;
    err_code = sd_softdevice_is_enabled(&softdevice_enabled);
    APP_ERROR_CHECK(err_code);

    // Start 16 MHz crystal oscillator .
    if (softdevice_enabled == 0)
    {
        NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;
        NRF_CLOCK->TASKS_HFCLKSTART     = 1;

        // Wait for the external oscillator to start up.
        while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
        {
            //Do nothing.
        }
    }
    else
    {
        sd_clock_hfclk_request(); 		
        while (ret_val == 0) 
        {
            sd_clock_hfclk_is_running(&ret_val);//Do nothing.
        }

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
void gpiote1_init(void)
{
    // Configure PWM_OUTPUT_PIN_NUMBER as an output.
    nrf_gpio_cfg_output(PWM_OUTPUT_PIN_NUMBER);

    //    NRF_GPIO->OUT = 0x00000000UL;

    //    // Configure GPIOTE channel 0 to toggle the PWM pin state
    //	// @note Only one GPIOTE task can be connected to an output pin.
    //    nrf_gpiote_task_config(0, PWM_OUTPUT_PIN_NUMBER, \
    //                           NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);

    nrf_gpio_pin_set(PWM_OUTPUT_PIN_NUMBER);

    NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos |
    GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos |
    PWM_OUTPUT_PIN_NUMBER << GPIOTE_CONFIG_PSEL_Pos;
}


/** @brief Function for initializing the Programmable Peripheral Interconnect peripheral.
*/
void ppi_init(void)
{
    uint8_t  softdevice_enabled,err_code;		
    // Configure PPI channel 0 to toggle PWM_OUTPUT_PIN on every TIMER2 COMPARE[0] match.

    err_code = sd_softdevice_is_enabled(&softdevice_enabled);
    APP_ERROR_CHECK(err_code);

    if (softdevice_enabled == 0)
    {		// Configure PPI channel 0 to toggle PWM_OUTPUT_PIN on every TIMER2 COMPARE[0] match.
        NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[0];
        NRF_PPI->CH[0].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0]; 
        // Enable PPI channel 0.
        NRF_PPI->CHEN = (PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos);
    }
    else
    {
        err_code = sd_ppi_channel_assign( 0 , &NRF_TIMER2->EVENTS_COMPARE[0], &NRF_GPIOTE->TASKS_OUT[0]);
        err_code = sd_ppi_channel_enable_set(PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos);		
    }
}


/**
* @brief Function for application main entry.
*/
int one_mhz_start(void)
{
    gpiote1_init();
    ppi_init();
    timer2_init();

    // Start the timer.
    NRF_TIMER2->TASKS_START = 1;
    return 0;		
}

/** @} */



