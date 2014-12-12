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
* Sruthi.k.s  10/01/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s	10/17/2014		 Added time stamp with alarm characteristic
* Shafy S     10/31/2014     Added changes in the alarm timestamp for sending last occurance of alarm on connection
* Sruthi.k.s  11/07/2014     Changed the notification property of alarm characteristics to indication
*/

#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"
#include "ble_pir_alarm_service.h"

extern 		bool        PIR_EVENT_FLAG;               /* Flag to indicate the event occurred on GPIO pin configured for PIR sensor*/
bool     	        		PIR_CONNECTED_STATE = false;  /* Indicates whether the PIR service is connected or not*/
extern 		uint8_t			var_receive_uuid;
extern    uint8_t			curr_pir_presence;            /* water pir value for broadcast*/
					uint8_t 		pir_alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; /*global variable for storing pir alarm*/
bool                  pir_alarm_set_changed = false;
bool     							m_pir_alarm_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending. */
extern bool     			m_movement_alarm_ind_conf_pending;
					
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

static void on_pir_evt(ble_pir_t * p_pir, ble_pir_alarm_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_PIR_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single temperature measurement
            //temperature_measurement_send();
            break;

        case BLE_PIR_EVT_INDICATION_CONFIRMED:
            m_pir_alarm_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
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
                ble_pir_alarm_evt_t evt;

                if (ble_srv_is_indication_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PIR_EVT_INDICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PIR_EVT_INDICATION_DISABLED;
                }

                on_pir_evt(p_pir, &evt);
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

				pir_alarm_set_changed = true;
        
    }

}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_temps_t   temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_hvc(ble_pir_t * p_pir, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;
    if (p_hvc->handle == p_pir->pir_alarm_handles.value_handle)
    {
        ble_pir_alarm_evt_t evt;
        evt.evt_type = BLE_PIR_EVT_INDICATION_CONFIRMED;
        on_pir_evt(p_pir, &evt);
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
		case BLE_GATTS_EVT_HVC:    //Handle Value Confirmation event
            on_hvc(p_pir, p_ble_evt);
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
    static uint8_t      current_pir_state = 0x00;

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
	
		//array to receive alarm with time stamp from pir service structure
    static uint8_t      pir_alarm_with_time_stamp[8];

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
    char_md.char_props.indicate = (p_pir->is_notification_supported) ? 1 : 0;
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

		//assigning pir alarm with time stamp service values to array
    pir_alarm_with_time_stamp[0]= p_pir_init->pir_alarm_with_time_stamp[0];
		pir_alarm_with_time_stamp[1]= p_pir_init->pir_alarm_with_time_stamp[1];
		pir_alarm_with_time_stamp[2]= p_pir_init->pir_alarm_with_time_stamp[2];
		pir_alarm_with_time_stamp[3]= p_pir_init->pir_alarm_with_time_stamp[3];
		pir_alarm_with_time_stamp[4]= p_pir_init->pir_alarm_with_time_stamp[4];
		pir_alarm_with_time_stamp[5]= p_pir_init->pir_alarm_with_time_stamp[5];
		pir_alarm_with_time_stamp[6]= p_pir_init->pir_alarm_with_time_stamp[6];
		pir_alarm_with_time_stamp[7]= p_pir_init->pir_alarm_with_time_stamp[7];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(pir_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(pir_alarm_with_time_stamp);
    attr_char_value.p_value      =pir_alarm_with_time_stamp;

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

    ble_uuid.type =var_receive_uuid;
		p_pir->uuid_type=var_receive_uuid;
    ble_uuid.uuid = SENTRY_PROFILE_PIR_SERVICE_UUID;

    // Initialize service structure
    p_pir->evt_handler               = p_pir_init->evt_handler;
    p_pir->write_evt_handler         = write_evt_handler;
    p_pir->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_pir->is_notification_supported = p_pir_init->support_notification;
    p_pir->pir_alarm_set             = p_pir_init->pir_alarm_set;      


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
* @param[in]   p_device     Device management  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_pir_alarm_check(ble_pir_t * p_pir,ble_device_t *p_device)
{
    uint32_t err_code;
    uint8_t  current_pir_state = 0x00;      		/* Current PIR value*/
    
    uint16_t len = sizeof(uint8_t);
		uint16_t len2=8;			                     //length of alarm with time stamp
		current_pir_state = nrf_gpio_pin_read(PIR_GPIOTE_PIN);

		//copy the current pir state for broadcast
		curr_pir_presence = current_pir_state;
   
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

        // If there is an  active high logic level on PIR event pin set the  alarm 
        if (current_pir_state == PIR_DETECTION)
        {
            pir_alarm[0] = SET_ALARM_PIR_DETECTION;
						pir_alarm[1]=p_device->device_time_stamp_set[0];
						pir_alarm[2]=p_device->device_time_stamp_set[1];
						pir_alarm[3]=p_device->device_time_stamp_set[2];
						pir_alarm[4]=p_device->device_time_stamp_set[3];
						pir_alarm[5]=p_device->device_time_stamp_set[4];
						pir_alarm[6]=p_device->device_time_stamp_set[5];
						pir_alarm[7]=p_device->device_time_stamp_set[6];
        }
     }		 
    else if(pir_alarm_set_changed)
    {	
        pir_alarm[0] = RESET_ALARM;								/*reset alarm to 0x00*/
				pir_alarm[1]=0x00;
				pir_alarm[2]=0x00;
				pir_alarm[3]=0x00;
				pir_alarm[4]=0x00;
				pir_alarm[5]=0x00;
				pir_alarm[6]=0x00;
				pir_alarm[7]=0x00;
				sd_ble_gatts_value_set(p_pir->pir_alarm_handles.value_handle, 0, &len2, pir_alarm);  /*clear the value of alarm characteristics*/
				p_pir->pir_alarm_with_time_stamp[0] = pir_alarm[0];
				pir_alarm_set_changed = false;
    }		
		
     if((pir_alarm[0]!= 0x00)&&(p_pir->pir_alarm_set == 0x01))  	/*check whether the alarm is on or alarmset is set as zero*/
    {	
				if((!m_pir_alarm_ind_conf_pending)&&(!m_movement_alarm_ind_conf_pending))   /*check whether the confirmation for indication is pending*/
				{
        // Send value if connected and notifying

        if ((p_pir->conn_handle != BLE_CONN_HANDLE_INVALID) && p_pir->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            len = sizeof(uint8_t);

            hvx_params.handle   = p_pir->pir_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len2;
            hvx_params.p_data   = pir_alarm;

            err_code = sd_ble_gatts_hvx(p_pir->conn_handle, &hvx_params);
						p_pir->pir_alarm_with_time_stamp[0] = pir_alarm[0];
						if(err_code == NRF_SUCCESS)
						{
							m_pir_alarm_ind_conf_pending = true;
						}						
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
				}
    }

    return err_code;

}


/**@brief Function checks for alarm condition on connect event occures.
*
* @param[in]   p_pir        PIR  Service structure.
*
* @param[in]   p_device     Device management  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t update_pir_alarmtimestamp_on_connect(ble_pir_t * p_pir,ble_device_t *p_device)
{
    uint32_t err_code=NRF_SUCCESS;
		uint16_t len2 = sizeof(pir_alarm);			//length of alarm with time stamp	
		
    if((pir_alarm[0]!= 0x00)&&(p_pir->pir_alarm_set == 0x01))  	/*check whether the alarm is on or alarmset is set as zero*/
    {	
				if((!m_pir_alarm_ind_conf_pending)&&(!m_movement_alarm_ind_conf_pending))   /*check whether the confirmation for indication is pending*/
				{
        // Send value if connected and notifying

        if ((p_pir->conn_handle != BLE_CONN_HANDLE_INVALID) && p_pir->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle   = p_pir->pir_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len2;
            hvx_params.p_data   = pir_alarm;

            err_code = sd_ble_gatts_hvx(p_pir->conn_handle, &hvx_params);
						p_pir->pir_alarm_with_time_stamp[0] = pir_alarm[0];
						if(err_code == NRF_SUCCESS)
						{
							m_pir_alarm_ind_conf_pending = true;
						}						
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
				}
    }

    return err_code;
}
