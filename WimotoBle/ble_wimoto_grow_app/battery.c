/** @file
*
* @{
* @brief Battery level reading using as ADC
*
* This file contains the source code for battery level reading using ADC
* Adapted from  battery.c source code provided by Nordic
* 
* Author : Hariprasad  C R
* Date   : 05/02/2014
*/

/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
*
* The information contained herein is property of Nordic Semiconductor ASA.
* Terms and conditions of usage are described in detail in NORDIC
* SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
*
* Licensees are granted free, non-transferable use of the information. NO
* WARRANTY of ANY KIND is provided. This heading must NOT be removed from
* the file.
*
*/


#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
//#include "ble_stack_handler.h"
#include "softdevice_handler.h"
#include "ble_bas.h"
#include "battery.h"
#include "app_util.h"

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                                      /**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION         3                                         /**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS       0                                         /**< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com. */

extern ble_bas_t                             bas;
extern uint8_t                               battery_lvl;                             /*battery level for broadcasting*/ 
/**@brief Macro to convert the result of ADC conversion in millivolts.
*
* @param[in]  ADC_VALUE   ADC result.
* @retval     Result converted to millivolts.
*/
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
    ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 255) * ADC_PRE_SCALING_COMPENSATION)


/**@brief    Function for reading Battery Level and express the result in terms of Percentage
* @details  This function will fetch the conversion result from the ADC, convert the value into
*           percentage and send it to peer.
*/
void battery_start(void)
{
    uint32_t err_code;
    uint8_t  adc_result;
    uint16_t batt_lvl_in_milli_volts;
    uint8_t  percentage_batt_lvl;

    // Configure ADC
    NRF_ADC->CONFIG     = (ADC_CONFIG_RES_8bit                        << ADC_CONFIG_RES_Pos)     |
    (ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)  |
    (ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos)  |
    (ADC_CONFIG_PSEL_Disabled                   << ADC_CONFIG_PSEL_Pos)    |
    (ADC_CONFIG_EXTREFSEL_None                  << ADC_CONFIG_EXTREFSEL_Pos);
    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;


    NRF_ADC->TASKS_START = 1;
    while(NRF_ADC->EVENTS_END == 0);
    NRF_ADC->EVENTS_END = 0;
    adc_result = NRF_ADC->RESULT;                                                /* ADC result after conversion*/

		// *** Fix for PAN #1
    NRF_ADC->TASKS_STOP = 1;
    // *** End of fix for PAN #2 
		 
    NRF_ADC->ENABLE         = ADC_ENABLE_ENABLE_Disabled;		


    batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result) +
    DIODE_FWD_VOLT_DROP_MILLIVOLTS;
    percentage_batt_lvl     = battery_level_in_percent(batt_lvl_in_milli_volts);
		battery_lvl             = percentage_batt_lvl;                         /*save battery level data  to a global variable for broadcasting*/  
    err_code = ble_bas_battery_level_update(&bas, percentage_batt_lvl);
    if (
            (err_code != NRF_SUCCESS)
            &&
            (err_code != NRF_ERROR_INVALID_STATE)
            &&
            (err_code != BLE_ERROR_NO_TX_BUFFERS)
            &&
            (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
            )
    {
        APP_ERROR_HANDLER(err_code);
    }		
    
}

/**
* @}
*/
