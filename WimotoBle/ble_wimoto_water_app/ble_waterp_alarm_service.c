/** @file
*
* @{
* @ingroup ble_sdk_app_hts
* @brief water presence Service file.
*
* This file contains the source code for water  presence alarm service
* It also includes the code for Device Information services.
* This application uses the @ref srvlib_conn_params module.
* Adapted from  Battery service provided by Nordic
* 
* Author : Sherin
* Date   : 11/19/2013
* Change log:
* Sherin    		    12/10/2013     Added write events for value fields
* Hariprasad        12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* sruthiraj         01/10/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
* Sruthi.k.s				10/17/2014		 Added time stamp with alarm services
* Shafy S           10/28/2014     Added changes to show last occurance timestamp of alarm
* Sruthi.k.s				11/07/2014 		 Changed the notification property of alarm characteristics to indication
*/

#include "ble_waterp_alarm_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "wimoto_sensors.h"

extern bool 			WATERP_EVENT_FLAG;              /* This flag indicates whether there is an event on gpiote */
bool     	    		WATERPS_CONNECTED_STATE=false;  /* Indicates whether the water presence service is connected or not*/
extern uint8_t		var_receive_uuid;								/*variable to receive uuid*/
extern uint8_t	  curr_waterpresence;             /* water presence value for broadcast*/
extern bool       CHECK_ALARM_TIMEOUT;
bool              waterp_alarm_set_changed = false;
bool     					m_waterps_alarm_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending. */
/**@brief Function for handling the Connect event.
*
* @param[in]   p_waterps   water presence Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_connect(ble_waterps_t * p_waterps, ble_evt_t * p_ble_evt)
{
    p_waterps->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    WATERPS_CONNECTED_STATE = true;  /*Set the flag to true so that state remains in connectable mode until disconnect*/
		CHECK_ALARM_TIMEOUT = true;			 /*Set the flag to do alarm check on connection*/
}


/**@brief Function for handling the Disconnect event.
*
* @param[in]   p_waterps   water presence Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_disconnect(ble_waterps_t * p_waterps, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    WATERPS_CONNECTED_STATE= false; 
    p_waterps->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling write events on values.
*
* @details This function will be called for all write events of water presence low, high values and alarm set 
*          which are passed to the application.
*/
static void write_evt_handler(void)
{   
    WATERP_EVENT_FLAG = true; 
}

static void on_waterps_evt(ble_waterps_t * p_waterps, ble_waterps_alarm_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_WATERPS_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single temperature measurement
            //temperature_measurement_send();
            break;

        case BLE_WATERPS_EVT_INDICATION_CONFIRMED:
            m_waterps_alarm_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
}
/**@brief Function for handling the Write event.
*
* @param[in]   p_waterps   water presence Service structure.
* @param[in]   p_ble_evt   Event received from the BLE stack.
*/
static void on_write(ble_waterps_t * p_waterps, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_waterps->is_notification_supported)
    {

        //write event for water presence alarm set

        if (
                (p_evt_write->handle == p_waterps->water_waterp_alarm_set_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterps->evt_handler != NULL)
            {
                ble_waterps_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_waterps->evt_handler(p_waterps, &evt);
            }
        } 
        //write event for current water presence

        if (
                (p_evt_write->handle == p_waterps->current_waterp_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterps->evt_handler != NULL)
            {
                ble_waterps_low_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERPS_LOW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERPS_LOW_EVT_NOTIFICATION_DISABLED;
                }

                p_waterps->evt_handler(p_waterps, &evt);
            }
        }
    }
				//write event for water presence alarm

        if (
                (p_evt_write->handle == p_waterps->water_waterp_alarm_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (p_waterps->evt_handler != NULL)
            {
                ble_waterps_alarm_evt_t evt;

                if (ble_srv_is_indication_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_WATERPS_EVT_INDICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_WATERPS_EVT_INDICATION_DISABLED;
                }

                on_waterps_evt(p_waterps, &evt);
            }
        } 
    // Write event for alarm set char value

    if (
            (p_evt_write->handle == p_waterps->water_waterp_alarm_set_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            )
    {
        // update the temperature service structure
        p_waterps->water_waterpresence_alarm_set =   p_evt_write->data[0];

			  //set the flag to indicate the alarm set characteristics is changed
			  waterp_alarm_set_changed = true;
        // call application event handler
        p_waterps->write_evt_handler();
    }
}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_temps_t   temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_hvc(ble_waterps_t * p_waterps, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;
    if (p_hvc->handle == p_waterps->water_waterp_alarm_handles.value_handle)
    {
        ble_waterps_alarm_evt_t evt;
        evt.evt_type = BLE_WATERPS_EVT_INDICATION_CONFIRMED;
        on_waterps_evt(p_waterps, &evt);
    }
}
void ble_waterps_on_ble_evt(ble_waterps_t * p_waterps, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_waterps, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
				m_waterps_alarm_ind_conf_pending = false;															//Clear pending confirmation flag if a disconnect occurs
        on_disconnect(p_waterps, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_waterps, p_ble_evt);
        break;
		case BLE_GATTS_EVT_HVC:    //Handle Value Confirmation event
            on_hvc(p_waterps, p_ble_evt);
            break;	
    default:
        break;
    }

}


/**@brief Function for adding the current water presence characteristics.
*
* @param[in]   p_waterps        water presence Service structure.
* @param[in]   p_waterps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t current_waterpresence_char_add(ble_waterps_t * p_waterps, const ble_waterps_init_t * p_waterps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      current_waterpresence;

    current_waterpresence= 0x00;
		
    if (p_waterps->is_notification_supported)
    {   
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_waterps_init->waterpresence_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }   

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_waterps->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_waterps->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;

    ble_uuid.type = p_waterps->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERPS_CURR_WATERP_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_waterps_init->waterpresence_char_attr_md.read_perm;
    attr_md.write_perm = p_waterps_init->waterpresence_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(current_waterpresence);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(current_waterpresence);
    attr_char_value.p_value      = &current_waterpresence;

    err_code = sd_ble_gatts_characteristic_add(p_waterps->service_handle, &char_md,
    &attr_char_value,
    &p_waterps->current_waterp_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the water presence alarm set characteristics.
*
* @param[in]   p_waterps        water presence Service structure.
* @param[in]   p_waterps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t waterpresence_alarm_set_char_add(ble_waterps_t * p_waterps, const ble_waterps_init_t * p_waterps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      water_waterpresence_alarm_set;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
		char_md.char_props.write    = 1;                   /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp = 1;              /*add fix for characteristic write issue*/
    char_md.char_props.notify = 0;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_waterps->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERPS_ALARM_SET_CHAR_UUID;		

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_waterps_init->waterpresence_char_attr_md.read_perm;
    attr_md.write_perm = p_waterps_init->waterpresence_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    water_waterpresence_alarm_set = p_waterps_init->water_waterpresence_alarm_set;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &water_waterpresence_alarm_set;

    err_code = sd_ble_gatts_characteristic_add(p_waterps->service_handle, &char_md,
    &attr_char_value,
    &p_waterps->water_waterp_alarm_set_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding the water presence alarm characteristics.
*
* @param[in]   p_waterps        water presence Service structure.
* @param[in]   p_waterps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t waterpresence_alarm_char_add(ble_waterps_t * p_waterps, const ble_waterps_init_t * p_waterps_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
	
		/* array for receiving alarm with time stamp from water presence service structure*/
    static uint8_t      waterps_alarm_with_time_stamp[8];

    // Add water presence alarm characteristic 
    if (p_waterps->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_waterps_init->waterpresence_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   	= 1;
    char_md.char_props.indicate = (p_waterps->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf        	  = NULL;
    char_md.p_user_desc_md    	= NULL;
    char_md.p_cccd_md         	= (p_waterps->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         	= NULL;

    ble_uuid.type = p_waterps->uuid_type;
    ble_uuid.uuid = WATER_PROFILE_WATERPS_ALARM_CHAR_UUID;			

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_waterps_init->waterpresence_char_attr_md.read_perm;
    attr_md.write_perm = p_waterps_init->waterpresence_char_attr_md2.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
		
		//assigning alarm with time stamp of water presence characteristics to alarm with time stamp array
    waterps_alarm_with_time_stamp[0] = p_waterps_init->waterps_alarm_with_time_stamp[0];
		waterps_alarm_with_time_stamp[1] = p_waterps_init->waterps_alarm_with_time_stamp[1];
		waterps_alarm_with_time_stamp[2] = p_waterps_init->waterps_alarm_with_time_stamp[2];
		waterps_alarm_with_time_stamp[3] = p_waterps_init->waterps_alarm_with_time_stamp[3];
		waterps_alarm_with_time_stamp[4] = p_waterps_init->waterps_alarm_with_time_stamp[4];
		waterps_alarm_with_time_stamp[5] = p_waterps_init->waterps_alarm_with_time_stamp[5];
		waterps_alarm_with_time_stamp[6] = p_waterps_init->waterps_alarm_with_time_stamp[6];
		waterps_alarm_with_time_stamp[7] = p_waterps_init->waterps_alarm_with_time_stamp[7];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(waterps_alarm_with_time_stamp);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(waterps_alarm_with_time_stamp);
    attr_char_value.p_value      = waterps_alarm_with_time_stamp;

    err_code = sd_ble_gatts_characteristic_add(p_waterps->service_handle, &char_md,
    &attr_char_value,
    &p_waterps->water_waterp_alarm_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/**@brief Function for initializing the water presence.
*
* @param[in]   p_waterps        water presence Service structure.
* @param[in]   p_waterps_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_waterps_init(ble_waterps_t * p_waterps, const ble_waterps_init_t * p_waterps_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
    
    // Add service
    ble_uuid.type = var_receive_uuid;
		p_waterps->uuid_type=var_receive_uuid;
    ble_uuid.uuid = WATER_PROFILE_WATERPS_SERVICE_UUID;

    // Initialize service structure
    p_waterps->evt_handler                    = p_waterps_init->evt_handler;
    p_waterps->write_evt_handler              = write_evt_handler;
    p_waterps->conn_handle                    = BLE_CONN_HANDLE_INVALID;
    p_waterps->is_notification_supported      = p_waterps_init->support_notification;
	
    p_waterps->water_waterpresence_alarm_set  = p_waterps_init->water_waterpresence_alarm_set;     



    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_waterps->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  current_waterpresence_char_add(p_waterps, p_waterps_init);    /* Add current water presence characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  waterpresence_alarm_set_char_add(p_waterps, p_waterps_init);  /* Add water presence alarm set characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  waterpresence_alarm_char_add(p_waterps, p_waterps_init);     /* Add water presence alarm characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}


/**@brief Function reads and updates the current water presence and checks for alarm condition.
*  Executes only when an event occurs on gpiote pin for water presence
*
* @param[in]   p_waterps        water presence  Service structure.
*
* @param[in]   p_device       Device management Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_waterps_alarm_check(ble_waterps_t * p_waterps,ble_device_t *p_device)
{
    uint32_t err_code;
    uint8_t  current_waterpresence;       				/* Current water presence*/
		uint8_t waterp_pin_reading;
    static uint8_t alarm[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint16_t len1 = sizeof(uint8_t);
		uint16_t len	= 8;	//length of alarm with time stamp characteristics
	
		//Set up necessary pins for presence measurement
		nrf_gpio_cfg_input(WATERP_GPIOTE_PIN,GPIO_PIN_CNF_PULL_Disabled);           /* Configure pin p0.01 as input with pull-up disabled*/
		nrf_gpio_cfg_output(WATER_SENSOR_ENERGIZE_PIN);                             /* Configure P0.02 as output to energize the water presence sensor */
		 
    nrf_gpio_pin_set(WATER_SENSOR_ENERGIZE_PIN);                    /* Set the value of P0.02 to high for water presence sensor*/
		delay_ms(10);																										/*delay for sensor response*/
    waterp_pin_reading = nrf_gpio_pin_read(WATERP_GPIOTE_PIN);			/*read the current water presence from p0.01. Active Low voltage indicates water presence*/
		delay_ms(10);																										/*delay for sensor response*/
		nrf_gpio_pin_clear(WATER_SENSOR_ENERGIZE_PIN);	                /* Clear the pin P0.02 after reading*/
		
		//Disable presence measurement pins
		NRF_GPIO->PIN_CNF[WATERP_GPIOTE_PIN] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
		
		NRF_GPIO->PIN_CNF[WATER_SENSOR_ENERGIZE_PIN] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
		
		current_waterpresence = !(waterp_pin_reading);									/* Active Low voltage in pin indicates water presence.So invert the waterp_pin_reading */
		curr_waterpresence =current_waterpresence; 											/*copy the current waterpresence value for global broadcast data*/							

		
    if ((p_waterps->conn_handle != BLE_CONN_HANDLE_INVALID) && p_waterps->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle   = p_waterps->current_waterp_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len1;
        hvx_params.p_data   = &current_waterpresence;

        err_code = sd_ble_gatts_hvx(p_waterps->conn_handle, &hvx_params);  /*Send the current water presence value*/
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

		if(p_waterps->water_waterpresence_alarm_set == 0x01)
    {
					
			  if (current_waterpresence == WATER_PRESENT)					/*Check the presence of water */
        {
            alarm[0] = 0x01;																/*If water is present set the alarm and capture the timestamp of last occurance*/
						alarm[1] = p_device->device_time_stamp_set[0];  
						alarm[2] = p_device->device_time_stamp_set[1];
						alarm[3] = p_device->device_time_stamp_set[2];
						alarm[4] = p_device->device_time_stamp_set[3];
						alarm[5] = p_device->device_time_stamp_set[4];
						alarm[6] = p_device->device_time_stamp_set[5];
						alarm[7] = p_device->device_time_stamp_set[6];
				}
        
    }
    else if(waterp_alarm_set_changed)
    {	
        								              
				alarm[0]=0x00;																		/* If alarm set is cleared, reset alarm and clear timestamp*/
				alarm[1]=0x00;
				alarm[2]=0x00;
				alarm[3]=0x00;
				alarm[4]=0x00;
				alarm[5]=0x00;
				alarm[6]=0x00;
				alarm[7]=0x00;
			
				waterp_alarm_set_changed = false;
				sd_ble_gatts_value_set(p_waterps->water_waterp_alarm_handles.value_handle, 0, &len, alarm);  /*clear the value of alarm characteristics*/
				p_waterps->waterps_alarm_with_time_stamp[0] = alarm[0];
				
    }	
		

    if((alarm[0]== 0x01)&&(p_waterps->water_waterpresence_alarm_set == 0x01)) 	/*check whether the alarm is ON and alarm set characteristics is ON*/
    {	
				if(!m_waterps_alarm_ind_conf_pending)
				{
        // Send the alarm value if connected and notifying

        if ((p_waterps->conn_handle != BLE_CONN_HANDLE_INVALID) && p_waterps->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));


            hvx_params.handle   = p_waterps->water_waterp_alarm_handles.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &len;
            hvx_params.p_data   = alarm;

            err_code = sd_ble_gatts_hvx(p_waterps->conn_handle, &hvx_params);
						p_waterps->waterps_alarm_with_time_stamp[0]  = alarm[0]; 
						if(err_code == NRF_SUCCESS)
						{
							m_waterps_alarm_ind_conf_pending = true;
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
