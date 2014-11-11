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
* Sruthi.k.s        10/01/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s 			  10/17/2014		 Added alarm characteristic with time stamp.
* Shafy S           10/28/2014     Added changes to show last occurance timestamp of alarm
*	Sruthi.k.s		 	  11/07/2014		 Changed notification property of alarm characteristics to indication
*/

#include "ble_light_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto_sensors.h"
#include "wimoto.h"

bool   						LIGHTS_CONNECTED_STATE=false;          /*This flag indicates whether a client is connected to the peripheral or not*/
extern bool     	CHECK_ALARM_TIMEOUT;         					 /*Flag to indicate whether to check for alarm conditions defined in connect.c*/
extern uint8_t	 	var_receive_uuid;											 /*variable for receiving uuid*/
extern uint8_t		light_level[2];            						 /*variable to store current light level value to broadcast*/
bool              light_alarm_set_changed = false;

bool     					m_lights_alarm_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending.*/
extern bool     	m_temps_alarm_ind_conf_pending;
extern bool     	m_hums_alarm_ind_conf_pending;

/**@brief Function for handling the Connect event.
*
* @param[in]   p_lights    Light Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_lights_t * p_lights, ble_evt_t * p_ble_evt)
{
    p_lights->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    LIGHTS_CONNECTED_STATE = true;  /*Set the flag to true so that state remains in connectable mode until disconnect*/
	  CHECK_ALARM_TIMEOUT   = true;     /*set the flag to true to check the alarm condition on connection*/
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

static void on_lights_evt(ble_lights_t * p_lights, ble_lights_alarm_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_LIGHTS_EVT_INDICATION_ENABLED:
            break;

        case BLE_LIGHTS_EVT_INDICATION_CONFIRMED:
            m_lights_alarm_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
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
                ble_lights_alarm_evt_t evt;

                if (ble_srv_is_indication_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_LIGHTS_EVT_INDICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_LIGHTS_EVT_INDICATION_DISABLED;
                }
                on_lights_evt(p_lights, &evt);
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

				light_alarm_set_changed = true;
        // call application event handler
        p_lights->write_evt_handler();
    }

}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_lights       lights Service structure.
 * @param[in]   p_ble_evt      Event received from the BLE stack.
 */
static void on_hvc(ble_lights_t * p_lights, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;
    if (p_hvc->handle == p_lights->climate_light_alarm_handles.value_handle)
    {
        ble_lights_alarm_evt_t evt;
        evt.evt_type = BLE_LIGHTS_EVT_INDICATION_CONFIRMED;
        on_lights_evt(p_lights, &evt);
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
		case BLE_GATTS_EVT_HVC:
            on_hvc(p_lights, p_ble_evt);
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
	
		/* array for receiving alarm with time stamp from light service structure*/
		static uint8_t      lights_alarm_with_time_stamp[8];

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
    char_md.char_props.indicate = (p_lights->is_notification_supported) ? 1 : 0;
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

		//assigning alarm with light characteristics to alarm with time stamp array
		
    lights_alarm_with_time_stamp[0] = p_lights_init->lights_alarm_with_time_stamp[0];
		lights_alarm_with_time_stamp[1] = p_lights_init->lights_alarm_with_time_stamp[1];
		lights_alarm_with_time_stamp[2] = p_lights_init->lights_alarm_with_time_stamp[2];
		lights_alarm_with_time_stamp[3] = p_lights_init->lights_alarm_with_time_stamp[3];
		lights_alarm_with_time_stamp[4] = p_lights_init->lights_alarm_with_time_stamp[4];
		lights_alarm_with_time_stamp[5] = p_lights_init->lights_alarm_with_time_stamp[5];
		lights_alarm_with_time_stamp[6] = p_lights_init->lights_alarm_with_time_stamp[6];
		lights_alarm_with_time_stamp[7] = p_lights_init->lights_alarm_with_time_stamp[7];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(lights_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(lights_alarm_with_time_stamp);
    attr_char_value.p_value      = lights_alarm_with_time_stamp;

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
    ble_uuid.type = var_receive_uuid;
	  p_lights->uuid_type = var_receive_uuid;
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
* @param[in]   p_device       Device management Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_lights_level_alarm_check(ble_lights_t * p_lights,ble_device_t * p_device)
{
    uint32_t err_code = NRF_SUCCESS;
    uint16_t current_light_level;
    uint8_t  current_light_level_array[2];
		
	  uint16_t light_level_low_value;					   /*light low value set by user as uint16*/
    uint16_t light_level_high_value;				   /*light low value set by user as uint16*/

    static uint16_t   previous_light_level = 0x00;
    static uint8_t alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    uint16_t	len  = 8;	//length of alarm with time stamp characteristics
    uint16_t  len1 = sizeof(current_light_level_array);

    current_light_level = read_light_level(); /* read the current light_level*/


    if(current_light_level != previous_light_level)  /*Check whether light_level value has changed*/
    {
        current_light_level_array[1] = current_light_level & LOWER_BYTE_MASK;   /*Convert the light_level to uint8_t array*/
        current_light_level_array[0]=  current_light_level >>8;
			
				//copy the current light level for broadcast
        light_level[1]=current_light_level_array[1];
				light_level[0]=current_light_level_array[0];
			
        // Send value if connected and notifying
        if ((p_lights->conn_handle != BLE_CONN_HANDLE_INVALID) && p_lights->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;
 
            memset(&hvx_params, 0, sizeof(hvx_params));

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
            alarm[0] = SET_ALARM_LOW;		         						/*set alarm to 01 if light_level is low */
						alarm[1]=p_device->device_time_stamp_set[0];		/*capture the timestamp when alarm occured*/
						alarm[2]=p_device->device_time_stamp_set[1];
						alarm[3]=p_device->device_time_stamp_set[2];
						alarm[4]=p_device->device_time_stamp_set[3];
						alarm[5]=p_device->device_time_stamp_set[4];
						alarm[6]=p_device->device_time_stamp_set[5];
						alarm[7]=p_device->device_time_stamp_set[6];
        }
        
        else if(current_light_level > light_level_high_value) 
        {
            alarm[0] = SET_ALARM_HIGH;            					/*set alarm to 02 if light_level is high */
						alarm[1]=p_device->device_time_stamp_set[0];		/*capture the timestamp when alarm occured*/
						alarm[2]=p_device->device_time_stamp_set[1];
						alarm[3]=p_device->device_time_stamp_set[2];
						alarm[4]=p_device->device_time_stamp_set[3];
						alarm[5]=p_device->device_time_stamp_set[4];
						alarm[6]=p_device->device_time_stamp_set[5];
						alarm[7]=p_device->device_time_stamp_set[6];
        } 
        
       

    }
    else if(light_alarm_set_changed)							/*check whether the alarm set characteristics is set to 00*/
    {	
        alarm[0] = RESET_ALARM;								    /*reset alarm to 0x00 and clear the timestamp*/
				alarm[1]=0x00;
				alarm[2]=0x00;
				alarm[3]=0x00;
				alarm[4]=0x00;
				alarm[5]=0x00;
				alarm[6]=0x00;
				alarm[7]=0x00;
				
				sd_ble_gatts_value_set(p_lights->climate_light_alarm_handles.value_handle, 0, &len, alarm);  /*clear the value of alarm characteristics*/
				p_lights->lights_alarm_with_time_stamp[0] = alarm[0];
				light_alarm_set_changed = false;
    }	
		
		/*reading of time stamp from device management service structure whether the alarm set*/
		

    if((alarm[0]!= 0x00)&&(p_lights->climate_light_alarm_set == 0x01))  /*check whether the alarm sets as non zero or alarm set characteristics set as zero*/
    {		
				//check whether the confirmation for indication is not pending
				if((!m_temps_alarm_ind_conf_pending)&&(!m_lights_alarm_ind_conf_pending)&&(!m_hums_alarm_ind_conf_pending))
				{
        // Send value if connected and notifying
        if ((p_lights->conn_handle != BLE_CONN_HANDLE_INVALID) && p_lights->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle   = p_lights->climate_light_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = alarm;

            err_code = sd_ble_gatts_hvx(p_lights->conn_handle, &hvx_params);
						p_lights->lights_alarm_with_time_stamp[0] = alarm[0];
						if(err_code == NRF_SUCCESS)
						{
							m_lights_alarm_ind_conf_pending = true;
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
    twi_turn_OFF();

    return current_light_level;
}	


