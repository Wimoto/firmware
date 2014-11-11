/** @file
*
* @defgroup ble_sdk_srv_bas Light Service
* @{
* @ingroup ble_sdk_srv
* @brief Light Service module.
*
* @details This module implements the Light level Service with the Light level alarm characteristic.
*          During initialization it adds the Light level Service  to the BLE stack database. 
*

*
* @note The application must propagate BLE stack events to the temperature Service module by calling
*       ble_temps_low_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Shafy
* Date   : 10/19/2013
*
*/

#ifndef BLE_LIGHTS_H__
#define BLE_LIGHTS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_device_mgmt_service.h"

/**@brief Light Service event type. */
typedef enum
{
    BLE_LIGHTS_LOW_EVT_NOTIFICATION_ENABLED,                             /**< Light value notification enabled event. */
    BLE_LIGHTS_LOW_EVT_NOTIFICATION_DISABLED                             /**< Light value notification disabled event. */
} ble_lights_low_evt_type_t;

/**@brief Light Service event. */
typedef struct
{
    ble_lights_low_evt_type_t evt_type;                                  /**< Type of event. */
} ble_lights_evt_t;
typedef enum
{
    BLE_LIGHTS_EVT_INDICATION_ENABLED,                                         /**< Lights  alarm value indication enabled event. */
    BLE_LIGHTS_EVT_INDICATION_DISABLED,                                        /**< Lights  alarm value indication disabled event. */
    BLE_LIGHTS_EVT_INDICATION_CONFIRMED                                        /**< Confirmation of a Lights  alarm indication has been received. */
} ble_lights_alarm_type_t;

typedef struct
{
    ble_lights_alarm_type_t evt_type;                                            /**< Type of event. */
} ble_lights_alarm_evt_t;

// Forward declaration of the ble_temps_low_t type. 
typedef struct ble_lights_s ble_lights_t;

/**@brief Light Service event handler type. */
typedef void (*ble_lights_evt_handler_t) (ble_lights_t * p_lights, ble_lights_evt_t * p_evt);

/**@brief Light Service write event handler type. */
typedef void (*ble_lights_write_evt_handler_t) (void);

/**@brief Light Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_lights_evt_handler_t       	evt_handler;                   		/**< Event handler to be called for handling events in the Light Service. */
    ble_lights_write_evt_handler_t 	write_evt_handler;            		/**< Event handler to be called for handling write events */
    bool                           	support_notification;          		/**< TRUE if notification of Light Level measurement is supported. */
    ble_srv_report_ref_t *         	p_report_ref;                  		/**< If not NULL, a Report Reference descriptor with the specified value will be added to the Light Level characteristic */
    uint8_t                        	climate_light_low_value[2];    		/**< temperature low level */
    uint8_t                        	climate_light_high_value[2];  		/**< temperature low level */
    uint8_t												 	climate_light_alarm_set;       		/**< Alarm set for temperature **/
    uint8_t													lights_alarm_with_time_stamp[8]; 	/**< Alarm for light with time of alarm **/
    ble_srv_cccd_security_mode_t   	lights_char_attr_md;          		/**< Initial security level for Light characteristics attribute */
    ble_srv_cccd_security_mode_t 		lights_char_attr_md2;          		/**< Initial security level for Light characteristics attribute */
    ble_gap_conn_sec_mode_t        	battery_level_report_read_perm;		/**< Initial security level for Light report read attribute */
} ble_lights_init_t;

/**@brief Light Service structure. This contains various status information for the service. */
typedef struct ble_lights_s
{
    ble_lights_evt_handler_t      evt_handler;                    /**< Event handler to be called for handling events in the Light Service. */
    ble_lights_write_evt_handler_t write_evt_handler;             /**< Event handler to be called for handling write events */
    uint16_t                      service_handle;                 /**< Handle of temperature Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      current_light_level_handles;    /**< Handles for temperature  characteristic. */
    ble_gatts_char_handles_t      light_low_level_handles;        /**< Handles for temperature low Level characteristic. */
    ble_gatts_char_handles_t      light_high_level_handles;       /**< Handles for temperature high Level characteristic. */
    ble_gatts_char_handles_t      climate_light_alarm_set_handles;/**< Handles for temperature alarm set characteristic. */
    ble_gatts_char_handles_t      climate_light_alarm_handles;    /**< Handles for temperature alarm characteristic. */
    uint16_t                      report_ref_handle;              /**< Handle of the Report Reference descriptor. */
    uint8_t                       climate_light_low_level[2];   	/**< temperature low level for alarm for temperature Service. */
    uint8_t                       climate_light_high_level[2];    /**< temperature high level for alarm for temperature Service. */
    uint8_t												climate_light_alarm_set;       	/**< Alarm set for temperature **/
    uint8_t												lights_alarm_with_time_stamp[8];/**< Alarm for light with time of alarm **/
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification of Light Level is supported. */
} ble_lights_t;



/**@brief Function for initializing the Light Service.
*
* @param[out]  p_bas       Light Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_temps_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_lights_init(ble_lights_t * lights, const ble_lights_init_t * p_lights_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Light Service.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       ble_temps_low_Light_level_update() must be called upon reconnection if the
*       Light level has changed while the service has been disconnected from a bonded
*       client.
*
* @param[in]   p_bas      Light Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_lights_on_ble_evt(ble_lights_t * p_lights, ble_evt_t * p_ble_evt);

/**@brief Function for updating the Light level.
*
* @details The application calls this function after having performed a Light measurement. If
*          notification has been enabled, the Light level characteristic is sent to the client.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       this function must be called upon reconnection if the Light level has changed
*       while the service has been disconnected from a bonded client.
*
* @param[in]   p_lights          Light Service structure.
*
* @param[in]   p_device       Device management Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_lights_level_alarm_check(ble_lights_t *, ble_device_t *);

/**@brief Function to read light_level from isl29023.
*
* @param[in]    void
* @param[out]   uint16_t current_light_level.
*/
uint16_t read_light_level(void);						

#endif // BLE_LIGHTS_H__

/** @} */
