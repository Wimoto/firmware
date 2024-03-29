/** @file
* This file contains the source code for Soil moisture sensor driver.
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
		//nrf_gpio_cfg_input_high_drive(ADC_SOIL_MOISTURE_PIN,NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(ADC_SOIL_MOISTURE_PIN, NRF_GPIO_PIN_NOPULL);
	
		NRF_ADC->CONFIG = ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos |                                 /*!< 8bit ADC resolution. */ 
                      ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos |  /*!< Analog input specified by PSEL with no prescaling used as input for the conversion. */
                      ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << ADC_CONFIG_REFSEL_Pos |       /*!< Use internal 1.2V bandgap voltage as reference for conversion. */
                      (ADC_CONFIG_PSEL_AnalogInput5) << ADC_CONFIG_PSEL_Pos;                        /*!< Use P0.01 as analog input 2. ) */
    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled; 
}


/**
*@brief  Function to read the sensor output value after using ADC (conversion of analog data into digital data using ADC)
*@retval 8 bit data from the ADC , after conversion 
*/
uint8_t do_soil_moisture_measurement()
{

    uint8_t adc_result;            /* Result after ADC convertion*/
		adc_init();
		//NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
    one_mhz_start();               /* Start 1Mhz timer*/
    delay_ms(1000); 

    NRF_ADC->TASKS_START = START_ADC;

    while(NRF_ADC->EVENTS_END == 0)
		{
		};
    NRF_ADC->EVENTS_END = STOP_RUNNING_CONVERTION;
    adc_result = NRF_ADC->RESULT;                   /* ADC result after conversion*/
		
    // *** Fix for PAN #1
    NRF_ADC->TASKS_STOP = 1;
    // *** End of fix for PAN #2 
		
    NRF_ADC->ENABLE         = ADC_ENABLE_ENABLE_Disabled;

		one_mhz_stop();                                 /* End the square wave and pull down the pin to low value*/
    return adc_result;		

}
