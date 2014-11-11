/** @file
*
* @defgroup ble_sdk_srv_bas waterpresence Service
* @{
* @ingroup ble_sdk_srv
* @brief waterpresence Service module.
*
* @details This module implements the waterpresence Service with the waterpresence alarm characteristic.
*          During initialization it adds the waterpresence Service  to the BLE stack database. 
*
* @note The application must propagate BLE stack events to the waterpresence Service module by calling
*       ble_waterps_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Sherin
* Date   : 11/19/2013
*
*/

#ifndef BLE_WATERPS_LOW_H__
#define BLE_WATERPS_LOW_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "wimoto_sensors.h"
#include "ble_device_mgmt_service.h"

/**@brief waterpresence Service event type. */
typedef enum
{
    BLE_WATERPS_LOW_EVT_NOTIFICATION_ENABLED,                             /**< waterpresence value notification enabled event. */
    BLE_WATERPS_LOW_EVT_NOTIFICATION_DISABLED                             /**< waterpresence value notification disabled event. */
} ble_waterps_low_evt_type_t;

/**@brief waterpresence Service event. */
typedef struct
{
    ble_waterps_low_evt_type_t evt_type;                                  /**< Type of event. */
} ble_waterps_low_evt_t;
typedef enum
{
    BLE_WATERPS_EVT_INDICATION_ENABLED,                                         /**< water presence  alarm value indication enabled event. */
    BLE_WATERPS_EVT_INDICATION_DISABLED,                                        /**< water presence  alarm value indication disabled event. */
    BLE_WATERPS_EVT_INDICATION_CONFIRMED                                        /**< Confirmation of a water presence  alarm indication has been received. */
} ble_waterps_alarm_type_t;

typedef struct
{
    ble_waterps_alarm_type_t evt_type;                                            /**< Type of event. */
} ble_waterps_alarm_evt_t;
// Forward declaration of the ble_waterps_low_t type. 
typedef struct ble_waterps_s ble_waterps_t;

/**@brief waterpresence Service event handler type. */
typedef void (*ble_waterps_evt_handler_t) (ble_waterps_t * p_bas, ble_waterps_low_evt_t * p_evt);

/**@brief waterpresence Service write event handler type. */
typedef void (*ble_waterps_write_evt_handler_t) (void);

/**@brief waterpresence   Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_waterps_evt_handler_t     evt_handler;                       /**< Event handler to be called for handling events in the waterpresence Service. */
    ble_waterps_write_evt_handler_t write_evt_handler;
    bool                          support_notification;              /**< TRUE if notification of waterpresence Level measurement is supported. */
    ble_srv_report_ref_t *        p_report_ref;                      /**< If not NULL, a Report Reference descriptor with the specified value will be added to the waterpresence Level characteristic */
    uint8_t												water_waterpresence_alarm_set;     /**< Alarm set for waterpresence **/
    uint8_t												waterps_alarm_with_time_stamp[8];	 /**< Alarm for water presence with time of alarm **/
    ble_srv_cccd_security_mode_t  waterpresence_char_attr_md;        /**< Initial security level for waterpresence characteristics attribute */
    ble_srv_cccd_security_mode_t  waterpresence_char_attr_md2;       /**< Initial security level for waterpresence characteristics attribute */
    ble_gap_conn_sec_mode_t       battery_level_report_read_perm;    /**< Initial security level for waterpresence report read attribute */
} ble_waterps_init_t;

/**@brief waterpresence Service structure. This contains various status information for the service. */
typedef struct ble_waterps_s
{
    ble_waterps_evt_handler_t     evt_handler;                      /**< Event handler to be called for handling events in the waterpresence Service. */
    ble_waterps_write_evt_handler_t write_evt_handler;
    uint16_t                      service_handle;                   /**< Handle of waterpresence Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      current_waterp_handles;          	/**< Handles for waterpresence  characteristic. */
    ble_gatts_char_handles_t      water_waterp_alarm_set_handles;  	/**< Handles for waterpresence alarm set characteristic. */
    ble_gatts_char_handles_t      water_waterp_alarm_handles;      	/**< Handles for waterpresence alarm characteristic. */
    uint16_t                      report_ref_handle;              	/**< Handle of the Report Reference descriptor. */
    uint8_t												water_waterpresence_alarm_set;   	/**< Alarm set for waterpresence **/
    uint8_t												waterps_alarm_with_time_stamp[8];	 /**< Alarm for water presence with time of alarm **/
    uint16_t                      conn_handle;                      /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;        /**< TRUE if notification of waterpresence  is supported. */
} ble_waterps_t;

/**@brief Function for initializing the waterpresence Service.
*
* @param[out]  p_bas       waterpresence Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_waterps_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_waterps_init(ble_waterps_t * p_bas, const ble_waterps_init_t * p_waterps_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the waterpresence Service.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       ble_waterps_low_waterpresence_level_update() must be called upon reconnection if the
*       waterpresence level has changed while the service has been disconnected from a bonded
*       client.
*
* @param[in]   p_bas      waterpresence Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_waterps_on_ble_evt(ble_waterps_t * p_bas, ble_evt_t * p_ble_evt);

/**@brief Function for updating the waterpresence level.
*
* @details The application calls this function after having performed a waterpresence measurement. If
*          notification has been enabled, the waterpresence level characteristic is sent to the client.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       this function must be called upon reconnection if the waterpresence  has changed
*       while the service has been disconnected from a bonded client.
*
* @param[in]   p_bas          waterpresence Service structure.
*
* @param[in]   p_device       Device management Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_waterps_alarm_check(ble_waterps_t *,ble_device_t *);

#endif // BLE_WATERPS_LOW_H__

/** @} */
