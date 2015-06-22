/** @file
*
* 
* @brief Device Management module.
*
* @details This module implements the Device Management Service 
*          
* @note The application must propagate BLE stack events to the Device Management Service module by calling
*       ble_device_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Hariprasad C R
* Date   : 11/20/2013
*
*/

#ifndef BLE_DEVICE_MGMT_H__
#define BLE_DEVICE_MGMT_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_date_time.h"


/**@brief Device Management Service event type. */
typedef enum
{
    BLE_DEVICE_LOW_EVT_NOTIFICATION_ENABLED,                             /**< Device notification enabled event. */
    BLE_DEVICE_LOW_EVT_NOTIFICATION_DISABLED                             /**< Device notification disabled event. */
} ble_device_low_evt_type_t;

/**@brief Device Management Service write event type. */
typedef struct
{
    ble_device_low_evt_type_t evt_type;                                  /**< Type of event. */
} ble_device_evt_t;

/**@brief Device Management Service value write event type. */
typedef enum
{
    BLE_DEVICE_DFU_WRITE,                                                /**< DFU write event. */
    BLE_DEVICE_MMA_SWITCH_WRITE,                                         /**< switch mode char write event. */
    BLE_DEVICE_TIME_STAMP_WRITE	                                         /**< time stamp write event. */           
} ble_device_write_evt_type_t;

/**@brief Device Management Service value write event. */
typedef struct
{
    ble_device_write_evt_type_t evt_type;                                /**< Type of write event. */
} ble_device_write_evt_t;


// Forward declaration of the ble_temps_low_t type. 
typedef struct ble_device_s ble_device_t;

/**@brief Device Management value write event handler type. */
typedef void (*ble_device_write_evt_handler_t) (ble_device_t * p_device, ble_device_write_evt_t * p_evt);

/**@brief Device Management Service event handler type. */
typedef void (*ble_device_evt_handler_t) (ble_device_t * p_device, ble_device_evt_t * p_evt);

/**@brief Device Management Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_device_evt_handler_t          evt_handler;                    /**< Event handler to be called for handling events in the Device Management Service. */
    ble_device_write_evt_handler_t    write_evt_handler;              /**< Event handler to be called for handling write events in the Device Management Service. */
    bool                              support_notification;           /**< TRUE if notification of Device Management Level measurement is supported. */
    ble_srv_report_ref_t *            p_report_ref;                   /**< If not NULL, a Report Reference descriptor with the specified value will be added to the characteristic */
    uint8_t												    device_dfu_mode_set;            /**< Device Firmware Update mode set **/
    uint8_t												    device_mma_switch_set;   				/**< mma switch set **/
    uint8_t												    device_time_stamp_set[7];       /**< time stamp set **/
    ble_srv_cccd_security_mode_t      device_char_attr_md;            /**< Initial security level for Device Management characteristics attribute */
    ble_srv_cccd_security_mode_t      device_char_attr_md2;           /**< Initial security level for Device Management characteristics attribute */
    ble_gap_conn_sec_mode_t           battery_level_report_read_perm; /**< Initial security level for Device Management report read attribute */
} ble_device_init_t;

/**@brief Device Management Service structure. This contains various status information for the service. */
typedef struct ble_device_s
{
    ble_device_evt_handler_t          evt_handler;                    /**< Event handler to be called for handling events in the Device Management Service. */
    ble_device_write_evt_handler_t    write_evt_handler;              /**< Event handler to be called for handling events in the Device Management Service. */
    uint16_t                          service_handle;                 /**< Handle of temperature Service (as provided by the BLE stack). */
    uint8_t                           uuid_type;
    ble_gatts_char_handles_t          mma_switch_handles;            	/**< Handles for Device Firmware Update characteristic. */
    ble_gatts_char_handles_t          dfu_mode_handles;             	/**< Handles for  MMA7660 switching characteristic. */
    ble_gatts_char_handles_t          time_stamp_handles;             /**< Handles for  time stamp characteristic. */
    uint8_t												    device_dfu_mode_set;            /**< Device Firmware Update mode set **/
    uint8_t												    device_mma_switch_set;   		  	/**< mma switch set **/
    uint8_t												    device_time_stamp_set[7];       /**< time stamp set **/
    uint16_t                          report_ref_handle;              /**< Handle of the Report Reference descriptor. */
    uint16_t                          conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                              is_notification_supported;      /**< TRUE if notification of Device Management is supported.*/
} ble_device_t;

/**@brief Function for initializing the Device Management Service.
*
* @param[out]  p_device    Device Management Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_device_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_device_init(ble_device_t * p_device, const ble_device_init_t * p_device_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Service.
*
*
* @param[in]   p_device   Device Management Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_device_on_ble_evt(ble_device_t * p_device, ble_evt_t * p_ble_evt);

/**@brief Function for creating a new time stamp in Device Management Service.
*
* @details The application calls this function after the user sets a new time.
*
*
* @param[in]   p_device         Device Management Service structure.
* @param[in]   p_time_stamp     Time stamp structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
void create_time_stamp(ble_device_t * p_device, ble_date_time_t * p_time_stamp);

/**@brief Function for updating time to the client.
*
* @details The application calls this function after every time update i.e, after every second.
*
*
* @param[in]   p_device         Device Management Service structure.
* @param[in]   p_time_stamp     Time stamp structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_time_update(ble_device_t * p_device, ble_date_time_t *p_time_stamp);
#endif 

/** @} */
