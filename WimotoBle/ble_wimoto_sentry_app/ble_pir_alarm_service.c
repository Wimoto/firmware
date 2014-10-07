/** @file
*
* @{
* @ingroup ble_sdk_app_hts
* @brief PIR Service file.
*
* This file contains the source code for PIR  alarm service
* It also includes the code for Device Information services.
* This application uses the @ref srvlib_conn_params module.
* Adapted from  Battery service provided by Nordic
* 
* Author : Hariprasad C R
* Date   : 11/29/2013
* Change log
* Sherin      12/10/2013     Added write event to value fields
* sruthi.k.s     01/10/2014       Migrated to soft device 7.0.0 and SDK 6.1.0
*/

#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"
#include "ble_pir_alarm_service.h"

extern bool        PIR_EVENT_FLAG;               /* Flag to indicate the event occurred on GPIO pin configured for PIR sensor*/
bool     	         PIR_CONNECTED_STATE = false;  /* Indicates whether the PIR service is connected or not*/
extern uint8_t	 m_service;

/**@brief Function for handling the Connect event.
*
* @param[in]   p_pir       PIR  presence Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_pir_t * p_pir, ble_evt_t * p_ble_evt)
{
    p_pir->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    PIR_CONNECTED_STATE = true;  /*Set the flag to true so that state remains in connectable mode until disconnect*/
}

/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_pir       PIR presence Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_pir_t * p_pir, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    PIR_CONNECTED_STATE = false; 
    p_pir->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of temperature low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    PIR_EVENT_FLAG = true; 
}

/**@brief Function for handling the Write event.
*
* @param[in]   p_pir       PIR Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_pir_t * p_pir, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_pir->is_notification_supported)
    {

        // Write event for PIR alarm set

        if (
                (p_evt_write->handle == p_pir->pir_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_pir->evt_handler != NULL)
            {
                ble_pir_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PIR_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PIR_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_pir->evt_handler(p_pir, &evt);
            }
        } 

        // Write event for PIR alarm

        if (
                (p_evt_write->handle == p_pir->pir_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_pir->evt_handler != NULL)
            {
                ble_pir_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PIR_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PIR_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_pir->evt_handler(p_pir, &evt);
            }
        } 


        // Write event for PIR

        if (
                (p_evt_write->handle == p_pir->current_pir_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_pir->evt_handler != NULL)
            {
                ble_pir_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PIR_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PIR_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_pir->evt_handler(p_pir, &evt);
            }
        }
    }

    /*Write event for alarm set char value.*/

    if (
            (p_evt_write->handle == p_pir->pir_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_pir->pir_alarm_set =   p_evt_write->data[0];

        // call application event handler
        p_pir->write_evt_handler();
    }

}


void ble_pir_on_ble_evt(ble_pir_t * p_pir, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_pir, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_pir, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_pir, p_ble_evt);
        break;

    default:
        break;
    }

}


/**@brief Function for adding the current PIR presence characteristics.
*
* @param[in]   p_pir            PIR Service structure.
* @param[in]   p_pir_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t pir_state_char_add(ble_pir_t * p_pir, const ble_pir_init_t * p_pir_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_pir_state = DEFAULT_PIR_STATE_ON_PULLUP;

    if (p_pir->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_pir_init->pir_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_pir->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_pir->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;

    ble_uuid.type = p_pir->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_PIR_CURR_STATE_CHAR_UUID; 

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_pir_init->pir_char_attr_md.read_perm;
    attr_md.write_perm = p_pir_init->pir_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_pir_state);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_pir_state);
    attr_char_value.p_value      = &current_pir_state;

    err_code = sd_ble_gatts_characteristic_add(p_pir->service_handle, &char_md,
    &attr_char_value,
    &p_pir->current_pir_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the PIR alarm set characteristics.
*
* @param[in]   p_pir        PIR Service structure.
* @param[in]   p_pir_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t pir_alarm_set_char_add(ble_pir_t * p_pir, const ble_pir_init_t * p_pir_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      pir_alarm_set;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write    = 1;                                   /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                          /*add fix for characteristic write issue*/
    char_md.char_props.notify = 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    // Adding cutom UUID's   
    ble_uuid.type = p_pir->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_PIR_ALARM_SET_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_pir_init->pir_char_attr_md.read_perm;
    attr_md.write_perm = p_pir_init->pir_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    pir_alarm_set = p_pir_init->pir_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &pir_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_pir->service_handle, &char_md,
    &attr_char_value,
    &p_pir->pir_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the PIR alarm characteristics.
*
* @param[in]   p_pir        PIR Service structure.
* @param[in]   p_pir_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t pir_alarm_char_add(ble_pir_t * p_pir, const ble_pir_init_t * p_pir_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      pir_presence_alarm;  

    // Add PIR alarm characteristic 
    if (p_pir->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_pir_init->pir_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = (p_pir->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_pir->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    // Adding cutom UUID's         
    ble_uuid.type = p_pir->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_PIR_ALARM_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_pir_init->pir_char_attr_md.read_perm;
    attr_md.write_perm = p_pir_init->pir_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;


    pir_presence_alarm = p_pir_init->pir_presence_alarm;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &pir_presence_alarm;

    err_code = sd_ble_gatts_characteristic_add(p_pir->service_handle, &char_md,
    &attr_char_value,
    &p_pir->pir_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for initializing the PIR.
*
* @param[in]   p_pir        PIR Service structure.
* @param[in]   p_pir_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_pir_init(ble_pir_t * p_pir, const ble_pir_init_t * p_pir_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    ble_uuid.type =m_service;
		p_pir->uuid_type=m_service;
    ble_uuid.uuid = SENTRY_PROFILE_PIR_SERVICE_UUID;

    // Initialize service structure
    p_pir->evt_handler               = p_pir_init->evt_handler;
    p_pir->write_evt_handler         = write_evt_handler;
    p_pir->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_pir->is_notification_supported = p_pir_init->support_notification;
    p_pir->pir_alarm_set             = p_pir_init->pir_alarm_set;     
    p_pir->pir_presence_alarm        = p_pir_init->pir_presence_alarm; 


    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_pir->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  pir_state_char_add(p_pir, p_pir_init);     /* Add current PIR characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  pir_alarm_set_char_add(p_pir, p_pir_init); /* Add PIR alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  pir_alarm_char_add(p_pir, p_pir_init);    /* Add PIR alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}

/**@brief Function reads and updates the current PIR data and checks for alarm condition.
*  Executes only when an event occurs on gpiote pin for PIR change
*
* @param[in]   p_pir        PIR  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_pir_alarm_check(ble_pir_t * p_pir)
{
    uint32_t err_code;
    uint8_t  current_pir_state;      		/* Current PIR value*/
    uint8_t  alarm;
    uint16_t len = sizeof(uint8_t);


    current_pir_state = nrf_gpio_pin_read(PIR_GPIOTE_PIN);

    // Update the current PIR level value to the service 

    if ((p_pir->conn_handle != BLE_CONN_HANDLE_INVALID) && p_pir->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));


        hvx_params.handle   = p_pir->current_pir_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len;
        hvx_params.p_data   = &current_pir_state;

        err_code = sd_ble_gatts_hvx(p_pir->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }



    if(p_pir->pir_alarm_set != 0x00)
    {

        // If there is an  active high logic level on pin P0.02 set the  alarm 
        if (nrf_gpio_pin_read(PIR_GPIOTE_PIN) == PIR_DETECTION)
        {
            alarm = SET_ALARM_PIR_DETECTION;
        }
        else if(nrf_gpio_pin_read(PIR_GPIOTE_PIN) == NO_PIR_DETECTION)
        {
            alarm = RESET_ALARM;	
        }
        else
        {
            alarm = RESET_ALARM;
        }
    }		 
    else
    {	
        alarm = RESET_ALARM;								/*reset alarm to 0x00*/
    }		

    if(alarm != p_pir->pir_presence_alarm )  	/*check whether the alarm value has changed and send the change*/
    {	
				
        // Send value if connected and notifying

        if ((p_pir->conn_handle != BLE_CONN_HANDLE_INVALID) && p_pir->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            len = sizeof(uint8_t);

            hvx_params.handle   = p_pir->pir_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = &alarm;

            err_code = sd_ble_gatts_hvx(p_pir->conn_handle, &hvx_params);
						p_pir->pir_presence_alarm = alarm;
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }

    return err_code;

}
