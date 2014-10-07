/** @file
*
*main.c
* 
*
* This file contains the source code for calling the temperature broadcast and Temperature alarm services
* Author : Shafy.
* Date   : 10/19/2013
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

bool 	  BROADCAST_MODE = false;									/*flag used to switch between broadcast and connectable modes*/

int main()
{

    /*The process starts with connectable_mode().  If user connects and sets the characteristics 'M' in temperature */
    /*service, BROADCAST_MODE will be set to true and broadcast_mode() executes indefinitely                        */

    NRF_POWER->GPREGRET = 0;  /*Initialize the value of general purpose retention register to 0 */

    for(;;)
    {			
        connectable_mode();   /*Advertise grow parameter alarm service */	
        broadcast_mode();     /*broadcast grow parameters  */

    }		
}	
