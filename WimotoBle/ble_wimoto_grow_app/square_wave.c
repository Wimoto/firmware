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




/** @brief Function for initializing the Timer 1 peripheral.
*/
void timer1_init(void)
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

    NRF_TIMER1->MODE        = TIMER_MODE_MODE_Timer;
    NRF_TIMER1->BITMODE     = TIMER_BITMODE_BITMODE_08Bit << TIMER_BITMODE_BITMODE_Pos;
    NRF_TIMER1->PRESCALER   = TIMER_PRESCALERS;

    //Enable Shortcut between CC[0] event and the CLEAR task. 
    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;

    // Clears the timer, sets it to 0.
    NRF_TIMER1->TASKS_CLEAR = 1;

    // Load the initial values to TIMER1 CC register.
    NRF_TIMER1->CC[0] = TICKS ;

    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

}


/** @brief Function for initializing the GPIO Tasks/Events peripheral.
*/
void gpiote1_init(void)
{
    // Configure PWM_OUTPUT_PIN_NUMBER as an output.
    nrf_gpio_cfg_output(PWM_OUTPUT_PIN_NUMBER);

    // Configure GPIOTE channel 0 to toggle the PWM pin state
    // @note Only one GPIOTE task can be connected to an output pin.
    

    nrf_gpio_pin_clear(PWM_OUTPUT_PIN_NUMBER);

    NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos |
    GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos |
    PWM_OUTPUT_PIN_NUMBER << GPIOTE_CONFIG_PSEL_Pos;
}


/** @brief Function for initializing the Programmable Peripheral Interconnect peripheral.
*/
void ppi_init(void)
{
    uint8_t  softdevice_enabled,err_code;		
    // Configure PPI channel 0 to toggle PWM_OUTPUT_PIN on every TIMER1 COMPARE[0] match.

    err_code = sd_softdevice_is_enabled(&softdevice_enabled);
    APP_ERROR_CHECK(err_code);

    if (softdevice_enabled == 0)
    {		// Configure PPI channel 0 to toggle PWM_OUTPUT_PIN on every TIMER1 COMPARE[0] match.
        NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TIMER1->EVENTS_COMPARE[0];
        NRF_PPI->CH[0].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0]; 
        // Enable PPI channel 0.
        NRF_PPI->CHEN = (PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos);
    }
    else
    {
        err_code = sd_ppi_channel_assign( 0 , &NRF_TIMER1->EVENTS_COMPARE[0], &NRF_GPIOTE->TASKS_OUT[0]);
        err_code = sd_ppi_channel_enable_set(PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos);		
    }
}


/**
* @brief Function for starting the square wave.
*/
int one_mhz_start(void)
{
    gpiote1_init();
    ppi_init();
    timer1_init();

    // Start the timer.
    NRF_TIMER1->TASKS_START = 1;
    return 0;		
}

/** @} */

/**
* @brief Function for stopping the square wave.
*/

int one_mhz_stop(void)
{
		NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos |     /*pull down the pwm pin*/
    GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos |
    PWM_OUTPUT_PIN_NUMBER << GPIOTE_CONFIG_PSEL_Pos;
	
		NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Disabled << GPIOTE_CONFIG_MODE_Pos;  /*Disable the GPIOTE*/

		sd_ppi_channel_enable_clr(PPI_CHEN_CH0_Disabled << PPI_CHEN_CH0_Pos);           /*Disable the PPI channel*/
		
	  NRF_TIMER1->TASKS_STOP  = 1; 	                                                  /* Stop TIMER 1 after conversion*/		
	
	  
    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Disabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos; /*Disable Shortcut between CC[0] event and the CLEAR task. */
		
		
		

	  sd_clock_hfclk_release();																										/* Releases the high frequency crystal oscillator */
		
		return 0;
}



