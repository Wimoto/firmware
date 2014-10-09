/** @file
*
* @{
* @ingroup ble_sdk_app_hts
* @brief Light Service file.
*
* This file contains the source code for Light level alarm service
* This application uses the @ref srvlib_conn_params module.
* Adapted from  Battery service provided by Nordic
* 
* Author : Shafy
* Date   : 10/22/2013
* Change log:
* Sherin    	    	12/10/2013     Added write events for value fields
* Hariprasad        12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* sruthi.k.s        01/10/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
*/

#include "ble_light_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto_sensors.h"
#include "wimoto.h"

bool   LIGHTS_CONNECTED_STATE=false;          /*This flag indicates whether a client is connected to the peripheral or not*/
extern bool 	  BROADCAST_MODE;               /*Flag used to switch between broadcast and connectable modes defined in main.c*/
extern bool     CHECK_ALARM_TIMEOUT;          /*Flag to indicate whether to check for alarm conditions defined in connect.c*/
extern uint8_t	 m_service;

/**@brief Function for handling the Connect event.
*
* @param[in]   p_lights    Light Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_lights_t * p_lights, ble_evt_t * p_ble_evt)
{
    p_lights->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    LIGHTS_CONNECTED_STATE = true;  /*Set the flag to true so that state remains in connectable mode until disconnect*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_lights    Light Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_lights_t * p_lights, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    LIGHTS_CONNECTED_STATE= false; 
    p_lights->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of Light low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    CHECK_ALARM_TIMEOUT = true; 
}

/**@brief Function for handling the Write event.
*
* @param[in]   p_lights    Light Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_lights_t * p_lights, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_lights->is_notification_supported)
    {
        //write event for light_level low value char cccd.   
        if (
                (p_evt_write->handle == p_lights->light_low_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_lights->evt_handler != NULL)
            {
                ble_lights_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_lights->evt_handler(p_lights, &evt);
            }
        }

        //write event for light_level high value char cccd. 

        if (
                (p_evt_write->handle == p_lights->light_high_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_lights->evt_handler != NULL)
            {
                ble_lights_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_lights->evt_handler(p_lights, &evt);
            }
        }

        //write event for light_level alarm set cccd.

        if (
                (p_evt_write->handle == p_lights->climate_light_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_lights->evt_handler != NULL)
            {
                ble_lights_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_lights->evt_handler(p_lights, &evt);
            }
        } 

        //write event for light_level alarm cccd.
        
        if (
                (p_evt_write->handle == p_lights->climate_light_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_lights->evt_handler != NULL)
            {
                ble_lights_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_lights->evt_handler(p_lights, &evt);
            }
        } 

        //write event for current light level cccd.

        if (
                (p_evt_write->handle == p_lights->current_light_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_lights->evt_handler != NULL)
            {
                ble_lights_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_LIGHTS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_lights->evt_handler(p_lights, &evt);
            }
        }
    }

    // write event for light low value char value. 
    if (
            (p_evt_write->handle == p_lights->light_low_level_handles.value_handle) 
            && 
            (p_evt_write->len == 2)
            )
    {
        // update the light service stucture
        p_lights->climate_light_low_level[0] =   p_evt_write->data[0];
        p_lights->climate_light_low_level[1] =   p_evt_write->data[1];

        // call application event handler
        p_lights->write_evt_handler();
    }

    // write event for light high value char value. 
    if (
            (p_evt_write->handle == p_lights->light_high_level_handles.value_handle) 
            && 
            (p_evt_write->len == 2)
            )
    {
        // update the light service structure
        p_lights->climate_light_high_level[0] =   p_evt_write->data[0];
        p_lights->climate_light_high_level[1] =   p_evt_write->data[1];

        // call application event handler
        p_lights->write_evt_handler();
    }

    // write event for alarm set value char value. 
    if (
            (p_evt_write->handle == p_lights->climate_light_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the light service structure
        p_lights->climate_light_alarm_set =   p_evt_write->data[0];

        // call application event handler
        p_lights->write_evt_handler();
    }

}


void ble_lights_on_ble_evt(ble_lights_t * p_lights, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_lights, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_lights, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_lights, p_ble_evt);
        break;

    default:
        break;
    }

}


/**@brief Function for adding the current light_level characteristics.
*
* @param[in]   p_lights        Light Service structure.
* @param[in]   p_lights_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_light_level_char_add(ble_lights_t * p_lights, const ble_lights_init_t * p_lights_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_light_level[2] = {0x00,0x00};

    if (p_lights->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_lights_init->lights_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_lights->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_lights->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;


    ble_uuid.type = p_lights->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_LIGHTS_CURR_LIGHT_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_lights_init->lights_char_attr_md.read_perm;
    attr_md.write_perm = p_lights_init->lights_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_light_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_light_level);
    attr_char_value.p_value      = current_light_level;

    err_code = sd_ble_gatts_characteristic_add(p_lights->service_handle, &char_md,
    &attr_char_value,
    &p_lights->current_light_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the light_level low value characteristics.
*
* @param[in]   p_lights        Light Service structure.
* @param[in]   p_lights_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t light_low_value_char_add(ble_lights_t * p_lights, const ble_lights_init_t * p_lights_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_light_low_value[2];

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read  					= 1;                   
		char_md.char_props.write					=	1;                        /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                        /*add fix for characteristic write issue*/
    char_md.p_char_user_desc 					=	NULL;
    char_md.p_char_user_desc 					=	NULL;
    char_md.p_char_pf        					= NULL;
    char_md.p_user_desc_md   					= NULL;
    char_md.p_cccd_md        					= NULL;
    char_md.p_sccd_md        					= NULL;
		


    ble_uuid.type = p_lights->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_LIGHTS_LIGHT_LOW_CHAR_UUID; 

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_lights_init->lights_char_attr_md.read_perm;
    attr_md.write_perm = p_lights_init->lights_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_light_low_value[1] = p_lights_init->climate_light_low_value[1];
    climate_light_low_value[0] = p_lights_init->climate_light_low_value[0];		
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(climate_light_low_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(climate_light_low_value);
    attr_char_value.p_value      = climate_light_low_value;

    err_code = sd_ble_gatts_characteristic_add(p_lights->service_handle, &char_md,
    &attr_char_value,
    &p_lights->light_low_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the light_level high value characteristics.
*
* @param[in]   p_lights        Light Service structure.
* @param[in]   p_lights_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t light_high_value_char_add(ble_lights_t * p_lights, const ble_lights_init_t * p_lights_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_light_high_level[2];

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read  					= 1;
		char_md.char_props.write					=	1;                         /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                         /*add fix for characteristic write issue*/
    char_md.p_char_user_desc 					= NULL;
    char_md.p_char_user_desc 					= NULL;
    char_md.p_char_pf        					= NULL;
    char_md.p_user_desc_md   					= NULL;
    char_md.p_cccd_md        					= NULL;
    char_md.p_sccd_md        					= NULL;

    ble_uuid.type = p_lights->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_LIGHTS_LIGHT_HIGH_CHAR_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_lights_init->lights_char_attr_md.read_perm;
    attr_md.write_perm = p_lights_init->lights_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_light_high_level[1] = p_lights_init->climate_light_high_value[1];  /*Default values for light_level high value*/
    climate_light_high_level[0] = p_lights_init->climate_light_high_value[0];	

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(climate_light_high_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(climate_light_high_level);
    attr_char_value.p_value      = climate_light_high_level;

    err_code = sd_ble_gatts_characteristic_add(p_lights->service_handle, &char_md,
    &attr_char_value,
    &p_lights->light_high_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the light_level alarm set characteristics.
*
* @param[in]   p_lights        Light Service structure.
* @param[in]   p_lights_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t light_alarm_set_char_add(ble_lights_t * p_lights, const ble_lights_init_t * p_lights_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_light_alarm_set;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   				= 1;
		char_md.char_props.write					=	1;                        /* add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                        /* add fix for characteristic write issue*/
    char_md.p_char_user_desc  				=	NULL;
    char_md.p_char_user_desc  				=	NULL;
    char_md.p_char_pf         				= NULL;
    char_md.p_user_desc_md    				= NULL;
    char_md.p_cccd_md         				= NULL;
    char_md.p_sccd_md         				= NULL;

    ble_uuid.type = p_lights->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_LIGHTS_LIGHT_ALARM_SET_CHAR_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_lights_init->lights_char_attr_md.read_perm;
    attr_md.write_perm = p_lights_init->lights_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_light_alarm_set = p_lights_init->climate_light_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &climate_light_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_lights->service_handle, &char_md,
    &attr_char_value,
    &p_lights->climate_light_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the light_level alarm characteristics.
*
* @param[in]   p_lights        Light Service structure.
* @param[in]   p_lights_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t light_alarm_char_add(ble_lights_t * p_lights, const ble_lights_init_t * p_lights_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_light_alarm;

    // Add light_level high level characteristic 
    if (p_lights->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        // According to BAS_SPEC_V10, the read operation on cccd should be possible without
        // authentication.
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_lights_init->lights_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = (p_lights->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_lights->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;


    ble_uuid.type = p_lights->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_LIGHTS_LIGHT_ALARM_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_lights_init->lights_char_attr_md.read_perm;
    attr_md.write_perm = p_lights_init->lights_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;


    climate_light_alarm = p_lights_init->climate_light_alarm;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &climate_light_alarm;

    err_code = sd_ble_gatts_characteristic_add(p_lights->service_handle, &char_md,
    &attr_char_value,
    &p_lights->climate_light_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for initializing the light_level.
*
* @param[in]   p_lights        Light Service structure.
* @param[in]   p_lights_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_lights_init(ble_lights_t * p_lights, const ble_lights_init_t * p_lights_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Add service
    ble_uuid.type = m_service;
	  p_lights->uuid_type = m_service;
    ble_uuid.uuid = CLIMATE_PROFILE_LIGHTS_SERVICE_UUID;

    // Initialize service structure
    p_lights->evt_handler                 = p_lights_init->evt_handler;
    p_lights->write_evt_handler           = write_evt_handler;
    p_lights->conn_handle                 = BLE_CONN_HANDLE_INVALID;
    p_lights->is_notification_supported   = p_lights_init->support_notification;
    p_lights->climate_light_low_level[0]  = p_lights_init->climate_light_low_value[0] ; 
    p_lights->climate_light_low_level[1]  = p_lights_init->climate_light_low_value[1] ; 
    p_lights->climate_light_high_level[0] = p_lights_init->climate_light_high_value[0]; 
    p_lights->climate_light_high_level[1] = p_lights_init->climate_light_high_value[1]; 
    p_lights->climate_light_alarm_set     = p_lights_init->climate_light_alarm_set;    
    p_lights->climate_light_alarm         = p_lights_init->climate_light_alarm;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_lights->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  current_light_level_char_add(p_lights, p_lights_init); /* Add current light_level characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  light_low_value_char_add(p_lights, p_lights_init);  /* Add light_level low value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  light_high_value_char_add(p_lights, p_lights_init); /* Add light_level high value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  light_alarm_set_char_add(p_lights, p_lights_init);  /* Add light_level alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  light_alarm_char_add(p_lights, p_lights_init);     /* Add light_level alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}

/**@brief Function reads and updates the current light_level and checks for alarm condition.
*
* @param[in]   p_lights        Light Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_lights_level_alarm_check(ble_lights_t * p_lights)
{
    uint32_t err_code = NRF_SUCCESS;
    uint16_t current_light_level;
    uint8_t  current_light_level_array[2];

    uint16_t light_level_low_value;					   /*light low value set by user as uint16*/
    uint16_t light_level_high_value;				   /*light low value set by user as uint16*/

    static uint16_t   previous_light_level = 0x00;
    uint8_t alarm = 0x00;

    uint16_t	len  = sizeof(uint8_t);
    uint16_t  len1 = sizeof(current_light_level_array);

    current_light_level = read_light_level(); /* read the current light_level*/


    if(current_light_level != previous_light_level)  /*Check whether light_level value has changed*/
    {
        current_light_level_array[1] = current_light_level & LOWER_BYTE_MASK;   /*Convert the light_level to uint8_t array*/
        current_light_level_array[0]=  current_light_level >>8;

        // Send value if connected and notifying

        if ((p_lights->conn_handle != BLE_CONN_HANDLE_INVALID) && p_lights->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;
 
            memset(&hvx_params, 0, sizeof(hvx_params));
            len = sizeof(uint8_t);

            hvx_params.handle   = p_lights->current_light_level_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len1;
            hvx_params.p_data   = current_light_level_array;

            err_code = sd_ble_gatts_hvx(p_lights->conn_handle, &hvx_params);
						previous_light_level = current_light_level;
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }

    }		

    /*Get the light_level low value set by the user from the service */

    light_level_low_value 						= 	(p_lights->climate_light_low_level[0])<<8;	               /*convert the 8 bit arrays to a 16 bit data*/
    light_level_low_value 						= 	light_level_low_value | (p_lights->climate_light_low_level[1]);
    
    /*Get the light_level high value set by the user from the service */

    light_level_high_value            = 	(p_lights->climate_light_high_level[0])<<8;		/*convert the 8 bit arrays to a 16 bit data*/
    light_level_high_value 			 			= 	light_level_high_value | (p_lights->climate_light_high_level[1]);
    
    /*Check whether the light_level is out of range if alarm is set by user */		
    if(p_lights->climate_light_alarm_set != 0x00)
    {
        if(current_light_level < light_level_low_value)
        {
            alarm = SET_ALARM_LOW;		         /*set alarm to 01 if light_level is low */
        }
        
        else if(current_light_level > light_level_high_value) 
        {
            alarm = SET_ALARM_HIGH;            /*set alarm to 02 if light_level is high */
        } 
        
        else
        {	
            alarm = RESET_ALARM;		           /*reset alarm to 0x00*/
        }	

    }
    else
    {	
        alarm = RESET_ALARM;								    /*reset alarm to 0x00*/
    }		


    if(alarm != p_lights->climate_light_alarm)  /*check whether the alarm value has changed and send the change*/
    {	
        // Send value if connected and notifying
        if ((p_lights->conn_handle != BLE_CONN_HANDLE_INVALID) && p_lights->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            len = sizeof(uint8_t);

            hvx_params.handle   = p_lights->climate_light_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = &alarm;

            err_code = sd_ble_gatts_hvx(p_lights->conn_handle, &hvx_params);
						p_lights->climate_light_alarm = alarm;
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }

    }

    return err_code;

}



/**@brief Function to read light_level from isl29023.
*
* @param[in]    void
* @param[out]   uint16_t current_light_level.
*/
uint16_t read_light_level()   
{
    static uint16_t current_light_level=0x0000;

    twi_turn_ON();
    current_light_level = ISL29023_get_one_time_ALS();
		simple_uart_putstring("ISL light measurement done\t");
    twi_turn_OFF();

    return current_light_level;
}	


