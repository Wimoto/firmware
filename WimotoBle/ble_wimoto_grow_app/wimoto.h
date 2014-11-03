/** @file
 * Author : Shafy
 * Date   : 10/30/2013
 *
 */

#ifndef WIMOTO_H__
#define WIMOTO_H__

#include <stdint.h>
#include <stdio.h>


////////////////////////////////////////////  CLIMATE PROFILE CUSTOM UUID  ////////////////////////////////////////////
/* Climate Profile Base UUID */
#define CLIMATE_PROFILE_BASE_UUID { 0xFD, 0x94, 0x0A, 0xC0, 0x19, 0x54, 0x3B, 0x9F, 0xD0, 0x4E, 0x48 , 0xEC, 0x07, 0x56, 0x03, 0xE0}
/*custom UUID definitions for temperature alarm service*/
#define CLIMATE_PROFILE_TEMPS_SERVICE_UUID                0x5608
#define CLIMATE_PROFILE_TEMPS_CURR_TEMP_UUID              0x5609
#define CLIMATE_PROFILE_TEMPS_LOW_CHAR_UUID               0x560A
#define CLIMATE_PROFILE_TEMPS_HIGH_CHAR_UUID              0x560B
#define CLIMATE_PROFILE_TEMPS_ALARM_SET_CHAR_UUID         0x560C
#define CLIMATE_PROFILE_TEMPS_ALARM_CHAR_UUID             0x560D
/*custom UUID definitions for Light alarm service */
#define CLIMATE_PROFILE_LIGHTS_SERVICE_UUID               0x560E
#define CLIMATE_PROFILE_LIGHTS_CURR_LIGHT_CHAR_UUID       0x560F
#define CLIMATE_PROFILE_LIGHTS_LIGHT_LOW_CHAR_UUID        0x5610
#define CLIMATE_PROFILE_LIGHTS_LIGHT_HIGH_CHAR_UUID       0x5611
#define CLIMATE_PROFILE_LIGHTS_LIGHT_ALARM_SET_CHAR_UUID  0x5612
#define CLIMATE_PROFILE_LIGHTS_LIGHT_ALARM_CHAR_UUID      0x5613
/*custom UUID definitions for humidity alarm service*/
#define CLIMATE_PROFILE_HUMS_SERVICE_UUID                 0x5614
#define CLIMATE_PROFILE_HUMS_CURR_HUM_CHAR_UUID           0x5615
#define CLIMATE_PROFILE_HUMS_HUM_LOW_CHAR_UUID            0x5616
#define CLIMATE_PROFILE_HUMS_HUM_HIGH_CHAR_UUID           0x5617
#define CLIMATE_PROFILE_HUMS_HUM_ALARM_SET_CHAR_UUID      0x5618
#define CLIMATE_PROFILE_HUMS_HUM_ALARM_CHAR_UUID          0x5619
/*custom UUID definitions for Data logger service*/
#define CLIMATE_PROFILE_DLOGS_SERVICE_UUID                0x561A
#define CLIMATE_PROFILE_DLOGS_DLOGS_EN_UUID               0x561B
#define CLIMATE_PROFILE_DLOGS_DATA_UUID                   0x561C
#define CLIMATE_PROFILE_DLOGS_READ_DATA_UUID              0x561D
/*custom UUID definitions for Device Management service.*/
#define CLIMATE_PROFILE_DEVICE_SERVICE_UUID               0x561E
#define CLIMATE_PROFILE_DEVICE_DFU_MODE_CHAR_UUID         0x561F
#define CLIMATE_PROFILE_DEVICE_SWITCH_MODE_CHAR_UUID      0x5620
#define CLIMATE_PROFILE_DEVICE_TIME_STAMP_CHAR_UUID       0x1805


////////////////////////////////////////////  GROW PROFILE CUSTOM UUID  ////////////////////////////////////////////
/* Grow Profile Base UUID */
#define GROW_PROFILE_BASE_UUID {0x31, 0x5E, 0x54, 0x5F, 0xAF, 0x62, 0x93, 0x92, 0xD8, 0x4D, 0xB0, 0xBF, 0x06, 0x47, 0xF4, 0xDA}
/*custom UUID definitions for temperature alarm service*/
#define GROW_PROFILE_TEMP_SERVICE_UUID                    0x4706
#define GROW_PROFILE_CURR_TEMP_UUID                       0x4707
#define GROW_PROFILE_TEMPS_LOW_CHAR_UUID                  0x4708
#define GROW_PROFILE_TEMPS_HIGH_CHAR_UUID                 0x4709
#define GROW_PROFILE_TEMPS_ALARM_SET_CHAR_UUID            0x470A
#define GROW_PROFILE_TEMPS_ALARM_CHAR_UUID                0x470B
/*custom UUID definitions for Light alarm service */
#define GROW_PROFILE_LIGHTS_SERVICE_UUID                  0x470C
#define GROW_PROFILE_LIGHTS_CURR_LIGHT_CHAR_UUID          0x470D
#define GROW_PROFILE_LIGHTS_LIGHT_LOW_CHAR_UUID           0x470E
#define GROW_PROFILE_LIGHTS_LIGHT_HIGH_CHAR_UUID          0x470F
#define GROW_PROFILE_LIGHTS_LIGHT_ALARM_SET_CHAR_UUID     0x4710
#define GROW_PROFILE_LIGHTS_LIGHT_ALARM_CHAR_UUID         0x4711
/*custom UUID definitions for Soil Moisture alarm service */
#define GROW_PROFILE_SOILS_SERVICE_UUID                   0x4712
#define GROW_PROFILE_SOILS_CURR_SOIL_CHAR_UUID            0x4713
#define GROW_PROFILE_SOILS_SOIL_LOW_CHAR_UUID             0x4714
#define GROW_PROFILE_SOILS_SOIL_HIGH_CHAR_UUID            0x4715
#define GROW_PROFILE_SOILS_SOIL_ALARM_SET_CHAR_UUID       0x4716
#define GROW_PROFILE_SOILS_SOIL_ALARM_CHAR_UUID           0x4717 
/*custom UUID definitions for Data logger service*/
#define GROW_PROFILE_DLOGS_SERVICE_UUID                   0x4718
#define GROW_PROFILE_DLOGS_DLOGS_EN_UUID                  0x4719
#define GROW_PROFILE_DLOGS_DATA_UUID                      0x471A
#define GROW_PROFILE_DLOGS_READ_DATA_UUID                 0x471B
/*custom UUID definitions for Device Management service.*/
#define GROW_PROFILE_DEVICE_MGMT_SERVICE_UUID             0x471C
#define GROW_PROFILE_DEVICE_DFU_MODE_CHAR_UUID            0x471D
#define GROW_PROFILE_DEVICE_SWITCH_MODE_CHAR_UUID         0x471E
#define GROW_PROFILE_DEVICE_TIME_STAMP_CHAR_UUID          0x1805


////////////////////////////////////////////  SENTRY PROFILE CUSTOM UUID  ////////////////////////////////////////////
/* Sentry Profile Base UUID */
#define SENTRY_PROFILE_BASE_UUID {0x12, 0xE3, 0xD2, 0xCC, 0xAA, 0xCD, 0xD8, 0x83, 0x20, 0x44, 0x33, 0xE4, 0x67, 0xDC, 0x09, 0x42}
/*custom UUID definitions for Accelerometer/Movement alarm service.*/
#define SENTRY_PROFILE_MOVEMENT_SERVICE_UUID              0xDC68
#define SENTRY_PROFILE_MOVEMENT_CURR_XYZ_POS_CHAR_UUID    0xDC69
#define SENTRY_PROFILE_MOVEMENT_ALARM_SET_CHAR_UUID       0xDC6A
#define SENTRY_PROFILE_MOVEMENT_ALARM_CLEAR_CHAR_UUID     0xDC6B
#define SENTRY_PROFILE_MOVEMENT_ALARM_CHAR_UUID           0xDC6C
/*custom UUID definitions for PIR alarm service.*/                                                          
#define SENTRY_PROFILE_PIR_SERVICE_UUID                   0xDC6D
#define SENTRY_PROFILE_PIR_CURR_STATE_CHAR_UUID           0xDC6E
#define SENTRY_PROFILE_PIR_ALARM_SET_CHAR_UUID            0xDC6F
#define SENTRY_PROFILE_PIR_ALARM_CHAR_UUID                0xDC70
/*custom UUID definitions for Data logger service*/                                                      
#define SENTRY_PROFILE_DLOGS_SERVICE_UUID                 0xDC71
#define SENTRY_PROFILE_DLOGS_DLOGS_EN_UUID                0xDC72
#define SENTRY_PROFILE_DLOGS_DATA_UUID                    0xDC73
#define SENTRY_PROFILE_DLOGS_READ_DATA_UUID               0xDC74                                                    
/*custom UUID definitions for Device Management service.*/                                                       
#define SENTRY_PROFILE_DEVICE_MGMT_SERVICE_UUID           0xDC75
#define SENTRY_PROFILE_DEVICE_DFU_MODE_CHAR_UUID          0xDC76
#define SENTRY_PROFILE_DEVICE_SWITCH_MODE_CHAR_UUID       0xDC77
#define SENTRY_PROFILE_DEVICE_TIME_STAMP_CHAR_UUID        0x1805


////////////////////////////////////////////  THERMO PROFILE CUSTOM UUID  ////////////////////////////////////////////
/* Thermo Profile Base UUID */
#define THERMO_PROFILE_BASE_UUID {0x38, 0x73, 0x49, 0xF2, 0x2C, 0xB1, 0xE9, 0x8F, 0x82, 0x4F, 0x1E, 0xB6, 0x4D, 0x8E, 0x7B, 0x49}
/*custom UUID definitions for Thermopile alarm service.*/
#define THERMO_PROFILE_SERVICE_UUID                       0x8E4E
#define THERMO_PROFILE_THERMOPS_CURR_THERMOP_UUID         0x8E4F
#define THERMO_PROFILE_THERMOPS_LOW_CHAR_UUID             0x8E50
#define THERMO_PROFILE_THERMOPS_HIGH_CHAR_UUID            0x8E51
#define THERMO_PROFILE_THERMOPS_ALARM_SET_CHAR_UUID       0x8E52
#define THERMO_PROFILE_THERMOPS_ALARM_CHAR_UUID           0x8E53
/*custom UUID definitions for Probe Temperature alarm service.*/
#define THERMO_PROFILE_PROBES_SERVICE_UUID                0x8E54
#define THERMO_PROFILE_CURR_PROBE_CHAR_UUID               0x8E55
#define THERMO_PROFILE_PROBE_LOW_CHAR_UUID                0x8E56
#define THERMO_PROFILE_PROBE_HIGH_CHAR_UUID               0x8E57
#define THERMO_PROFILE_PROBE_ALARM_SET_CHAR_UUID       	  0x8E58
#define THERMO_PROFILE_PROBE_ALARM_CHAR_UUID              0x8E59 
/*custom UUID definitions for Data logger service*/                                                               
#define THERMO_PROFILE_DLOGS_SERVICE_UUID                 0x8E5A       
#define THERMO_PROFILE_DLOGS_DLOGS_EN_UUID                0x8E5B       
#define THERMO_PROFILE_DLOGS_DATA_UUID                    0x8E5C       
#define THERMO_PROFILE_DLOGS_READ_DATA_UUID            	  0x8E5D       
/*custom UUID definitions for Device Management service.*/                                                              
#define THERMO_PROFILE_DEVICE_SERVICE_UUID                0x8E5E         
#define THERMO_PROFILE_DEVICE_DFU_MODE_CHAR_UUID          0x8E5F 
#define THERMO_PROFILE_DEVICE_SWITCH_MODE_CHAR_UUID       0x8E60 
#define THERMO_PROFILE_DEVICE_TIME_STAMP_CHAR_UUID        0x1805  


////////////////////////////////////////////  WATER PROFILE CUSTOM UUID  ////////////////////////////////////////////
/* Water Profile Base UUID */
#define WATER_PROFILE_UUID_BASE {0xDA, 0xDB, 0xC2, 0xCA, 0x48, 0x0E, 0x2E, 0xAB, 0xC2, 0x43, 0x78, 0x9D, 0xDA, 0xC7, 0xD8, 0x35}
/*custom UUID definitions for Water Presence alarm service.*/
#define WATER_PROFILE_WATERPS_SERVICE_UUID                0xC7DB
#define WATER_PROFILE_WATERPS_CURR_WATERP_UUID            0xC7DC
#define WATER_PROFILE_WATERPS_ALARM_SET_CHAR_UUID         0xC7DD
#define WATER_PROFILE_WATERPS_ALARM_CHAR_UUID             0xC7DE
/*custom UUID definitions for Water Level alarm service.*/
#define WATER_PROFILE_WATERLS_SERVICE_UUID                0xC7DF
#define WATER_PROFILE_WATERLS_CURR_WATERL_CHAR_UUID       0xC7E0
#define WATER_PROFILE_WATERLS_LOW_CHAR_UUID               0xC7E1
#define WATER_PROFILE_WATERLS_HIGH_CHAR_UUID              0xC7E2
#define WATER_PROFILE_WATERLS_ALARM_SET_CHAR_UUID         0xC7E3
#define WATER_PROFILE_WATERLS_ALARM_CHAR_UUID             0xC7E4
/*custom UUID definitions for Data logger service*/
#define WATER_PROFILE_DLOGS_SERVICE_UUID                  0xC7E5
#define WATER_PROFILE_DLOGS_DLOGS_EN_UUID                 0xC7E6
#define WATER_PROFILE_DLOGS_DATA_UUID                     0xC7E7
#define WATER_PROFILE_DLOGS_READ_DATA_UUID             	  0xC7E8
/*custom UUID definitions for Device Management service.*/
#define WATER_PROFILE_DEVICE_SERVICE_UUID                 0xC7E9
#define WATER_PROFILE_DEVICE_DFU_MODE_CHAR_UUID           0xC7EA
#define WATER_PROFILE_DEVICE_SWITCH_MODE_CHAR_UUID        0xC7EB
#define WATER_PROFILE_DEVICE_TIME_STAMP_CHAR_UUID         0x1805






#define TMP102_RESOLUTION                         0.0625      /**< Resolution of tmp102 sensor*/
#define ISL_RESOLUTION                            0.96        /**< Resolution of ISL29023 light sensor*/


#define RESET_ALARM                               0x00        /**< Alarm value when there is no alarm*/
#define SET_ALARM_LOW                             0x01        /**< Alarm value when current sensor value is lower than the low value set by user */
#define SET_ALARM_HIGH                            0x02        /**< Alarm value when current sensor value is greater than the high value set by user */
#define DEFAULT_ALARM_SET                         0x00        /**< Default value of alarm initially*/

#define SET_ALARM_TEMP_LOW                        0x01        /**< Temperature alarm value when current sensor value is lower than the low value set by user*/
#define SET_ALARM_TEMP_HIGH                       0x02        /**< Temperature alarm value when current sensor value is greater than the high value set by user*/

#define SET_ALARM_THERMOP_LOW                     0x01        /**< Thermpopile temperature alarm value when current sensor value is lower than the low value set by user*/
#define SET_ALARM_THERMOP_HIGH                    0x02        /**< Thermpopile temperature alarm value when current sensor value is greater than the high value set by user*/

#define SET_ALARM_LIGHT_LOW                       0x01        /**< Light alarm value when current sensor value is lower than the low value set by user*/
#define SET_ALARM_LIGHT_HIGH                      0x02        /**< Light alarm value when current sensor value is greater than the high value set by user*/

#define SET_ALARM_HUM_LOW                         0x01        /**< Humidity alarm value when current sensor value is lower than the low value set by user*/
#define SET_ALARM_HUM_HIGH                        0x02        /**< Humidity alarm value when current sensor value is greater than the high value set by user*/

#define SET_ALARM_WATERL_LOW                      0x01        /**< Water alarm value when current sensor value is lower than the low value set by user*/
#define SET_ALARM_WATERL_HIGH                     0x02        /**< Water alarm value when current sensor value is greater than the high value set by user*//
 
#define SET_ALARM_FOR_MOVENMENT                   0x01        /**< Alarm value set when there is a movement*/
#define SET_ALARM_NO_MOVENMENT                    0x00        /**< Alarm value set when there is no movement*/

#define SET_ALARM_PIR_DETECTION                	  0x01        /**< Alarm value set when the PIR sensor triggers a interrupt */
#define NO_PIR_DETECTION                          0x00        /**< GPIO Pin value read when PIR sensor when PIR senor is not generating an interrupt*/

#define LOWER_BYTE_MASK                           0x00FF	    /**< Mask to take only the lower byte of a 16 bit variable*/
#define TWELTH_BIT_SIGN_MASK                      0x0800      /**< Mask to check whether the 12th bit is set*/

#define SET_ALARM_NO_WATER                    	  0x01        /**< Set the alarm when water is not present*/
#define DEFAULT_WATER_PRESENCE_ON_PULLUP          0x01        /**< Default value on GPIO pin detecting water presence in pull up enabled condition */
#define WATER_NOT_PRESENT                         0x00        /**< GPIO Pin value read if the water is not present*/


/*Values to be assigned to the pins when used for ADC */
/**< = Analog input pin P0.26  = 0       */
/**< = Analog input pin P0.27  = 1       */
/**< = Analog input pin P0.01  = 2       */
/**< = Analog input pin P0.02  = 3       */
/**< = Analog input pin P0.03  = 4       */
/**< = Analog input pin P0.04  = 5       */
/**< = Analog input pin P0.05  = 6       */
/**< = Analog input pin P0.06  = 7       */

/*ADC input pin Pin for Soil moisture sensor */
#define ADC_SOIL_MOISTURE_PIN                       1         /**< Soil moisture pin is p0.01(value is configured according to ADC analog input)*/

/*ADC input pin Pin for Probe temperature sensor*/
#define PROBE_SENSOR_ENERGIZE_PIN                  GPIO_OUTCLR_PIN2_Pos  /**< Pin P0.02 is used to energize the soil moisture sensor*/
#define PROBE_ADC_INPUT_AIN2_P01                    2         /**< Use analog input 2 as ADC input. (P0.01) (value is configured according to ADC analog input)*/

/*ADC input pin Pin for Water level sensor*/
#define WATER_LEVEL_PIN                             2         /**< Pin for water level sensor is P0.01 (value is configured according to ADC analog input) */

/*Pin for Movement detection GPIOTE. */                                                       
#define NO_MOVEMENT                               0x01        /**< GPIOP in value read if there is no movement*/
#define MOVEMENT                                  0x00        /**< GPIO Pin value read if there is a movement*/

#define MOVEMENT_GPIOTE_PIN                         4         /**< Select pin P0.04 for interfacing MMA7660 accelerometer INT1 interrupt pin>*/
#define MOVEMENT_PINS_LOW_TO_HIGH_MASK            0x08000010  /**< Pin selection, so that a LOW to HIGH logic on chosen pin generates an interrupt  >*/
#define MOVEMENT_PINS_HIGH_TO_LOW_MASK            0x08000010  /**< Pin selection, so that a HIGH to LOW logic on chosen pin generates an interrupt  >*/
/*Pin for PIR detection GPIOTE. */      
#define PIR_GPIOTE_PIN                              2         /**< Select pin P0.02 for interfacing digital PIR senor>*/
#define PIR_PINS_LOW_TO_HIGH_MASK                 0x20000004  /**< Pin selection, so that a LOW to HIGH logic on chosen pin generates an interrupt  >*/
#define PIR_PINS_HIGH_TO_LOW_MASK                 0x20000004  /**< Pin selection, so that a HIGH to LOW logic on chosen pin generates an interrupt  >*/       
#define DEFAULT_PIR_STATE_ON_PULLUP               0x00        /**< Default value on GPIO pin when PIR sensor when not generating interrupt (ACTIVE HIGH SENSOR)*/             
#define PIR_DETECTION                             0x01        /**< Default value on GPIO pin when PIR sensor has generated interrupt(ACTIVE HIGH SENSOR)*/

 /*Pin for water presence GPIOTE. */
#define WATERP_GPIOTE_PIN                           0 
#define WATERP_PINS_LOW_TO_HIGH_MASK              0x80000001  /**< Pin selection, so that a LOW to HIGH logic on chosen pin generates an interrupt >*/
#define WATERP_PINS_HIGH_TO_LOW_MASK              0x80000001  /**< Pin selection, so that a HIGH to LOW logic on chosen pin generates an interrupt  >*/

#define HTU21_DEFAULT_LOW_VALUE_LOWER_BYTE        0x00        /**< Default value of lowest temperature that HTU21 sensor can measure>*/
#define HTU21_DEFAULT_LOW_VALUE_HIGHER_BYTE       0x00             
 
#define TEMP_DEFAULT_LOW_VALUE_LOWER_BYTE         0x0C        /**< Default value of temperature low value>*/
#define TEMP_DEFAULT_LOW_VALUE_HIGHER_BYTE        0x90             
#define TEMP_DEFAULT_HIGH_VALUE_LOWER_BYTE        0x07        /**< Default value of temperature low value>*/		
#define TEMP_DEFAULT_HIGH_VALUE_HIGHER_BYTE       0xFF             

#define HUM_DEFAULT_LOW_VALUE_LOWER_BYTE          0x00        /**< Default value of temperature low value>*/
#define HUM_DEFAULT_LOW_VALUE_HIGHER_BYTE         0x00             
#define HUM_DEFAULT_HIGH_VALUE_LOWER_BYTE         0xFF        /**< Default value of temperature low value>*/		
#define HUM_DEFAULT_HIGH_VALUE_HIGHER_BYTE        0xFF             

#define LIGHT_DEFAULT_LOW_VALUE_LOWER_BYTE        0x00        /**< Default value of light level low value>*/
#define LIGHT_DEFAULT_LOW_VALUE_HIGHER_BYTE       0x00             
#define LIGHT_DEFAULT_HIGH_VALUE_LOWER_BYTE       0xFF        /**< Default value of light level high value>*/
#define LIGHT_DEFAULT_HIGH_VALUE_HIGHER_BYTE      0xFF             
 
#define SOIL_MOIS_DEFAULT_LOW_VALUE               0x00        /**< Default value of soil moisture low value>*/
#define SOIL_MOIS_DEFAULT_HIGH_VALUE              0xFF        /**< Default value of soil moisture low value>*/
  
#define THERMOP_CHAR_SIZE                         0x05        /**< Thermopile temperature is 5 characters*/
#define THERMOP_DEFAULT_LOW_VALUE                 0x30        /**< Default value of thermopile tempreature low value>*/
#define THERMOP_DEFAULT_HIGH_VALUE                0x39        /**< Default value of thermopile tempreature low value>*/
#define PERIOD                                    0x2E        /**< Hex value of Period (.)*/

#define WATERL_LEVEL_DEFAULT_LOW_VALUE            0x00        /**< Default value of soil moisture low value>*/
#define WATERL_LEVEL_DEFAULT_HIGH_VALUE           0xFF        /**< Default value of soil moisture low value>*/

#define PROBE_TEMP_DEFAULT_LOW_VALUE              0x00        /**< Default value of soil moisture low value>*/
#define PROBE_TEMP_DEFAULT_HIGH_VALUE             0xFF        /**< Default value of soil moisture low value>*/
 
#define DATA_LOGGER_BUFFER_START_PAGE             0xC0        /**< first flash page of the datalogger cyclic buffer*/
#define DATA_LOGGER_BUFFER_END_PAGE               0xC3        /**< last flash page of the datalogger cyclic buffer*/
#define COMPANY_IDENTIFER                         0x1701      /**< comapany identifier*/                                                                 
  
void connectable_mode(void);                                  /**< Function to advertise peripheral services */
void twi_turn_OFF(void);                                      /**< Function to turn OFF twi for power saving */
void twi_turn_ON(void);                                       /**< Function to turn ON twi								   */
 
#endif // WIMOTO_H__

/** @} */
