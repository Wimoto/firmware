/** @file
 * This file contains the source code for Water level sensor driver.
 * 
 * Date   : 11/06/2013
 * Author : Hariprasad CR
 * Change log:
 * Sruthi.K.S						24/10/2013     configured sensor energize pin in water level ADC measurement
 */

#include "wimoto_sensors.h"
#include "wimoto.h"

/**
 *@brief Function for ADC initialization
*/
void adc_init(void)
{	
		// interrupt ADC
		NRF_ADC->INTENSET = (ADC_INTENSET_END_Enabled << ADC_INTENSET_END_Pos);																				/*!< Interrupt enabled. */
		
		
	// config ADC    
		NRF_ADC->CONFIG = (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos)                                							   	 /*!< 8 bit ADC resolution. */ 
											|	(ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)  							 /*!< Analog input specified by PSEL with 1/3 prescaling used as input for the conversion */
                      |	(ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << ADC_CONFIG_REFSEL_Pos)      								 /*!< Use supply voltage with 1/3 prescaling as reference for conversion. */
                      |	(ADC_CONFIG_PSEL_AnalogInput2) << ADC_CONFIG_PSEL_Pos;                    								 /*!< Use analog input 2 as ADC input. (P0.01) */
		
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
		
		nrf_gpio_cfg_output(PROBE_SENSOR_ENERGIZE_PIN);                              /* Configure P0.02 as output */
    nrf_gpio_pin_dir_set(PROBE_SENSOR_ENERGIZE_PIN,NRF_GPIO_PIN_DIR_OUTPUT);     /* Set the direction of P0.02 as output*/
    nrf_gpio_pin_set(PROBE_SENSOR_ENERGIZE_PIN);                                 /* Set the value of P0.02 to high for water level sensor*/
		
		nrf_gpio_cfg_input(WATER_LEVEL_PIN, NRF_GPIO_PIN_NOPULL);	/*configure p0.01 as ADC input*/
	
    adc_init();                    /* Initialize ADC for water level measurement */
  
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
