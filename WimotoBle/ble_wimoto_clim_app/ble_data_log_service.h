/** @file
*
* @brief Data logger module.
*
* @details This module implements the Data logger service.
*          During initialization it adds the Data logger  to the BLE stack. 
*
*
* @note The application must propagate BLE stack events to the Data logger module by calling
*       ble_dlogs_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Shafy
* Date   : 10/19/2013
*
*/

#ifndef BLE_DLOGS_LOW_H__
#define BLE_DLOGS_LOW_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_date_time.h"

/**@brief Data logger event type. */
typedef enum
{
    BLE_DLOGS_EVT_NOTIFICATION_ENABLED,                             /**< Notification enabled event. */
    BLE_DLOGS_EVT_NOTIFICATION_DISABLED                             /**< Notification disabled event. */
} ble_dlogs_evt_type_t;

/**@brief Data logger event. */
typedef struct
{
    ble_dlogs_evt_type_t evt_type;                                  /**< Type of event. */
} ble_dlogs_evt_t;

/**@brief Data logger Service value write event type. */
typedef enum
{
    BLE_DLOGS_ENABLE_WRITE,                                        /**< Data logger enable write event. */
    BLE_DLOGS_READ_SWITCH_WRITE                                    /**< Data log read char write event. */
}ble_dlogs_write_evt_type_t;

/**@brief Data logger Service value write event. */
typedef struct
{
    ble_dlogs_write_evt_type_t evt_type;                          /**< Type of write event. */
} ble_dlogs_write_evt_t;

// Forward declaration of the ble_dlogs_t type. 
typedef struct ble_dlogs_s ble_dlogs_t;

/**@brief Data logger event handler type. */
typedef void (*ble_dlogs_evt_handler_t) (ble_dlogs_t * p_dlogs, ble_dlogs_evt_t * p_evt);

/**@brief Data logger  value write event handler type. */
typedef void (*ble_dlogs_write_evt_handler_t) (ble_dlogs_t * p_dlogs, ble_dlogs_write_evt_t * p_evt);

/**@brief Data logger Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_dlogs_evt_handler_t       evt_handler;                  /**< Event handler to be called for handling events in the Data logger. */
    ble_dlogs_write_evt_handler_t write_evt_handler;            /**< Event handler to be called for handling write events in the Data logger. */
    bool                          support_notification;         /**< TRUE if notification of Temperature Level measurement is supported. */
    ble_srv_report_ref_t *        p_report_ref;                 /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Temperature Level characteristic */
    uint8_t                       data_logger_enable;  					/**< switch to enable data logging functionality */
    uint8_t                       read_data_switch;  						 /**< switch to start reading data */
    uint8_t                       data[8]; 											/**< data logged in flash to send over BLE */
    ble_srv_cccd_security_mode_t  dlogs_char_attr_md;         	/**< Initial security level for data logger characteristics attribute */
    ble_srv_cccd_security_mode_t  dlogs_char_attr_md2;         	/**< Initial security level for data logger characteristics attribute */
    ble_gap_conn_sec_mode_t       dlogs_report_read_perm;       /**< Initial security level for data logger read attribute */
} ble_dlogs_init_t;

/**@brief Data logger structure. This contains various status information for the service. */
typedef struct ble_dlogs_s
{
    ble_dlogs_evt_handler_t       evt_handler;                   /**< Event handler to be called for handling events in the Data logger. */
    ble_dlogs_write_evt_handler_t write_evt_handler;             /**< Event handler to be called for handling write events in the Data logger. */
    uint16_t                      service_handle;                /**< Handle of Data logger (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      data_logger_enable_handles;    /**< Handles for temperature  characteristic. */
    ble_gatts_char_handles_t      data_handles;          	       /**< Handles for temperature low Level characteristic. */
    ble_gatts_char_handles_t      read_data_handles;          	 /**< Handles for temperature low Level characteristic. */
    uint16_t                      report_ref_handle;             /**< Handle of the Report Reference descriptor. */
    uint8_t                       data_logger_enable;  					 /**< switch to enable data logging functionality */
    uint8_t                       read_data_switch;  						 /**< switch to start reading data */
    uint8_t                       data[16]; 										 /**< data logged in flash to send over BLE */
    uint16_t                      conn_handle;                   /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;     /**< TRUE if notification of Temperature Level is supported. */
} ble_dlogs_t;

typedef enum
{
    READ,
    TXMIT,
    READ_COMPLETE
} send_state;

/**@brief Function for initializing the Data logger.
*
* @param[out]  p_dlogs       Data logger structure. This structure will have to be supplied by
*                            the application. It will be initialized by this function, and will later
*                            be used to identify this particular service instance.
* @param[in]   p_dlogs_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_dlogs_init(ble_dlogs_t * p_dlogs, const ble_dlogs_init_t * p_dlogs_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Data logger.
*
*
* @param[in]   p_dlogs      Data logger structure.
* @param[in]   p_ble_evt    Event received from the BLE stack.
*/
void ble_dlogs_on_ble_evt(ble_dlogs_t * p_dlogs, ble_evt_t * p_ble_evt);

/**@brief Function write sensor data to flash.
*
* @param[in]   data             Data buffer.
* 
*/
void write_data_flash(uint32_t * data);																

/**@brief Function to send data to the connected BLE central device.
*
* @param[in]   ble_dlogs        Data logger service structure.
*
* 
*/
void send_data(ble_dlogs_t * ble_dlogs);               

/**@brief Function reading data to flash and sending to the connected BLE central device.
*
* @param[in]   ble_dlogs        Data logger service structure.
* @param[in]   data             Data buffer.
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t read_data_flash(ble_dlogs_t * ble_dlogs, uint32_t * data);

/**@brief Function to send the data to the connected central device.
*
* @param[in]   ble_dlogs  Data logger service structure.
* @param[in]   data       Data buffer.
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t send_data_to_central(ble_dlogs_t * ble_dlogs,uint32_t *data);

/**@brief Function to reset the data logger enable and read data switch characteristics.
*
* @param[in]   ble_dlogs        Data logger service structure.
* 
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t reset_data_log(ble_dlogs_t * ble_dlogs);									


#endif // BLE_DLOGS_H__

/** @} */

