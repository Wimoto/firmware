/** @file
*
* @defgroup ble_sdk_srv_bas Humidity Service
* @{
* @ingroup ble_sdk_srv
* @brief Humidity Service module.
*
* @details This module implements the humidity Service with the humidity level alarm characteristic.
* During initialization it adds the Humidity Service  to the BLE stack database. 
*

*
* @note The application must propagate BLE stack events to the humidity Service module by calling
*       ble_hums_low_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Shafy
* Date   : 10/27/2013
*
*/

#ifndef BLE_HUMS_H__
#define BLE_HUMS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"


/**@brief Humidity Service event type. */
typedef enum
{
    BLE_HUMS_LOW_EVT_NOTIFICATION_ENABLED,                             /**< Humidity value notification enabled event. */
    BLE_HUMS_LOW_EVT_NOTIFICATION_DISABLED                             /**< Humidity value notification disabled event. */
} ble_hums_low_evt_type_t;

/**@brief Humidity Service event. */
typedef struct
{
    ble_hums_low_evt_type_t evt_type;                                  /**< Type of event. */
} ble_hums_evt_t;

// Forward declaration of the ble_hums_t type. 
typedef struct ble_hums_s ble_hums_t;

/**@brief Humidity Service event handler type. */
typedef void (*ble_hums_evt_handler_t) (ble_hums_t * p_hums, ble_hums_evt_t * p_evt);

/**@brief Humidity Service write event handler type. */
typedef void (*ble_hums_write_evt_handler_t) (void);

/**@brief Humidity Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_hums_evt_handler_t       evt_handler;                     /**< Event handler to be called for handling events in the Humidity Service. */
    ble_hums_write_evt_handler_t write_evt_handler;               /**< Event handler to be called for handling write events */
    bool                         support_notification;            /**< TRUE if notification of Humidity Level measurement is supported. */
    ble_srv_report_ref_t *       p_report_ref;                    /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Humidity Level characteristic */
    uint8_t                      climate_hum_low_value[2];        /**< humidity low level */
    uint8_t                      climate_hum_high_value[2];       /**< humidity low level */
    uint8_t											 climate_hum_alarm_set;           /**< Alarm set for humidity **/
    uint8_t											 climate_hum_alarm;   			      /**< Alarm for humidity **/
    ble_srv_cccd_security_mode_t hums_char_attr_md;               /**< Initial security level for Humidity characteristics attribute */
    ble_srv_cccd_security_mode_t hums_char_attr_md2;              /**< Initial security level for Humidity characteristics attribute */
    ble_gap_conn_sec_mode_t      humidity_level_report_read_perm; /**< Initial security level for Humidity report read attribute */
} ble_hums_init_t;

/**@brief Humidity Service structure. This contains various status information for the service. */
typedef struct ble_hums_s
{
    ble_hums_evt_handler_t        evt_handler;                    /**< Event handler to be called for handling events in the Humidity Service. */
    ble_hums_write_evt_handler_t write_evt_handler;               /**< Event handler to be called for handling write events */
    uint16_t                      service_handle;                 /**< Handle of humidity Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      current_hum_level_handles;      /**< Handles for humidity  characteristic. */
    ble_gatts_char_handles_t      hum_low_level_handles;          /**< Handles for humidity low Level characteristic. */
    ble_gatts_char_handles_t      hum_high_level_handles;         /**< Handles for humidity high Level characteristic. */
    ble_gatts_char_handles_t      climate_hum_alarm_set_handles;  /**< Handles for humidity alarm set characteristic. */
    ble_gatts_char_handles_t      climate_hum_alarm_handles;      /**< Handles for humidity alarm characteristic. */
    uint16_t                      report_ref_handle;              /**< Handle of the Report Reference descriptor. */
    uint8_t                       climate_hum_low_level[2];   	  /**< humidity low level for alarm for humidity Service. */
    uint8_t                       climate_hum_high_level[2];      /**< humidity high level for alarm for humidity Service. */
    uint8_t												climate_hum_alarm_set;   	      /**< Alarm set for humidity **/
    uint8_t												climate_hum_alarm;   			      /**< Alarm for humidity **/
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification of Humidity Level is supported. */
} ble_hums_t;


/**@brief Function for initializing the Humidity Service.
*
* @param[out]  p_hums       Humidity Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_temps_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_hums_init(ble_hums_t * hums, const ble_hums_init_t * p_hums_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Humidity Service.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       ble_hums_level_alarm_check() must be called upon reconnection if the
*       Humidity level has changed while the service has been disconnected from a bonded
*       client.
*
* @param[in]   p_hums      Humidity Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_hums_on_ble_evt(ble_hums_t * p_hums, ble_evt_t * p_ble_evt);

/**@brief Function for updating the Humidity level.
*
* @details The application calls this function after having performed a Humidity measurement. If
*          notification has been enabled, the Humidity level characteristic is sent to the client.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       this function must be called upon reconnection if the Humidity level has changed
*       while the service has been disconnected from a bonded client.
*
* @param[in]   p_hums          Humidity Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_hums_level_alarm_check(ble_hums_t *);

/**@brief Function to read humidity level from htu21d.
*
* @param[in]   void
* @param[out]  uint16_t current_hum_level.
*/
uint16_t read_hum_level(void);											/**@brief Function for reading humidity from sensor **/

#endif // BLE_HUMS_H__

/** @} */
