/** @file
*
* @defgroup ble_sdk_srv_bas PIR Service
* @{
* @ingroup ble_sdk_srv
* @brief PIR Service module.
*
* @details This module implements the PIR Service with the PIR alarm characteristic.
*          During initialization it adds the PIR Service  to the BLE stack database. 
*
* @note The application must propagate BLE stack events to the waterpresence Service module by calling
*       ble_pir_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Hariprasad C R
* Date   : 11/29/2013
*
*/

#ifndef BLE_PR_LOW_H__
#define BLE_PIR_LOW_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "wimoto_sensors.h"
#include "ble_device_mgmt_service.h"

/**@brief PIR Service event type. */
typedef enum
{
    BLE_PIR_LOW_EVT_NOTIFICATION_ENABLED,                          /**< PIR value notification enabled event. */
    BLE_PIR_LOW_EVT_NOTIFICATION_DISABLED                          /**< PIR value notification disabled event. */
} ble_pir_low_evt_type_t;

/**@brief PIR Service event. */
typedef struct
{
    ble_pir_low_evt_type_t evt_type;                               /**< Type of event. */
} ble_pir_low_evt_t;

// Forward declaration of the ble_pir_low_t type. 
typedef struct ble_pir_s ble_pir_t;

/**@brief PIR Service event handler type. */
typedef void (*ble_pir_evt_handler_t) (ble_pir_t * p_bas, ble_pir_low_evt_t * p_evt);

/**@brief PIR Service write event handler type. */
typedef void (*ble_pir_write_evt_handler_t) (void);

/**@brief PIR   Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{                                                                 
    ble_pir_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the PIR Service. */
    ble_pir_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    bool                          support_notification;           /**< TRUE if notification of PIR Level measurement is supported. */
    ble_srv_report_ref_t *        p_report_ref;                   /**< If not NULL, a Report Reference descriptor with the specified value will be added to the PIR Level characteristic */
    uint8_t												pir_alarm_set;                  /** Alarm set for PIR */
    uint8_t												pir_alarm_with_time_stamp[8];   /** Alarm for PIR with time stamp */
    ble_srv_cccd_security_mode_t  pir_char_attr_md;               /**< Initial security level for PIR characteristics attribute */
    ble_srv_cccd_security_mode_t  pir_char_attr_md2;              /**< Initial security level for PIR characteristics attribute */
    ble_gap_conn_sec_mode_t       battery_level_report_read_perm; /**< Initial security level for PIR report read attribute */
} ble_pir_init_t;

/**@brief PIR Service structure. This contains various status information for the service. */
typedef struct ble_pir_s
{
    ble_pir_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the PIR Service. */
    ble_pir_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    uint16_t                      service_handle;                 /**< Handle of PIR Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;                      
    ble_gatts_char_handles_t      current_pir_handles;            /**< Handles for PIR  characteristic. */
    ble_gatts_char_handles_t      pir_alarm_set_handles;  	      /**< Handles for PIR alarm set characteristic. */
    ble_gatts_char_handles_t      pir_alarm_handles;      	      /**< Handles for PIR alarm characteristic. */
    uint16_t                      report_ref_handle;      	      /**< Handle of the Report Reference descriptor. */
    uint8_t												pir_alarm_set;          	      /**< Alarm set for PIR */
    uint8_t												pir_alarm_with_time_stamp[8];   /** Alarm for PIR with time stamp */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification of PIR  is supported. */
} ble_pir_t;


/**@brief Function for initializing the PIR Service.
*
* @param[out]  p_pir       PIR Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_pir_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_pir_init(ble_pir_t * p_pir, const ble_pir_init_t * p_pir_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the PIR Service
*
* @param[in]   p_pir      PIR Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_pir_on_ble_evt(ble_pir_t * p_pir, ble_evt_t * p_ble_evt);

/**@brief Function for updating the PIR .
*
* @details The application calls this function after having performed a PIR measurement. If
*          notification has been enabled, the PIR level characteristic is sent to the client.
*
* @param[in]   p_pir          PIR Service structure.
*
* @param[in]   p_device     Device management  Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_pir_alarm_check(ble_pir_t *p_pir,ble_device_t *p_device);


#endif 

/** @} */
