/** @file
*
* @brief Soil moisture alarm Service module.
*
* @details This module implements the Soil moisture Service with the Soil moisture level alarm characteristic.
* During initialization it adds the Soil moisture Service  to the BLE stack database. 
*

*
* @note The application must propagate BLE stack events to the soil moisture Service module by calling
*       ble_soils_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Shafy
* Date   : 11/06/2013
*
*/

#ifndef BLE_SOILS_H__
#define BLE_SOILS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_device_mgmt_service.h"

/**@brief Soil Service event type. */
typedef enum
{
    BLE_SOILS_EVT_NOTIFICATION_ENABLED,                             /**< Soil moisture value notification enabled event. */
    BLE_SOILS_EVT_NOTIFICATION_DISABLED                             /**< Soil moisture value notification disabled event. */
} ble_soils_evt_type_t;

/**@brief Soil Service event. */
typedef struct
{
    ble_soils_evt_type_t evt_type;                                  /**< Type of event. */
} ble_soils_evt_t;

// Forward declaration of the ble_soils_t type. 
typedef struct ble_soils_s ble_soils_t;

/**@brief Soil moisture Service event handler type. */
typedef void (*ble_soils_evt_handler_t) (ble_soils_t * p_soils, ble_soils_evt_t * p_evt);

/**@brief Soil moisture Service write event handler type. */
typedef void (*ble_soils_write_evt_handler_t) (void);

/**@brief Soil moisture Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_soils_evt_handler_t      evt_handler;                       /**< Event handler to be called for handling events in the soil moisture Service. */
    ble_soils_write_evt_handler_t write_evt_handler;                /**< Event handler to be called for handling write events */
    bool                         support_notification;              /**< TRUE if notification of Soil moisture Level measurement is supported. */
    ble_srv_report_ref_t *       p_report_ref;                      /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Soil moisture Level characteristic */
    uint8_t                      soil_mois_low_value;               /**< Soil moisture low level */
    uint8_t                      soil_mois_high_value;              /**< Soil moisture low level */
    uint8_t											 soil_mois_alarm_set;               /**< Alarm set for Soil moisture **/
    uint8_t											 soil_alarm_with_time_stamp[8];		 /**< Alarm for Soil moisture with time of alarm **/
    ble_srv_cccd_security_mode_t soil_mois_char_attr_md;            /**< Initial security level for Soil moisture characteristics attribute */
    ble_srv_cccd_security_mode_t soil_mois_char_attr_md2;           /**< Initial security level for Soil moisture characteristics attribute */
    ble_gap_conn_sec_mode_t      soil_mois_level_report_read_perm;  /**< Initial security level for Soil moisture report read attribute */
} ble_soils_init_t;

/**@brief Soil moisture Service structure. This contains various status information for the service. */
typedef struct ble_soils_s
{
    ble_soils_evt_handler_t       evt_handler;                      /**< Event handler to be called for handling events in the Soil moisture Service. */
    ble_soils_write_evt_handler_t write_evt_handler;                /**< Event handler to be called for handling write events */
    uint16_t                      service_handle;                   /**< Handle of Soil moisture Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      curr_soil_mois_level_handles;     /**< Handles for current soil moisture characteristic. */
    ble_gatts_char_handles_t      soil_mois_low_level_handles;      /**< Handles for soil moisture low Level characteristic. */
    ble_gatts_char_handles_t      soil_mois_high_level_handles;     /**< Handles for soil moisture high Level characteristic. */
    ble_gatts_char_handles_t      soil_mois_alarm_set_handles;  	  /**< Handles for soil moisture alarm set characteristic. */
    ble_gatts_char_handles_t      soil_mois_alarm_handles;      	  /**< Handles for soil moisture alarm characteristic. */
    uint16_t                      report_ref_handle;              	/**< Handle of the Report Reference descriptor. */
    uint8_t                       soil_mois_low_level;   				    /**< soil moisture low level value. */
    uint8_t                       soil_mois_high_level;   					/**< soil moisture high level value. */
    uint8_t												soil_mois_alarm_set;   						/**< Alarm set for soil moisture **/
    uint8_t											 soil_alarm_with_time_stamp[8];		 /**< Alarm for Soil moisture with time of alarm **/
    uint16_t                      conn_handle;                    	/**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;     	  /**< TRUE if notification of soil moisture Level is supported. */
} ble_soils_t;


/**@brief Function for initializing the soil moisture Service.
*
* @param[out]  p_soils     Soil moisture Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_soils_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_soils_init(ble_soils_t * p_soils, const ble_soils_init_t * p_soils_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the soil moisture Service.
*
*
* @param[in]   p_soils    Soil moisture Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_soils_on_ble_evt(ble_soils_t * p_soils, ble_evt_t * p_ble_evt);

/**@brief Function for updating the soil moisture level.
*
* @details The application call this function for soil moisture level measurement and checking the alarm condition. If
*          notification has been enabled, the soil moisture level characteristic is sent to the client.
*
*
* @param[in]   p_soils  soil moistue level Service structure.
*
* @param[in]   p_Device         Device management Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_soils_level_alarm_check(ble_soils_t *,ble_device_t *);

/**@brief Function to read soil moisture level from sensor interfaced to ADC.
*
* @param[in]   void
* @param[out]  uint16_t current_soil_mois_level.
*/
uint8_t read_soil_mois_level(void);											/**@brief Function for reading soil moisture from sensor **/


#endif 

/** @} */
