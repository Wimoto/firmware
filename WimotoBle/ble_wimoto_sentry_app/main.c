/** @file
*
* main.c
* 
*
* This file contains the source code for calling the water broadcast and water alarm services
* Author : Hariprasad C R
* Date   : 11/29/2013
* 
*/
#include <stdint.h>
#include <stdio.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "ble_radio_notification.h"
#include "nrf_gpio.h"
#include "ble_flash.h"
#include "ble_debug_assert_handler.h"
#include "nrf_temp.h"
#include "twi_master.h"
#include "wimoto_sensors.h"
#include "wimoto.h"

#include "simple_uart.h"


bool 	  BROADCAST_MODE = false;									/*flag used to switch between broadcast and connectable modes*/

int main()
{  
    /*
* The loop starts with conectable_mode().  
* Once the connected Peripheral device sets the switch mode characteristic and disconnects,
* then the BROADCAST_MODE flag is set to true and enters broadcast_mode().  
* Otherwise it remains in connected state.
*/ 
		
	
    NRF_POWER->GPREGRET = 0;  /*Initialize the value of general purpose retention register to 0 */
	
		simple_uart_config(RTS_PIN_NUMBER, 9, CTS_PIN_NUMBER, 10, false); /*Initialize UART*/
		simple_uart_putstring((const uint8_t *)"Welcome to the sentry\r");
		simple_uart_putstring((const uint8_t *)"-------------------------------------------\r\r");
	

		
	
    for(;;)
    {			
        connectable_mode();   /*Advertise water parameter alarm service */	
        broadcast_mode();     /*broadcast water parameters  */
    }		
}	
