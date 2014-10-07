/** @file
*
* @defgroup ble_sdk_srv_bas Temperature Service
* @{
* @ingroup ble_sdk_srv
* @brief Temperature Service module.
*
* @details This module implements the Temperature Service with the Temperature alarm characteristic.
*          During initialization it adds the Temperature Service  to the BLE stack database. 
*
* @note The application must propagate BLE stack events to the temperature Service module by calling
*       ble_temps_low_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Shafy
* Date   : 10/19/2013
*
*/

#ifndef BLE_TEMPS_LOW_H__
#define BLE_TEMPS_LOW_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/**@brief Temperature Service event type. */
typedef enum
{
    BLE_TEMPS_LOW_EVT_NOTIFICATION_ENABLED,                             /**< Temperature value notification enabled event. */
    BLE_TEMPS_LOW_EVT_NOTIFICATION_DISABLED                             /**< Temperature value notification disabled event. */
} ble_temps_low_evt_type_t;

/**@brief Temperature Service event. */
typedef struct
{
    ble_temps_low_evt_type_t evt_type;                                  /**< Type of event. */
} ble_temps_low_evt_t;

// Forward declaration of the ble_temps_low_t type. 
typedef struct ble_temps_s ble_temps_t;

/**@brief Temperature Service event handler type. */
typedef void (*ble_temps_evt_handler_t) (ble_temps_t * p_bas, ble_temps_low_evt_t * p_evt);

/**@brief Temperature Service write event handler type. */
typedef void (*ble_temps_write_evt_handler_t) (void);

/**@brief Temperature   Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_temps_evt_handler_t       evt_handler;                       /**< Event handler to be called for handling events in the Temperature Service. */
    ble_temps_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    bool                          support_notification;              /**< TRUE if notification of Temperature Level measurement is supported. */
    ble_srv_report_ref_t *        p_report_ref;                      /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Temperature Level characteristic */
    uint8_t                       temperature_low_level[2];          /**< temperature low level */
    uint8_t                       temperature_high_level[2];         /**< temperature high level */
    uint8_t												temperature_alarm_set;             /** Alarm set for temperature **/
    uint8_t												temperature_alarm;   			         /** Alarm for temperature **/
    ble_srv_cccd_security_mode_t  temperature_char_attr_md;          /**< Initial security level for Temperature characteristics attribute */
    ble_srv_cccd_security_mode_t  temperature_char_attr_md2;         /**< Initial security level for Temperature characteristics attribute */
    ble_gap_conn_sec_mode_t       battery_level_report_read_perm;    /**< Initial security level for Temperature report read attribute */
} ble_temps_init_t;

/**@brief Temperature Service structure. This contains various status information for the service. */
typedef struct ble_temps_s
{
    ble_temps_evt_handler_t       evt_handler;                       /**< Event handler to be called for handling events in the Temperature Service. */
    ble_temps_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    uint16_t                      service_handle;                    /**< Handle of temperature Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      current_temp_handles;              /**< Handles for temperature  characteristic. */
    ble_gatts_char_handles_t      temp_low_level_handles;          	 /**< Handles for temperature low Level characteristic. */
    ble_gatts_char_handles_t      temp_high_level_handles;         	 /**< Handles for temperature high Level characteristic. */
    ble_gatts_char_handles_t      temp_alarm_set_handles;  	         /**< Handles for temperature alarm set characteristic. */
    ble_gatts_char_handles_t      temp_alarm_handles;      	         /**< Handles for temperature alarm characteristic. */
    ble_gatts_char_handles_t      switch_mode_handles;      	       /**< Handles for temperature alarm characteristic. */
    uint16_t                      report_ref_handle;                 /**< Handle of the Report Reference descriptor. */
    uint8_t                       temperature_low_level[2];   	     /**< temperature low level for alarm for temperature Service. */
    uint8_t                       temperature_high_level[2];         /**< temperature high level for alarm for temperature Service. */
    uint8_t												temperature_alarm_set;   	         /**< Alarm set for temperature **/
    uint8_t												temperature_alarm;   			         /**< Alarm for temperature **/
    uint16_t                      conn_handle;                       /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;         /**< TRUE if notification of Temperature Level is supported. */
} ble_temps_t;

/**@brief Function for initializing the Temperature Service.
*
* @param[out]  p_temps       Temperature Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_temps_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_temps_init(ble_temps_t * p_temps, const ble_temps_init_t * p_temps_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Temperature Service.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       ble_temps_low_Temperature_level_update() must be called upon reconnection if the
*       Temperature level has changed while the service has been disconnected from a bonded
*       client.
*
* @param[in]   p_temps      Temperature Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_temps_on_ble_evt(ble_temps_t * p_temps, ble_evt_t * p_ble_evt);

/**@brief Function for updating the Temperature level.
*
* @details The application calls this function after having performed a Temperature measurement. If
*          notification has been enabled, the Temperature level characteristic is sent to the client.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       this function must be called upon reconnection if the Temperature level has changed
*       while the service has been disconnected from a bonded client.
*
* @param[in]   p_temps          Temperature Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_temps_level_alarm_check(ble_temps_t *p_temps);

/**@brief Function to read temperature from tmp102.
*
* @param[in]   void
* @param[out]   uint16_t current_temperature.
*/
uint16_t read_temperature(void);									 /** Function for reading temperature from sensor **/

/**@brief Function to convert 12-bit signed temperature value to float value a.
*
* @param[in]   uint16_t temp_unsigned - contains the signed 12 bit temperature value received from tmp102
* @param[out]   float temp_float
*/
float    convert_temperature_to_float(uint16_t );  /*function for converting 12-bit temperature to float*/

#endif // BLE_TEMPS_H__

/** @} */
