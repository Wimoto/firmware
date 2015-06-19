/** @file
*
* @brief Device Management Service file.
*
* This file contains the source code for Device Management service
* 
* Author : Hariprasad C R
* Date   : 11/20/2013
* Change log:
* Sherin    		 11/29/2013     Added Time sync characteristic to the profile
* Sherin         12/10/2013     Added write event to value fields
* sruthi.k.s     01/10/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
*
*/

#include "ble_device_mgmt_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "ble_date_time.h"

bool   DEVICE_CONNECTED_STATE = false;                  /* This flag indicates whether a client is connected to the peripheral or not*/
bool   DFU_ENABLE             = false;                  /* Flah to check whether DFU feature has been enabled or not*/
extern ble_date_time_t m_time_stamp;                    /* Time stamp defined in connect.c. */
extern bool TIME_SET;                                   /* Flag to start time updation, defined in connect.c*/
extern uint8_t	 var_receive_uuid;

/**@brief Function for handling the Connect event.
*
* @param[in]   p_device    Device Management Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_device_t * p_device, ble_evt_t * p_ble_evt)
{
    p_device->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    DEVICE_CONNECTED_STATE = true;                      /* Set the flag to true so that state remains in connectable mode until disconnect*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_device    Device Management Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_device_t * p_device, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    DEVICE_CONNECTED_STATE= false;                     /* Set the flag to false so that state remains in disconnected*/       
    p_device->conn_handle = BLE_CONN_HANDLE_INVALID; 
}

/**@brief Function for handling the write event.
*
* @param[in]   p_device    Device Management Service structure.
* @param[in]   p_evt       Device Management Service write event type.
*/

static void write_evt_handler (ble_device_t * p_device, ble_device_write_evt_t * p_evt)
{	  
    switch (p_evt->evt_type)
    {
    case BLE_DEVICE_DFU_WRITE:
        if(p_device->device_dfu_mode_set != 0x00)
        {
            DFU_ENABLE = true;
        }	
        else
        {
            DFU_ENABLE = false;
        }	 
        break;
    case BLE_DEVICE_TIME_STAMP_WRITE:
        TIME_SET = true;
        break;  
    default:
        break;
    }
}

/**@brief Function for handling the Write event.
*
* @param[in]   p_device    Device Management Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/

static void on_write(ble_device_t * p_device, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_device->is_notification_supported)
    {
        /*Write event for dfu mode set cccd*/

        if (
                (p_evt_write->handle == p_device->dfu_mode_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {   
            // CCCD written, call application event handler
            if (p_device->evt_handler != NULL)
            {
                ble_device_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_DEVICE_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_DEVICE_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_device->evt_handler(p_device, &evt);

            }
        } 

        /*Write event for current time characteristics cccd*/
        if (
                (p_evt_write->handle == p_device->time_stamp_handles.cccd_handle)
                &&
                (p_evt_write->len == 2 )
                )
        {   
            // CCCD written, call application event handler
            if (p_device->evt_handler != NULL)
            {   
                ble_device_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_DEVICE_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_DEVICE_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_device->evt_handler(p_device, &evt);
            }
        }


    }




    /*Write event for mode dfu mode char value.*/

    if (
            (p_evt_write->handle == p_device->dfu_mode_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            &&
            (p_device->write_evt_handler != NULL)
            )
    {  
        ble_device_write_evt_t evt;
        evt.evt_type           = BLE_DEVICE_DFU_WRITE;

        // update the service structure
        p_device->device_dfu_mode_set =   p_evt_write->data[0];

        // call application event handler
        p_device->write_evt_handler(p_device, &evt);
    }

    /*Write event for current time characteristics value*/
    if (
            (p_evt_write->handle == p_device->time_stamp_handles.value_handle)
            &&
            (p_evt_write->len == 7)
            &&
            (p_device->write_evt_handler != NULL)
            )
    {  
        ble_device_write_evt_t evt;
        evt.evt_type           = BLE_DEVICE_TIME_STAMP_WRITE;

        // update the service structure
        p_device->device_time_stamp_set[0] =   p_evt_write->data[0];
        p_device->device_time_stamp_set[1] =   p_evt_write->data[1];
        p_device->device_time_stamp_set[2] =   p_evt_write->data[2];
        p_device->device_time_stamp_set[3] =   p_evt_write->data[3];
        p_device->device_time_stamp_set[4] =   p_evt_write->data[4];
        p_device->device_time_stamp_set[5] =   p_evt_write->data[5];
        p_device->device_time_stamp_set[6] =   p_evt_write->data[6];

        // call application event handler
        p_device->write_evt_handler(p_device, &evt);
    }
}
void ble_device_on_ble_evt(ble_device_t * p_device, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_device, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_device, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_device, p_ble_evt);
        break;

    default:
        break;
    }

}

/**@brief Function for adding the characteristics for Device firmware update the mode.
*
* @param[in]   p_device       Device Management Service structure.
* @param[in]   p_device_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t dfu_mode_char_add(ble_device_t * p_device, const ble_device_init_t * p_device_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      dfu_mode_char = 0x00;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write    = 1;                                 /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                            /*add fix for characteristic write issue*/
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    //Adding custom UUID
    ble_uuid.type = p_device->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_DEVICE_DFU_MODE_CHAR_UUID;       

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_device_init->device_char_attr_md.read_perm;
    attr_md.write_perm = p_device_init->device_char_attr_md.write_perm;
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
    attr_char_value.p_value      = &dfu_mode_char;

    err_code = sd_ble_gatts_characteristic_add(p_device->service_handle, &char_md,
    &attr_char_value,
    &p_device->dfu_mode_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the characteristics to timestamp from Peripheral to Broadcast.
*
* @param[in]   p_device       Device Management Service structure.
* @param[in]   p_device_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t time_stamp_char_add(ble_device_t * p_device, const ble_device_init_t * p_device_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      time_stamp_char[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};


    if (p_device->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        // According to BAS_SPEC_V10, the read operation on cccd should be possible without authentication
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_device_init->device_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write    = 1;                                /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                           /*add fix for characteristic write issue*/
    char_md.char_props.notify = (p_device->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_device->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    // Adding custom UUID   
    BLE_UUID_BLE_ASSIGN(ble_uuid, SENTRY_PROFILE_DEVICE_TIME_STAMP_CHAR_UUID);  /* Using a standard UUID for Current time*/
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_device_init->device_char_attr_md.read_perm;
    attr_md.write_perm = p_device_init->device_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    time_stamp_char[0] = p_device_init->device_time_stamp_set[0];
    time_stamp_char[1] = p_device_init->device_time_stamp_set[1];		
    time_stamp_char[2] = p_device_init->device_time_stamp_set[2];
    time_stamp_char[3] = p_device_init->device_time_stamp_set[3];		
    time_stamp_char[4] = p_device_init->device_time_stamp_set[4];
    time_stamp_char[5] = p_device_init->device_time_stamp_set[5];		
    time_stamp_char[6] = p_device_init->device_time_stamp_set[6];


    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(time_stamp_char);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(time_stamp_char);
    attr_char_value.p_value      = time_stamp_char;

    err_code = sd_ble_gatts_characteristic_add(p_device->service_handle, &char_md,
    &attr_char_value,
    &p_device->time_stamp_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding MMA switch characteristic.
*
* @param[in]   p_device       Device Management Service structure.
* @param[in]   p_device_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t mma_switch_char_add(ble_device_t * p_device, const ble_device_init_t * p_device_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      mma_status = 0x01;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write  = 1;                                	/*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;                           /*add fix for characteristic write issue*/
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_user_desc  =	NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    //Adding custom UUID
    ble_uuid.type = p_device->uuid_type;
    ble_uuid.uuid = SENTRY_PROFILE_DEVICE_MMA_SWITCH_CHAR_UUID;       

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_device_init->device_char_attr_md.read_perm;
    attr_md.write_perm = p_device_init->device_char_attr_md.write_perm;
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
    attr_char_value.p_value      = &mma_status;

    err_code = sd_ble_gatts_characteristic_add(p_device->service_handle, &char_md,
    &attr_char_value,
    &p_device->mma_switch_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for initializing the Device management service.
*
* @param[in]   p_device        Device Management Service structure.
* @param[in]   p_device_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_device_init(ble_device_t * p_device, const ble_device_init_t * p_device_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    ble_uuid.type = var_receive_uuid;
		p_device->uuid_type= var_receive_uuid;
    ble_uuid.uuid = SENTRY_PROFILE_DEVICE_MGMT_SERVICE_UUID;

    // Initialize service structure 
    p_device->evt_handler               = p_device_init->evt_handler;
    p_device->write_evt_handler         = write_evt_handler;
    p_device->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_device->is_notification_supported = p_device_init->support_notification;
    p_device->device_dfu_mode_set       = p_device_init->device_dfu_mode_set;    
		p_device->device_mma_switch_set			= p_device_init->device_mma_switch_set;

    // Add service 
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_device->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  dfu_mode_char_add(p_device, p_device_init);    /* Add device firmware update characteristic */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  time_stamp_char_add(p_device, p_device_init);  /* Add time stamp characteristic */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		
		err_code = mma_switch_char_add(p_device, p_device_init);	/* Add function to turn on and off accelerometer, power savings! */
		if (err_code != NRF_SUCCESS)
		{
				return err_code;
		}

    return NRF_SUCCESS;

}

/**@brief Function to get the time set by the user and create a time stamp structure
*
* @param[in]   p_device         Device Management Service structure.
* @param[in]   p_time_stamp     Time stamp structure.
*
* 
*/

void create_time_stamp(ble_device_t * p_device, ble_date_time_t * p_time_stamp)
{
    /*Create new time stamp  */
    p_time_stamp->year    = (p_device->device_time_stamp_set[0]<<8)|p_device->device_time_stamp_set[1];
    p_time_stamp->month   = p_device->device_time_stamp_set[2];
    p_time_stamp->day     = p_device->device_time_stamp_set[3];
    p_time_stamp->hours   = p_device->device_time_stamp_set[4];
    p_time_stamp->minutes = p_device->device_time_stamp_set[5];
    p_time_stamp->seconds = p_device->device_time_stamp_set[6];

}


/**@brief Function updates the current time to the client.
*
* @param[in]   p_device         Device Management Service structure.
* @param[in]   p_time_stamp     Time stamp structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_time_update(ble_device_t * p_device, ble_date_time_t *p_time_stamp)
{
    uint32_t err_code = NRF_SUCCESS;  
    uint16_t  len = 7;

    //update the service structure
    p_device->device_time_stamp_set[0]= ((p_time_stamp->year)& 0xFF00)>>8;
    p_device->device_time_stamp_set[1]= p_time_stamp->year;
    p_device->device_time_stamp_set[2]= p_time_stamp->month;
    p_device->device_time_stamp_set[3]= p_time_stamp->day;
    p_device->device_time_stamp_set[4]= p_time_stamp->hours;
    p_device->device_time_stamp_set[5]= p_time_stamp->minutes;
    p_device->device_time_stamp_set[6]= p_time_stamp->seconds;


    if ((p_device->conn_handle != BLE_CONN_HANDLE_INVALID) && p_device->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle   = p_device->time_stamp_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len;
        hvx_params.p_data   = p_device->device_time_stamp_set;

        err_code = sd_ble_gatts_hvx(p_device->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }
    return err_code;

}
