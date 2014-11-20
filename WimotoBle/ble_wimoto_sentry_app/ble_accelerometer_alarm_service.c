/** @file
*
* @brief Movement Service file.
*
* This file contains the source code for Accelerometer/Movement alarm service
* It also includes the code for Device Information services.
* This application uses the @ref srvlib_conn_params module.
* Adapted from  Battery service provided by Nordic
*
* Author : Haiprasad C R
* Date   : 11/29/2013
* Change log
* Sherin         12/10/2013       Added write event to value fields
* sruthi.k.s     01/10/2014       Migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s	 	 10/17/2014		  	Added time stamp with alarm service
* Shafy S        10/31/2014       Added changes in the alarm timestamp for sending last occurance of alarm on connection
* Sruthi.k.s  	 11/07/2014     	Changed the notification property of alarm characteristics to indication
*/

#include <string.h>
#include <stdio.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"
#include "ble_accelerometer_alarm_service.h"
#include "app_error.h"

extern   bool       	MOVEMENT_EVENT_FLAG;
extern   bool 	      CLEAR_MOVE_ALARM;
extern   uint8_t      movement_gpio_pin_val;
bool 					        ACCELEROMETER_CONNECTED_STATE=false; /*Indicates whether the movement service is connected or not*/
				 uint8_t      current_xyz_array[3];                /* Read value of X Y Z data*/
extern   uint8_t	    var_receive_uuid;						/*variable to receive uuid*/
extern 	 uint32_t     xyz_coordinates;           /*accelerometer value for broadcast*/
static    uint8_t 		movement_alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; /*movement alarm with timestamp*/
extern   bool         CENTRAL_DEVICE_CONNECTED;

bool     							m_movement_alarm_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending. */
extern bool     			m_pir_alarm_ind_conf_pending;
/**@brief Function for handling the Connect event.
*
* @param[in]   p_movement  Movement Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_movement_t * p_movement, ble_evt_t * p_ble_evt)
{
    p_movement->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    ACCELEROMETER_CONNECTED_STATE = true;   /*Set the flag to true so that state remains in connectable mode until disconnect*/
		CENTRAL_DEVICE_CONNECTED  = true;
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_movement  Movement Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_movement_t * p_movement, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ACCELEROMETER_CONNECTED_STATE= false;
    p_movement->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the write event.
*
* @param[in]   p_movement    Movement Service structure.
* @param[in]   p_evt         Movement Service write event type.
*/

static void write_evt_handler (ble_movement_t * p_movement, ble_movement_write_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
    case BLE_MOVEMENT_ALARM_SET_WRITE:
        if(p_movement->movement_alarm_set != 0x00)
        {
            MOVEMENT_EVENT_FLAG = true;
        }
        else
        {
            MOVEMENT_EVENT_FLAG = false;
        }
        break;
    case BLE_MOVEMENT_ALARM_CLEAR_WRITE:

        if(p_movement-> movement_alarm_clear!= 0x00)
        {
            CLEAR_MOVE_ALARM = true;
        }
        else
        {
            CLEAR_MOVE_ALARM = false;
        }
        break;
    default:
        break;
    }
}

static void on_movement_evt(ble_movement_t * p_movement, ble_movement_alarm_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_MOVEMENT_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single temperature measurement
            //temperature_measurement_send();
            break;

        case BLE_MOVEMENT_EVT_INDICATION_CONFIRMED:
            m_movement_alarm_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
}
/**@brief Function for handling the Write event.
*
* @param[in]   p_movement  Movement Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_movement_t * p_movement, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_movement -> is_notification_supported)
    {
        //write event for Movement alarm set cccd

        if (
                (p_evt_write->handle == p_movement->movement_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_movement->evt_handler != NULL)
            {
                ble_movement_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_MOVEMENT_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_MOVEMENT_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_movement->evt_handler(p_movement, &evt);
            }
        }

        //write event for movement alarm clear cccd

        if (
                (p_evt_write->handle == p_movement->movement_alarm_clear_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_movement->evt_handler != NULL)
            {
                ble_movement_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_MOVEMENT_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_MOVEMENT_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_movement->evt_handler(p_movement, &evt);
            }
        }

        //write event for current x,y,z  level cccd

        if (
                (p_evt_write->handle == p_movement->current_movement_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_movement->evt_handler != NULL)
            {
                ble_movement_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_MOVEMENT_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_MOVEMENT_LOW_EVT_NOTIFICATION_ENABLED;
                }

                p_movement->evt_handler(p_movement, &evt);
            }
        }

    }
       //write event for movement alarm cccd

        if (
                (p_evt_write->handle == p_movement->movement_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_movement->evt_handler != NULL)
            {
                ble_movement_alarm_evt_t evt;

                if (ble_srv_is_indication_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_MOVEMENT_EVT_INDICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_MOVEMENT_EVT_INDICATION_DISABLED;
                }

                on_movement_evt(p_movement, &evt);
            }
        }
    // Write event for alarm set char value

    if (
            (p_evt_write->handle == p_movement->movement_alarm_set_handles.value_handle)
            &&
            (p_evt_write->len == 1)
            &&
            (p_movement->write_evt_handler != NULL)
            )
    {
        ble_movement_write_evt_t evt;
        evt.evt_type           = BLE_MOVEMENT_ALARM_SET_WRITE;

        // update the service structure
        p_movement->movement_alarm_set =   p_evt_write->data[0];

        // call application event handler
        p_movement->write_evt_handler(p_movement, &evt);
    }

    // Write event for alarm clear char value

    if (
            (p_evt_write->handle == p_movement->movement_alarm_clear_handles.value_handle)
            &&
            (p_evt_write->len == 1)
            &&
            (p_movement->write_evt_handler != NULL)
            )
    {
        ble_movement_write_evt_t evt;
        evt.evt_type           = BLE_MOVEMENT_ALARM_CLEAR_WRITE;

        // update the service structure
        p_movement->movement_alarm_clear =   p_evt_write->data[0];

        // call application event handler
        p_movement->write_evt_handler(p_movement, &evt);
    }


}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_temps_t   temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_hvc(ble_movement_t * p_movement, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;
    if (p_hvc->handle == p_movement->movement_alarm_handles.value_handle)
    {
        ble_movement_alarm_evt_t evt;
        evt.evt_type = BLE_MOVEMENT_EVT_INDICATION_CONFIRMED;
        on_movement_evt(p_movement, &evt);
    }
}
void ble_movement_on_ble_evt(ble_movement_t * p_movement, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_movement, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_movement, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_movement, p_ble_evt);
        break;
		case BLE_GATTS_EVT_HVC:    //Handle Value Confirmation event
            on_hvc(p_movement, p_ble_evt);
            break;
    default:
        break;
    }

}


/**@brief Function for adding the current X Y Z data characteristics.
*
* @param[in]   p_movement      Movement Service structure.
* @param[in]   p_movement_init Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_xyz_coordinates_char_add(ble_movement_t * p_movement, const ble_movement_init_t * p_movement_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_coordinates[3];

    if (p_movement->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_movement_init->movement_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_movement->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_movement->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;

    ble_uuid.type = p_movement->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_MOVEMENT_CURR_XYZ_POS_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_movement_init->movement_char_attr_md.read_perm;
    attr_md.write_perm = p_movement_init->movement_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_coordinates);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_coordinates);
    attr_char_value.p_value      = current_coordinates;

    err_code = sd_ble_gatts_characteristic_add(p_movement->service_handle, &char_md,
    &attr_char_value,
    &p_movement->current_movement_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the movement alarm set characteristics.
*
* @param[in]   p_movement        movement Service structure.
* @param[in]   p_movement_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t movement_alarm_set_char_add(ble_movement_t * p_movement, const ble_movement_init_t * p_movement_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      movement_alarm_set;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write    = 1;                           /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp= 1;                       /*add fix for characteristic write issue*/
    char_md.char_props.notify = 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    // Adding cutom UUID
    ble_uuid.type = p_movement->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_MOVEMENT_ALARM_SET_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_movement_init->movement_char_attr_md.read_perm;
    attr_md.write_perm = p_movement_init->movement_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    movement_alarm_set = p_movement_init->movement_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &movement_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_movement->service_handle, &char_md,
    &attr_char_value,
    &p_movement->movement_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the movement alarm clear characteristics.
*
* @param[in]   p_movement        movement Service structure.
* @param[in]   p_movement_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t movement_alarm_clear_char_add(ble_movement_t * p_movement, const ble_movement_init_t * p_movement_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      movement_alarm_clear;

    // Add movement alarm clear characteristic
    if (p_movement->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_movement_init->movement_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write    = 1;                                /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp= 1;                            /*add fix for characteristic write issue*/
    char_md.char_props.notify = (p_movement->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_movement->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    // Adding cutom UUID
    ble_uuid.type = p_movement->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_MOVEMENT_ALARM_CLEAR_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_movement_init->movement_char_attr_md.read_perm;
    attr_md.write_perm = p_movement_init->movement_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    movement_alarm_clear = p_movement_init->movement_alarm_clear;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &movement_alarm_clear;

    err_code = sd_ble_gatts_characteristic_add(p_movement->service_handle, &char_md,
    &attr_char_value,
    &p_movement->movement_alarm_clear_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the movement alarm characteristics.
*
* @param[in]   p_movement       Movement Service structure.
* @param[in]   p_movement_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t movement_alarm_char_add(ble_movement_t * p_movement, const ble_movement_init_t * p_movement_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
	//array to read time stamp withalarm characteristics
    static uint8_t      move_alarm_with_time_stamp[8];

    // Add movement alarm characteristic
    if (p_movement->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_movement_init->movement_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.indicate = (p_movement->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_movement->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    // Adding cutom UUID
    ble_uuid.type = p_movement->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_MOVEMENT_ALARM_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_movement_init->movement_char_attr_md.read_perm;
    attr_md.write_perm = p_movement_init->movement_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;


    move_alarm_with_time_stamp[0]= p_movement_init->move_alarm_with_time_stamp[0];
		move_alarm_with_time_stamp[1]= p_movement_init->move_alarm_with_time_stamp[1];
		move_alarm_with_time_stamp[2]= p_movement_init->move_alarm_with_time_stamp[2];
		move_alarm_with_time_stamp[3]= p_movement_init->move_alarm_with_time_stamp[3];
		move_alarm_with_time_stamp[4]= p_movement_init->move_alarm_with_time_stamp[4];
		move_alarm_with_time_stamp[5]= p_movement_init->move_alarm_with_time_stamp[5];
		move_alarm_with_time_stamp[6]= p_movement_init->move_alarm_with_time_stamp[6];
		move_alarm_with_time_stamp[7]= p_movement_init->move_alarm_with_time_stamp[7];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(move_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(move_alarm_with_time_stamp);
    attr_char_value.p_value      = move_alarm_with_time_stamp;

    err_code = sd_ble_gatts_characteristic_add(p_movement->service_handle, &char_md,
    &attr_char_value,
    &p_movement->movement_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the Movement service.
*
* @param[in]   p_movement        Movement Service structure.
* @param[in]   p_movement_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_movement_init(ble_movement_t * p_movement, const ble_movement_init_t * p_movement_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
		ble_uuid.type = var_receive_uuid ;
		p_movement->uuid_type= var_receive_uuid;
    ble_uuid.uuid = SENTRY_PROFILE_MOVEMENT_SERVICE_UUID;

    // Initialize service structure
    p_movement->evt_handler               = p_movement_init->evt_handler;
    p_movement->write_evt_handler         = write_evt_handler;
    p_movement->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_movement->is_notification_supported = p_movement_init->support_notification;
    p_movement->movement_alarm_set        = p_movement_init->movement_alarm_set;
    p_movement->movement_alarm_clear      = p_movement_init->movement_alarm_clear;


    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_movement->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  current_xyz_coordinates_char_add(p_movement, p_movement_init); /* Add current movement characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  movement_alarm_set_char_add(p_movement, p_movement_init);     /* Add movement alarm  set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  movement_alarm_clear_char_add(p_movement, p_movement_init);   /* Add movement alarm clear characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  movement_alarm_char_add(p_movement, p_movement_init);         /* Add movement alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}


/**@brief Function reads and updates the current movement and checks for alarm condition.
*  Executes only when an event occurs on gpiote pin for movement
*
* @param[in]   p_movement        Movement  Service structure.
*
* @param[in]   p_device     		Device management  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_movement_alarm_check(ble_movement_t * p_movement,ble_device_t *p_device)
{
    uint32_t err_code;

    uint32_t current_xyz;

    uint16_t len = sizeof(movement_alarm);		//length of the time stamp with alarm
    uint16_t len1 = sizeof(current_xyz_array);

		char output[20];
	  char dimension[] = {'x','y','z'};
		uint8_t i = 0;



    twi_turn_ON();


    if(false == MMA7660_read_xyz_reg_one_time(&current_xyz))
			return false;

		//copy the current accelerometer value for broadcast
		xyz_coordinates=current_xyz;

		twi_turn_OFF();
    current_xyz_array[0] = current_xyz;
    current_xyz_array[1] = current_xyz >> 8;
    current_xyz_array[2] = current_xyz >> 16;



    if ((p_movement->conn_handle != BLE_CONN_HANDLE_INVALID) && p_movement->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle   = p_movement->current_movement_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len1;
        hvx_params.p_data   = current_xyz_array;

        err_code = sd_ble_gatts_hvx(p_movement->conn_handle, &hvx_params);

    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    if(p_movement->movement_alarm_set != 0x00)
    {
        // If the logic level on pin P0.11 is active low , which is the interrupt from the pin INT of mma7660, then set the alarm
        if (movement_gpio_pin_val == MOVEMENT)
        {
            movement_alarm[0] = SET_ALARM_FOR_MOVEMENT;
						movement_alarm[1] = p_device->device_time_stamp_set[0];					/*capture the timestamp when alarm occured*/
						movement_alarm[2] = p_device->device_time_stamp_set[1];
						movement_alarm[3] = p_device->device_time_stamp_set[2];
						movement_alarm[4] = p_device->device_time_stamp_set[3];
						movement_alarm[5] = p_device->device_time_stamp_set[4];
						movement_alarm[6] = p_device->device_time_stamp_set[5];
						movement_alarm[7] = p_device->device_time_stamp_set[6];
            movement_gpio_pin_val = 0x01;
        }

        MMA7660_ClearInterrupts();
				// Send value if connected and notifying
				if(movement_alarm[0]!= 0x00)																			/*Sent alarm*/
			  {		
					 if((!m_pir_alarm_ind_conf_pending)&&(!m_movement_alarm_ind_conf_pending))   /*check whether the confirmation for indication is pending*/
					 {
           if ((p_movement->conn_handle != BLE_CONN_HANDLE_INVALID) && p_movement->is_notification_supported)
           {
               ble_gatts_hvx_params_t hvx_params;

               memset(&hvx_params, 0, sizeof(hvx_params));

               hvx_params.handle   = p_movement->movement_alarm_handles.value_handle;
               hvx_params.type     = BLE_GATT_HVX_INDICATION;
               hvx_params.offset   = 0;
               hvx_params.p_len    = &len;
               hvx_params.p_data   = movement_alarm;

               err_code = sd_ble_gatts_hvx(p_movement->conn_handle, &hvx_params);
						   p_movement->move_alarm_with_time_stamp[0] = movement_alarm[0];
							 if(err_code == NRF_SUCCESS)
							 {
									m_movement_alarm_ind_conf_pending = true;
							 }
           }
        else
        {
					 err_code = NRF_ERROR_INVALID_STATE;
        }
				}
			}
    }


    return err_code;


}

/**@brief Function to reset alarm if user specifies alarm can be cleared.
*
* @param[in]   p_movement        Movement  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t reset_alarm(ble_movement_t * p_movement)
{
    uint32_t err_code = 0x00;
    uint8_t  alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t  clear_alarm = 0x00;
    uint16_t len = sizeof(uint8_t);
		uint16_t len1=8;

    p_movement->move_alarm_with_time_stamp[0] = alarm[0];
    p_movement->movement_alarm_clear = clear_alarm;
		if((!m_pir_alarm_ind_conf_pending)&&(!m_movement_alarm_ind_conf_pending))           /*check whether the confirmation for indication is pending*/
		{
    // Send value if connected and notifying

    if ((p_movement->conn_handle != BLE_CONN_HANDLE_INVALID) && p_movement->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle   = p_movement->movement_alarm_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_INDICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len1;
        hvx_params.p_data   = alarm;

        err_code = sd_ble_gatts_hvx(p_movement->conn_handle, &hvx_params);
				if(err_code == NRF_SUCCESS)
				{
					m_movement_alarm_ind_conf_pending = true;
				}				
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }
	  }
    if (err_code != NRF_SUCCESS)
    {
        return err_code;

    }
		// Send value if connected and notifying

    if ((p_movement->conn_handle != BLE_CONN_HANDLE_INVALID) && p_movement->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle   = p_movement->movement_alarm_clear_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len;
        hvx_params.p_data   = &clear_alarm;

        err_code = sd_ble_gatts_hvx(p_movement->conn_handle, &hvx_params);
    }
    else
    {

        err_code = NRF_ERROR_INVALID_STATE;
    }
    if (err_code != NRF_SUCCESS)
    {
        return err_code;

    }

    return NRF_SUCCESS;
}


/**@brief Function for sending the last occurance timestamp and xyz coordinates when alarm occured to central on connection
*
* @param[in]   p_movement        Movement  Service structure.
*
* @param[in]   p_device     		Device management  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
void update_movement_alarmtimestamp_on_connect(ble_movement_t * p_movement,ble_device_t *p_device)
{
    uint32_t err_code = 0x00;
		uint16_t len = sizeof(movement_alarm);		//length of the time stamp with alarm
    uint16_t len1 = sizeof(current_xyz_array);


    if(p_movement->movement_alarm_set != 0x00)
    {
				// Send value if connected and notifying
				if(movement_alarm[0]!= 0x00)																			/*Sent alarm*/
			  {		
					 if((!m_pir_alarm_ind_conf_pending)&&(!m_movement_alarm_ind_conf_pending))  /*check whether the confirmation for indication is pending*/
					 {
           if ((p_movement->conn_handle != BLE_CONN_HANDLE_INVALID) && p_movement->is_notification_supported)
           {
               ble_gatts_hvx_params_t hvx_params;

               memset(&hvx_params, 0, sizeof(hvx_params));

               hvx_params.handle   = p_movement->movement_alarm_handles.value_handle;
               hvx_params.type     = BLE_GATT_HVX_INDICATION;
               hvx_params.offset   = 0;
               hvx_params.p_len    = &len;
               hvx_params.p_data   = movement_alarm;

               err_code = sd_ble_gatts_hvx(p_movement->conn_handle, &hvx_params);
						   p_movement->move_alarm_with_time_stamp[0] = movement_alarm[0];
							 if(err_code == NRF_SUCCESS)
							 {
									m_movement_alarm_ind_conf_pending = true;
							 }
           }
					 else
					 {
							err_code = NRF_ERROR_INVALID_STATE;

					 }
					 }
					 if ((err_code != NRF_SUCCESS) &&
            (err_code != NRF_ERROR_INVALID_STATE) &&
            (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
            (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
            )
					 {
							APP_ERROR_HANDLER(err_code);
					 }


					 if ((p_movement->conn_handle != BLE_CONN_HANDLE_INVALID) && p_movement->is_notification_supported)
					 {
							ble_gatts_hvx_params_t hvx_params;

							memset(&hvx_params, 0, sizeof(hvx_params));

							hvx_params.handle   = p_movement->current_movement_handles.value_handle;
							hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
							hvx_params.offset   = 0;
							hvx_params.p_len    = &len1;
							hvx_params.p_data   = current_xyz_array;

							err_code = sd_ble_gatts_hvx(p_movement->conn_handle, &hvx_params);

						}
						else
						{
								err_code = NRF_ERROR_INVALID_STATE;
						}

						if ((err_code != NRF_SUCCESS) &&
            (err_code != NRF_ERROR_INVALID_STATE) &&
            (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
            (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
            )
						{
							APP_ERROR_HANDLER(err_code);
						}


			 }

    }

}


