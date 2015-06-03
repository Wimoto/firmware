/** @file
*
* This file contains the source code for sentry alarm service application
* It also includes the code for Device Information services.
* This application uses the @ref srvlib_conn_params module.
* Adapted from  sample applications provided by Nordic
* 
* Author : Haiprasad C R
* Date   : 11/29/2013
*
* Change log:
* 
* Sherin    		 11/25/2013     Added Data logger service to the profile
* Sherin 		     01/20/2014     Changed datalogging interval to 15 minutes, included one more page for data logging
* Hariprasad C R 05/01/2014     Chaneged the clock source to LFCLKSRC_RC in ble_stack_init()
* sruthi.k.s     01/10/2014     Migrated to soft device 7.0.0 and SDK 6.1.0
* sruthiraj			 10/17/2014     Added concurrent broadcast of sensor data  in active connection
*/

#include <stdint.h>
#include <string.h>
#include "device_manager.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_data_log_service.h"
#include "ble_hts.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_gpiote.h"
#include "app_button.h"
#include "ble_error_log.h"
#include "ble_radio_notification.h"
#include "ble_flash.h"
#include "ble_debug_assert_handler.h"
#include "ble_bas.h"
#include "wimoto_sensors.h"
#include "wimoto.h"
#include "ble_device_mgmt_service.h"
#include "ble_pir_alarm_service.h"
#include "ble_accelerometer_alarm_service.h"
#include <stdbool.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "battery.h"
#include "pstorage.h"


#define DEVICE_NAME                          "Sentry_"                            /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                    "Wimoto"                                   /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM                            "Wimoto_Sentry"                            /**< Model number. Will be passed to Device Information Service. */
#define MANUFACTURER_ID                      0x1122334455                               /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                        0x667788                                   /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */
#define FIRMWARE_ID 												 "1.20"

#define APP_ADV_INTERVAL                     0x808                                      /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS           0x0000                                     /**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER                  0                                          /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS                 5                                          /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE              4                                          /**< Size of timer operation queues. */
																														 
#define SENTRY_LEVEL_MEAS_INTERVAL           APP_TIMER_TICKS(60000, APP_TIMER_PRESCALER)/**< sentry level measurement interval (ticks). */
#define CONNECTED_MODE_TIMEOUT_INTERVAL      APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)/**< Connected mode timeout interval (ticks). */
#define SECONDS_INTERVAL                     APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER) /**< seconds measurement interval (ticks). */
#define BROADCAST_INTERVAL       						 APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER) /**< updating interval of broadcast data*/ 

#define WATER_TYPE_AS_CHARACTERISTIC         0                                          /**< Determines if water type is given as characteristic (1) or as a field of measurement (0). */

#define MIN_CONN_INTERVAL                    MSEC_TO_UNITS(500, UNIT_1_25_MS)           /**< Minimum acceptable connection interval (25 milliseconds) */
#define MAX_CONN_INTERVAL                    MSEC_TO_UNITS(1000, UNIT_1_25_MS)          /**< Maximum acceptable connection interval (125 millisecond). */
#define SLAVE_LATENCY                        0                                          /**< Slave latency. */
#define CONN_SUP_TIMEOUT                     MSEC_TO_UNITS(4000, UNIT_10_MS)            /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of indication) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY        APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update . */
#define MAX_CONN_PARAMS_UPDATE_COUNT         3                                          /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_GPIOTE_MAX_USERS                 5                                          /**< Maximum number of users of the GPIOTE handler. */

#define BUTTON_DETECTION_DELAY               APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)   /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SEC_PARAM_TIMEOUT                    30                                         /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                       1                                          /**< Perform bonding. */
#define SEC_PARAM_MITM                       0                                          /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES            BLE_GAP_IO_CAPS_NONE                       /**< No I/O capabilities. */
#define SEC_PARAM_OOB                        0                                          /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE               7                                          /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE               16                                         /**< Maximum encryption key size. */

#define FLASH_PAGE_SYS_ATTR                 (BLE_FLASH_PAGE_END - 3)                    /**< Flash page used for bond manager system attribute information. */
#define FLASH_PAGE_BOND                     (BLE_FLASH_PAGE_END - 1)                    /**< Flash page used for bond manager bonding information. */

#define DEAD_BEEF                            0xDEADBEEF                                 /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

volatile bool                                m_radio_event = false;                 

uint8_t                                      movement_gpio_pin_val;
static uint16_t                              m_conn_handle = BLE_CONN_HANDLE_INVALID;   /**< Handle of the current connection. */
static ble_gap_sec_params_t                  m_sec_params;                              /**< Security requirements for this application. */
static ble_gap_adv_params_t                  m_adv_params;                              /**< Parameters to be passed to the stack when starting advertising. */
static ble_dlogs_t                           m_dlogs;																	  /**< Structure used to identify the data logger service. */
static ble_device_t                          m_device;                                  /**< Structure used to identify the Device management service. */
static ble_pir_t                             m_pir;                                     /**< Structure used to identify the Passive Infrared alarm service. */
static ble_movement_t                        m_movement;                                /**< Structure used to identify the Aceelerometer alarm service. */
static app_timer_id_t                        sentry_measurement_timer;                  /**< Sentry profile measurement timer. */
static app_timer_id_t                        real_time_timer;                           /**< Time keeping timer. */
static app_timer_id_t                        delay_timer;                               /**< Timer for implementing delay. */

app_gpiote_user_id_t 								         pir_measurement_gpiote;                    /**< PIR presence measurement gpiote. */
app_gpiote_user_id_t 	                       movement_measurement_gpiote;               /**< Movement presence measurement gpiote. */
ble_date_time_t                              m_time_stamp;                              /**< Time stamp. */
uint8_t 																		 battery_level=0;                           /**< Battery level variable */
ble_bas_t                             			 bas;                                       /**< Structure used to identify the battery service. */

bool 																				 ENABLE_DATA_LOG = false;										/**< Flag to enable data logger */
bool 																				 READ_DATA       = false;
bool 																				 START_DATA_READ = true;									  /**< flag to start data logging */
bool 																				 TX_COMPLETE     = false;
bool                                         TIME_SET          = false;                 /**< Flag to start time updation */
bool                                         PIR_EVENT_FLAG = false;                    /**< Flag to indicate the event occurred on GPIO pin configured for PIR sensor */  
bool                                         MOVEMENT_EVENT_FLAG = false;               /**< Event occurred on Movement/accelerometer interrupt pin */
bool                                         CHECK_ALARM_TIMEOUT=false;                 /**< Flag to indicate whether to check for alarm conditions*/
bool                                         DATA_LOG_CHECK=false;
bool                                         CLEAR_MOVE_ALARM=false;
bool                                         CENTRAL_DEVICE_CONNECTED = false;          /**<Flag to indicate when a central device is connected >**/    
bool                                         MEAS_BATTERY_LEVEL = false;                /**< Flag for measuring the battery level */
bool                                         delay_complete = false;                    /**< Flag to indicate the completion of delay*/

extern bool																	 DLOGS_CONNECTED_STATE;                     /**< Specifies data logger service is connected or not */
extern bool  																 DFU_ENABLE;                                /**< This flag indicates DFU mode is enabled/not */       
extern bool                                  DEVICE_CONNECTED_STATE;                    /**< This flag indicates device management service is in connected start or now */
extern bool     	                           PIR_CONNECTED_STATE;                       /**< Flag indicates Passive INfrared alarm service is in connected start or now */
extern bool                                  ACCELEROMETER_CONNECTED_STATE;             /**< Flag indicates accelerometer alarm service is in connected start or now */ 
volatile bool                                ACTIVE_CONN_FLAG = false;                  /**<flag indicating active connection*/

extern uint8_t  current_xyz_array[3];
static dm_application_instance_t             m_app_handle;                              /**< Application identifier allocated by device manager */
static bool                                  m_memory_access_in_progress = false;       /**< Flag to keep track of ongoing operations on persistent memory. */
uint32_t buf[4];																																				/*buffer for flash write operation*/
static void device_init(void);
static void dlogs_init(void);
static void dis_init(void);
static void pir_init(void);
static void movement_init(void);
static void bas_init(void);
void data_log_sys_event_handler(uint32_t sys_evt);																			/**<function definition of datalog event handler>*/
static void advertising_init(void);

static uint8_t															 rev_no;																		/**<Revision number of silicon*/

uint8_t										 var_receive_uuid;    											                  /**< varible for receiving the uuid type>**/
uint8_t				             curr_pir_presence;                              	            /* water pir value for broadcast*/
uint32_t                   xyz_coordinates;                                             /*accelerometer value for broadcast*/
uint8_t                    battery_lvl;                                                 /*battery level for broadcasting*/
#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                                       /**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION         3                                          /**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS       0                                        /**< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com. */

/**@brief Macro to convert the result of ADC conversion in millivolts.
*
* @param[in]  ADC_VALUE   ADC result.
* @retval     Result converted to millivolts.
*/
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
    ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 255) * ADC_PRE_SCALING_COMPENSATION)


/**@brief Function for error handling, which is called when an error has occurred. 
*
* @warning This handler is an example only and does not fit a final product. You need to analyze 
*          how your product is supposed to react in case of error.
*
* @param[in] error_code  Error code supplied to the handler.
* @param[in] line_num    Line number where the handler is called.
* @param[in] p_file_name Pointer to the file name. 
*/
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    //ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover on reset
    NVIC_SystemReset();
}


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for performing a Sentry Profile parameters level measurement, and  check for the alarm condition.
*/
static void sentry_param_meas_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    static uint8_t minutes_count = 0x01;
    if (minutes_count < 0x0F)
    {
        minutes_count++;
    }
    else 
    {
        minutes_count =0x01;
        DATA_LOG_CHECK=true;
    }
}


/**@brief Function for performing time keeping.
*/
static void real_time_timeout_handler(void * p_context)
{
    uint32_t err_code;
    uint8_t days_in_month[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    static uint8_t battery_meas_timeout  = 0x00;

    if((m_time_stamp.year% 4 == 0 && m_time_stamp.year%100 != 0) || m_time_stamp.year%400 == 0)
    {
        days_in_month[2] = 29;
    }
    else
    {
        days_in_month[2] = 28;
    }

    m_time_stamp.seconds += 1;
		NRF_WDT->RR[0] = 0x6E524635;					//kick the dog every second
    if (m_time_stamp.seconds > 59)
    {
        m_time_stamp.seconds -= 60;
        m_time_stamp.minutes++;
        if (m_time_stamp.minutes > 59)
        {
            m_time_stamp.minutes = 0;
            m_time_stamp.hours++;
            if (m_time_stamp.hours > 23)
            {
                m_time_stamp.hours = 0;
                m_time_stamp.day++;
                if (m_time_stamp.day > (days_in_month[m_time_stamp.month]))	/* Increment day till days_in_month[time_stamp.month]*/
                {
                    m_time_stamp.day = 1;
                    m_time_stamp.month++;
                    if (m_time_stamp.month > 12)	
                    {
                        m_time_stamp.month = 1;
                        m_time_stamp.year++;
                    }
                }
            }
        }
				battery_meas_timeout++;
				if(battery_meas_timeout >= BATTERY_MEAS_INTERVAL) /*Check whether the battery measurement interval is reached*/
				{
					battery_meas_timeout = 0;
				  MEAS_BATTERY_LEVEL = true;                      /*Set the flag to do battery measurement in main loop*/
				}
    }

    err_code= ble_time_update(&m_device, &m_time_stamp);

    if ((err_code != NRF_SUCCESS) &&
            (err_code != NRF_ERROR_INVALID_STATE) &&
            (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
            (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
            )
    {
        APP_ERROR_HANDLER(err_code);
    }

}


/* This function measures the battery voltage using the band gap as a reference.
* 3.6 V will return 100 %, so depending on battery voltage, it might need scaling. */
void init_battery_level(void)
{
    uint8_t     adc_result;
    uint16_t    batt_lvl_in_milli_volts;
    
    NRF_ADC->CONFIG = ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos |
    ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos |
    ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos;
    NRF_ADC->ENABLE = 1;

    NRF_ADC->TASKS_START = 1;
    while(!NRF_ADC->EVENTS_END);
    adc_result = NRF_ADC->RESULT;
		
	  // *** Fix for PAN #1
    NRF_ADC->TASKS_STOP = 1;
    // *** End of fix for PAN #2 
	
    NRF_ADC->ENABLE = 0;
    batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result) + DIODE_FWD_VOLT_DROP_MILLIVOLTS;

    battery_lvl     = battery_level_in_percent(batt_lvl_in_milli_volts);

}



/**@brief Function for initializing the non-connectable Advertising[broadcasting] functionality.
*
* @details Encodes the required broadcast data and passes it to the stack.      
*/
static void advertising_nonconn_init(void)
{
		uint32_t                   err_code;
    ble_advdata_t              advdata;
    uint8_t                    flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    ble_advdata_manuf_data_t   manuf_specific_data;
    uint8_t                    manuf_data_array[5];

    manuf_data_array[0] = xyz_coordinates;       
    manuf_data_array[1] = xyz_coordinates >> 8;  
    manuf_data_array[2] = xyz_coordinates >> 16 ;
    manuf_data_array[3] = curr_pir_presence;                               			/* PIR alarm is 1 when an active high is at the pin P0.02*/
		manuf_data_array[4] = battery_lvl;
	
    manuf_specific_data.company_identifier = COMPANY_IDENTIFER;                 /* COMPANY IDENTIFIER */
    manuf_specific_data.data.p_data = manuf_data_array;
    manuf_specific_data.data.size = sizeof(manuf_data_array);

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    advdata.p_manuf_specific_data   = &manuf_specific_data;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Time out handler for the delay timer.
*/
static void delay_timer_timeout_handler(void * p_context)
{
  delay_complete = true;                    /*Set the flag to indicate that the delay is complete*/
}

/**@brief Function for implementing non blocking delay.
*/

void delay_ms(uint32_t time_ms)
{
	  uint32_t err_code;

    delay_complete = false;

    // Start timer
    err_code = app_timer_start(delay_timer, APP_TIMER_TICKS(time_ms, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
	
    //wait till the delay timer timeouts
	  while(delay_complete == false)
		{
       uint32_t err_code = sd_app_evt_wait();
       APP_ERROR_CHECK(err_code);
		}
}


/**@brief Function for the Timer initialization.
*
* @details Initializes the timer module. This creates and starts application timers.
*/
static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers
    err_code = app_timer_create(&sentry_measurement_timer, /*Timer for sentry  measurement time-out*/
    APP_TIMER_MODE_REPEATED,
    sentry_param_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Create timer for time keeping
    err_code = app_timer_create(&real_time_timer,          /*Timer for climate parameters measurement time out*/
    APP_TIMER_MODE_REPEATED,
    real_time_timeout_handler);
    APP_ERROR_CHECK(err_code);
	
	  err_code = app_timer_create(&delay_timer,         /* Timer for implemeting delay(ms)*/
    APP_TIMER_MODE_SINGLE_SHOT,
    delay_timer_timeout_handler);
    APP_ERROR_CHECK(err_code);

} 

/**@brief Function for starting application timers.
*/
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start measurement time-out timer
    err_code = app_timer_start(sentry_measurement_timer, SENTRY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    // Start the time keeping timer
    err_code = app_timer_start(real_time_timer, SECONDS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code); 
		
}


/**@brief Function for the GAP initialization.
*
* @details This function shall be used to set-up all the necessary GAP (Generic Access Profile)
*          parameters of the device. It also sets the permissions and appearance.
*/
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
		char deviceName[20];
		uint8_t mac_add[3];

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
	
		//MAC ADDRESS CODE
		mac_add[0] = (NRF_FICR->DEVICEADDR0 & 0x00FF0000) >> 16;
		mac_add[1] = (NRF_FICR->DEVICEADDR0 & 0x0000FF00) >> 8;
		mac_add[2] = (NRF_FICR->DEVICEADDR0 & 0x000000FF);
	
		sprintf(&deviceName[0], "%s%02x%02x%02x", &DEVICE_NAME[0], mac_add[0], mac_add[1], mac_add[2]);
	
		//END MAC ADDRESS CODE
	
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)deviceName, strlen(deviceName));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_THERMOMETER);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
*
* @details Encodes the required advertising data and passes it to the stack.
*          Also builds a structure to be passed to the stack when starting advertising.
*/
static void advertising_init(void)
{
    uint32_t      err_code;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    // Base UUID of Sentry Profile
    static uint8_t Vendor_Spec_Uuid[16] = {0x42, 0x09, 0xDC, 0x67, 0xE4, 0x33, 0x44, 0x20, 0x83, 0xD8, 0xCD, 0xAA, 0xCC, 0xD2, 0xE3, 0x12};
    ble_advdata_manuf_data_t   manuf_data;

    // Setting up base UUID as manufacturing data
    manuf_data.company_identifier = COMPANY_IDENTIFER;                                  /* Company Identifier */
    manuf_data.data.size          = 16;
    manuf_data.data.p_data        = Vendor_Spec_Uuid;

    ble_uuid_t adv_uuids[] = 
    {
        {SENTRY_PROFILE_MOVEMENT_SERVICE_UUID,	  				BLE_UUID_TYPE_BLE}, 
        {SENTRY_PROFILE_PIR_SERVICE_UUID,									BLE_UUID_TYPE_BLE},
    };

    // Initialize advertising parameters (used when starting advertising)
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                                        /* Undirected advertisement */
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_ADV_INTERVAL;
    m_adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;
		
		// Build and set broadcast data
    ble_advdata_t              advdata1;
		ble_advdata_t              advdata3;/*variable to set the scan response data*/
    ble_advdata_manuf_data_t   manuf_specific_data;
    uint8_t                    manuf_data_array[5];

    manuf_data_array[0] = xyz_coordinates;       
    manuf_data_array[1] = xyz_coordinates >> 8;  
    manuf_data_array[2] = xyz_coordinates >> 16 ;
    manuf_data_array[3] = curr_pir_presence;                               /* PIR alarm is 1 when an active high is at the pin P0.02*/
		manuf_data_array[4] = battery_lvl;
		
    manuf_specific_data.company_identifier = COMPANY_IDENTIFER;                            /* COMPANY IDENTIFIER */
    manuf_specific_data.data.p_data = manuf_data_array;
    manuf_specific_data.data.size = sizeof(manuf_data_array);

    memset(&advdata1, 0, sizeof(advdata1));

    advdata1.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata1.flags.size              = sizeof(flags);
    advdata1.flags.p_data            = &flags;
    advdata1.p_manuf_specific_data   = &manuf_specific_data;
		//build sets the scan response data 
		memset(&advdata3, 0, sizeof(advdata3));

    advdata3.name_type               = BLE_ADVDATA_NO_NAME;
    advdata3.include_appearance      = false;
    advdata3.flags.size              = 0;
    advdata3.p_manuf_specific_data   = &manuf_data;//set the company identifier
    advdata3.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata3.uuids_complete.p_uuids  = adv_uuids;
		
		err_code = ble_advdata_set(&advdata1,&advdata3);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for initializing services that will be used by the application.
*
* @details Initialize the movement/PIR alarm ,and Device Information services.
*/
uint32_t services_init(void)
{
		uint32_t   err_code;
	  ble_uuid128_t base_uuid =SENTRY_PROFILE_BASE_UUID;
  	// Add custom base UUID
    err_code = sd_ble_uuid_vs_add(&base_uuid, &var_receive_uuid);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    movement_init();	   /* Initialize accelerometer/movement alarm service */
    pir_init();          /* Initialize passive infrared alarm service */
    dlogs_init();				 /* Initialize the data logger service */	
    device_init();       /* Initialize device management service */
    dis_init();          /* Initialize device information service */
    bas_init();	         /* Initialize battery service*/
		return NRF_SUCCESS;
}


/**@brief Function for initializing battery service
*/
static void bas_init(void)
{

    uint32_t       err_code;
    ble_bas_init_t bas_init;

    // Initialize Battery Service
    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;     /* 100% battery level */

    err_code = ble_bas_init(&bas, &bas_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing PIR alarm service*/
static void pir_init(void)
{

    uint32_t         err_code;
    ble_pir_init_t    pir_init;

    // Initialize PIR presence alarm Service
    memset(&pir_init, 0, sizeof(pir_init));

    // Here the sec level for the PIR Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&pir_init.pir_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&pir_init.pir_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&pir_init.pir_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&pir_init.pir_char_attr_md2.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&pir_init.battery_level_report_read_perm);

    pir_init.evt_handler          = NULL;
    pir_init.write_evt_handler    = NULL;
    pir_init.support_notification = true;
    pir_init.p_report_ref         = NULL; 
    pir_init.pir_alarm_set        = DEFAULT_ALARM_SET;
	
		//pir alarm with time stamp characteristics set as zero
		pir_init.pir_alarm_with_time_stamp[0]      = RESET_ALARM;
		pir_init.pir_alarm_with_time_stamp[1]			 =0x00;
		pir_init.pir_alarm_with_time_stamp[2]			 =0x00;
		pir_init.pir_alarm_with_time_stamp[3]			 =0x00;
		pir_init.pir_alarm_with_time_stamp[4]  		 =0x00;
		pir_init.pir_alarm_with_time_stamp[5]			 =0x00;
		pir_init.pir_alarm_with_time_stamp[6]			 =0x00;
		pir_init.pir_alarm_with_time_stamp[7]			 =0x00;
		
    err_code = ble_pir_init(&m_pir, &pir_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing Movement alarm service*/
static void movement_init(void)
{

    uint32_t         err_code;
    ble_movement_init_t    movement_init;

    // Initialize Movement alarm Service
    memset(&movement_init, 0, sizeof(movement_init));

    // Here the sec level for the Movement Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&movement_init.movement_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&movement_init.movement_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&movement_init.movement_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&movement_init.movement_char_attr_md2.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&movement_init.battery_level_report_read_perm);

    movement_init.evt_handler          = NULL;
    movement_init.write_evt_handler    = NULL;
    movement_init.support_notification = true;
    movement_init.p_report_ref         = NULL; 
    movement_init.movement_alarm_set	 = DEFAULT_ALARM_SET; 
    movement_init.movement_alarm_clear = RESET_ALARM;
		
    //movement alarm with time stamp characteristics set as zero
		movement_init.move_alarm_with_time_stamp[0]      = RESET_ALARM;
		movement_init.move_alarm_with_time_stamp[1]			 =0x00;
		movement_init.move_alarm_with_time_stamp[2]			 =0x00;
		movement_init.move_alarm_with_time_stamp[3]			 =0x00;
		movement_init.move_alarm_with_time_stamp[4]  		 =0x00;
		movement_init.move_alarm_with_time_stamp[5]			 =0x00;
		movement_init.move_alarm_with_time_stamp[6]			 =0x00;
		movement_init.move_alarm_with_time_stamp[7]			 =0x00;

    err_code = ble_movement_init(&m_movement, &movement_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing device information service*/
static void dis_init(void )
{  
    uint32_t         err_code;
    ble_dis_init_t   dis_init;
    ble_dis_sys_id_t sys_id;
	
		uint64_t mac_address = 0;
		uint32_t mac_address2 = 0;

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str,     MODEL_NUM);
		ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, FIRMWARE_ID);
	
    //sys_id.manufacturer_id            = MANUFACTURER_ID;
    //sys_id.organizationally_unique_id = ORG_UNIQUE_ID;
    //dis_init.p_sys_id                 = &sys_id;
	
		//replacing System ID with MAC Address		
		//assemble mac_address to work with the sys_id
		
		mac_address = mac_address | (((NRF_FICR->DEVICEADDR1 & 0x0000FF00) | 0x0000C000) <<8) | 
																((NRF_FICR->DEVICEADDR1 & 0x000000FF) << 24) | 
																((long long)(NRF_FICR->DEVICEADDR0 & 0xFF000000) << 8) ;
																
		mac_address2 = mac_address2 | ((NRF_FICR->DEVICEADDR0 & 0x00FF0000) >> 16) | 
																	(NRF_FICR->DEVICEADDR0 & 0x0000FF00) | 
																	((NRF_FICR->DEVICEADDR0 & 0x000000FF) << 16);
		
		sys_id.manufacturer_id 						= mac_address;																		
		sys_id.organizationally_unique_id = mac_address2;																	
    dis_init.p_sys_id                 = &sys_id;
		

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing data logger service*/
static void dlogs_init(void)
{

    uint32_t           err_code;
    ble_dlogs_init_t   dlogs_init;

    // Initialize 
    memset(&dlogs_init, 0, sizeof(dlogs_init));

    // Here the sec level for the data logger Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dlogs_init.dlogs_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dlogs_init.dlogs_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dlogs_init.dlogs_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dlogs_init.dlogs_char_attr_md2.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dlogs_init.dlogs_report_read_perm);

    dlogs_init.evt_handler          = NULL;
    dlogs_init.write_evt_handler    = NULL;
    dlogs_init.support_notification = true;
    dlogs_init.p_report_ref         = NULL; 
    dlogs_init.data_logger_enable   = DEFAULT_ALARM_SET;
    dlogs_init.read_data_switch     = DEFAULT_ALARM_SET;

    err_code = ble_dlogs_init(&m_dlogs, &dlogs_init);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for initializing Device management alarm service*/
static void device_init(void)
{

    uint32_t         err_code;
    ble_device_init_t   device_init;	

    // Initialize temperature range Service
    memset(&device_init, 0, sizeof(device_init));

    // Here the sec level for the Device management Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&device_init.device_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&device_init.device_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&device_init.device_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&device_init.device_char_attr_md2.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&device_init.battery_level_report_read_perm);

    device_init.evt_handler          = NULL;
    device_init.write_evt_handler   = NULL;
    device_init.support_notification = true;
    device_init.p_report_ref         = NULL; 

    // Set the default low value for DFU and Switch mode characteristics 
    device_init.device_dfu_mode_set          = DEFAULT_ALARM_SET;
    device_init.device_mode_switch_set       = DEFAULT_ALARM_SET;

    // Start Time satmp from initial values
    device_init.device_time_stamp_set[0] =  0x00;
    device_init.device_time_stamp_set[1] =	0x00;	
    device_init.device_time_stamp_set[2] =  0x00;
    device_init.device_time_stamp_set[3] =	0x00;	
    device_init.device_time_stamp_set[4] =  0x00;
    device_init.device_time_stamp_set[5] =	0x00;	
    device_init.device_time_stamp_set[6] =  0x00;

    err_code = ble_device_init(&m_device, &device_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing security parameters.*/
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;  
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}


/**@brief Function for starting advertising.*/
static void advertising_start(void)
{
    uint32_t err_code;
    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameters Module.
*
* @details This function will be called for all events in the Connection Parameters Module which
*          are passed to the application.
* @note All this function does is to disconnect. This could have been done by simply
*                setting the disconnect_on_fail config parameter, but instead we use the event
*                handler mechanism to demonstrate its use.
*
* @param[in]   p_evt   Event received from the Connection Parameters Module.
*/
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
*
* @param[in]   nrf_error   Error code containing information about what went wrong.
*/
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
*/
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Start non_connectable advertising.
*/
static void advertising_nonconn_start(void)
{
    uint32_t err_code;
    ble_gap_adv_params_t                  adv_params;
	
    // Initialise advertising parameters (used when starting advertising)
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    adv_params.p_peer_addr = NULL;                          
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
	  adv_params.interval    = APP_ADV_INTERVAL;                     /* non connectable advertisements cannot be faster than 100ms.*/
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;
		
		err_code = sd_ble_gap_adv_start(&adv_params);
		APP_ERROR_CHECK(err_code);

}


/**@brief Function for handling the Application's BLE Stack events.
*
* @param[in]   p_ble_evt   Bluetooth stack event.
*/
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
		
				ACTIVE_CONN_FLAG=true;
		
				//starting non-connectable advertising
				advertising_nonconn_start(); 
        break;

    case BLE_GAP_EVT_DISCONNECTED:
    
        m_conn_handle               = BLE_CONN_HANDLE_INVALID;

           
		    //stop non-connectable advertising
				sd_ble_gap_adv_stop();
				
				ACTIVE_CONN_FLAG=false;
				
				//connectable advertising starts
		    advertising_start();
        break;

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
        BLE_GAP_SEC_STATUS_SUCCESS, 
        &m_sec_params);
        break;

    case BLE_GAP_EVT_TIMEOUT:
        if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
        {
            // Go to system-off mode (this function will not return; wakeup will cause a reset).
            err_code = sd_power_system_off();    
        }
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        if (p_ble_evt->evt.gatts_evt.params.timeout.src == BLE_GATT_TIMEOUT_SRC_PROTOCOL)
        {
            err_code = sd_ble_gap_disconnect(m_conn_handle,
            BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        }
        break;
    case BLE_EVT_TX_COMPLETE:											/*Event indicated the transmission of a characteristics is complete*/
        TX_COMPLETE=true;
        break;
    default:
        break;
    }

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip in System OFF Mode
 */
static void system_off_mode_enter(void)
{
    uint32_t err_code;
    uint32_t count;
    
    // Verify if there is any flash access pending, if yes delay starting advertising until 
    // it's complete.
      err_code = pstorage_access_status_get(&count);
      APP_ERROR_CHECK(err_code);
    
    if (count != 0)
    {
        m_memory_access_in_progress = true;
        return;
    }

    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the Application's system events.
 *
 * @param[in]   sys_evt   system event.
 */
static void on_sys_evt(uint32_t sys_evt)
{
    switch(sys_evt)
    {
        case NRF_EVT_FLASH_OPERATION_SUCCESS:
        case NRF_EVT_FLASH_OPERATION_ERROR:
            if (m_memory_access_in_progress)
            {
                m_memory_access_in_progress = false;
                system_off_mode_enter(); 
            }
            break;
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
*
* @details This function is called from the BLE Stack event interrupt handler after a BLE stack
*          event has been received.
*
* @param[in]   p_ble_evt   Bluetooth stack event.
*/
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{   

    ble_dlogs_on_ble_evt(&m_dlogs, p_ble_evt); 
    ble_pir_on_ble_evt(&m_pir, p_ble_evt);
    ble_bas_on_ble_evt(&bas, p_ble_evt);
    ble_movement_on_ble_evt(&m_movement, p_ble_evt);
    ble_device_on_ble_evt(&m_device, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    dm_ble_evt_handler(p_ble_evt);                            /*added for migrating to soft device 7.0.0 and SDK 6.1.0*/
    on_ble_evt(p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
		data_log_sys_event_handler(sys_evt);                      /*added event handler for flash write operation*/
    on_sys_evt(sys_evt);
}


/**@brief Function for initializing the BLE stack.
*
* @details Initializes the SoftDevice and the BLE event interrupt.
*/
static void ble_stack_init(void)
{
		uint32_t err_code;
	  ble_enable_params_t 		p_ble_enable_params;
    
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_4000MS_CALIBRATION, false);      /*clock changed for HRM board*/
	  
	  memset(&p_ble_enable_params, 0, sizeof(p_ble_enable_params));
		
		//enable service change characteristic 
		p_ble_enable_params.gatts_enable_params.service_changed =1;
	
		err_code = sd_ble_enable(&p_ble_enable_params);
		APP_ERROR_CHECK(err_code);
    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
    
    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief event handler for  the PIR GPIOTE module.
*/
static void pir_gpiote_evt_handler(uint32_t pins_low_to_high_mask, uint32_t pins_high_to_low_mask)
{ 
    PIR_EVENT_FLAG=true;																					/*the flag is set when an event occurs on gpiote*/
}

/**@brief event handler for  the Movement GPIOTE module.
*/
static void movement_gpiote_evt_handler(uint32_t pins_low_to_high_mask, uint32_t pins_high_to_low_mask)
{
    movement_gpio_pin_val = nrf_gpio_pin_read(MOVEMENT_GPIOTE_PIN);	
    MOVEMENT_EVENT_FLAG=true;																			/*the flag is set when an event occurs on gpiote*/
}


/**@brief Function for initializing the GPIOTE handler module.
*/
static void gpiote_init(void)
{  
    uint32_t err_code;	

    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);

    // Configure GPIO pin as input which is connected PIR sensor output
    nrf_gpio_cfg_input(PIR_GPIOTE_PIN, GPIO_PIN_CNF_PULL_Disabled); 
    // Configure GPIO pin as input which is connected INT1 pin of MMA7660 accelerometer
    nrf_gpio_cfg_input(MOVEMENT_GPIOTE_PIN, NRF_GPIO_PIN_PULLUP);

    // Calls an event handler whenever a HIGH->LOW or LOW->HIGH transition is incurred on P0.02 GPIO pin
    err_code = app_gpiote_user_register(&pir_measurement_gpiote, 
    PIR_PINS_LOW_TO_HIGH_MASK, 
    PIR_PINS_HIGH_TO_LOW_MASK, 
    pir_gpiote_evt_handler);        /* Register the gpiote user for PIR sensor*/
    if (err_code != NRF_SUCCESS )                                     
    {                                                                 
        APP_ERROR_HANDLER(err_code);                                   
    }                                                                 
    
    err_code = app_gpiote_user_enable(pir_measurement_gpiote);         /* Enable the registered user */
    if (err_code != NRF_SUCCESS )                                     
    {                                                                 
        APP_ERROR_HANDLER(err_code);                                    
    }                                                                 


    // Calls an event handler whenever a HIGH->LOW or LOW->HIGH transition is incurred on P0.04 GPIO pin
    err_code = app_gpiote_user_register(&movement_measurement_gpiote, 
    NULL, 
    MOVEMENT_PINS_HIGH_TO_LOW_MASK, 
    movement_gpiote_evt_handler);  /* Register the gpiote user for accelerometer */
    if (err_code != NRF_SUCCESS )                                     
    {                                                                 
        APP_ERROR_HANDLER(err_code);                                   
    }                                                                 

    err_code = app_gpiote_user_enable(movement_measurement_gpiote);    /* Enable the registered user */
    if (err_code != NRF_SUCCESS )
    {
        APP_ERROR_HANDLER(err_code);
    }

}


/**@brief Function for handling the Device Manager events.
 *
 * @param[in]   p_evt   Data associated to the device manager event.
 */
static uint32_t device_manager_evt_handler(dm_handle_t const    * p_handle,
                                           dm_event_t const     * p_event,
                                           api_result_t           event_result)
{
    APP_ERROR_CHECK(event_result);
    return NRF_SUCCESS;
}

/**@brief Function for the Device Manager initialization.
 */
static void device_manager_init(void)
{
    uint32_t                err_code;
    dm_init_param_t         init_data;
    dm_application_param_t  register_param;
    
     // Initialize persistent storage module.
     err_code = pstorage_init();
     APP_ERROR_CHECK(err_code);

    err_code = dm_init(&init_data);
    APP_ERROR_CHECK(err_code);

    memset(&register_param.sec_param, 0, sizeof(ble_gap_sec_params_t));
    
    register_param.sec_param.timeout      = SEC_PARAM_TIMEOUT;
    register_param.sec_param.bond         = SEC_PARAM_BOND;
    register_param.sec_param.mitm         = SEC_PARAM_MITM;
    register_param.sec_param.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    register_param.sec_param.oob          = SEC_PARAM_OOB;
    register_param.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    register_param.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    register_param.evt_handler            = device_manager_evt_handler;
    register_param.service_type           = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

    err_code = dm_register(&m_app_handle, &register_param);
    APP_ERROR_CHECK(err_code);
}


/**@brief Radio Notification event handler.
*/
void radio_active_evt_handler_rev1(bool radio_active)
{	
		uint32_t err_code;
	
    m_radio_event = radio_active;
    ble_flash_on_radio_active_evt(m_radio_event);   /*call the event handler in ble_flash.c*/
	
		//PAN14 FIX
		if(radio_active)
		{
			err_code = sd_power_mode_set(NRF_POWER_MODE_CONSTLAT);
		}
		else
		{
			err_code = sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
		}
		APP_ERROR_CHECK(err_code);
}

/**@brief Radio Notification event handler when not using rev 1 silicon
*/
void radio_active_evt_handler(bool radio_active)
{		
		m_radio_event = radio_active;	
}

/**@brief Function for initializing the Radio Notification events.
*/
static void radio_notification_init(void)
{
    uint32_t err_code;

		if (rev_no == 0x01) {
			err_code = ble_radio_notification_init(NRF_APP_PRIORITY_LOW,
			NRF_RADIO_NOTIFICATION_DISTANCE_800US,
			radio_active_evt_handler_rev1);
			APP_ERROR_CHECK(err_code);
		}
		else {
			err_code = ble_radio_notification_init(NRF_APP_PRIORITY_HIGH,
			NRF_RADIO_NOTIFICATION_DISTANCE_4560US,
			radio_active_evt_handler);
			APP_ERROR_CHECK(err_code);
		}
}


/**@brief Function for the Power manager.
*/
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


static void create_log_data(uint32_t * data)
{
    uint16_t current_pir_presence = 0;
    current_pir_presence = nrf_gpio_pin_read(PIR_GPIOTE_PIN);					

    data[0] = (m_time_stamp.year<<16)|(m_time_stamp.month<<8)|m_time_stamp.day;				 /* First word written to memory contains date (YYYYMMDD)*/
    data[1] = (m_time_stamp.hours<<16)|(m_time_stamp.minutes<<8)|m_time_stamp.seconds; /* Second word contains time HHMMSS*/
    // first word is X data,second word is Y data and third word is Z data 
    data[2] = (current_xyz_array[0] << 16) | (current_xyz_array[1] << 8) | current_xyz_array[2] ;			
    data[3] = current_pir_presence;                                                    /* Fourth word contains PIR state */

}

/**@brief Function for checking whether to log data.
*/
static void data_log_check()
{
    uint32_t log_data[4];                /*array storing the data to be logged */

    if(ENABLE_DATA_LOG && !READ_DATA)									    /*if enabled, start data logging functionality*/
    {   
				create_log_data(log_data);                        /*create the data to be logged */
        write_data_flash(log_data);	                      /*log the data to flash */
    }
}

/* Turn OFF TWI if TWI is not using , considering power optimization*/
void twi_turn_OFF(void)
{
    NRF_TWI0->POWER = 0; 
    NRF_TWI1->POWER = 0;
}

/* Turn ON TWI (used only after turning it OFF)*/
void twi_turn_ON(void)
{
    NRF_TWI1->POWER = 1;
    twi_master_init();
}

/** @brief Function for initiation WDT
*/
void WDT_init(void)
{		
	
		NRF_WDT->CONFIG = WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos |							//pause WDT when device in debug mode
											WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos;								//continue WDT when device is in sleep mode
		
		NRF_WDT->CRV = 4*32768;								//set watchdog to have 2 second timeout
		NRF_WDT->RREN |= WDT_RREN_RR0_Msk;		//enable reload register0
		NRF_WDT->TASKS_START = 1;							//start watchdog timer
		
		NRF_WDT->RR[0] = 0x6E524635;					//kick the dog every second
	
}

void LED_ON(void)
{
		// ON LED indicator
		nrf_gpio_cfg_output(20);
		nrf_gpio_pin_write(20, 1);
		nrf_delay_ms(1000);
		NRF_GPIO->PIN_CNF[20] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
	
}

/**@brief HFCLK request function
*/
void HFCLK_request(void)
{
		uint32_t err_code;

    // Make sure 16 MHz clock is requested when calibration starts

    err_code = sd_ppi_channel_assign(2, &NRF_CLOCK->EVENTS_CTTO, &NRF_TIMER2->TASKS_START);
    APP_ERROR_CHECK(err_code);
	
    err_code = sd_ppi_channel_assign(1, &NRF_CLOCK->EVENTS_DONE, &NRF_TIMER2->TASKS_STOP);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ppi_channel_enable_set((1 << 2) | (1 << 1));
    APP_ERROR_CHECK(err_code);
	
	
}

/**@brief Get silicon revision
*/
void get_die_revision_no(void)

{	
	/*
	0x0001  0x01  = rev.1 128k FLASH/ 16k RAM or 256k FLASH/ 16k RAM
	0x0001  0x04  = rev.2 128k FLASH/ 16k RAM or 256k FLASH/ 16k RAM
	0x0001  0x07  = rev.3 256k FLASH/ 16k RAM
	0x0001  0x08  = rev.3 128k FLASH/ 16k RAM
	0x0001  0x09  = rev.3 256k FLASH/ 32k RAM*/
	
  rev_no = (uint8_t) ((*(uint32_t *)0xF0000FE8) & 0x000000F0);
  rev_no = ((rev_no >> 0x04) & (0x0F));

}

/**@brief Function for application main entry.
*/
void connectable_mode(void)
{
    uint32_t err_code;

    // Initialization.
		get_die_revision_no();								 	/*Get silicon revision before init*/
    ble_stack_init();
    twi_master_init(); 
    MMA7660_config_standby_and_initialize();
    timers_init();
    gpiote_init();
    device_manager_init();
    gap_params_init();
	  init_battery_level();                 /*measure the battery level before advertisement*/
    advertising_init();
    services_init();
    conn_params_init();
    sec_params_init();
    radio_notification_init();
    MMA7660_enable_active_mode();
    twi_turn_OFF();
    application_timers_start();
	
		if(rev_no == 0x01){											/* Activate HFCLK workaround for PAN14 if rev 1 silicon*/
			HFCLK_request();
		}
	
		WDT_init();
    advertising_start();
		LED_ON();
	


    // Enter main loop.
    for (;;)
    {

        // If the dfu enable flag is true go to the bootloader 
        if(DFU_ENABLE)  
        {
             
            sd_power_gpregret_set(1);            /* If DFU mode is enabled, set the general purpose retention register to 1*/
     
						err_code=sd_ble_gatts_service_changed(m_conn_handle,0x01,0x4D); /*function for service change indication*/
						
						sd_nvic_SystemReset();               /* Apply a system reset for jumping into bootloader*/                     
        }                                       

        // If the PIR event mode flag is true and go for alarm check condition
        if(PIR_EVENT_FLAG)
        {

            // Check whether alarm has to be send to indicate pir presence 
            err_code = ble_pir_alarm_check(&m_pir,&m_device);             
            if ((err_code != NRF_SUCCESS) &&									/*Passed device management service structure to pir service to get time stamp*/
                    (err_code != NRF_ERROR_INVALID_STATE) &&
                    (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
                    (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
                    )
            {
                APP_ERROR_HANDLER(err_code);
            } 

						//updating the advertise/broadcast data
						if(ACTIVE_CONN_FLAG==false)               /* no active connection*/
							advertising_init();                     
						else																			/*an active connection exists*/
							advertising_nonconn_init();
            PIR_EVENT_FLAG=false;				         /* Reset the gpiote event flag*/

        }
				
        // If the Movement event mode flag is true and go for alarm check condition			 
        if(MOVEMENT_EVENT_FLAG) 
        {
            // Check whether alarm has to be send to indicate movement 
            err_code = ble_movement_alarm_check(&m_movement,&m_device);   	/*Passed device management service structure to movement service to get time stamp*/         
            if ((err_code != NRF_SUCCESS) &&
                    (err_code != NRF_ERROR_INVALID_STATE) &&
                    (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
                    (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
                    )
            {
                APP_ERROR_HANDLER(err_code);
            }  
						delay_ms(100);
						//updating the advertise/broadcast data
						if(ACTIVE_CONN_FLAG==false)               /* no active connection*/
							advertising_init();                     
						else																			/*an active connection exists*/
							advertising_nonconn_init();
            MOVEMENT_EVENT_FLAG=false;					 /* Reset the gpiote event flag*/
        }
        if (DATA_LOG_CHECK)
        {
						data_log_check();
            DATA_LOG_CHECK= false;
        }


        // If READ_DATA flag is set, start sending data to the connected device	
        if(READ_DATA)																			                /* If enabled, start sending data to the connected device */
        {
            err_code=app_timer_stop(sentry_measurement_timer);		        /* Stop the timers before start sending the historical data */
            APP_ERROR_CHECK(err_code);
            err_code=app_gpiote_user_disable(pir_measurement_gpiote);     /* Disable the PIR gpiote*/
            err_code=app_gpiote_user_disable(movement_measurement_gpiote);/* Disable the movement gpiote*/
            APP_ERROR_CHECK(err_code);
            READ_DATA = false;
            ENABLE_DATA_LOG = false;                                      /* Disable data logging functionality */
            send_data(&m_dlogs);																          /* Start sending the data */												
            application_timers_start();												            /* Restart the timers when sending is finished */
            err_code=app_gpiote_user_enable(pir_measurement_gpiote);      /* Re-enable PIR gpiote */
            err_code=app_gpiote_user_enable(movement_measurement_gpiote); /* Re-enable movement gpiote */
            APP_ERROR_CHECK(err_code);
            err_code=reset_data_log(&m_dlogs);									          /* Reset the data logger enable and data read switches*/
            APP_ERROR_CHECK(err_code);	
        }    

        // If TIME_SET flag is set, create new time stamp
        if(TIME_SET)
        {
            create_time_stamp(&m_device, &m_time_stamp);          /* Create new time stamp from user set time*/
            TIME_SET = false;                                              /* Reset the flag*/
            
        }                                                                 

        if (CLEAR_MOVE_ALARM)	
        {   
            err_code = reset_alarm(&m_movement);             /* Check whether alarm clear in movement service is set */
            if ((err_code != NRF_SUCCESS) &&
                    (err_code != NRF_ERROR_INVALID_STATE) &&
                    (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
                    (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
                    )
            {
                APP_ERROR_HANDLER(err_code);

            }
						delay_ms(100);
            CLEAR_MOVE_ALARM= false;
        }
				
				if(CENTRAL_DEVICE_CONNECTED)
				{
					update_movement_alarmtimestamp_on_connect(&m_movement,&m_device);   	/*Function to update the last occurance of movement alarm when a central device is connected*/         
          
					err_code = update_pir_alarmtimestamp_on_connect(&m_pir,&m_device);     /*Function to update the last occurance of pir alarm when a central device is connected*/                 
            if ((err_code != NRF_SUCCESS) &&									 
                    (err_code != NRF_ERROR_INVALID_STATE) &&
                    (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
                    (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
                    )
            {
                APP_ERROR_HANDLER(err_code);
            }
						delay_ms(100);
					CENTRAL_DEVICE_CONNECTED = false;
					
				}
        if(MEAS_BATTERY_LEVEL)
				{
					  battery_start();		                              /* Measure battery level*/
						MEAS_BATTERY_LEVEL = false;
				}
        power_manage(); 

    }

}


// * @}
