/** @file
*
* @{
* @brief probe temperature Service file.
*
* This file contains the source code for probe temperature level alarm service
* Adapted from  Battery service provided by Nordic
* 
* Author : Sherin Deena
* Date   : 11/21/2013
* Change log
* Sherin           12/10/2013     Added write events for value fields
* Hariprasad       12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* sruthiraj        10/01/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s			 10/17/2014			Added time stamp with alarm characteristics
* Sruthi.k.s       11/07/2014     Changed the notification property of alarm characteristics to indication
*/

#include "ble_probe_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"
#include "app_error.h"

bool 	PROBES_CONNECTED_STATE=false;      /*This flag indicates whether a client is connected to the peripheral in probe temperature service*/
extern bool 	 		CHECK_ALARM_TIMEOUT;
extern uint8_t	 	var_receive_uuid;										/*variable to receive uuid*/
extern uint8_t	  curr_probe_temp_level[2];   /*variable to store current probe temperature to broadcast*/
extern bool       CHECK_ALARM_TIMEOUT;
bool              probe_alarm_set_changed = false;
bool     					m_probes_alarm_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending. */
extern bool     	m_thermops_alarm_ind_conf_pending;

/**@brief Function for handling the Connect event.
*
* @param[in]   p_probes    probe temperature Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_probes_t * p_probes, ble_evt_t * p_ble_evt)
{
    p_probes->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    PROBES_CONNECTED_STATE = true;      /*Set the flag to true so that state remains in connectable mode until disconnected*/
		CHECK_ALARM_TIMEOUT    = true;      /*set the flag to check alarm condition on connect*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_probes    probe temperature Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_probes_t * p_probes, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    PROBES_CONNECTED_STATE = false;     /*Indicate that probe temperature service is disconnected*/
    p_probes->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of probe temperature low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    CHECK_ALARM_TIMEOUT = true; 
}

static void on_probes_evt(ble_probes_t * p_probes, ble_probes_alarm_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_PROBES_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single temperature measurement
            //temperature_measurement_send();
            break;

        case BLE_PROBES_EVT_INDICATION_CONFIRMED:
            m_probes_alarm_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
}
/**@brief Function for handling the Write event.
*
* @param[in]   p_probes    probe temperature Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_probes_t * p_probes, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_probes->is_notification_supported)
    {

        //write event for probe temperature low value char.   
        if (
                (p_evt_write->handle == p_probes->probe_temp_low_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_probes->evt_handler != NULL)
            {
                ble_probes_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_DISABLED;
                }

                p_probes->evt_handler(p_probes, &evt);
            }
        }

        //write event for probe temperature level high value char. 

        if (
                (p_evt_write->handle == p_probes->probe_temp_high_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_probes->evt_handler != NULL)
            {
                ble_probes_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_DISABLED;
                }

                p_probes->evt_handler(p_probes, &evt);
            }
        }

        //write event for probe temperature alarm set

        if (
                (p_evt_write->handle == p_probes->probe_temp_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_probes->evt_handler != NULL)
            {
                ble_probes_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_DISABLED;
                }

                p_probes->evt_handler(p_probes, &evt);
            }
        } 

        //write event for current probe temperature level


        if (
                (p_evt_write->handle == p_probes->curr_probe_temp_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_probes->evt_handler != NULL)
            {
                ble_probes_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PROBES_EVT_NOTIFICATION_DISABLED;
                }

                p_probes->evt_handler(p_probes, &evt);
            }
        }
    }

		//handling CCCD write  for alarm
		
		if (
                (p_evt_write->handle == p_probes->probe_temp_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_probes->evt_handler != NULL)
            {
                ble_probes_alarm_evt_t evt;
								
                if (ble_srv_is_indication_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_PROBES_EVT_INDICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_PROBES_EVT_INDICATION_DISABLED;
                }
                on_probes_evt(p_probes, &evt);
            }
        }
    // write event for soil moisture low value char value. 
    if (
            (p_evt_write->handle == p_probes->probe_temp_low_level_handles.value_handle) 
            && 
            (p_evt_write->len == 2)
            )
    {
        // update the temperature service structure
        p_probes->probe_temp_low_level[0] =   p_evt_write->data[0];
				p_probes->probe_temp_low_level[1] =   p_evt_write->data[1];
   
			// call application event handler
        p_probes->write_evt_handler();
    }		

    // write event for soil moisture high value char value. 
    if (
            (p_evt_write->handle == p_probes->probe_temp_high_level_handles.value_handle) 
            && 
            (p_evt_write->len == 2)
            )
    {
        // update the temperature service structure
        p_probes->probe_temp_high_level[0] =   p_evt_write->data[0];
				p_probes->probe_temp_high_level[1] =   p_evt_write->data[1];
       
				// call application event handler
        p_probes->write_evt_handler();
    }

    // write event for alarm set value char value. 
    if (
            (p_evt_write->handle == p_probes->probe_temp_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_probes->probe_temp_alarm_set =   p_evt_write->data[0];
			
			  //set the flag to indicate the alarm set characteristics is changed
				probe_alarm_set_changed = true;

        // call application event handler
        p_probes->write_evt_handler();
    }		

}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_probe     probe Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_hvc(ble_probes_t * p_probes, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;
    if (p_hvc->handle == p_probes->probe_temp_alarm_handles.value_handle)
    {
        ble_probes_alarm_evt_t evt;
        evt.evt_type = BLE_PROBES_EVT_INDICATION_CONFIRMED;
        on_probes_evt(p_probes, &evt);
    }
}
void ble_probes_on_ble_evt(ble_probes_t * p_probes, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_probes, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
				m_probes_alarm_ind_conf_pending = false;													//Clear pending confirmation flag if a disconnect occurs
        on_disconnect(p_probes, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_probes, p_ble_evt);
        break;
		case BLE_GATTS_EVT_HVC:
            on_hvc(p_probes, p_ble_evt);
            break;
    default:
        break;
    }

}


/**@brief Function for adding the current probe temperature level characteristics.
*
* @param[in]   p_probes        probe temperature Service structure.
* @param[in]   p_probes_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_probe_temp_level_char_add(ble_probes_t * p_probes, const ble_probes_init_t * p_probes_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_probe_temp_level[2];

    if (p_probes->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_probes_init->probe_temp_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_probes->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_probes->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;


    // Adding custom UUID
    ble_uuid.type = p_probes->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_CURR_PROBE_CHAR_UUID;   //using a custom characteristics uuid

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_probes_init->probe_temp_char_attr_md.read_perm;
    attr_md.write_perm = p_probes_init->probe_temp_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_probe_temp_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_probe_temp_level);
    attr_char_value.p_value      = current_probe_temp_level;

    err_code = sd_ble_gatts_characteristic_add(p_probes->service_handle, &char_md,
    &attr_char_value,
    &p_probes->curr_probe_temp_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the probe temperature low value characteristics.
*
* @param[in]   p_probes        probe temperature Service structure.
* @param[in]   p_probes_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t probe_temp_low_value_char_add(ble_probes_t * p_probes, const ble_probes_init_t * p_probes_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      probe_temp_low_value[2];

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                                    /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                               /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Adding custom UUID
    ble_uuid.type = p_probes->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_PROBE_LOW_CHAR_UUID;   //using a custom characteristics uuid

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_probes_init->probe_temp_char_attr_md.read_perm;
    attr_md.write_perm = p_probes_init->probe_temp_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    probe_temp_low_value[1] = p_probes_init->probe_temp_low_value[1];
		probe_temp_low_value[0] = p_probes_init->probe_temp_low_value[0];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(probe_temp_low_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(probe_temp_low_value);
    attr_char_value.p_value      = probe_temp_low_value;

    err_code = sd_ble_gatts_characteristic_add(p_probes->service_handle,&char_md,
    &attr_char_value,
    &p_probes->probe_temp_low_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the probe temperature high value characteristics.
*
* @param[in]   p_probes        probe temperature Service structure.
* @param[in]   p_probes_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t probe_temp_high_value_char_add(ble_probes_t * p_probes, const ble_probes_init_t * p_probes_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      probe_temp_high_value[2];

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                               /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                          /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Adding custom UUID
    ble_uuid.type = p_probes->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_PROBE_HIGH_CHAR_UUID;   //using a custom characteristics uuid

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_probes_init->probe_temp_char_attr_md.read_perm;
    attr_md.write_perm = p_probes_init->probe_temp_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    probe_temp_high_value[1] = p_probes_init->probe_temp_high_value[1];  /*Default values for probe temperature high value*/
		probe_temp_high_value[0] = p_probes_init->probe_temp_high_value[0];  /*Default values for probe temperature high value*/

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(probe_temp_high_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(probe_temp_high_value);
    attr_char_value.p_value      = probe_temp_high_value;

    err_code = sd_ble_gatts_characteristic_add(p_probes->service_handle, &char_md,
    &attr_char_value,
    &p_probes->probe_temp_high_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the probe temperature alarm set characteristics.
*
* @param[in]   p_probes        probe temperature Service structure.
* @param[in]   p_probes_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t probe_temp_alarm_set_char_add(ble_probes_t * p_probes, const ble_probes_init_t * p_probes_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      probe_temp_alarm_set;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                                   /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp= 1;                               /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Adding custom UUID
    ble_uuid.type = p_probes->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_PROBE_ALARM_SET_CHAR_UUID;   //using a custom characteristics uuid

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_probes_init->probe_temp_char_attr_md.read_perm;
    attr_md.write_perm = p_probes_init->probe_temp_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    probe_temp_alarm_set = p_probes_init->probe_temp_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &probe_temp_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_probes->service_handle, &char_md,
    &attr_char_value,
    &p_probes->probe_temp_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the probe temperature level alarm characteristics.
*
* @param[in]   p_probes       probe temperature Service structure.
* @param[in]   p_probes_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t probe_temp_alarm_char_add(ble_probes_t * p_probes, const ble_probes_init_t * p_probes_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
	
		/* array for receiving alarm with time stamp from probe temperature service structure*/
    static uint8_t      probe_alarm_with_time_stamp[8]; 

    // Add probe temperature high level characteristic 
    if (p_probes->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_probes_init->probe_temp_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.indicate = (p_probes->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_probes->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    // Adding custom UUID
    ble_uuid.type = p_probes->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_PROBE_ALARM_CHAR_UUID;   //using a custom characteristics uuid

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_probes_init->probe_temp_char_attr_md.read_perm;
    attr_md.write_perm = p_probes_init->probe_temp_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
		//assigning alarm with time stamp of probe temperature characteristics to alarm with time stamp array
    probe_alarm_with_time_stamp[0] = p_probes_init->probe_alarm_with_time_stamp[0];
		probe_alarm_with_time_stamp[1] = p_probes_init->probe_alarm_with_time_stamp[1];
		probe_alarm_with_time_stamp[2] = p_probes_init->probe_alarm_with_time_stamp[2];
		probe_alarm_with_time_stamp[3] = p_probes_init->probe_alarm_with_time_stamp[3];
		probe_alarm_with_time_stamp[4] = p_probes_init->probe_alarm_with_time_stamp[4];
		probe_alarm_with_time_stamp[5] = p_probes_init->probe_alarm_with_time_stamp[5];
		probe_alarm_with_time_stamp[6] = p_probes_init->probe_alarm_with_time_stamp[6];
		probe_alarm_with_time_stamp[7] = p_probes_init->probe_alarm_with_time_stamp[7];
		
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(probe_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(probe_alarm_with_time_stamp);
    attr_char_value.p_value      = probe_alarm_with_time_stamp;

    err_code = sd_ble_gatts_characteristic_add(p_probes->service_handle, &char_md,
    &attr_char_value,
    &p_probes->probe_temp_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the probe temperature service.
*
* @param[in]   p_probes        probe temperature Service structure.
* @param[in]   p_probes_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_probes_init(ble_probes_t * p_probes, const ble_probes_init_t * p_probes_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Add service
    ble_uuid.type = var_receive_uuid;
		p_probes->uuid_type=var_receive_uuid;
    ble_uuid.uuid = THERMO_PROFILE_PROBES_SERVICE_UUID;

    // Initialize service structure
    p_probes->evt_handler               = p_probes_init->evt_handler;
    p_probes->write_evt_handler         = write_evt_handler;
    p_probes->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_probes->is_notification_supported = p_probes_init->support_notification;

    p_probes->probe_temp_low_level[0]   = p_probes_init->probe_temp_low_value[0];  
    p_probes->probe_temp_low_level[1]   = p_probes_init->probe_temp_low_value[1];
		p_probes->probe_temp_high_level[0]  = p_probes_init->probe_temp_high_value[0]; 
    p_probes->probe_temp_high_level[1]  = p_probes_init->probe_temp_high_value[1];
		p_probes->probe_temp_alarm_set      = p_probes_init->probe_temp_alarm_set;  
  		

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_probes->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  current_probe_temp_level_char_add(p_probes, p_probes_init); /* Add current probe temperature level characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  probe_temp_low_value_char_add(p_probes, p_probes_init);    /* Add probe temperature level low value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  probe_temp_high_value_char_add(p_probes, p_probes_init);   /* Add probe temperature level high value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  probe_temp_alarm_set_char_add(p_probes, p_probes_init);    /* Add probe temperature level alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  probe_temp_alarm_char_add(p_probes, p_probes_init);       /* Add probe temperature level alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;

}


/**@brief Function reads and updates the current probe temperature level and checks for alarm condition.
*
* @param[in]   p_probes        probe temperature  Service structure.
*
* @param[in]   p_Device          Device management Service structure
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_probes_level_alarm_check(ble_probes_t * p_probes,ble_device_t *p_device)
{
    uint32_t err_code = NRF_SUCCESS;
    uint16_t current_probe_temp_level;
		uint8_t current_probe_temp_level_array[2];
	
		uint16_t probe_temp_low_value;
		uint16_t probe_temp_high_value;
	
    static uint16_t previous_probe_temp_level = 0x00;
    static uint8_t alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint16_t	len = sizeof(alarm);																						//length of alarm with time stamp characteristics 
		uint16_t  len1 = sizeof(current_probe_temp_level_array);

    current_probe_temp_level = read_probe_temp_level(); /* read the current probe temperature level*/
		
    if(current_probe_temp_level != previous_probe_temp_level)  /*Check whether probe temperature value has changed*/
    {
        current_probe_temp_level_array[1] = (uint8_t)(current_probe_temp_level & LOWER_BYTE_MASK);   /* Convert the temperature to uint8_t array*/
        current_probe_temp_level_array[0] =  current_probe_temp_level >>8;
			
				/*copy the current probe temperature for broadcast*/
				curr_probe_temp_level[1]=current_probe_temp_level_array[1];
				curr_probe_temp_level[0]=current_probe_temp_level_array[0];
        
				// Send value if connected and notifying		
        if ((p_probes->conn_handle != BLE_CONN_HANDLE_INVALID) && p_probes->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            len = sizeof(uint8_t);

            hvx_params.handle   = p_probes->curr_probe_temp_level_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len1;
            hvx_params.p_data   = current_probe_temp_level_array;

            err_code = sd_ble_gatts_hvx(p_probes->conn_handle, &hvx_params);
						previous_probe_temp_level = current_probe_temp_level;
					
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }		

    // Get the temperature low value set by the user from the service structure

    probe_temp_low_value 						= 	(p_probes->probe_temp_low_level[0])<<8;	  /* Convert the 8 bit arrays to a 16 bit data*/
    probe_temp_low_value 						= 	probe_temp_low_value | (p_probes->probe_temp_low_level[1]);	
    
  // Get the temperature high value set by the user from the service structure

    probe_temp_high_value            = 	(p_probes->probe_temp_high_level[0])<<8;  /* Convert the 8 bit arrays to a 16 bit data*/
    probe_temp_high_value 			 			= probe_temp_high_value | (p_probes->probe_temp_high_level[1]);	

    /*Check whether the probe temperature is out of range if alarm is set by user */		
    if(p_probes->probe_temp_alarm_set  != 0x00)
    {

        if(current_probe_temp_level < probe_temp_low_value)
        {
            alarm[0] = SET_ALARM_LOW;		           				  /*set alarm to 01 if probe temperature level is low */
						alarm[1] = p_device->device_time_stamp_set[0];	/*capture the timestamp of occurance of alarm*/
						alarm[2] = p_device->device_time_stamp_set[1];
						alarm[3] = p_device->device_time_stamp_set[2];
						alarm[4] = p_device->device_time_stamp_set[3];
						alarm[5] = p_device->device_time_stamp_set[4];
						alarm[6] = p_device->device_time_stamp_set[5];
						alarm[7] = p_device->device_time_stamp_set[6];
        }

        else if(current_probe_temp_level > probe_temp_high_value )
        {
            alarm[0] = SET_ALARM_HIGH;		                  /*set alarm to 02 if probe temperature level is high */
						alarm[1] = p_device->device_time_stamp_set[0];	/*capture the timestamp of occurance of alarm*/
						alarm[2] = p_device->device_time_stamp_set[1];
						alarm[3] = p_device->device_time_stamp_set[2];
						alarm[4] = p_device->device_time_stamp_set[3];
						alarm[5] = p_device->device_time_stamp_set[4];
						alarm[6] = p_device->device_time_stamp_set[5];
						alarm[7] = p_device->device_time_stamp_set[6];
        } 

    }
    else if(probe_alarm_set_changed)
    {	
        alarm[0] = RESET_ALARM;							      /*reset alarm to 0x00*/
				alarm[1] = 0x00;													/*clear the timestamp*/
				alarm[2] = 0x00;
				alarm[3] = 0x00;
				alarm[4] = 0x00;
				alarm[5] = 0x00;
				alarm[6] = 0x00;
				alarm[7] = 0x00;
			 
			  sd_ble_gatts_value_set(p_probes->probe_temp_alarm_handles.value_handle, 0, &len, alarm);  /*clear the value of alarm characteristics*/
				p_probes->probe_alarm_with_time_stamp[0] = alarm[0];
				probe_alarm_set_changed = false;
    }		
		
    if((alarm[0]!= 0x00)&&(p_probes->probe_temp_alarm_set == 0x01))  /*check whether the alarm sets as non zero or alarm set characteristics set as zero*/
    {		
				//check whether confrmation for indication is not pending
				if((!m_thermops_alarm_ind_conf_pending)&&(!m_probes_alarm_ind_conf_pending))
				{
        // Send value if connected and notifying

        if (p_probes->conn_handle != BLE_CONN_HANDLE_INVALID) 
					{
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            
            hvx_params.handle   = p_probes->probe_temp_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = alarm;

            err_code = sd_ble_gatts_hvx(p_probes->conn_handle, &hvx_params);
						p_probes->probe_alarm_with_time_stamp[0] = alarm[0];
						if(err_code == NRF_SUCCESS)
						{
							m_probes_alarm_ind_conf_pending = true;
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

/**@brief Function to read probe temperature level from sensor interfaced to ADC.
*
* @param[in]   void
* @param[out]  uint16_t current_probe_temp_level.
*/
uint16_t read_probe_temp_level()   
{
    static uint16_t current_probe_temp_level=0x00;                            
    current_probe_temp_level = do_probe_temperature_measurement();  /*Read probe temperature level from ADC */
    return current_probe_temp_level;
}	






