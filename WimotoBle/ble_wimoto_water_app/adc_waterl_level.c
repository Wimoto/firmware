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
    NRF_ADC->CONFIG = ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos |                                 /*!< 8bit ADC resolution. */ 
                      ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos |  /*!< Analog input specified by PSEL with no prescaling used as input for the conversion. */
                      ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << ADC_CONFIG_REFSEL_Pos |       /*!< Use internal 1.2V bandgap voltage as reference for conversion. */
                     (1 << WATER_LEVEL_PIN) << ADC_CONFIG_PSEL_Pos;                               /*!< Use analog input 2 as analog input. (P0.01) */
    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;   
}


/**
 *@brief  Function to read the sensor output value after using ADC (conversion of analog data into digital data using ADC)
 *@retval 8 bit data from the ADC , after conversion 
*/
uint8_t do_waterl_adc_measurement()
{
	
    uint8_t adc_result;            /* Result after ADC conversion*/

    adc_init();                    /* Initialize ADC for probe temperature measurement */
  
    NRF_ADC->TASKS_START = START_ADC;
    while(NRF_ADC->EVENTS_END == 0);
    NRF_ADC->EVENTS_END = STOP_RUNNING_CONVERTION;
    adc_result = NRF_ADC->RESULT;                                                /* ADC result after conversion*/
	
    NRF_ADC->ENABLE         = ADC_ENABLE_ENABLE_Disabled;
		
    return adc_result;
		
}
