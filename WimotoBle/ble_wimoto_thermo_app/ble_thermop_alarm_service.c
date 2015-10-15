/** @file
*
* @{
* @ingroup ble_sdk_app_hts
* @brief Thermopile temperature Service file.
*
* This file contains the source code for Thermopile temperature alarm service
* It also includes the code for Device Information services.
* This application uses the @ref srvlib_conn_params module.
* Adapted from  Battery service provided by Nordic
* 
* Author : Sherin 
* Date   : 11/21/2013
* 
* Change log
* Sherin           12/10/2013     Added write events for value fields
* Hariprasad       12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* sruthiraj        01/10/2014     migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s			 10/17/2014			Added time stamp with alarm characteristics
* Sruthi.k.s       11/07/2014     Changed the notification property of alarm characteristics to indication
*/

#include "ble_thermop_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"
#include "app_error.h"

bool     	      	THERMOPS_CONNECTED_STATE=false;  /*Indicates whether the thermopile service is connected or not*/
extern bool 	  	CHECK_ALARM_TIMEOUT;
uint8_t         	current_thermopile_temp_store[THERMOP_CHAR_SIZE];
extern uint8_t	 	var_receive_uuid;									/*variable to receive uuid*/
extern uint8_t		thermopile[5];                    /*variable to store current Thermopile temperature to broadcast*/
extern bool       CHECK_ALARM_TIMEOUT;							
bool              thermop_alarm_set_changed = false;

bool     				m_thermops_alarm_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending. */
extern bool     m_probes_alarm_ind_conf_pending;

/**@brief Function for handling the Connect event.
*
* @param[in]   p_thermops       Thermopile Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_thermops_t * p_thermops, ble_evt_t * p_ble_evt)
{
    p_thermops->conn_handle  = p_ble_evt->evt.gap_evt.conn_handle;
    THERMOPS_CONNECTED_STATE = true;  /*Set the flag to true so that state remains in connectable mode until disconnect*/
		CHECK_ALARM_TIMEOUT      = true;  /*set the flag to check alarm condition on connect*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_thermops       Thermopile Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_thermops_t * p_thermops, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    THERMOPS_CONNECTED_STATE= false; 
    p_thermops->conn_handle = BLE_CONN_HANDLE_INVALID;
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

static void on_thermops_evt(ble_thermops_t * p_thermops, ble_thermops_alarm_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_THERMOPS_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single temperature measurement
            //temperature_measurement_send();
            break;

        case BLE_THERMOPS_EVT_INDICATION_CONFIRMED:
            m_thermops_alarm_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
}
/**@brief Function for handling the Write event.
*
* @param[in]   p_thermops       Thermopile Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_thermops_t * p_thermops, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_thermops->is_notification_supported)
    {

        //write event for thermopile low value char cccd.   
        if (
                (p_evt_write->handle == p_thermops->thermop_low_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_thermops->evt_handler != NULL)
            {
                ble_thermops_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_thermops->evt_handler(p_thermops, &evt);
            }
        }

        //write event for thermopile high value char cccd. 

        if (
                (p_evt_write->handle == p_thermops->thermop_high_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_thermops->evt_handler != NULL)
            {
                ble_thermops_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_thermops->evt_handler(p_thermops, &evt);
            }
        }

        //write event for thermopile alarm set cccd

        if (
                (p_evt_write->handle == p_thermops->thermo_thermop_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_thermops->evt_handler != NULL)
            {
                ble_thermops_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_thermops->evt_handler(p_thermops, &evt);
            }
        } 

        //write event for thermopile alarm cccd

        if (
                (p_evt_write->handle == p_thermops->thermo_thermop_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_thermops->evt_handler != NULL)
            {
                ble_thermops_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_thermops->evt_handler(p_thermops, &evt);
            }
        } 

        //write event for thermopile cccd
        if (
                (p_evt_write->handle == p_thermops->current_thermop_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_thermops->evt_handler != NULL)
            {
                ble_thermops_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_THERMOPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_thermops->evt_handler(p_thermops, &evt);
            }
        } 



    }
		//handling CCCD write  for alarm
		
		if (
                (p_evt_write->handle == p_thermops->thermo_thermop_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_thermops->evt_handler != NULL)
            {
                ble_thermops_alarm_evt_t evt;
								
                if (ble_srv_is_indication_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_THERMOPS_EVT_INDICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_THERMOPS_EVT_INDICATION_DISABLED;
                }
                on_thermops_evt(p_thermops, &evt);
            }
        }
    // write event for temperature low value char value. 
    if (
            (p_evt_write->handle == p_thermops->thermop_low_level_handles.value_handle) 
            && 
            (p_evt_write->len == 5)
            )
    {
        // update the temperature service structure
        p_thermops->thermo_thermopile_low_level[0] =   p_evt_write->data[0];
        p_thermops->thermo_thermopile_low_level[1] =   p_evt_write->data[1];
        p_thermops->thermo_thermopile_low_level[2] =   p_evt_write->data[2];
        p_thermops->thermo_thermopile_low_level[3] =   p_evt_write->data[3];
        p_thermops->thermo_thermopile_low_level[4] =   p_evt_write->data[4];

        // call application event handler
        p_thermops->write_evt_handler();
    }

    // write event for temperature high value char value. 
    if (
            (p_evt_write->handle == p_thermops->thermop_high_level_handles.value_handle) 
            && 
            (p_evt_write->len == 5)
            )
    {
        // update the temperature service structure
        p_thermops->thermo_thermopile_high_level[0] =   p_evt_write->data[0];
        p_thermops->thermo_thermopile_high_level[1] =   p_evt_write->data[1];
        p_thermops->thermo_thermopile_high_level[2] =   p_evt_write->data[2];
        p_thermops->thermo_thermopile_high_level[3] =   p_evt_write->data[3];
        p_thermops->thermo_thermopile_high_level[4] =   p_evt_write->data[4];

        // call application event handler
        p_thermops->write_evt_handler();
    }

    // write event for alarm set value char value. 
    if (
            (p_evt_write->handle == p_thermops->thermo_thermop_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_thermops->thermo_thermopile_alarm_set =   p_evt_write->data[0];
			
				//set the flag to indicate that the alarm set characteristics is changed
				thermop_alarm_set_changed = true;

        // call application event handler
        p_thermops->write_evt_handler();
    }		
}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_thermops  thermoprobe Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_hvc(ble_thermops_t * p_thermops, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;
    if (p_hvc->handle == p_thermops->thermo_thermop_alarm_handles.value_handle)
    {
        ble_thermops_alarm_evt_t evt;
        evt.evt_type = BLE_THERMOPS_EVT_INDICATION_CONFIRMED;
        on_thermops_evt(p_thermops, &evt);
    }
}
void ble_thermops_on_ble_evt(ble_thermops_t * p_thermops, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_thermops, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
				m_thermops_alarm_ind_conf_pending = false;															//Clear pending confirmation flag if a disconnect occurs
        on_disconnect(p_thermops, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_thermops, p_ble_evt);
        break;
		case BLE_GATTS_EVT_HVC:
            on_hvc(p_thermops, p_ble_evt);
            break;

    default:
        break;
    }

}


/**@brief Function for adding the current thermopile characteristics.
*
* @param[in]   p_thermops        Thermopile Service structure.
* @param[in]   p_thermops_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_thermopile_char_add(ble_thermops_t * p_thermops, const ble_thermops_init_t * p_thermops_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_thermopile[5];

    if (p_thermops->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_thermops_init->thermopile_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_thermops->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_thermops->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;

    // Adding custom UUID
    ble_uuid.type = p_thermops->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_THERMOPS_CURR_THERMOP_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_thermops_init->thermopile_char_attr_md.read_perm;
    attr_md.write_perm = p_thermops_init->thermopile_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_thermopile);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_thermopile);
    attr_char_value.p_value      = current_thermopile;

    err_code = sd_ble_gatts_characteristic_add(p_thermops->service_handle, &char_md,
    &attr_char_value,
    &p_thermops->current_thermop_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the thermopile low value characteristics.
*
* @param[in]   p_thermops        Thermopile Service structure.
* @param[in]   p_thermops_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t thermopile_low_value_char_add(ble_thermops_t * p_thermops, const ble_thermops_init_t * p_thermops_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      thermo_thermopile_low_level[5];

    // Add Thermopile low value characteristic
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
		char_md.char_props.write    = 1;                    /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;               /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Adding custom UUID
    ble_uuid.type = p_thermops->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_THERMOPS_LOW_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_thermops_init->thermopile_char_attr_md.read_perm;
    attr_md.write_perm = p_thermops_init->thermopile_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    thermo_thermopile_low_level[4] = p_thermops_init->thermo_thermopile_low_level[4];
    thermo_thermopile_low_level[3] = p_thermops_init->thermo_thermopile_low_level[3];
    thermo_thermopile_low_level[2] = p_thermops_init->thermo_thermopile_low_level[2];		
    thermo_thermopile_low_level[1] = p_thermops_init->thermo_thermopile_low_level[1];
    thermo_thermopile_low_level[0] = p_thermops_init->thermo_thermopile_low_level[0];		
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(thermo_thermopile_low_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(thermo_thermopile_low_level);
    attr_char_value.p_value      = thermo_thermopile_low_level;

    err_code = sd_ble_gatts_characteristic_add(p_thermops->service_handle, &char_md,
    &attr_char_value,
    &p_thermops->thermop_low_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the thermopile high value characteristics.
*
* @param[in]   p_thermops        Thermopile Service structure.
* @param[in]   p_thermops_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t thermopile_high_value_char_add(ble_thermops_t * p_thermops, const ble_thermops_init_t * p_thermops_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      thermo_thermopile_high_level[5];


    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1; 
		char_md.char_props.write    = 1;                   /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;              /*add fix for characteristic write issue*/
    char_md.char_props.notify   = 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    // Adding custom UUID
    ble_uuid.type = p_thermops->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_THERMOPS_HIGH_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_thermops_init->thermopile_char_attr_md.read_perm;
    attr_md.write_perm = p_thermops_init->thermopile_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    thermo_thermopile_high_level[4] = p_thermops_init->thermo_thermopile_high_level[4];
    thermo_thermopile_high_level[3] = p_thermops_init->thermo_thermopile_high_level[3];  /*Default values for thermopile high value*/
    thermo_thermopile_high_level[2] = p_thermops_init->thermo_thermopile_high_level[2];	
    thermo_thermopile_high_level[1] = p_thermops_init->thermo_thermopile_high_level[1];  /*Default values for thermopile high value*/
    thermo_thermopile_high_level[0] = p_thermops_init->thermo_thermopile_high_level[0];	

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(thermo_thermopile_high_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(thermo_thermopile_high_level);
    attr_char_value.p_value      = thermo_thermopile_high_level;

    err_code = sd_ble_gatts_characteristic_add(p_thermops->service_handle, &char_md,
    &attr_char_value,
    &p_thermops->thermop_high_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the thermopile alarm set characteristics.
*
* @param[in]   p_thermops        Thermopile Service structure.
* @param[in]   p_thermops_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t thermopile_alarm_set_char_add(ble_thermops_t * p_thermops, const ble_thermops_init_t * p_thermops_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      thermo_thermopile_alarm_set;

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
    ble_uuid.type = p_thermops->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_THERMOPS_ALARM_SET_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_thermops_init->thermopile_char_attr_md.read_perm;
    attr_md.write_perm = p_thermops_init->thermopile_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    thermo_thermopile_alarm_set = p_thermops_init->thermo_thermopile_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &thermo_thermopile_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_thermops->service_handle, &char_md,
    &attr_char_value,
    &p_thermops->thermo_thermop_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the thermopile alarm characteristics.
*
* @param[in]   p_thermops        Thermopile Service structure.
* @param[in]   p_thermops_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t thermopile_alarm_char_add(ble_thermops_t * p_thermops, const ble_thermops_init_t * p_thermops_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
	
		/* array for receiving alarm with time stamp from thermopile service structure*/
    static uint8_t     thermo_alarm_with_time_stamp[8];

    // Add thermopile high level characteristic 
    if (p_thermops->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        // According to BAS_SPEC_V10, the read operation on cccd should be possible without
        // authentication.
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_thermops_init->thermopile_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.indicate = (p_thermops->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_thermops->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    // Adding custom UUID
    ble_uuid.type = p_thermops->uuid_type;
    ble_uuid.uuid = THERMO_PROFILE_THERMOPS_ALARM_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_thermops_init->thermopile_char_attr_md.read_perm;
    attr_md.write_perm = p_thermops_init->thermopile_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

		//assigning alarm with time stamp of thermopile characteristics to alarm with time stamp array
    thermo_alarm_with_time_stamp[0] = p_thermops_init->thermo_alarm_with_time_stamp[0];
		thermo_alarm_with_time_stamp[1] = p_thermops_init->thermo_alarm_with_time_stamp[1];
		thermo_alarm_with_time_stamp[2] = p_thermops_init->thermo_alarm_with_time_stamp[2];
		thermo_alarm_with_time_stamp[3] = p_thermops_init->thermo_alarm_with_time_stamp[3];
		thermo_alarm_with_time_stamp[4] = p_thermops_init->thermo_alarm_with_time_stamp[4];
		thermo_alarm_with_time_stamp[5] = p_thermops_init->thermo_alarm_with_time_stamp[5];
		thermo_alarm_with_time_stamp[6] = p_thermops_init->thermo_alarm_with_time_stamp[6];
		thermo_alarm_with_time_stamp[7] = p_thermops_init->thermo_alarm_with_time_stamp[7];
		
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(thermo_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(thermo_alarm_with_time_stamp);
    attr_char_value.p_value      = thermo_alarm_with_time_stamp;

    err_code = sd_ble_gatts_characteristic_add(p_thermops->service_handle, &char_md,
    &attr_char_value,
    &p_thermops->thermo_thermop_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the thermopile.
*
* @param[in]   p_thermops        Thermopile Service structure.
* @param[in]   p_thermops_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_thermops_init(ble_thermops_t * p_thermops, const ble_thermops_init_t * p_thermops_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

		// Add service
    ble_uuid.type =	var_receive_uuid;
		p_thermops->uuid_type=var_receive_uuid;
    ble_uuid.uuid = THERMO_PROFILE_SERVICE_UUID;

    // Initialize service structure
    p_thermops->evt_handler               = p_thermops_init->evt_handler;
    p_thermops->write_evt_handler         = write_evt_handler;
    p_thermops->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_thermops->is_notification_supported = p_thermops_init->support_notification;


    p_thermops->thermo_thermopile_high_level[4] = p_thermops_init->thermo_thermopile_high_level[4];
    p_thermops->thermo_thermopile_high_level[3] = p_thermops_init->thermo_thermopile_high_level[3];  /*Default values for thermopile high value*/
    p_thermops->thermo_thermopile_high_level[2] = p_thermops_init->thermo_thermopile_high_level[2];	
    p_thermops->thermo_thermopile_high_level[1] = p_thermops_init->thermo_thermopile_high_level[1];  /*Default values for thermopile high value*/
    p_thermops->thermo_thermopile_high_level[0] = p_thermops_init->thermo_thermopile_high_level[0];	

    p_thermops->thermo_thermopile_low_level[4] = p_thermops_init->thermo_thermopile_low_level[4];
    p_thermops->thermo_thermopile_low_level[3] = p_thermops_init->thermo_thermopile_low_level[3];
    p_thermops->thermo_thermopile_low_level[2] = p_thermops_init->thermo_thermopile_low_level[2];		
    p_thermops->thermo_thermopile_low_level[1] = p_thermops_init->thermo_thermopile_low_level[1];
    p_thermops->thermo_thermopile_low_level[0] = p_thermops_init->thermo_thermopile_low_level[0];		

    p_thermops->thermo_thermopile_alarm_set    = p_thermops_init->thermo_thermopile_alarm_set;


    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_thermops->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }



    err_code =  current_thermopile_char_add(p_thermops, p_thermops_init);   /* Add current thermopile characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  thermopile_low_value_char_add(p_thermops, p_thermops_init); /* Add thermopile low value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code =  thermopile_high_value_char_add(p_thermops, p_thermops_init); /* Add thermopile high value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code =  thermopile_alarm_set_char_add(p_thermops, p_thermops_init); /* Add thermopile alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  thermopile_alarm_char_add(p_thermops, p_thermops_init);     /* Add thermopile alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}


/**@brief Function reads and updates the current thermopile and checks for alarm condition.
*
* @param[in]   p_thermops        Thermopile Service structure.
*
* @param[in]   p_Device          Device management Service structure
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_thermops_level_alarm_check(ble_thermops_t * p_thermops,ble_device_t *p_device)
{
    uint32_t err_code = NRF_SUCCESS;
    float current_thermopile;
    uint8_t  current_thermopile_array[THERMOP_CHAR_SIZE];
	  float thermopile_low_value;		
    float thermopile_high_value;		
	
		
		
    static float  previous_thermopile = 0x00;
	
    static uint8_t alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    uint16_t len = sizeof(alarm);																			//length of alarm with time stamp characteristics 
    uint16_t len1 = sizeof(current_thermopile_array);

    read_thermopile_connectable(current_thermopile_array, &current_thermopile); /* read the current thermopile*/
		
		int i;
		for (i=0;i<THERMOP_CHAR_SIZE;i++)
    {
        current_thermopile_temp_store[i] =current_thermopile_array[i]; /*store the current thermopile to a global array to be used for data logging*/
				
				thermopile[i]=current_thermopile_array[i];										 /*copy the current Thermopile temperature for broadcast*/
		}
		    if(current_thermopile != previous_thermopile)                   /*Check whether thermopile value has changed*/
    {   

				if ((p_thermops->conn_handle != BLE_CONN_HANDLE_INVALID) && p_thermops->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));


            hvx_params.handle   = p_thermops->current_thermop_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len1;
            hvx_params.p_data   = current_thermopile_array;

            err_code = sd_ble_gatts_hvx(p_thermops->conn_handle, &hvx_params);
						previous_thermopile = current_thermopile;
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }		

    /*Get the thermopile low value set by the user from the service */
    thermopile_low_value  = stof((char *)(p_thermops->thermo_thermopile_low_level));

    /*Get the thermopile high value set by the user from the service */
    thermopile_high_value = stof(((char *)p_thermops->thermo_thermopile_high_level));

   
		/*Check whether the thermopile temperature is out of range if alarm is set by user */		
    if(p_thermops->thermo_thermopile_alarm_set != 0x00)
    {
			  if(current_thermopile < thermopile_low_value)
        {
            alarm[0] = SET_ALARM_THERMOP_LOW;		               /*set alarm to 01 if thermopile is low*/
						alarm[1]=p_device->device_time_stamp_set[0];       /*capture the current timestamp when alarm occured*/
				    alarm[2]=p_device->device_time_stamp_set[1];
				    alarm[3]=p_device->device_time_stamp_set[2];
				    alarm[4]=p_device->device_time_stamp_set[3];
				    alarm[5]=p_device->device_time_stamp_set[4];
				    alarm[6]=p_device->device_time_stamp_set[5];
				    alarm[7]=p_device->device_time_stamp_set[6];
				}

        else if(current_thermopile > thermopile_high_value)
        {
            alarm[0] = SET_ALARM_THERMOP_HIGH;									//set alarm to 02 if thermopile is high
						alarm[1]=p_device->device_time_stamp_set[0];       //capture the current timestamp when alarm occured
				    alarm[2]=p_device->device_time_stamp_set[1];
				    alarm[3]=p_device->device_time_stamp_set[2];
				    alarm[4]=p_device->device_time_stamp_set[3];
				    alarm[5]=p_device->device_time_stamp_set[4];
				    alarm[6]=p_device->device_time_stamp_set[5];
				    alarm[7]=p_device->device_time_stamp_set[6];
				} 

    }
    else if(thermop_alarm_set_changed)       /*check whether the alarm set characteristics is cleared to 00*/
    {	
        alarm[0] = RESET_ALARM;						   /*if alarm set is cleared by user reset alarm to 0x00*/
				alarm[1] = 0x00;											 /*and clear timestamp value*/			
				alarm[2] = 0x00;
				alarm[3] = 0x00;
				alarm[4] = 0x00;
				alarm[5] = 0x00;
				alarm[6] = 0x00;
				alarm[7] = 0x00;
				sd_ble_gatts_value_set(p_thermops->thermo_thermop_alarm_handles.value_handle, 0, &len, alarm);  /*clear the value of alarm characteristics*/
				p_thermops->thermo_alarm_with_time_stamp[0] = alarm[0];
				thermop_alarm_set_changed = false;
		}	
		
		

    if((alarm[0]!= 0x00)&&(p_thermops->thermo_thermopile_alarm_set == 0x01)) /*check whether the alarm  is tripped and  alarm set characteristics in ON*/
    {	
				//check whether confrmation for indication is not pending
				if((!m_thermops_alarm_ind_conf_pending)&&(!m_probes_alarm_ind_conf_pending))
				{
				// Send value if connected and notifying


        if  (p_thermops->conn_handle != BLE_CONN_HANDLE_INVALID) 
						{
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));


            hvx_params.handle   = p_thermops->thermo_thermop_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = alarm;

            err_code = sd_ble_gatts_hvx(p_thermops->conn_handle, &hvx_params);
						p_thermops->thermo_alarm_with_time_stamp[0] = alarm[0];
						if(err_code == NRF_SUCCESS)
						{
							m_thermops_alarm_ind_conf_pending = true;
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

/**@brief Function to read thermopile from tmp006.
*
* @param[out]   uint8_t *buffer 
* @param[out]   float *fTemp
*/
void read_thermopile_connectable(uint8_t *buffer, float *fTemp)   
{
    twi_turn_ON();
    do_thermopile_measurement(buffer, fTemp);
    twi_turn_OFF();
    return;
}	

