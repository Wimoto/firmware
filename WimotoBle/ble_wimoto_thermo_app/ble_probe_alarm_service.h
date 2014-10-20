/** @file
*
* @brief probe temperature alarm Service module.
*
* @details This module implements the probe temperature Service with the probe temperature level alarm characteristic.
* During initialization it adds the probe temperature Service  to the BLE stack database. 
*

*
* @note The application must propagate BLE stack events to the probe temperature Service module by calling
*       ble_hums_low_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Shafy
* Date   : 11/06/2013
*
*/

#ifndef BLE_PROBES_H__
#define BLE_PROBES_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_device_mgmt_service.h"

/**@brief probe Service event type. */
typedef enum
{
    BLE_PROBES_EVT_NOTIFICATION_ENABLED,                             /**< probe temperature value notification enabled event. */
    BLE_PROBES_EVT_NOTIFICATION_DISABLED                             /**< probe temperature value notification disabled event. */
} ble_probes_evt_type_t;

/**@brief probe Service event. */
typedef struct
{
    ble_probes_evt_type_t evt_type;                                  /**< Type of event. */
} ble_probes_evt_t;

// Forward declaration of the ble_probes_t type. 
typedef struct ble_probes_s ble_probes_t;

/**@brief probe temperature Service event handler type. */
typedef void (*ble_probes_evt_handler_t) (ble_probes_t * p_probes, ble_probes_evt_t * p_evt);

/**@brief probe temperature Service write event handler type. */
typedef void (*ble_probes_write_evt_handler_t) (void);

/**@brief probe temperature Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_probes_evt_handler_t      evt_handler;                       /**< Event handler to be called for handling events in the probe temperature Service. */
    ble_probes_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    bool                         support_notification;              /**< TRUE if notification of probe temperature Level measurement is supported. */
    ble_srv_report_ref_t *       p_report_ref;                      /**< If not NULL, a Report Reference descriptor with the specified value will be added to the probe temperature Level characteristic */
    uint8_t                      probe_temp_low_value[2];               /**< probe temperature low level */
    uint8_t                      probe_temp_high_value[2];              /**< probe temperature low level */
    uint8_t											 probe_temp_alarm_set;               /** Alarm set for probe temperature **/
    uint8_t											 probe_alarm_with_time_stamp[8];   	 /** Alarm for probe temperature with time stamp**/
    ble_srv_cccd_security_mode_t probe_temp_char_attr_md;            /**< Initial security level for probe temperature characteristics attribute */
    ble_srv_cccd_security_mode_t probe_temp_char_attr_md2;           /**< Initial security level for probe temperature characteristics attribute */
    ble_gap_conn_sec_mode_t      probe_temp_level_report_read_perm;  /**< Initial security level for probe temperature report read attribute */
} ble_probes_init_t;

/**@brief probe temperature Service structure. This contains various status information for the service. */
typedef struct ble_probes_s
{
    ble_probes_evt_handler_t       evt_handler;                      /**< Event handler to be called for handling events in the probe temperature Service. */
    ble_probes_write_evt_handler_t write_evt_handler;                 /**< Event handler to be called for handling write events */
    uint16_t                      service_handle;                   /**< Handle of probe temperature Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      curr_probe_temp_level_handles;     /**< Handles for current probe temperature characteristic. */
    ble_gatts_char_handles_t      probe_temp_low_level_handles;      /**< Handles for probe temperature low Level characteristic. */
    ble_gatts_char_handles_t      probe_temp_high_level_handles;     /**< Handles for probe temperature high Level characteristic. */
    ble_gatts_char_handles_t      probe_temp_alarm_set_handles;  	  /**< Handles for probe temperature alarm set characteristic. */
    ble_gatts_char_handles_t      probe_temp_alarm_handles;      	  /**< Handles for probe temperature alarm characteristic. */
    uint16_t                      report_ref_handle;              	/**< Handle of the Report Reference descriptor. */
    uint8_t                       probe_temp_low_level[2];   				    /**< probe temperature low level value. */
    uint8_t                       probe_temp_high_level[2];   					/**< probe temperature high level value. */
    uint8_t												probe_temp_alarm_set;   						/** Alarm set for probe temperature **/
    uint8_t											  probe_alarm_with_time_stamp[8];   	 /** Alarm for probe temperature with time stamp**/
    uint16_t                      conn_handle;                    	/**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;     	  /**< TRUE if notification of probe temperature Level is supported. */
} ble_probes_t;

/**@brief Function for initializing the probe temperature Service.
*
* @param[out]  p_probes     probe temperature Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_temps_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_probes_init(ble_probes_t * probes, const ble_probes_init_t * p_probes_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the probe temperature Service.
*
*
* @param[in]   p_probes    probe temperature Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_probes_on_ble_evt(ble_probes_t * p_probes, ble_evt_t * p_ble_evt);

/**@brief Function for updating the probe temperature level.
*
* @details The application call this function for probe temperature level measurement and checking the alarm condition. If
*          notification has been enabled, the probe temperature level characteristic is sent to the client.
*
* @param[in]   p_probes  probe temperature level Service structure.
*
* @param[in]   p_Device          Device management Service structure
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_probes_level_alarm_check(ble_probes_t *,ble_device_t *);
float convert_probe_level_to_float(uint16_t);

uint16_t read_probe_temp_level(void);											/**@brief Function for reading probe temperature from sensor **/


#endif 

/** @} */
