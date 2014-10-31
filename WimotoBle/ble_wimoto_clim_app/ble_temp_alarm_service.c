/** @file
*
* @{
* @ingroup ble_sdk_app_hts
* @brief Temperature Service file.
*
* This file contains the source code for Temperature alarm service
* It also includes the code for Device Information services.
* This application uses the @ref srvlib_conn_params module.
* Adapted from  Battery service provided by Nordic
* 
* Author : Shafy
* Date   : 10/19/2013
* Change log:
* Sherin    		 12/10/2013       Added write events for value fields
* Hariprasad     12/11/2013       Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* Sruthi.k.s     10/01/2014       migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s 		 10/17/2014			  Added alarm characteristic with time stamp.
* Shafy S        10/28/2014       Added changes to show last occurance timestamp of alarm
*/

#include "ble_temp_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"
#include "app_error.h"  

bool     	      TEMPS_CONNECTED_STATE=false;  /*Indicates whether the temperature service is connected or not*/
extern bool     CHECK_ALARM_TIMEOUT;          /*Flag to indicate whether to check for alarm conditions defined in connect.c*/
extern uint8_t	var_receive_uuid;							/*variable for receiving uuid*/
extern  uint8_t	temperature[2];               /*variable to store current temperature value to broadcast*/
bool            temp_alarm_set_changed = false; 
/**@brief Function for handling the Connect event.
*
* @param[in]   p_temps       Temperature Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    p_temps->conn_handle  = p_ble_evt->evt.gap_evt.conn_handle;
		TEMPS_CONNECTED_STATE = true;  /*Set the flag to true so that state remains in connectable mode until disconnect*/
		CHECK_ALARM_TIMEOUT   = true;  /*set the flag to true to check the alarm condition on connection*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_temps       Temperature Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    TEMPS_CONNECTED_STATE= false; 
    p_temps->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of temperature low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    CHECK_ALARM_TIMEOUT = true; 
}


/**@brief Function for handling the Write event.
*
* @param[in]   p_temps       Temperature Service structure.
* @param[in]   p_ble_evt     Event received from the BLE stack.
*/
static void on_write(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    if (p_temps->is_notification_supported)
    {

        //write event for temperature low value char cccd.   
        if (
                (p_evt_write->handle == p_temps->temp_low_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_temps->evt_handler != NULL)
            {
                ble_temps_low_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_temps->evt_handler(p_temps, &evt);
            }
        }
        
        //write event for temperature high value char cccd. 

        if (
                (p_evt_write->handle == p_temps->temp_high_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_temps->evt_handler != NULL)
            {
                ble_temps_low_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_temps->evt_handler(p_temps, &evt);
            }
        }
        
        //write event for temperature alarm set cccd
        
        if (
                (p_evt_write->handle == p_temps->climate_temp_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_temps->evt_handler != NULL)
            {
                ble_temps_low_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_temps->evt_handler(p_temps, &evt);
            }
        } 
        
        //write event for temperature alarm with time stamp cccd 
        
        if (
                (p_evt_write->handle == p_temps->climate_temp_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_temps->evt_handler != NULL)
            {
                ble_temps_low_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_temps->evt_handler(p_temps, &evt);
            }
        } 
        
        //write event for temperature cccd
        
        if (
                (p_evt_write->handle == p_temps->current_temp_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_temps->evt_handler != NULL)
            {
                ble_temps_low_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_TEMPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_temps->evt_handler(p_temps, &evt);
            }
        }

    }

    // write event for temperature low value char value. 
    if (
            (p_evt_write->handle == p_temps->temp_low_level_handles.value_handle) 
            && 
            (p_evt_write->len == 2)
            )
    {
        // update the temperature service structure
        p_temps->climate_temperature_low_level[0] =   p_evt_write->data[0];
        p_temps->climate_temperature_low_level[1] =   p_evt_write->data[1];

        // call application event handler
        p_temps->write_evt_handler();
    }


    // write event for temperature high value char value. 
    if (
            (p_evt_write->handle == p_temps->temp_high_level_handles.value_handle) 
            && 
            (p_evt_write->len == 2)
            )
    {
        // update the temperature service structure
        p_temps->climate_temperature_high_level[0] =   p_evt_write->data[0];
        p_temps->climate_temperature_high_level[1] =   p_evt_write->data[1];

        // call application event handler
        p_temps->write_evt_handler();
    }


    // write event for alarm set value char value. 
    if (
            (p_evt_write->handle == p_temps->climate_temp_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_temps->climate_temperature_alarm_set =   p_evt_write->data[0];

				//set the flag to indicate that the alarm set characteristics is changed
				temp_alarm_set_changed = true;
			 
        // call application event handler
        p_temps->write_evt_handler();
    }


}


void ble_temps_on_ble_evt(ble_temps_t * p_temps, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_temps, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_temps, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_temps, p_ble_evt);
        break;

    default:
        break;
    }
    
}


/**@brief Function for adding the current temperature characteristics.
*
* @param[in]   p_temps        Temperature Service structure.
* @param[in]   p_temps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_temperature_char_add(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_temperature[2];

    if (p_temps->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_temps_init->temperature_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_temps->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc        =	NULL;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_temps->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;


    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_TEMPS_CURR_TEMP_UUID; 

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_temps_init->temperature_char_attr_md.read_perm;
    attr_md.write_perm = p_temps_init->temperature_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_temperature);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_temperature);
    attr_char_value.p_value      = current_temperature;

    err_code = sd_ble_gatts_characteristic_add(p_temps->service_handle, &char_md,
    &attr_char_value,
    &p_temps->current_temp_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the temperature low value characteristics.
*
* @param[in]   p_temps        Temperature Service structure.
* @param[in]   p_temps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t temperature_low_value_char_add(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_temperature_low_level[2];

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     			= 1;                     
		char_md.char_props.write					=	1;											/*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                      /*add fix for characteristic write issue*/
    char_md.char_props.notify  				= 0;
    char_md.p_char_user_desc    			=	NULL;
    char_md.p_char_pf           			= NULL;
    char_md.p_user_desc_md      			= NULL;
    char_md.p_cccd_md           			= NULL;
    char_md.p_sccd_md           			= NULL;


    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_TEMPS_LOW_CHAR_UUID; 

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_temps_init->temperature_char_attr_md.read_perm;
    attr_md.write_perm = p_temps_init->temperature_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_temperature_low_level[1] = p_temps_init->climate_temperature_low_level[1];
    climate_temperature_low_level[0] = p_temps_init->climate_temperature_low_level[0];		
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(climate_temperature_low_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(climate_temperature_low_level);
    attr_char_value.p_value      = climate_temperature_low_level;

    err_code = sd_ble_gatts_characteristic_add(p_temps->service_handle, &char_md,
    &attr_char_value,
    &p_temps->temp_low_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the temperature high value characteristics.
*
* @param[in]   p_temps        Temperature Service structure.
* @param[in]   p_temps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t temperature_high_value_char_add(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_temperature_high_level[2];   		

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     			= 1;
		char_md.char_props.write					=	1;                               /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                               /*add fix for characteristic write issue*/
    char_md.char_props.notify   			= 0;
    char_md.p_char_user_desc    			=	NULL;
    char_md.p_char_pf          				= NULL;
    char_md.p_user_desc_md      			= NULL;
    char_md.p_cccd_md           			= NULL;
    char_md.p_sccd_md           			= NULL;

    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_TEMPS_HIGH_CHAR_UUID;     

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_temps_init->temperature_char_attr_md.read_perm;
    attr_md.write_perm = p_temps_init->temperature_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_temperature_high_level[1] = p_temps_init->climate_temperature_high_level[1];  /*Default values for temperature high value*/
    climate_temperature_high_level[0] = p_temps_init->climate_temperature_high_level[0];	

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(climate_temperature_high_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(climate_temperature_high_level);
    attr_char_value.p_value      = climate_temperature_high_level;

    err_code = sd_ble_gatts_characteristic_add(p_temps->service_handle, &char_md,
    &attr_char_value,
    &p_temps->temp_high_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the temperature alarm set characteristics.
*
* @param[in]   p_temps        Temperature Service structure.
* @param[in]   p_temps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t temperature_alarm_set_char_add(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_temperature_alarm_set;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   				= 1;                                 
		char_md.char_props.write					=	1;                                 /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                                 /*add fix for characteristic write issue*/
    char_md.char_props.notify 				= 0;
    char_md.p_char_user_desc  				=	NULL;
    char_md.p_char_pf         				= NULL;
    char_md.p_user_desc_md    				= NULL;
    char_md.p_cccd_md         				= NULL;
    char_md.p_sccd_md         				= NULL;

    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_TEMPS_ALARM_SET_CHAR_UUID;        

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_temps_init->temperature_char_attr_md.read_perm;
    attr_md.write_perm = p_temps_init->temperature_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_temperature_alarm_set = p_temps_init->climate_temperature_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &climate_temperature_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_temps->service_handle, &char_md,
    &attr_char_value,
    &p_temps->climate_temp_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the temperature alarm characteristics.
*
* @param[in]   p_temps        Temperature Service structure.
* @param[in]   p_temps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t temperature_alarm_char_add(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
	
		/* array for receiving alarm with time stamp from temperature service structure*/
    static uint8_t      temps_alarm_with_time_stamp[8];

    // Add temperature high level characteristic 
    if (p_temps->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        // According to BAS_SPEC_V10, the read operation on cccd should be possible without
        // authentication.
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_temps_init->temperature_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = (p_temps->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_temps->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;


    ble_uuid.type = p_temps->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_TEMPS_ALARM_CHAR_UUID;  

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_temps_init->temperature_char_attr_md.read_perm;
    attr_md.write_perm = p_temps_init->temperature_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

		//assigning alarm with temperature characteristics to alarm with time stamp array
    temps_alarm_with_time_stamp[0] = p_temps_init->temps_alarm_with_time_stamp[0];
		temps_alarm_with_time_stamp[1] = p_temps_init->temps_alarm_with_time_stamp[1];
		temps_alarm_with_time_stamp[2] = p_temps_init->temps_alarm_with_time_stamp[2];
		temps_alarm_with_time_stamp[3] = p_temps_init->temps_alarm_with_time_stamp[3];
		temps_alarm_with_time_stamp[4] = p_temps_init->temps_alarm_with_time_stamp[4];
		temps_alarm_with_time_stamp[5] = p_temps_init->temps_alarm_with_time_stamp[5];
		temps_alarm_with_time_stamp[6] = p_temps_init->temps_alarm_with_time_stamp[6];
		temps_alarm_with_time_stamp[7] = p_temps_init->temps_alarm_with_time_stamp[7];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(temps_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(temps_alarm_with_time_stamp);
    attr_char_value.p_value      = temps_alarm_with_time_stamp;

    err_code = sd_ble_gatts_characteristic_add(p_temps->service_handle, &char_md,
    &attr_char_value,
    &p_temps->climate_temp_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the temperature.
*
* @param[in]   p_temps        Temperature Service structure.
* @param[in]   p_temps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_temps_init(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
	
    // Add service
    ble_uuid.type = var_receive_uuid;
		p_temps->uuid_type = var_receive_uuid;
    ble_uuid.uuid = CLIMATE_PROFILE_TEMPS_SERVICE_UUID;

    // Initialize service structure
    p_temps->evt_handler                      = p_temps_init->evt_handler;
    p_temps->write_evt_handler                = write_evt_handler;
    p_temps->conn_handle                      = BLE_CONN_HANDLE_INVALID;
    p_temps->is_notification_supported        = p_temps_init->support_notification;
	
    p_temps->climate_temperature_low_level[0] = p_temps_init->climate_temperature_low_level[0]  ;
    p_temps->climate_temperature_low_level[1] = p_temps_init->climate_temperature_low_level[1]  ;
    p_temps->climate_temperature_high_level[0]= p_temps_init->climate_temperature_high_level[0] ;
    p_temps->climate_temperature_high_level[1]= p_temps_init->climate_temperature_high_level[1] ;
		
		
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_temps->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }


    err_code =  current_temperature_char_add(p_temps, p_temps_init);    /* Add current temperature characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  temperature_low_value_char_add(p_temps, p_temps_init);  /* Add temperature low value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code =  temperature_high_value_char_add(p_temps, p_temps_init); /* Add temperature high value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code =  temperature_alarm_set_char_add(p_temps, p_temps_init);  /* Add temperature alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  temperature_alarm_char_add(p_temps, p_temps_init);      /* Add temperature alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}


/**@brief Function reads and updates the current temperature and checks for alarm condition.
*
* @param[in]   p_temps        Temperature Service structure.
*
* @param[in]   p_device       Device management Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_temps_level_alarm_check(ble_temps_t * p_temps,ble_device_t *p_device)
{
    uint32_t err_code = NRF_SUCCESS;
    uint16_t current_temperature;
    uint8_t  current_temperature_array[2];
		
		uint16_t temperature_low_value;					   			/* Temperature low value set by user as uint16*/
    uint16_t temperature_high_value;				   			/* Temperature low value set by user as uint16*/

    static uint16_t previous_temperature = 0x00;
    static uint8_t alarm[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    uint16_t	len  = sizeof(alarm);										/*length of alarm with time stamp characteristics*/
    uint16_t  len1 = sizeof(current_temperature_array);
    

    current_temperature = read_temperature();        /* Read the current temperature*/
    if(current_temperature != previous_temperature)  /* Check whether temperature value has changed*/
    {
        current_temperature_array[1] = (uint8_t)(current_temperature & LOWER_BYTE_MASK);   /* Convert the temperature to uint8_t array*/
        current_temperature_array[0] =  current_temperature >>8;
				
				/*copy the current temperature value for broadcast*/
				temperature[1]=current_temperature_array[1];
				temperature[0]=current_temperature_array[0];
			
				if ((p_temps->conn_handle != BLE_CONN_HANDLE_INVALID) && p_temps->is_notification_supported)			
        {		
            ble_gatts_hvx_params_t hvx_params;
            memset(&hvx_params, 0, sizeof(hvx_params)); 

            hvx_params.handle   = p_temps->current_temp_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len1;
            hvx_params.p_data   = current_temperature_array;
					
            err_code = sd_ble_gatts_hvx(p_temps->conn_handle, &hvx_params);
						previous_temperature	= current_temperature;

					
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }		
    
    // Get the temperature low value set by the user from the service structure

    temperature_low_value 						= 	(p_temps->climate_temperature_low_level[0])<<8;	  /* Convert the 8 bit arrays to a 16 bit data*/
    temperature_low_value 						= 	temperature_low_value | (p_temps->climate_temperature_low_level[1]);	
    
    // Get the temperature high value set by the user from the service structure

    temperature_high_value            = 	(p_temps->climate_temperature_high_level[0])<<8;  /* Convert the 8 bit arrays to a 16 bit data*/
    temperature_high_value 			 			= 	temperature_high_value | (p_temps->climate_temperature_low_level[1]);	

    
    // Check whether the temperature is out of range if alarm is set by user 		
    if(p_temps->climate_temperature_alarm_set != 0x00)
    {

        if(current_temperature < temperature_low_value)
        {   
            alarm[0] = SET_ALARM_LOW;											/*set alarm to 01 if temperature is low */
						alarm[1]=p_device->device_time_stamp_set[0];	/*capture the timestamp when alarm occured*/
						alarm[2]=p_device->device_time_stamp_set[1];
						alarm[3]=p_device->device_time_stamp_set[2];
						alarm[4]=p_device->device_time_stamp_set[3];
						alarm[5]=p_device->device_time_stamp_set[4];
						alarm[6]=p_device->device_time_stamp_set[5];
						alarm[7]=p_device->device_time_stamp_set[6];
        }
        
        else if(current_temperature > temperature_high_value)
        {   
            alarm[0] = SET_ALARM_HIGH;										/*set alarm to 02 if temperature is high */
						alarm[1]=p_device->device_time_stamp_set[0];	/*capture the timestamp when alarm occured*/
						alarm[2]=p_device->device_time_stamp_set[1];
						alarm[3]=p_device->device_time_stamp_set[2];
						alarm[4]=p_device->device_time_stamp_set[3];
						alarm[5]=p_device->device_time_stamp_set[4];
						alarm[6]=p_device->device_time_stamp_set[5];
						alarm[7]=p_device->device_time_stamp_set[6];
        } 
        	
    }
    else if(temp_alarm_set_changed)						/*check whether the alarm set characteristics is set to 00*/
    {	
        alarm[0] = RESET_ALARM;								/*reset alarm to 0x00 and clear the timestamp*/
				alarm[1]=0x00;
				alarm[2]=0x00;
				alarm[3]=0x00;
				alarm[4]=0x00;
				alarm[5]=0x00;
				alarm[6]=0x00;
				alarm[7]=0x00;
			
				sd_ble_gatts_value_set(p_temps->climate_temp_alarm_handles.value_handle, 0, &len, alarm);  /*clear the value of alarm characteristics*/
				p_temps->temps_alarm_with_time_stamp[0] = alarm[0];
				temp_alarm_set_changed = false;
    }		
		 
	 
    if((alarm[0]!= 0x00)&&(p_temps->climate_temperature_alarm_set == 0x01))  	/*check whether the alarm is tripped and alarm set characteristics is set to ON*/
    {	
        // Send value if connected and notifying
        
        if ((p_temps->conn_handle != BLE_CONN_HANDLE_INVALID) && p_temps->is_notification_supported)
        {		
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            
            hvx_params.handle   = p_temps->climate_temp_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = alarm;

            err_code = sd_ble_gatts_hvx(p_temps->conn_handle, &hvx_params);
						p_temps->temps_alarm_with_time_stamp[0] = alarm[0];
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
        
    }
    return err_code;

}

/**@brief Function to read temperature from htu21d.
*
* @param[in]    void
* @param[out]   uint16_t current_temperature.
*/
uint16_t read_temperature()   
{
    uint16_t current_temperature = 0;

    twi_turn_ON();
    current_temperature = eDRV_HTU21_MeasureTemperature();
		twi_turn_OFF();

    return current_temperature;
}	

