/** @file
*
* @defgroup ble_sdk_srv_bas movement Service
* @{
* @ingroup ble_sdk_srv
* @brief  movement Service module.
*
* @details This module implements the Movement Service with the movement alarm characteristic.
*          During initialization it adds the movement Service  to the BLE stack database. 
*

*
* @note The application must propagate BLE stack events to the movement Service module by calling
*       ble_movement_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Hariprasad C R
* Date   : 11/29/2013
*
*/

#ifndef BLE_ACCELEROMETER_LOW_H__
#define BLE_ACCELEROMETER_LOW_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "wimoto_sensors.h"
#include "ble_device_mgmt_service.h"

/**@brief movement Service event type. */
typedef enum
{
    BLE_MOVEMENT_LOW_EVT_NOTIFICATION_ENABLED,                    /**< movement value notification enabled event. */
    BLE_MOVEMENT_LOW_EVT_NOTIFICATION_DISABLED                    /**< movement value notification disabled event. */
} ble_movement_low_evt_type_t;

/**@brief movement Service event. */
typedef struct
{
    ble_movement_low_evt_type_t evt_type;                         /**< Type of event. */
} ble_movement_low_evt_t;
typedef enum
{
    BLE_MOVEMENT_EVT_INDICATION_ENABLED,                                         /**< movement  alarm value indication enabled event. */
    BLE_MOVEMENT_EVT_INDICATION_DISABLED,                                        /**< movement  alarm value indication disabled event. */
    BLE_MOVEMENT_EVT_INDICATION_CONFIRMED                                        /**< Confirmation of a movement  alarm indication has been received. */
} ble_movement_alarm_type_t;

typedef struct
{
    ble_movement_alarm_type_t evt_type;                                            /**< Type of event. */
} ble_movement_alarm_evt_t;
/**@brief movement Service value write event type. */
typedef enum
{
    BLE_MOVEMENT_ALARM_SET_WRITE,                             /**< Alarm set write event. */
    BLE_MOVEMENT_ALARM_CLEAR_WRITE                            /**< Alarm clear write event. */          
} ble_movement_write_evt_type_t;

/**@brief movement Service value write event. */
typedef struct
{
    ble_movement_write_evt_type_t evt_type;                                  /**< Type of write event. */
} ble_movement_write_evt_t;

// Forward declaration of the ble_movement_low_t type. 
typedef struct ble_movement_s ble_movement_t;

/**@brief movement value write event handler type. */
typedef void (*ble_movement_write_evt_handler_t) (ble_movement_t * p_device, ble_movement_write_evt_t * p_evt);

/**@brief movement Service event handler type. */
typedef void (*ble_movement_evt_handler_t) (ble_movement_t * p_bas, ble_movement_low_evt_t * p_evt);

/**@brief movement   Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct                                                    
{                                                                 
    ble_movement_evt_handler_t    evt_handler;                    /**< Event handler to be called for handling events in the movement Service. */
    ble_movement_write_evt_handler_t    write_evt_handler;              /**< Event handler to be called for handling write events in the Device Management Service. */
    bool                          support_notification;           /**< TRUE if notification of movement measurement is supported. */
    ble_srv_report_ref_t *        p_report_ref;                   /**< If not NULL, a Report Reference descriptor with the specified value will be added to the movement Level characteristic */
    uint8_t												movement_alarm_set;             /**< Alarm set for movement */
    uint8_t                       movement_alarm_clear;           /**< Alarm clear for movement */
    uint8_t												move_alarm_with_time_stamp[8];	/**< Alarm for movement with time of alarm **/
    ble_srv_cccd_security_mode_t  movement_char_attr_md;          /**< Initial security level for movement characteristics attribute */
    ble_srv_cccd_security_mode_t  movement_char_attr_md2;         /**< Initial security level for movement characteristics attribute */
    ble_gap_conn_sec_mode_t       battery_level_report_read_perm; /**< Initial security level for movement report read attribute */
} ble_movement_init_t;

/**@brief movement Service structure. This contains various status information for the service. */
typedef struct ble_movement_s
{                                                                 
    ble_movement_evt_handler_t    evt_handler;                    /**< Event handler to be called for handling events in the movement Service. */
    ble_movement_write_evt_handler_t    write_evt_handler;              /**< Event handler to be called for handling write events in the Device Management Service. */
    uint16_t                      service_handle;                 /**< Handle of movement Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;                   
    ble_gatts_char_handles_t      current_movement_handles;   	  /**< Handles for movement characteristic. */
    ble_gatts_char_handles_t      movement_alarm_set_handles;  	  /**< Handles for movement alarm set characteristic. */
    ble_gatts_char_handles_t      movement_alarm_clear_handles;   /**< Handles for movement alarm clear characteristic. */
    ble_gatts_char_handles_t      movement_alarm_handles;      	  /**< Handles for movement alarm characteristic. */
    ble_gatts_char_handles_t      current_movement_cordinates_handles;
    uint16_t                      report_ref_handle;          	  /**< Handle of the Report Reference descriptor. */
    uint8_t												movement_alarm_set;   	        /**< Alarm set for movement **/
    uint8_t												movement_alarm_clear;   	      /**< Alarm clear for movement **/
    uint8_t												move_alarm_with_time_stamp[8];	/**< Alarm for movement with time of alarm **/
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification of movement  is supported. */
} ble_movements_t;                                                


/**@brief Function for initializing the movement Service.
*
* @param[out]  p_bas       movement Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_movement_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_movement_init(ble_movement_t * p_movement, const ble_movement_init_t * p_movement_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the movement Service.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       ble_waterps_low_movement_level_update() must be called upon reconnection if the
*       movement level has changed while the service has been disconnected from a bonded
*       client.
*
* @param[in]   p_movement      Movement Service structure.
* @param[in]   p_ble_evt       Event received from the BLE stack.
*/
void ble_movement_on_ble_evt(ble_movement_t * p_movement, ble_evt_t * p_ble_evt);

/**@brief Function for updating the movement service characteristics.
*
* @details The application calls this function after having performed a movement measurement. If
*          notification has been enabled, the movement level characteristic is sent to the client.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       this function must be called upon reconnection if the movement  has changed
*       while the service has been disconnected from a bonded client.
*
* @param[in]   p_movement          movement Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_movement_alarm_check(ble_movement_t * p_movement,ble_device_t *p_device);


void update_movement_alarmtimestamp_on_connect(ble_movement_t * p_movement,ble_device_t *p_device);

/**@brief Function for clearing alarm if alarm-clear is set the movement level.
*
* @param[in]   p_movement          movement Service structure.
*
* @param[in]   p_device     Device management  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t reset_alarm(ble_movement_t * p_movement);


/**@brief Function for LED blinking (Used for debugging).
*/
void blink_LED(void);
#endif 

/** @} */
