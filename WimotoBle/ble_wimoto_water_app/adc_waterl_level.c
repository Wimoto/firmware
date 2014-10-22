/** @file
 * This file contains the source code for Water level sensor driver.
 * 
 * Date   : 11/06/2013
 * Author : Hariprasad CR
 */

#include "wimoto_sensors.h"
#include "wimoto.h"

/**
 *@brief Function for ADC initialization
*/
void adc_init(void)
{	
		// interrupt ADC
		NRF_ADC->INTENSET = (ADC_INTENSET_END_Enabled << ADC_INTENSET_END_Pos);											/*!< Interrupt enabled. */
		
		// config ADC
		NRF_ADC->CONFIG	= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos) 									/* Bits 17..16 : ADC external reference pin selection. */
										| (ADC_CONFIG_PSEL_AnalogInput3 << ADC_CONFIG_PSEL_Pos)											/*!< Use analog input3(p0.02) as ADC input. */
										| (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)													/*!< Use internal 1.2V bandgap voltage as reference for conversion. */
										| (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) /*!< Analog input specified by PSEL with no prescaling used as input for the conversion. */
										| (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos)	;										  					/*configure 8 bit resolution*/
		
		// enable ADC		
		NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;					  																			
}


/**
 *@brief  Function to read the sensor output value after using ADC (conversion of analog data into digital data using ADC)
 *@retval 8 bit data from the ADC , after conversion 
*/
uint8_t do_waterl_adc_measurement()
{
	
    uint8_t adc_result;            /* Result after ADC conversion*/
	
		nrf_gpio_cfg_input(WATER_LEVEL_PIN, NRF_GPIO_PIN_NOPULL);	/*configure p0.02 as ADC input*/
	
    adc_init();                    /* Initialize ADC for probe temperature measurement */
  
		// start ADC conversion
		NRF_ADC->TASKS_START = 1;
	
		// wait for conversion to end
		while (!NRF_ADC->EVENTS_END)
		{}
			
		NRF_ADC->EVENTS_END	= 0;
			
		//Save your ADC result	
    adc_result = NRF_ADC->RESULT;                                                /* ADC result after conversion*/
			
		NRF_ADC->TASKS_STOP = 1;	
			
    nrf_gpio_pin_clear(PROBE_SENSOR_ENERGIZE_PIN);	                             /* Clear the pin P0.02 after the use of sensor*/
		
    return adc_result;
		
		
}
