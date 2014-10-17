/** @file
*
* @{
* @brief Soil Moisture Service file.
*
* This file contains the source code for Soil Moisture level alarm service
* Adapted from  Battery service provided by Nordic
* 
* Author : Shafy
* Date   : 11/06/2013
* Change log:
* Sherin    		    12/10/2013     Added write events for value fields
* Hariprasad        12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* sruthi.k.s        01/10/2014     migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s				10/17/2014		 Added time stamp with soil moisture  alarm characteristics
*/

#include "ble_soil_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"
#include "app_error.h"

bool SOILS_CONNECTED_STATE=false;             /*This flag indicates whether a client is connected to the peripheral in soil moisture service*/
extern bool 	  CHECK_ALARM_TIMEOUT;
extern uint8_t	 var_receive_uuid;								/*variable to receive uuid*/
extern uint8_t  curr_soil_mois_level;            /*variable to store current Humidity value from htu21d to broadcast*/


/**@brief Function for handling the Connect event.
*
* @param[in]   p_soils      soil moisture Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_soils_t * p_soils, ble_evt_t * p_ble_evt)
{
    p_soils->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    SOILS_CONNECTED_STATE = true;      /*Set the flag to true so that state remains in connectable mode until disconnect*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_soils     Soil moisture Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_soils_t * p_soils, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    SOILS_CONNECTED_STATE = false;     /*Indicate that soil moisture service is disconnected*/
    p_soils->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of soil low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    CHECK_ALARM_TIMEOUT = true; 
}


/**@brief Function for handling the Write event.
*
* @param[in]   p_soils      Soil moisture Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_soils_t * p_soils, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_soils->is_notification_supported)
    {

        //write event for soil moisture low value char cccd.   
        if (
                (p_evt_write->handle == p_soils->soil_mois_low_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_soils->evt_handler != NULL)
            {
                ble_soils_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_DISABLED;
                }

                p_soils->evt_handler(p_soils, &evt);
            }
        }

        //write event for soil moisture level high value char cccd. 

        if (
                (p_evt_write->handle == p_soils->soil_mois_high_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_soils->evt_handler != NULL)
            {
                ble_soils_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_DISABLED;
                }

                p_soils->evt_handler(p_soils, &evt);
            }
        }

        //write event for soil moisture alarm set cccd

        if (
                (p_evt_write->handle == p_soils->soil_mois_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_soils->evt_handler != NULL)
            {
                ble_soils_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_DISABLED;
                }

                p_soils->evt_handler(p_soils, &evt);
            }
        } 

        //write event for soil moisture alarm cccd

        if (
                (p_evt_write->handle == p_soils->soil_mois_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_soils->evt_handler != NULL)
            {
                ble_soils_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_DISABLED;
                }

                p_soils->evt_handler(p_soils, &evt);
            }
        } 

        //write event for current soil moisture level cccd

        if (
                (p_evt_write->handle == p_soils->curr_soil_mois_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_soils->evt_handler != NULL)
            {
                ble_soils_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_SOILS_EVT_NOTIFICATION_DISABLED;
                }

                p_soils->evt_handler(p_soils, &evt);
            }
        }
    }
    // write event for soil moisture low value char value. 
    if (
            (p_evt_write->handle == p_soils->soil_mois_low_level_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the soil moisture service structure
        p_soils->soil_mois_low_level =   p_evt_write->data[0];

        // call application event handler
        p_soils->write_evt_handler();
    }		

    // write event for soil moisture high value char value. 
    if (
            (p_evt_write->handle == p_soils->soil_mois_high_level_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the soil moisture service structure
        p_soils->soil_mois_high_level =   p_evt_write->data[0];

        // call application event handler
        p_soils->write_evt_handler();
    }

    // write event for alarm set value char value. 
    if (
            (p_evt_write->handle == p_soils->soil_mois_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the soil moisture service structure
        p_soils->soil_mois_alarm_set =   p_evt_write->data[0];

        // call application event handler
        p_soils->write_evt_handler();
    }		
}


void ble_soils_on_ble_evt(ble_soils_t * p_soils, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_soils, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_soils, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_soils, p_ble_evt);
        break;

    default:
        break;
    }

}


/**@brief Function for adding the current soil moisture level characteristics.
*
* @param[in]   p_soils        Soil moisture Service structure.
* @param[in]   p_soils_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_soil_mois_level_char_add(ble_soils_t * p_soils, const ble_soils_init_t * p_soils_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_soil_mois_level;

    if (p_soils->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_soils_init->soil_mois_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_soils->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_soils->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;

    // Add custom UUID to the characteristic 
    ble_uuid.type = p_soils->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_SOILS_CURR_SOIL_CHAR_UUID; 

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_soils_init->soil_mois_char_attr_md.read_perm;
    attr_md.write_perm = p_soils_init->soil_mois_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_soil_mois_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_soil_mois_level);
    attr_char_value.p_value      = &current_soil_mois_level;

    err_code = sd_ble_gatts_characteristic_add(p_soils->service_handle, &char_md,
    &attr_char_value,
    &p_soils->curr_soil_mois_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the soil moisture low value characteristics.
*
* @param[in]   p_soils        Soil moisture Service structure.
* @param[in]   p_soils_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t soil_mois_low_value_char_add(ble_soils_t * p_soils, const ble_soils_init_t * p_soils_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      soil_mois_low_value;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                            /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                       /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Add custom UUID to the characteristic 
    ble_uuid.type = p_soils->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_SOILS_SOIL_LOW_CHAR_UUID;   

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_soils_init->soil_mois_char_attr_md.read_perm;
    attr_md.write_perm = p_soils_init->soil_mois_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    soil_mois_low_value = p_soils_init->soil_mois_low_value;


    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(soil_mois_low_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(soil_mois_low_value);
    attr_char_value.p_value      = &soil_mois_low_value;

    err_code = sd_ble_gatts_characteristic_add(p_soils->service_handle,&char_md,
    &attr_char_value,
    &p_soils->soil_mois_low_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the soil moisture high value characteristics.
*
* @param[in]   p_soils        Soil moisture Service structure.
* @param[in]   p_soils_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t soil_mois_high_value_char_add(ble_soils_t * p_soils, const ble_soils_init_t * p_soils_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      soil_mois_high_value;		

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;           /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Add custom UUID to the characteristic 
    ble_uuid.type = p_soils->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_SOILS_SOIL_HIGH_CHAR_UUID;   

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_soils_init->soil_mois_char_attr_md.read_perm;
    attr_md.write_perm = p_soils_init->soil_mois_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    soil_mois_high_value = p_soils_init->soil_mois_high_value;  /* Default values for soil moisture high value*/

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(soil_mois_high_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(soil_mois_high_value);
    attr_char_value.p_value      = &soil_mois_high_value;

    err_code = sd_ble_gatts_characteristic_add(p_soils->service_handle, &char_md,
    &attr_char_value,
    &p_soils->soil_mois_high_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the soil moisture alarm set characteristics.
*
* @param[in]   p_soils        soil moisture Service structure.
* @param[in]   p_soils_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t soil_mois_alarm_set_char_add(ble_soils_t * p_soils, const ble_soils_init_t * p_soils_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      soil_mois_alarm_set;	

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                      /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                 /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Add custom UUID to the characteristic 
    ble_uuid.type = p_soils->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_SOILS_SOIL_ALARM_SET_CHAR_UUID;  

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_soils_init->soil_mois_char_attr_md.read_perm;
    attr_md.write_perm = p_soils_init->soil_mois_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    soil_mois_alarm_set = p_soils_init->soil_mois_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &soil_mois_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_soils->service_handle, &char_md,
    &attr_char_value,
    &p_soils->soil_mois_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the soil moisture level alarm characteristics.
*
* @param[in]   p_soils       Soil moisture Service structure.
* @param[in]   p_soils_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t soil_mois_alarm_char_add(ble_soils_t * p_soils, const ble_soils_init_t * p_soils_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
	
		/* array for receiving alarm with time stamp from soil moisture service structure*/
    static uint8_t      soil_alarm_with_time_stamp[8]; 

    // Add soil moisture high level characteristic 
    if (p_soils->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_soils_init->soil_mois_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = (p_soils->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_soils->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    // Add custom UUID to the characteristic 
    ble_uuid.type = p_soils->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_SOILS_SOIL_ALARM_CHAR_UUID;  

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_soils_init->soil_mois_char_attr_md.read_perm;
    attr_md.write_perm = p_soils_init->soil_mois_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

		//assigning alarm with time stamp of soil moisture characteristics to alarm with time stamp array
    soil_alarm_with_time_stamp[0] = p_soils_init->soil_alarm_with_time_stamp[0];
		soil_alarm_with_time_stamp[1] = p_soils_init->soil_alarm_with_time_stamp[1];
		soil_alarm_with_time_stamp[2] = p_soils_init->soil_alarm_with_time_stamp[2];
		soil_alarm_with_time_stamp[3] = p_soils_init->soil_alarm_with_time_stamp[3];
		soil_alarm_with_time_stamp[4] = p_soils_init->soil_alarm_with_time_stamp[4];
		soil_alarm_with_time_stamp[5] = p_soils_init->soil_alarm_with_time_stamp[5];
		soil_alarm_with_time_stamp[6] = p_soils_init->soil_alarm_with_time_stamp[6];
		soil_alarm_with_time_stamp[7] = p_soils_init->soil_alarm_with_time_stamp[7];
		
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(soil_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(soil_alarm_with_time_stamp);
    attr_char_value.p_value      = soil_alarm_with_time_stamp;

    err_code = sd_ble_gatts_characteristic_add(p_soils->service_handle, &char_md,
    &attr_char_value,
    &p_soils->soil_mois_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the soil moisture service.
*
* @param[in]   p_soils        Soil moisture Service structure.
* @param[in]   p_soils_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_soils_init(ble_soils_t * p_soils, const ble_soils_init_t * p_soils_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Add service
    ble_uuid.type =var_receive_uuid;
		p_soils->uuid_type=var_receive_uuid;
    ble_uuid.uuid = GROW_PROFILE_SOILS_SERVICE_UUID;

    // Initialize service structure
    p_soils->evt_handler               = p_soils_init->evt_handler;
    p_soils->write_evt_handler         = write_evt_handler;
    p_soils->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_soils->is_notification_supported = p_soils_init->support_notification;

    p_soils->soil_mois_low_level       = p_soils_init->soil_mois_low_value;
    p_soils->soil_mois_high_level      = p_soils_init->soil_mois_high_value;
    p_soils->soil_mois_alarm_set       = p_soils_init->soil_mois_alarm_set;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_soils->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  current_soil_mois_level_char_add(p_soils, p_soils_init); /* Add current soil moisture level characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  soil_mois_low_value_char_add(p_soils, p_soils_init);    /* Add soil moisture level low value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  soil_mois_high_value_char_add(p_soils, p_soils_init);   /* Add soil moisture level high value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  soil_mois_alarm_set_char_add(p_soils, p_soils_init);    /* Add soil moisture level alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  soil_mois_alarm_char_add(p_soils, p_soils_init);       /* Add soil moisture level alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;

}


/**@brief Function reads and updates the current soil moisture level and checks for alarm condition.
*
* @param[in]   p_soils        soil moisture  Service structure.
*
*	@param[in]   p_device        Device management service structure Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_soils_level_alarm_check(ble_soils_t * p_soils,ble_device_t *p_device)
{
    uint32_t err_code = NRF_SUCCESS;
    uint8_t current_soil_mois_level;

		bool     SOIL_ALARM_SET_TIME_READ=false; 									 /*This flag for soil moisture service alarm set time read*/
		bool     SOIL_ALARM_RESET_TIME_STAMP=false;								 /*This flag for soil moisture alarm reset read whether alarm set is 0x00 */
	
    static uint16_t previous_soil_mois_level = 0x00;
    uint8_t alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 


    uint16_t len1 =sizeof(current_soil_mois_level);	
		uint16_t len=8;	//length of alarm with time stamp characteristics
    current_soil_mois_level = read_soil_mois_level();         /* Read the current soil moisture level*/
		
		/*copy the current soil moisture level value for broadcast*/
		curr_soil_mois_level=current_soil_mois_level;

    if(current_soil_mois_level != previous_soil_mois_level)   /* Check whether soil moisture value has changed*/
    {
        // Send value if connected and notifying
        if ((p_soils->conn_handle != BLE_CONN_HANDLE_INVALID) && p_soils->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle   = p_soils->curr_soil_mois_level_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len1;
            hvx_params.p_data   = &current_soil_mois_level;

            err_code = sd_ble_gatts_hvx(p_soils->conn_handle, &hvx_params);
						previous_soil_mois_level = current_soil_mois_level;
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }		


    /*Check whether the soil moisture is out of range if alarm is set by user */		
    if(p_soils->soil_mois_alarm_set != 0x00)
    {

        if(current_soil_mois_level < p_soils->soil_mois_low_level)
        {
            alarm[0] = SET_ALARM_LOW;		      /* Set alarm to 01 if soil moisture level is low */
						SOIL_ALARM_SET_TIME_READ=true;
        }

        else if(current_soil_mois_level > p_soils->soil_mois_high_level)
        {
            alarm[0] = SET_ALARM_HIGH;		    /* Set alarm to 02 if soil moisture level is high */
						SOIL_ALARM_SET_TIME_READ=true;
        }                                   
        
        else                                
        {	                                  
            alarm[0] = RESET_ALARM;		        /* Reset alarm to 0x00*/
        }	                                  
    }                                       
    else                                    
    {	
        alarm[0] = RESET_ALARM;					      /* Reset alarm to 0x00*/
				SOIL_ALARM_RESET_TIME_STAMP=true;
    }		
		
		/*reading of time stamp from device management service structure whether the alarm set*/
		if(SOIL_ALARM_SET_TIME_READ)
		{
				alarm[1]=p_device->device_time_stamp_set[0];
				alarm[2]=p_device->device_time_stamp_set[1];
				alarm[3]=p_device->device_time_stamp_set[2];
				alarm[4]=p_device->device_time_stamp_set[3];
				alarm[5]=p_device->device_time_stamp_set[4];
				alarm[6]=p_device->device_time_stamp_set[5];
				alarm[7]=p_device->device_time_stamp_set[6];
				SOIL_ALARM_SET_TIME_READ=false;
		}
		
		/*resetting of alarm time to zero whether the alarm set characteristics set as zero*/
		if(SOIL_ALARM_RESET_TIME_STAMP)
		{		
				alarm[0]=0x00;
				alarm[1]=0x00;
				alarm[2]=0x00;
				alarm[3]=0x00;
				alarm[4]=0x00;
				alarm[5]=0x00;
				alarm[6]=0x00;
				alarm[7]=0x00;
				SOIL_ALARM_RESET_TIME_STAMP=false;
		}
    if((alarm[0]!= 0)||(p_soils->soil_mois_alarm_set == 0x00))  /*check whether the alarm sets as non zero or alarm set characteristics set as zero*/
    {	
				// Send value if connected and notifying
        if ((p_soils->conn_handle != BLE_CONN_HANDLE_INVALID) && p_soils->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle   = p_soils->soil_mois_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = alarm;

            err_code = sd_ble_gatts_hvx(p_soils->conn_handle, &hvx_params);
						p_soils->soil_alarm_with_time_stamp[0] = alarm[0];
						
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }
    return err_code;
}

/**@brief Function to read soil moisture level from sensor interfaced to ADC.
*
* @param[in]   void
* @param[out]  uint16_t current_soil_mois_level.
*/
uint8_t read_soil_mois_level()   
{
    static uint8_t current_soil_mois_level=0x00;

    current_soil_mois_level = do_soil_moisture_measurement();  /* Read soil moisture level from ADC */
		return current_soil_mois_level;
}	

