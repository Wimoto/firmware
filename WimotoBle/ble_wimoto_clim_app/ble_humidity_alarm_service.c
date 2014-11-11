/** @file
*
* @{
* @ingroup ble_sdk_app_hts
* @brief Humidity Service file.
*
* This file contains the source code for humidity level alarm service
* Adapted from  Battery service provided by Nordic
* 
* Author : Shafy
* Date   : 10/22/2013
* Change log:
* Sherin           12/10/2013     Added write events for value fields
* Hariprasad       12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* Sruthi.k.s     	 10/01/2014     migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s 			 10/17/2014			Added alarm characteristic with time stamp.
* Shafy S          10/28/2014     Added changes to show last occurance timestamp of alarm
*	Sruthi.k.s		 	 11/07/2014			Changed notification property of alarm characteristics to indication
*/

#include "ble_humidity_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"

extern bool     CHECK_ALARM_TIMEOUT;          /*Flag to indicate whether to check for alarm conditions defined in connect.c*/
bool 						HUMS_CONNECTED_STATE=false;   /*This flag indicates whether a client is connected to the peripheral in humidity service*/
extern uint8_t	var_receive_uuid;							/*variable for receiving uuid*/
extern uint8_t	htu_hum_level[2];             /*variable to store current humidity value to broadcast*/ 
bool            hum_alarm_set_changed = false;

bool     				m_hums_alarm_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending.*/
extern bool     m_temps_alarm_ind_conf_pending;
extern bool     m_lights_alarm_ind_conf_pending;

/**@brief Function for handling the Connect event.
*
* @param[in]   p_hums      Humidity Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_hums_t * p_hums, ble_evt_t * p_ble_evt)
{		
    p_hums->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    HUMS_CONNECTED_STATE = true;      /*Set the flag to true so that state remains in connectable mode until disconnect*/
		CHECK_ALARM_TIMEOUT   = true;     /*set the flag to true to check the alarm condition on connection*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_hums      Humidity Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_hums_t * p_hums, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    HUMS_CONNECTED_STATE = false;     /*Indicate humidity service is disconnected*/
    p_hums->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of humidity low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    CHECK_ALARM_TIMEOUT = true; 
}

static void on_hums_evt(ble_hums_t * p_hums, ble_hums_alarm_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_HUMS_EVT_INDICATION_ENABLED:
            break;

        case BLE_HUMS_EVT_INDICATION_CONFIRMED:
            m_hums_alarm_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
}
/**@brief Function for handling the Write event.
*
* @param[in]   p_hums      humidity Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_hums_t * p_hums, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_hums->is_notification_supported)
    {

        //write event for hum_level low value char cccd.   
        if (
                (p_evt_write->handle == p_hums->hum_low_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_hums->evt_handler != NULL)
            {
                ble_hums_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_hums->evt_handler(p_hums, &evt);
            }
        }

        //write event for hum_level high value char cccd. 

        if (
                (p_evt_write->handle == p_hums->hum_high_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_hums->evt_handler != NULL)
            {
                ble_hums_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_hums->evt_handler(p_hums, &evt);
            }
        }

        //write event for hum_level alarm set cccd

        if (
                (p_evt_write->handle == p_hums->climate_hum_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_hums->evt_handler != NULL)
            {
                ble_hums_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_hums->evt_handler(p_hums, &evt);
            }
        } 

        //write event for current Humidity level cccd

        if (
                (p_evt_write->handle == p_hums->current_hum_level_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_hums->evt_handler != NULL)
            {
                ble_hums_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_HUMS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_hums->evt_handler(p_hums, &evt);
            }
        }
        // write event for humidity low value char value. 
        if (
                (p_evt_write->handle == p_hums->hum_low_level_handles.value_handle) 
                && 
                (p_evt_write->len == 2)
                )
        {
            // update the humidity service structure
            p_hums->climate_hum_low_level[0] =   p_evt_write->data[0];
            p_hums->climate_hum_low_level[1] =   p_evt_write->data[1];

            // call application event handler
            p_hums->write_evt_handler();
        }

        // write event for humidity high value char value. 
        if (
                (p_evt_write->handle == p_hums->hum_high_level_handles.value_handle) 
                && 
                (p_evt_write->len == 2)
                )
        {
            // update the humidity service structure
            p_hums->climate_hum_high_level[0] =   p_evt_write->data[0];
            p_hums->climate_hum_high_level[1] =   p_evt_write->data[1];

            // call application event handler
            p_hums->write_evt_handler();
        }

        // write event for alarm set value char value. 
        if (
                (p_evt_write->handle == p_hums->climate_hum_alarm_set_handles.value_handle) 
                && 
                (p_evt_write->len == 1)
                )
        {
            // update the humidity service structure
            p_hums->climate_hum_alarm_set =   p_evt_write->data[0];
						
						//set the flag to indicate the alarm set characteristics is changed
						hum_alarm_set_changed = true;
					
            // call application event handler
            p_hums->write_evt_handler();
        }


    }
		    //write event for hum_level alarm cccd

        if (
                (p_evt_write->handle == p_hums->climate_hum_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_hums->evt_handler != NULL)
            {
                ble_hums_alarm_evt_t evt;

                if (ble_srv_is_indication_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_HUMS_EVT_INDICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_HUMS_EVT_INDICATION_DISABLED;
                }
               on_hums_evt(p_hums, &evt);
            }
        }		
}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_lights       lights Service structure.
 * @param[in]   p_ble_evt      Event received from the BLE stack.
 */
static void on_hvc(ble_hums_t * p_hums, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;
    if (p_hvc->handle == p_hums->climate_hum_alarm_handles.value_handle)
    {
        ble_hums_alarm_evt_t evt;

        evt.evt_type = BLE_HUMS_EVT_INDICATION_CONFIRMED;
        on_hums_evt(p_hums, &evt);
    }
}
void ble_hums_on_ble_evt(ble_hums_t * p_hums, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_hums, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_hums, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_hums, p_ble_evt);
        break;
		case BLE_GATTS_EVT_HVC:
            on_hvc(p_hums, p_ble_evt);
            break;
    default:
        break;
    }

}


/**@brief Function for adding the current hum_level characteristics.
*
* @param[in]   p_hums        Humidity Service structure.
* @param[in]   p_hums_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_hum_level_char_add(ble_hums_t * p_hums, const ble_hums_init_t * p_hums_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_hum_level[2];

    if (p_hums->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_hums_init->hums_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_hums->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc        =	NULL;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_hums->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;


    ble_uuid.type = p_hums->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_HUMS_CURR_HUM_CHAR_UUID; 

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_hums_init->hums_char_attr_md.read_perm;
    attr_md.write_perm = p_hums_init->hums_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;


    current_hum_level[0] = 0x00;
    current_hum_level[1] = 0x00;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_hum_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_hum_level);
    attr_char_value.p_value      = current_hum_level;

    err_code = sd_ble_gatts_characteristic_add(p_hums->service_handle, &char_md,
    &attr_char_value,
    &p_hums->current_hum_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the hum_level low value characteristics.
*
* @param[in]   p_hums        Humidity Service structure.
* @param[in]   p_hums_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t hum_low_value_char_add(ble_hums_t * p_hums, const ble_hums_init_t * p_hums_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_hum_low_value[2];

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     			= 1;
		char_md.char_props.write					=	1;                          /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                          /*add fix for characteristic write issue*/
    char_md.char_props.notify  				= 0;
    char_md.p_char_user_desc    			=	NULL;
    char_md.p_char_pf           			= NULL;
    char_md.p_user_desc_md      			= NULL;
    char_md.p_cccd_md           			= NULL;
    char_md.p_sccd_md           			= NULL;

    ble_uuid.type = p_hums->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_HUMS_HUM_LOW_CHAR_UUID;  

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_hums_init->hums_char_attr_md.read_perm;
    attr_md.write_perm = p_hums_init->hums_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_hum_low_value[1] = p_hums_init->climate_hum_low_value[1];
    climate_hum_low_value[0] = p_hums_init->climate_hum_low_value[0];		

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(climate_hum_low_value);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(climate_hum_low_value);
    attr_char_value.p_value      = climate_hum_low_value;

    err_code = sd_ble_gatts_characteristic_add(p_hums->service_handle, &char_md,
    &attr_char_value,
    &p_hums->hum_low_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the hum_level high value characteristics.
*
* @param[in]   p_hums        Humidity Service structure.
* @param[in]   p_hums_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t hum_high_value_char_add(ble_hums_t * p_hums, const ble_hums_init_t * p_hums_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_hum_high_level[2];

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     			= 1;
		char_md.char_props.write					=	1;                                /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                                /*add fix for characteristic write issue*/
    char_md.char_props.notify  				= 0;
    char_md.p_char_user_desc    			=	NULL;
    char_md.p_char_pf           			= NULL;
    char_md.p_user_desc_md      			= NULL;
    char_md.p_cccd_md           			= NULL;
    char_md.p_sccd_md           			= NULL;

    ble_uuid.type = p_hums->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_HUMS_HUM_HIGH_CHAR_UUID;     

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_hums_init->hums_char_attr_md.read_perm;
    attr_md.write_perm = p_hums_init->hums_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_hum_high_level[1] = p_hums_init->climate_hum_high_value[1];  /*Default values for hum_level high value*/
    climate_hum_high_level[0] = p_hums_init->climate_hum_high_value[0];	

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(climate_hum_high_level);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(climate_hum_high_level);
    attr_char_value.p_value      = climate_hum_high_level;

    err_code = sd_ble_gatts_characteristic_add(p_hums->service_handle, &char_md,
    &attr_char_value,
    &p_hums->hum_high_level_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the hum_level alarm set characteristics.
*
* @param[in]   p_hums        Humidity Service structure.
* @param[in]   p_hums_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t hum_alarm_set_char_add(ble_hums_t * p_hums, const ble_hums_init_t * p_hums_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      climate_hum_alarm_set=0x00;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     			= 1;
		char_md.char_props.write					=	1;                                 /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                                 /*add fix for characteristic write issue*/
    char_md.char_props.notify  				= 0;
    char_md.p_char_user_desc    			=	NULL;
    char_md.p_char_pf           			= NULL;
    char_md.p_user_desc_md      			= NULL;
    char_md.p_cccd_md           			= NULL;
    char_md.p_sccd_md           			= NULL;;

    ble_uuid.type = p_hums->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_HUMS_HUM_ALARM_SET_CHAR_UUID;     

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_hums_init->hums_char_attr_md.read_perm;
    attr_md.write_perm = p_hums_init->hums_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    climate_hum_alarm_set = p_hums_init->climate_hum_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &climate_hum_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_hums->service_handle, &char_md,
    &attr_char_value,
    &p_hums->climate_hum_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the hum_level alarm characteristics.
*
* @param[in]   p_hums        Humidity Service structure.
* @param[in]   p_hums_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t hum_alarm_char_add(ble_hums_t * p_hums, const ble_hums_init_t * p_hums_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    
		/* array for receiving alarm with time stamp from humidity service structure*/
		static uint8_t      hums_alarm_with_time_stamp[8]; 

    // Add hum_level high level characteristic 
    if (p_hums->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_hums_init->hums_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.indicate = (p_hums->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_hums->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_hums->uuid_type;
    ble_uuid.uuid = CLIMATE_PROFILE_HUMS_HUM_ALARM_CHAR_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_hums_init->hums_char_attr_md.read_perm;
    attr_md.write_perm = p_hums_init->hums_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

		//assigning alarm with humidity characteristics to alarm with time stamp array
		hums_alarm_with_time_stamp[0] = p_hums_init->hums_alarm_with_time_stamp[0];
		hums_alarm_with_time_stamp[1] = p_hums_init->hums_alarm_with_time_stamp[1];
		hums_alarm_with_time_stamp[2] = p_hums_init->hums_alarm_with_time_stamp[2];
		hums_alarm_with_time_stamp[3] = p_hums_init->hums_alarm_with_time_stamp[3];
		hums_alarm_with_time_stamp[4] = p_hums_init->hums_alarm_with_time_stamp[4];
		hums_alarm_with_time_stamp[5] = p_hums_init->hums_alarm_with_time_stamp[5];
		hums_alarm_with_time_stamp[6] = p_hums_init->hums_alarm_with_time_stamp[6];
		hums_alarm_with_time_stamp[7] = p_hums_init->hums_alarm_with_time_stamp[7];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(hums_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(hums_alarm_with_time_stamp);
    attr_char_value.p_value      = hums_alarm_with_time_stamp;

    err_code = sd_ble_gatts_characteristic_add(p_hums->service_handle, &char_md,
    &attr_char_value,
    &p_hums->climate_hum_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the hum_level.
*
* @param[in]   p_hums        Humidity Service structure.
* @param[in]   p_hums_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_hums_init(ble_hums_t * p_hums, const ble_hums_init_t * p_hums_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Add service
		ble_uuid.type = var_receive_uuid;
		p_hums->uuid_type=var_receive_uuid;
    ble_uuid.uuid = CLIMATE_PROFILE_HUMS_SERVICE_UUID;

    // Initialize service structure
    p_hums->evt_handler               = p_hums_init->evt_handler;
    p_hums->write_evt_handler         = write_evt_handler;
    p_hums->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_hums->is_notification_supported = p_hums_init->support_notification;
    p_hums->climate_hum_low_level[0]  = p_hums_init->climate_hum_low_value[0];
    p_hums->climate_hum_low_level[1] 	= p_hums_init->climate_hum_low_value[1];
    p_hums->climate_hum_high_level[0] = p_hums_init->climate_hum_high_value[0];
    p_hums->climate_hum_high_level[1]	= p_hums_init->climate_hum_high_value[1];
    p_hums->climate_hum_alarm_set  	  = p_hums_init->climate_hum_alarm_set;

    // Add service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_hums->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  current_hum_level_char_add(p_hums, p_hums_init); /* Add current humidity level characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  hum_low_value_char_add(p_hums, p_hums_init);    /* Add humidity level low value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  hum_high_value_char_add(p_hums, p_hums_init);   /* Add humidity level high value characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  hum_alarm_set_char_add(p_hums, p_hums_init);    /* Add humidity level alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  hum_alarm_char_add(p_hums, p_hums_init);       /* Add humidity level alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;

}


/**@brief Function reads and updates the current humidity level and checks for alarm condition.
*
* @param[in]   p_hums        Humidity Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_hums_level_alarm_check(ble_hums_t * p_hums,ble_device_t *p_device)
{
    uint32_t err_code = NRF_SUCCESS;
    uint16_t current_hum_level;
    uint8_t  current_hum_level_array[2];
		
	  uint16_t hum_level_low_value;					   									 /*humidity low value set by user as uint16*/
    uint16_t hum_level_high_value;				   									 /*humidity low value set by user as uint16*/

    static uint16_t previous_hum_level = 0x00;
    static uint8_t alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


    uint16_t	len = sizeof(alarm);								/*length of alarm with time stamp characteristics*/
    uint16_t len1 = sizeof(current_hum_level_array);

    current_hum_level = read_hum_level(); 				/* read the current hum_level*/


    if(current_hum_level != previous_hum_level)  /*Check whether hum_level value has changed*/
    {
        current_hum_level_array[1] = current_hum_level & LOWER_BYTE_MASK;   /*Convert the hum_level to uint8_t array*/
        current_hum_level_array[0] =  current_hum_level >> 8;
				
				//copy the current humidity value for broadcast
				htu_hum_level[1]=current_hum_level_array[1];
				htu_hum_level[0]=current_hum_level_array[0];

				// Send value if connected and notifying
        if ((p_hums->conn_handle != BLE_CONN_HANDLE_INVALID) && (p_hums->is_notification_supported))
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            
            hvx_params.handle   = p_hums->current_hum_level_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len1;
            hvx_params.p_data   = current_hum_level_array;

            err_code = sd_ble_gatts_hvx(p_hums->conn_handle, &hvx_params);
						previous_hum_level = current_hum_level;
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }		

    /*Get the hum_level low value set by the user from the service */

    hum_level_low_value 						= 	(p_hums->climate_hum_low_level[0])<<8;  /*convert the 8 bit arrays to a 16 bit data*/
    hum_level_low_value 						= 	hum_level_low_value | (p_hums->climate_hum_low_level[1]);	


    /*Get the hum_level high value set by the user from the service */

    hum_level_high_value            = 	(p_hums->climate_hum_high_level[0])<<8;	/*convert the 8 bit arrays to a 16 bit data*/
    hum_level_high_value 			 			= 	hum_level_high_value | (p_hums->climate_hum_high_level[1]);
    
    
    /*Check whether the hum_level is out of range if alarm is set by user */		
    if(p_hums->climate_hum_alarm_set != 0x00)
    {
        
        if(current_hum_level < hum_level_low_value)
        {
            alarm[0] = SET_ALARM_LOW;		/*set alarm to 01 if humidity level is low */
						alarm[1]=p_device->device_time_stamp_set[0];
						alarm[2]=p_device->device_time_stamp_set[1];
						alarm[3]=p_device->device_time_stamp_set[2];
						alarm[4]=p_device->device_time_stamp_set[3];
						alarm[5]=p_device->device_time_stamp_set[4];
						alarm[6]=p_device->device_time_stamp_set[5];
						alarm[7]=p_device->device_time_stamp_set[6];
        }
        
        else if(current_hum_level > hum_level_high_value)
        {
            alarm[0] = SET_ALARM_HIGH;		       /*set alarm to 02 if humidity level is high */
						alarm[1]=p_device->device_time_stamp_set[0];
						alarm[2]=p_device->device_time_stamp_set[1];
						alarm[3]=p_device->device_time_stamp_set[2];
						alarm[4]=p_device->device_time_stamp_set[3];
						alarm[5]=p_device->device_time_stamp_set[4];
						alarm[6]=p_device->device_time_stamp_set[5];
						alarm[7]=p_device->device_time_stamp_set[6];
        } 
	
    }
    else if(hum_alarm_set_changed)						/*check whether the alarm set characteristics is set to 00*/
    {	
        alarm[0] = RESET_ALARM;							   /*reset alarm to 0x00 and clear the timestamp*/
				alarm[1]=0x00;
				alarm[2]=0x00;
				alarm[3]=0x00;
				alarm[4]=0x00;
				alarm[5]=0x00;
				alarm[6]=0x00;
				alarm[7]=0x00;
			
				sd_ble_gatts_value_set(p_hums->climate_hum_alarm_handles.value_handle, 0, &len, alarm);  /*clear the value of alarm characteristics*/
				p_hums->hums_alarm_with_time_stamp[0]= alarm[0];
				hum_alarm_set_changed = false;
    }	
		
    if((alarm[0]!= 0x00)&&(p_hums->climate_hum_alarm_set == 0x01))  /*check whether the alarm is tripped and alarm set characteristics is set to ON*/
    {		
				//check whether the confirmation for indication is not pending
        if((!m_temps_alarm_ind_conf_pending)&&(!m_lights_alarm_ind_conf_pending)&&(!m_hums_alarm_ind_conf_pending))
				{
				// Send value if connected and notifying
        if ((p_hums->conn_handle != BLE_CONN_HANDLE_INVALID) && p_hums->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));
            
            hvx_params.handle   = p_hums->climate_hum_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = alarm;

            err_code = sd_ble_gatts_hvx(p_hums->conn_handle, &hvx_params);
						p_hums->hums_alarm_with_time_stamp[0]= alarm[0];
						if(err_code == NRF_SUCCESS)
						{
							m_hums_alarm_ind_conf_pending = true;
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

/**@brief Function to read humidity level from htu21d.
*
* @param[in]   void
* @param[out]  uint16_t current_hum_level.
*/
uint16_t read_hum_level()   
{
    static uint16_t current_hum_level = 0x0000;

    twi_turn_ON();
    current_hum_level = eDRV_HTU21_MeasureHumidity();  /* Read Humidity from HTU21D sensor*/
    twi_turn_OFF(); 

    return current_hum_level;
}	











