/** @file
*
* @defgroup ble_sdk_srv_bas Thermopile Service
* @{
* @ingroup ble_sdk_srv
* @brief Thermopile Service module.
*
* @details This module implements the Thermopile Service with the Thermopile alarm characteristic.
*          During initialization it adds the Thermopile Service  to the BLE stack database. 
*

*
* @note The application must propagate BLE stack events to the thermopile Service module by calling
*       ble_thermops_low_on_ble_evt() from the from the @ref ble_stack_handler callback.
* 
* Author : Shafy
* Date   : 10/19/2013
*
*/

#ifndef BLE_THERMOPS_LOW_H__
#define BLE_THERMOPS_LOW_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "wimoto_sensors.h"


/**@brief Thermopile Service event type. */
typedef enum
{
    BLE_THERMOPS_LOW_EVT_NOTIFICATION_ENABLED,                             /**< Thermopile value notification enabled event. */
    BLE_THERMOPS_LOW_EVT_NOTIFICATION_DISABLED                             /**< Thermopile value notification disabled event. */
} ble_thermops_low_evt_type_t;

/**@brief Thermopile Service event. */
typedef struct
{
    ble_thermops_low_evt_type_t evt_type;                                  /**< Type of event. */
} ble_thermops_low_evt_t;

// Forward declaration of the ble_thermops_low_t type. 
typedef struct ble_thermops_s ble_thermops_t;

/**@brief Thermopile Service event handler type. */
typedef void (*ble_thermops_evt_handler_t) (ble_thermops_t * p_bas, ble_thermops_low_evt_t * p_evt);

/**@brief Thermopile Service write event handler type. */
typedef void (*ble_thermops_write_evt_handler_t) (void);

/**@brief Thermopile   Service init structure. This contains all options and data needed for
*        initialization of the service.*/
typedef struct
{
    ble_thermops_evt_handler_t       evt_handler;                     /**< Event handler to be called for handling events in the Thermopile Service. */
    ble_thermops_write_evt_handler_t write_evt_handler;
    bool                          support_notification;              /**< TRUE if notification of Thermopile Level measurement is supported. */
    ble_srv_report_ref_t *        p_report_ref;                      /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Thermopile Level characteristic */
    uint8_t                       thermo_thermopile_low_level[5];  /**< thermopile low level */
    uint8_t                       thermo_thermopile_high_level[5]; /**< thermopile high level */
    uint8_t												thermo_thermopile_alarm_set;     /** Alarm set for thermopile **/
    uint8_t												thermo_thermopile_alarm;   			 /** Alarm for thermopile **/
    ble_srv_cccd_security_mode_t  thermopile_char_attr_md;          /**< Initial security level for Thermopile characteristics attribute */
    ble_srv_cccd_security_mode_t  thermopile_char_attr_md2;        /**< Initial security level for Thermopile characteristics attribute */
    ble_gap_conn_sec_mode_t       battery_level_report_read_perm;    /**< Initial security level for Thermopile report read attribute */
} ble_thermops_init_t;

/**@brief Thermopile Service structure. This contains various status information for the service. */
typedef struct ble_thermops_s
{
    ble_thermops_evt_handler_t    evt_handler;                    /**< Event handler to be called for handling events in the Thermopile Service. */
    ble_thermops_write_evt_handler_t write_evt_handler;
    uint16_t                      service_handle;                 /**< Handle of thermopile Service (as provided by the BLE stack). */
    uint8_t                       uuid_type;
    ble_gatts_char_handles_t      current_thermop_handles;          	/**< Handles for thermopile  characteristic. */
    ble_gatts_char_handles_t      thermop_low_level_handles;          	/**< Handles for thermopile low Level characteristic. */
    ble_gatts_char_handles_t      thermop_high_level_handles;         	/**< Handles for thermopile high Level characteristic. */
    ble_gatts_char_handles_t      thermo_thermop_alarm_set_handles;  	/**< Handles for thermopile alarm set characteristic. */
    ble_gatts_char_handles_t      thermo_thermop_alarm_handles;      	/**< Handles for thermopile alarm characteristic. */
    ble_gatts_char_handles_t      switch_mode_handles;      	/**< Handles for thermopile alarm characteristic. */
    uint16_t                      report_ref_handle;              	/**< Handle of the Report Reference descriptor. */
    uint8_t                       thermo_thermopile_low_level[5];   	/**< thermopile low level for alarm for thermopile Service. */
    uint8_t                       thermo_thermopile_high_level[5];   /**< thermopile high level for alarm for thermopile Service. */
    uint8_t						            thermo_thermopile_alarm_set;   	/** Alarm set for thermopile **/
    uint8_t						            thermo_thermopile_alarm;   			/** Alarm for thermopile **/
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification of Thermopile Level is supported. */
} ble_thermops_t;



extern bool  BROADCAST_MODE; /*broadcase mode flag, defined in main.c */



/**@brief Function for initializing the Thermopile Service.
*
* @param[out]  p_bas       Thermopile Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_thermops_low_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_thermops_init(ble_thermops_t * p_bas, const ble_thermops_init_t * p_thermops_low_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Thermopile Service.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       ble_thermops_low_Thermopile_level_update() must be called upon reconnection if the
*       Thermopile level has changed while the service has been disconnected from a bonded
*       client.
*
* @param[in]   p_bas      Thermopile Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_thermops_on_ble_evt(ble_thermops_t * p_bas, ble_evt_t * p_ble_evt);

/**@brief Function for updating the Thermopile level.
*
* @details The application calls this function after having performed a Thermopile measurement. If
*          notification has been enabled, the Thermopile level characteristic is sent to the client.
*
* @note For the requirements in the BAS specification to be fulfilled,
*       this function must be called upon reconnection if the Thermopile level has changed
*       while the service has been disconnected from a bonded client.
*
* @param[in]   p_bas          Thermopile Service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_thermops_level_alarm_check(ble_thermops_t *);


//uint32_t read_thermopile(void);									 /** Function for reading thermopile from sensor **/
void read_thermopile_connectable(uint8_t *, float *);
static uint8_t  do_battery_measurement(void);						 /*funtion for taking the battery measurement*/
//float    convert_thermopile_to_float(uint16_t );  /*function for converting 12-bit thermopile to float*/
float stof(char*);
#endif 

/** @} */
