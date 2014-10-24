/** @file
*
* @brief water level alarm Service module.
*
* @details This module implements the water level Service with the water level level alarm characteristic.
* During initialization it adds the water level Service  to the BLE stack database. 
*

*
* @note The application must propagate BLE stack events to the water level Service module by calling
*       ble_waterls_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Sherin
* Date   : 11/19/2013
*
*/

#ifndef BLE_WATERLS_H__
#define BLE_WATERLS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_device_mgmt_service.h"

/**@brief water level service event type. */
typedef enum
{
    BLE_WATERLS_EVT_NOTIFICATION_ENABLED,                             /**< water level value notification enabled event. */
    BLE_WATERLS_EVT_NOTIFICATION_DISABLED                             /**< water level value notification disabled event. */
} ble_waterls_evt_type_t;

/**@brief water level service event. */
typedef struct
{
    ble_waterls_evt_type_t evt_type;                                  /**< Type of event. */
} ble_waterls_evt_t;

// Forward declaration of the ble_waterls_t type. 
typedef struct ble_waterls_s ble_waterls_t;

/**@brief water level Service event handler type. */
typedef void (*ble_waterls_evt_handler_t) (ble_waterls_t * p_waterls, ble_waterls_evt_t * p_evt);

/**@brief water level Service write event handler type. */
typedef void (*ble_waterls_write_evt_handler_t) (void);

/**@brief water level Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_waterls_evt_handler_t    evt_handler;                          /**< Event handler to be called for handling events in the water level Service. */
    ble_waterls_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    bool                         support_notification;                 /**< TRUE if notification of water level Level measurement is supported. */
    ble_srv_report_ref_t *       p_report_ref;                         /**< If not NULL, a Report Reference descriptor with the specified value will be added to the water level Level characteristic */
    uint8_t                      waterl_level_low_value;               /**< water level low level */
    uint8_t                      waterl_level_high_value;              /**< water level low level */
    uint8_t											 waterl_level_alarm_set;               /** Alarm set for water level **/
    uint8_t											 waterl_alarm_with_time_stamp[8];	 			/**< Alarm for water level with time of alarm **/
    ble_srv_cccd_security_mode_t waterl_level_char_attr_md;            /**< Initial security level for water level characteristics attribute */
    ble_srv_cccd_security_mode_t waterl_level_char_attr_md2;           /**< Initial security level for water level characteristics attribute */
    ble_gap_conn_sec_mode_t      waterl_level_level_report_read_perm;  /**< Initial security level for water level report read attribute */
} ble_waterls_init_t;

/**@brief water level Service structure. This contains various status information for the service. */
typedef struct ble_waterls_s
{
    ble_waterls_evt_handler_t     evt_handler;                         /**< Event handler to be called for handling events in the water level Service. */
    ble_waterls_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    uint16_t                      service_handle;                      /**< Handle of water level Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      curr_waterl_level_level_handles;     /**< Handles for current water level characteristic. */
    ble_gatts_char_handles_t      waterl_level_low_level_handles;      /**< Handles for water level low Level characteristic. */
    ble_gatts_char_handles_t      waterl_level_high_level_handles;     /**< Handles for water level high Level characteristic. */
    ble_gatts_char_handles_t      waterl_level_alarm_set_handles;  	   /**< Handles for water level alarm set characteristic. */
    ble_gatts_char_handles_t      waterl_level_alarm_handles;      	   /**< Handles for water level alarm characteristic. */
    ble_gatts_char_handles_t      switch_mode_handles;      	         /**< Handles for switch mode characteristic. */
    uint16_t                      report_ref_handle;                 	 /**< Handle of the Report Reference descriptor. */
    uint8_t                       waterl_level_low_level;   				   /**< water level low level value. */
    uint8_t                       waterl_level_high_level;   				   /**< water level high level value. */
    uint8_t												waterl_level_alarm_set;   					 /** Alarm set for water level **/
    uint8_t											  waterl_alarm_with_time_stamp[8];	 			/**< Alarm for water level with time of alarm **/
    uint16_t                      conn_handle;                    	   /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;     	     /**< TRUE if notification of water level Level is supported. */
} ble_waterls_t;

/**@brief Function for initializing the water level Service.
*
* @param[out]  p_waterls   water level Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_waterls_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_waterls_init(ble_waterls_t * waterls, const ble_waterls_init_t * p_waterls_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the water level Service.
*
*
* @param[in]   p_waterls    water level Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_waterls_on_ble_evt(ble_waterls_t * p_waterls, ble_evt_t * p_ble_evt);

/**@brief Function for updating the water level level.
*
* @details The application call this function for water level level measurement and checking the alarm condition. If
*          notification has been enabled, the water level level characteristic is sent to the client.
*
*
* @param[in]   p_waterls  water level level Service structure.
*
* @param[in]   p_device       Device management Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_waterls_level_alarm_check(ble_waterls_t *,ble_device_t *);

/**@brief Function for reading water level from sensor **/
uint8_t read_waterl_level(void);											


#endif //BLE_WATERLS_H__

/** @} */
