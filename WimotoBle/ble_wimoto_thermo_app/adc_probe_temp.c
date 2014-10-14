/** @file
 * This file contains the source code for probe temperature sensor driver.
 * 
 * Date   : 11/06/2013
 * Author : Hariprasad CR
 */

#include "wimoto_sensors.h"
#include "wimoto.h"
#include "nrf_delay.h"


/**
 *@brief Function for ADC initialization
*/
void adc_init()
{	

    NRF_ADC->CONFIG = (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos) |                                 /*!< 8bit ADC resolution. */ 
                      (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) |  /*!< Analog input specified by PSEL with 1/3 prescaling. */
                      (ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << ADC_CONFIG_REFSEL_Pos) |       /*!< Use 1/3 prescaling for reference voltage */
											(ADC_CONFIG_PSEL_AnalogInput5 << ADC_CONFIG_PSEL_Pos)|                      /*!< Use analog input 5 as analog input. (P0.04)  changed way mask was being shifted*/
											(ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);
    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;                                                  /*!< Enable ADC*/

}


/**
 *@brief  Function to read the sensor output value after using ADC (conversion of analog data into digital data using ADC)
 *@retval 8 bit data from the ADC , after conversion 
*/
uint8_t do_probe_temperature_measurement()
{
    uint8_t adc_result;            /* Result after ADC conversion*/

	  adc_init();                    /* Initialize ADC for probe temperature measurement */
	  
	
    nrf_gpio_cfg_output(PROBE_SENSOR_ENERGIZE_PIN);                              /* Configure P0.02 as output */
    nrf_gpio_pin_dir_set(PROBE_SENSOR_ENERGIZE_PIN,NRF_GPIO_PIN_DIR_OUTPUT);     /* Set the direction of P0.02 as output*/
    nrf_gpio_pin_set(PROBE_SENSOR_ENERGIZE_PIN);                                 /* Set the value of P0.02 to high for soil moisture sensor*/
		//nrf_delay_ms(20);
		
	
    NRF_ADC->TASKS_START = START_ADC;
    while(NRF_ADC->EVENTS_END == 0);
    NRF_ADC->EVENTS_END = STOP_RUNNING_CONVERTION;
    adc_result = NRF_ADC->RESULT;                                                /* ADC result after conversion*/
	
    NRF_ADC->ENABLE         = ADC_ENABLE_ENABLE_Disabled;

    nrf_gpio_pin_clear(PROBE_SENSOR_ENERGIZE_PIN);	                             /* Clear the pin P0.02 after the use of sensor*/
		
    return adc_result;
		
}
