/** @file
 * This file contains the source code for probe temperature sensor driver.
 * 
 * Date   : 11/06/2013
 * Author : Hariprasad CR
 */

#include "wimoto_sensors.h"
#include "wimoto.h"


/**
 *@brief Function for ADC initialization
*/
void adc_init()
{	
		// interrupt ADC
		NRF_ADC->INTENSET = (ADC_INTENSET_END_Enabled << ADC_INTENSET_END_Pos);																				/*!< Interrupt enabled. */
		
		// config ADC    
		NRF_ADC->CONFIG = (ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos)                                							   /*!< 10bit ADC resolution. */ 
											|	(ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)  							 /*!< Analog input specified by PSEL with no pre-scaling used as input for the conversion. */
                      |	(ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << ADC_CONFIG_REFSEL_Pos)      								 /*!< Use supply voltage with 1/3 prescaling as reference for conversion. */
                      |	(ADC_CONFIG_PSEL_AnalogInput5) << ADC_CONFIG_PSEL_Pos;                    								 /*!< Use analog input 6 as analog input. (P0.05) */
		// enable ADC		
		NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;					  																											 /* Bit 0 : ADC enable. */	

}


/**
 *@brief  Function to read the sensor output value after using ADC (conversion of analog data into digital data using ADC)
 *@retval 10 bit data from the ADC , after conversion 
*/
uint16_t do_probe_temperature_measurement()
{
    uint16_t adc_result;                                                         /* Result after ADC conversion*/
	
		nrf_gpio_cfg_output(PROBE_SENSOR_ENERGIZE_PIN);                              /* Configure energize pin as output */
    nrf_gpio_pin_dir_set(PROBE_SENSOR_ENERGIZE_PIN,NRF_GPIO_PIN_DIR_OUTPUT);     /* Set the direction of output pin*/
    nrf_gpio_pin_set(PROBE_SENSOR_ENERGIZE_PIN);                                 /* Set the value of energize pin for ADC probe*/
		
		nrf_gpio_cfg_input(PROBE_ADC_INPUT_AIN5_P04, NRF_GPIO_PIN_NOPULL);					 /*configure ADC input*/
	
	  adc_init();                                                                  /* Initialize ADC for probe temperature measurement */
    
		// start ADC conversion
		NRF_ADC->TASKS_START = 1;
	
		// wait for conversion to end
		while (!NRF_ADC->EVENTS_END)
		{}
			
		NRF_ADC->EVENTS_END	= 0;
			
		//Save your ADC result	
    adc_result = NRF_ADC->RESULT;                                                /* ADC result after conversion*/
		
		NRF_ADC->TASKS_STOP = 1;	
			
    nrf_gpio_pin_clear(PROBE_SENSOR_ENERGIZE_PIN);	                             /* Clear the energize pin after the use of sensor*/
		
    return adc_result;
		
}
