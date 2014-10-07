/** 
* This application implements an nRF51822/S110 application sending data 
* in the format of the nRF Temp smart phone application. 
* Adapter from sample application provided by Nordic
* It could be further power optimized by removing the busy loops in the 
* temperature and battery measurement functions. 
*
* Author : Shafy
* Date   : 10/19/2013
* Change log:
* sruthi.k.s     01/10/2014    migrated to soft device 7.0.0 and SDK 6.1.0
*/

#include <stdint.h>
#include <stdio.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "ble_radio_notification.h"
#include "nrf_gpio.h"
#include "ble_flash.h"
#include "ble_debug_assert_handler.h"
#include "nrf_temp.h"
#include "twi_master.h"
#include "app_timer.h"
#include "wimoto_sensors.h"
#include "wimoto.h"
#include "pstorage.h"

#define APP_ADV_INTERVAL                     0x81A                                      /**< The advertising interval (in units of 0.625 ms. This value corresponds to ~1.2 s). */
#define APP_ADV_TIMEOUT_IN_SECONDS           0                                          /**< The advertising timeout in units of seconds. */

#define DEAD_BEEF                            0xDEADBEEF                                 /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define HTU21_ADDRESS 									 		 0x80
#define APP_TIMER_PRESCALER                  0                                          /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS                 3                                          /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE              4                                          /**< Size of timer operation queues. */
#define MODE_SWITCH_INTERVAL                 APP_TIMER_TICKS(20000, APP_TIMER_PRESCALER)

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                                      /**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION         3                                         /**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS       270                                       /**< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com. */

/**@brief Macro to convert the result of ADC conversion in millivolts.
*
* @param[in]  ADC_VALUE   ADC result.
* @retval     Result converted to millivolts.
*/
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
    ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 255) * ADC_PRE_SCALING_COMPENSATION)


//static app_timer_id_t                        timer_id;                                  /**<  timer. */	
extern bool 	  BROADCAST_MODE;
static volatile bool m_do_update = false;
static bool                                  m_memory_access_in_progress = false;       /**< Flag to keep track of ongoing operations on persistent memory. */
/*****************************************************************************
* Error Handling Functions
*****************************************************************************/


/**@brief Error handler function, which is called when an error has occurred. 
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
    //  nrf_gpio_pin_set(LED0);
    //  nrf_gpio_pin_set(LED1);

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}


/**@brief Assert macro callback function.
*
* @details This function will be called in case of an assert in the SoftDevice.
*
* @warning This handler is an example only and does not fit a final product. You need to analyze 
*          how your product is supposed to react in case of Assert.
* @warning On assert from the SoftDevice, the system can only recover on reset.
*
* @param[in]   line_num   Line number of the failing ASSERT call.
* @param[in]   file_name  File name of the failing ASSERT call.
*/
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/*****************************************************************************
* Static Event Handling Functions
*****************************************************************************/
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
/**@brief Application's BLE Stack event handler.
*
* @param[in]   p_ble_evt   Bluetooth stack event.
*/
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t        err_code      = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
    default:
        break;
    }

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

/**@brief Dispatches a BLE stack event to all modules with a BLE stack event handler.
*
* @details This function is called from the BLE Stack event interrupt handler after a BLE stack
*          event has been received.
*
* @param[in]   p_ble_evt   Bluetooth stack event.
*/
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
}

/**@brief GAP initialization.
*
* @details This function shall be used to setup all the necessary GAP (Generic Access Profile)
*          parameters of the device. It also sets the permissions and appearance.
*/
static void gap_params_init(void)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    uint32_t                err_code;
    ble_gap_conn_sec_mode_t sec_mode;
		ble_enable_params_t 		p_ble_enable_params;
	
		err_code = sd_ble_enable(&p_ble_enable_params);
		APP_ERROR_CHECK(err_code);
    char device_name[9];

    device_name[0] = hex_digits[(NRF_FICR->DEVICEADDR[0] >> 28) & 0xf];
    device_name[1] = hex_digits[(NRF_FICR->DEVICEADDR[0] >> 24) & 0xf];
    device_name[2] = hex_digits[(NRF_FICR->DEVICEADDR[0] >> 20) & 0xf];
    device_name[3] = hex_digits[(NRF_FICR->DEVICEADDR[0] >> 16) & 0xf];
    device_name[4] = hex_digits[(NRF_FICR->DEVICEADDR[0] >> 12) & 0xf];
    device_name[5] = hex_digits[(NRF_FICR->DEVICEADDR[0] >> 8) & 0xf];
    device_name[6] = hex_digits[(NRF_FICR->DEVICEADDR[0] >> 4) & 0xf];
    device_name[7] = hex_digits[NRF_FICR->DEVICEADDR[0] & 0xf];
    device_name[8] = '\0';

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (uint8_t *) device_name, strlen(device_name));
    APP_ERROR_CHECK(err_code);

}


/* This function measures the battery voltage using the bandgap as a reference.
* 3.6 V will return 100 %, so depending on battery voltage, it might need scaling. */
static uint32_t do_battery_measurement(void)
{
    uint8_t     adc_result;
    uint16_t    batt_lvl_in_milli_volts;
    uint8_t     percentage_batt_lvl;

    NRF_ADC->CONFIG = ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos |
    ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos |
    ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos;
    NRF_ADC->ENABLE = 1;

    NRF_ADC->TASKS_START = 1;
    while(!NRF_ADC->EVENTS_END);
    adc_result = NRF_ADC->RESULT;
    NRF_ADC->ENABLE = 0;

    batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result) + DIODE_FWD_VOLT_DROP_MILLIVOLTS;

    percentage_batt_lvl     = battery_level_in_percent(batt_lvl_in_milli_volts);

    return percentage_batt_lvl;
}

/**@brief Read temperature from tmp102 sensor.
*
* @param[in] uint8_t buffer
*  
*/
void do_temperature_measurement(uint8_t *buffer) 
{
    uint16_t current_temperature;

    current_temperature = eDRV_HTU21_MeasureTemperature(); /* Read temperature from htu21d */
    buffer[1]           = current_temperature & 0x00FF;    /* Convert the 16 bit temperature variable to unit8 array */
    buffer[0]           = current_temperature >>8;

}

void do_light_measurement(uint8_t *buffer) 
{
    uint16_t current_light;

    current_light      = ISL29023_get_one_time_ALS();      /* Read Light level from isl2903sensor*/
    buffer[1]          = current_light & 0x00FF;           /* Convert the 16 bit light value to unit8 array */
    buffer[0]          = current_light >>8;                
    
}                                                          

void do_humidity_measurement(uint8_t *buffer)              
{                                                          
    uint16_t htu_hum;                                      
    
    htu_hum           = eDRV_HTU21_MeasureHumidity();      /* Read humidity from htu21d sensor*/
    buffer[1]         = htu_hum & 0x00FF;                  /* Convert the 16 bit humidity variable to unit8 array*/
    buffer[0]         = htu_hum >>8;

}

/**@brief Advertising functionality initialization.
*
* @details Encodes the required advertising data and passes it to the stack.
*          Also builds a structure to be passed to the stack when starting advertising.
*/
static void advertising_init()
{

    uint32_t                   err_code;
    ble_advdata_t              advdata;
    ble_advdata_service_data_t service_data[3];
    uint8_t                    flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    uint8_t				             temperature[2]   = {0x00,0x00};              /* Temperature value*/
    uint8_t				             light_level[2]   = {0x00,0x00};              /* Light value*/
    uint8_t				             htu_hum_level[2] = {0x00,0x00};              /* Humidity value*/
    ble_advdata_manuf_data_t   manuf_specific_data;
    uint8_t                    manuf_data_array[6];	

    do_light_measurement(light_level);

    do_temperature_measurement(temperature);                /* Read temperature from htu21d sensor */

    do_light_measurement(light_level);                      /* Read light from ISL29023 sensor*/

    do_humidity_measurement(htu_hum_level);                 /* Read humidity from htu21d sensor*/

    manuf_data_array[0] = temperature[0];
    manuf_data_array[1] = temperature[1];
    manuf_data_array[2] = light_level[0];
    manuf_data_array[3] = light_level[1];
    manuf_data_array[4] = htu_hum_level[0];
    manuf_data_array[5] = htu_hum_level[1];

    manuf_specific_data.company_identifier = COMPANY_IDENTIFER;     /* COMPANY IDENTIFIER */
    manuf_specific_data.data.p_data = manuf_data_array;
    manuf_specific_data.data.size = sizeof(manuf_data_array);

    uint8_t battery              = do_battery_measurement();
    service_data[0].service_uuid = BLE_UUID_BATTERY_SERVICE;
    service_data[0].data.p_data  = &battery;
    service_data[0].data.size    = sizeof(battery);


    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    advdata.p_service_data_array    = service_data;
    advdata.service_data_count      = 1;
    advdata.p_manuf_specific_data   = &manuf_specific_data;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);
}


void radio_notification_callback(bool is_radio_active)
{
    m_do_update = is_radio_active;
}


/**@brief Initialize Radio Notification event handler.
*/
static void radio_notification_init(void)
{
    uint32_t err_code;

    err_code = ble_radio_notification_init(NRF_APP_PRIORITY_HIGH,
    NRF_RADIO_NOTIFICATION_DISTANCE_1740US,
    radio_notification_callback);
    APP_ERROR_CHECK(err_code);
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
    on_sys_evt(sys_evt);
}

/**@brief BLE stack initialization.
*
* @details Initializes the SoftDevice and the BLE event interrupt.
*/
static void ble_stack_init(void)
{
    uint32_t err_code;
    
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
    
    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Start advertising.
*/
static void advertising_start(void)
{
    uint32_t err_code;
    ble_gap_adv_params_t                  adv_params;

    // Initialise advertising parameters (used when starting advertising)
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    adv_params.p_peer_addr = NULL;                                      /* Undirected advertisement */
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = sd_ble_gap_adv_start(&adv_params);
    APP_ERROR_CHECK(err_code);

}

/* Turn OFF TWI if TWI is not using , considering power optimazation*/
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

/*****************************************************************************
* broadcast_mode
*****************************************************************************/

/**@brief Broadcast parameters .
*/
void broadcast_mode(void)
{

    uint32_t err_code;


    if(BROADCAST_MODE)											/*If broadcast mode is set True from connect.c start broadcasting*/
    {	
        ble_stack_init();
        twi_master_init(); 						  			/*configure twi*/
        ISL29023_config_FSR_and_powerdown();  /*Configure isl29023 */
        radio_notification_init();
        gap_params_init();              			/*Initialize Bluetooth Stack parameters*/
        advertising_init();             
        advertising_start();	          			/*Start advertising*/
        twi_turn_OFF();

        for (;;)
        {

            if (m_do_update)                  
            {
                twi_turn_ON();
                advertising_init();       			/*Advertising is done only if radio is active */
                m_do_update = false;
                twi_turn_OFF();
            }
            else
            {
                twi_turn_OFF();									/*Switch OFF twi when not radio off*/		
            }
            // Switch to a low power state until an event is available for the application
            err_code = sd_app_evt_wait();
            APP_ERROR_CHECK(err_code);
            
        }

    }
}

/**
* @}
*/
