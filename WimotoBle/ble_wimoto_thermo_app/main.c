/** @file
*
* main.c
* 
*
* This file contains the source code for calling the thermo broadcast and thermo alarm services
* Author : Sherin
* Date   : 11/20/2013
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


int main()
{
    /*The loop starts with conectable_mode(). If a client device is connected and */
    /*Otherwise it remains in connected state. */
    NRF_POWER->GPREGRET = 0;  /*Initialize the value of general purpose retention register to 0 */

    for(;;)
    {			
        connectable_mode();   /*Advertise thermo parameter alarm service */	
       
    }		
}	
