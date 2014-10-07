/** @file
*
* @{
* @brief water level Service file.
*
* This file contains the source code for water level level alarm service
* Adapted from  Battery service provided by Nordic
* 
* Author : Sherin
* Date   : 11/19/2013
* Change log:
* Hariprasad        12/10/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics
* Sherin    		    12/10/2013     Added write events for value fields
* Hariprasad        12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics
* sruthiraj         01/10/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
*/

#include "ble_waterl_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"

bool            WATERLS_CONNECTED_STATE=false;      /*This flag indicates whether a client is connected to the peripheral in water level service*/
extern bool 	  BROADCAST_MODE;
extern bool     CHECK_ALARM_TIMEOUT;                /*Flag to indicate whether to check for alarm conditions defined in connect.c*/
extern uint8_t	 m_service;
/**@brief Function for handling the Connect event.
*
* @param[in]   p_waterls      water level Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_waterls_t * p_waterls, ble_evt_t * p_ble_evt)
{
    p_waterls->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    WATERLS_CONNECTED_STATE = true;      /*Set the flag to true so that state remains in connectable mode until disconnect*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_waterls     water level Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_waterls_t * p_waterls, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    WATERLS_CONNECTED_STATE = false;     /*Indicate that water level service is disconnected*/
    p_waterls->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of water level low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    CHECK_ALARM_TIMEOUT = true; 
}
/**@brief Function for handling the Write event.
*
* @param[in]   p_waterls      water level Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_waterls_t * p_waterls, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_waterls->is_notification_supported)
    {

        //write event for water level low value char.   
        if (
                (p_evt_write->handle == p_waterls->waterl_level_low_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterls->evt_handler != NULL)
            {
                ble_waterls_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_DISABLED;
                }

                p_waterls->evt_handler(p_waterls, &evt);
            }
        }

        //write event for water level level high value char. 

        if (
                (p_evt_write->handle == p_waterls->waterl_level_high_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterls->evt_handler != NULL)
            {
                ble_waterls_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_DISABLED;
                }

                p_waterls->evt_handler(p_waterls, &evt);
            }
        }

        //write event for water level alarm set

        if (
                (p_evt_write->handle == p_waterls->waterl_level_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterls->evt_handler != NULL)
            {
                ble_waterls_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_DISABLED;
                }

                p_waterls->evt_handler(p_waterls, &evt);
            }
        } 

        //write event for water level alarm

        if (
                (p_evt_write->handle == p_waterls->waterl_level_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterls->evt_handler != NULL)
            {
                ble_waterls_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_DISABLED;
                }

                p_waterls->evt_handler(p_waterls, &evt);
            }
        } 

        //write event for current water level level

        if (
                (p_evt_write->handle == p_waterls->curr_waterl_level_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterls->evt_handler != NULL)
            {
                ble_waterls_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERLS_EVT_NOTIFICATION_DISABLED;
                }

                p_waterls->evt_handler(p_waterls, &evt);
            }
        }
    }

    // write event for water level low value char value. 
    if (
            (p_evt_write->handle == p_waterls->waterl_level_low_level_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_waterls->waterl_level_low_level =   p_evt_write->data[0];

        // call application event handler
        p_waterls->write_evt_handler();
    }

    // write event for water level high value char value. 
    if (
            (p_evt_write->handle == p_waterls->waterl_level_high_level_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_waterls->waterl_level_high_level =   p_evt_write->data[0];

        // call application event handler
        p_waterls->write_evt_handler();
    }

    // write event for alarm set value char value. 
    if (
            (p_evt_write->handle == p_waterls->waterl_level_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_waterls->waterl_level_alarm_set =   p_evt_write->data[0];

        // call application event handler
        p_waterls->write_evt_handler();
    }

}


void ble_waterls_on_ble_evt(ble_waterls_t * p_waterls, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_waterls, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_waterls, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_waterls, p_ble_evt);
        break;

    default:
        break;
    }

}

/**@brief Function for adding the current water level level characteristics.
*
* @param[in]   p_waterls        water level Service structure.
* @param[in]   p_waterls_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_waterl_level_level_char_add(ble_waterls_t * p_waterls, const ble_waterls_init_t * p_waterls_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_waterl_level;

    if (p_waterls->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_waterls_init->waterl_level_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_waterls->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_waterls->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;

    ble_uuid.type = p_waterls->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERLS_CURR_WATERL_CHAR_UUID;		

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_waterls_init->waterl_level_char_attr_md.read_perm;
    attr_md.write_perm = p_waterls_init->waterl_level_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_waterl_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_waterl_level);
    attr_char_value.p_value      = &current_waterl_level;

    err_code = sd_ble_gatts_characteristic_add(p_waterls->service_handle, &char_md,
    &attr_char_value,
    &p_waterls->curr_waterl_level_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the water level low value characteristics.
*
* @param[in]   p_waterls        water level Service structure.
* @param[in]   p_waterls_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t waterl_level_low_value_char_add(ble_waterls_t * p_waterls, const ble_waterls_init_t * p_waterls_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      waterl_level_low_value;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;                   
		char_md.char_props.write    = 1;                                 /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp= 1;                             /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    ble_uuid.type = p_waterls->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERLS_LOW_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_waterls_init->waterl_level_char_attr_md.read_perm;
    attr_md.write_perm = p_waterls_init->waterl_level_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    waterl_level_low_value = p_waterls_init->waterl_level_low_value;


    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(waterl_level_low_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(waterl_level_low_value);
    attr_char_value.p_value      = &waterl_level_low_value;

    err_code = sd_ble_gatts_characteristic_add(p_waterls->service_handle,&char_md,
    &attr_char_value,
    &p_waterls->waterl_level_low_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the water level high value characteristics.
*
* @param[in]   p_waterls        water level Service structure.
* @param[in]   p_waterls_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t waterl_level_high_value_char_add(ble_waterls_t * p_waterls, const ble_waterls_init_t * p_waterls_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      waterl_level_high_value;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                                    /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                               /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    ble_uuid.type = p_waterls->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERLS_HIGH_CHAR_UUID;		

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_waterls_init->waterl_level_char_attr_md.read_perm;
    attr_md.write_perm = p_waterls_init->waterl_level_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    waterl_level_high_value = p_waterls_init->waterl_level_high_value;  /*Default values for water level high value*/

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(waterl_level_high_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(waterl_level_high_value);
    attr_char_value.p_value      = &waterl_level_high_value;

    err_code = sd_ble_gatts_characteristic_add(p_waterls->service_handle, &char_md,
    &attr_char_value,
    &p_waterls->waterl_level_high_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the water level alarm set characteristics.
*
* @param[in]   p_waterls        water level Service structure.
* @param[in]   p_waterls_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t waterl_level_alarm_set_char_add(ble_waterls_t * p_waterls, const ble_waterls_init_t * p_waterls_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      waterl_level_alarm_set;	

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write    = 1;                                   /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp    = 1;                           /*add fix for characteristic write issue*/
    char_md.char_props.notify = 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_waterls->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERLS_ALARM_SET_CHAR_UUID;			

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_waterls_init->waterl_level_char_attr_md.read_perm;
    attr_md.write_perm = p_waterls_init->waterl_level_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    waterl_level_alarm_set = p_waterls_init->waterl_level_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &waterl_level_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_waterls->service_handle, &char_md,
    &attr_char_value,
    &p_waterls->waterl_level_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the water level level alarm characteristics.
*
* @param[in]   p_waterls       water level Service structure.
* @param[in]   p_waterls_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t waterl_level_alarm_char_add(ble_waterls_t * p_waterls, const ble_waterls_init_t * p_waterls_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      waterl_level_alarm; 

    waterl_level_alarm = RESET_ALARM;                               /*Initialize alarm to 0x00*/

    // Add water level high level characteristic 
    if (p_waterls->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_waterls_init->waterl_level_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = (p_waterls->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_waterls->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_waterls->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERLS_ALARM_CHAR_UUID;	

    memset(&attr_md, 0, sizeof(attr_md));


    ble_uuid.type = p_waterls->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERLS_ALARM_CHAR_UUID;	

    attr_md.read_perm  = p_waterls_init->waterl_level_char_attr_md.read_perm;
    attr_md.write_perm = p_waterls_init->waterl_level_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;


    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &waterl_level_alarm;

    err_code = sd_ble_gatts_characteristic_add(p_waterls->service_handle, &char_md,
    &attr_char_value,
    &p_waterls->waterl_level_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the water level service.
*
* @param[in]   p_waterls        water level Service structure.
* @param[in]   p_waterls_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_waterls_init(ble_waterls_t * p_waterls, const ble_waterls_init_t * p_waterls_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
    
    // Add service
    ble_uuid.type = m_service;
		p_waterls->uuid_type=m_service;
    ble_uuid.uuid = WATER_PROFILE_WATERLS_SERVICE_UUID;

    // Initialize service structure
    p_waterls->evt_handler               = p_waterls_init->evt_handler;
    p_waterls->write_evt_handler         = write_evt_handler;
    p_waterls->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_waterls->is_notification_supported = p_waterls_init->support_notification;
    p_waterls->waterl_level_low_level    = p_waterls_init->waterl_level_low_value  ;
    p_waterls->waterl_level_high_level   = p_waterls_init->waterl_level_high_value ;
    p_waterls->waterl_level_alarm_set    = p_waterls_init->waterl_level_alarm      ;
    p_waterls->waterl_level_alarm  	     = p_waterls_init->waterl_level_alarm_set  ;


    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_waterls->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  current_waterl_level_level_char_add(p_waterls, p_waterls_init); /* Add current water level level characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  waterl_level_low_value_char_add(p_waterls, p_waterls_init);     /* Add water level level low value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  waterl_level_high_value_char_add(p_waterls, p_waterls_init);    /* Add water level level high value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  waterl_level_alarm_set_char_add(p_waterls, p_waterls_init);     /* Add water level level alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  waterl_level_alarm_char_add(p_waterls, p_waterls_init);         /* Add water level level alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function reads and updates the current water level level and checks for alarm condition.
*
* @param[in]   p_waterls        water level  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_waterls_level_alarm_check(ble_waterls_t * p_waterls)
{
    uint32_t err_code = NRF_SUCCESS;

    uint8_t  current_waterl_level;

    static uint16_t previous_waterl_level_level = 0x00;
    uint8_t alarm = 0x00;

    uint16_t len = sizeof(uint8_t);

    current_waterl_level = read_waterl_level(); 			 /* read the current water level level*/

    if(current_waterl_level != previous_waterl_level_level)  /*Check whether water level value has changed*/
    {
       
        // Send value if connected and notifying

        if ((p_waterls->conn_handle != BLE_CONN_HANDLE_INVALID) && p_waterls->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            len = sizeof(uint8_t);

            hvx_params.handle   = p_waterls->curr_waterl_level_level_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = &current_waterl_level;

            err_code = sd_ble_gatts_hvx(p_waterls->conn_handle, &hvx_params);
						previous_waterl_level_level = current_waterl_level;
				}
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }		



    /*Check whether the water level is out of range if alarm is set by user */		
    if(p_waterls->waterl_level_alarm_set != 0x00)
    {

        if(current_waterl_level < p_waterls->waterl_level_low_level)
        {
            alarm = SET_ALARM_WATERL_LOW;		/*set alarm to 01 if water level level is low */
        }

        else if(current_waterl_level > p_waterls->waterl_level_high_level)
        {
            alarm = SET_ALARM_HIGH;
        } 

        else
        {	
            alarm = RESET_ALARM;		       /*reset alarm to 0x00*/
        }	
    }
    else
    {	
        alarm = RESET_ALARM;							 /*reset alarm to 0x00*/
    }		


    if(alarm != p_waterls->waterl_level_alarm )  /*check whether the alarm value has changed and send the change*/
    {	
       // Send value if connected and notifying


        if ((p_waterls->conn_handle != BLE_CONN_HANDLE_INVALID) && p_waterls->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            len = sizeof(uint8_t);

            hvx_params.handle   = p_waterls->waterl_level_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = &alarm;

            err_code = sd_ble_gatts_hvx(p_waterls->conn_handle, &hvx_params);
						p_waterls->waterl_level_alarm = alarm;

        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }

    return err_code;

}

/**@brief Function to read water level level from sensor interfaced to ADC.
*
* @param[in]   void
* @param[out]  uint16_t current_waterl_level_level.
*/
uint8_t read_waterl_level()   
{
    static uint8_t current_waterl_level = 0x00; 

    // Initialize ADC for interfacing the water level sensor
    adc_init();		

    // Read water level level from ADC 
    current_waterl_level = do_waterl_adc_measurement(); 

    return current_waterl_level;
}	

