/**
 *@FILE     HEADER FILE FOR TMP102, TMP006, ISL29023, MMA7660FC, HTU21D SENSORS
 *
 *@FEATURES This file contains the header files for all the Sensors (TMP102, TMP006, ISL29023, MMA7660FC, HTU21D)
 *
 *Date      : 10/25/2013
 *Author    : Hariprasad C R - GadgEon Systems
 */
 
 
/********************************************************************************************/ 
/* MODULE START*/
/********************************************************************************************/
#ifndef _WIMOTO_SENSORS_
#define _WIMOTO_SENSORS_
 
 
/********************************************************************************************/ 
/* Include Files*/
/********************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "twi_master.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include <math.h>

/*------------------------------------------------------------------------------------------*/
/* TMP102 DRIVER																  		    */
/*------------------------------------------------------------------------------------------*/
/**
*@Brief   TMP102 DRIVER
*@Featurs Provides functions to     
*            1. Read contents of  data registers  of TMP102 temperature sensor.
*            2. Write data to the data registers of TMP102 temperature sensor.
*            3. Configuring TMP102 in SHUT-DOWN MODE
*            4. Reading temperature using ONE-SHOT MODE
*/


/**< Macros       */
#define TMP102_ADDRESS                        0x90   /**< Slave address of TMP102 (ADD0 pin should be connected to ground)                           */

#define TMP102_TEMP_REG                       0x00   /**< Base address of TMP102 TEMPERATURE   register                                              */
#define TMP102_CONFIG_REG                     0x01   /**< Base address of TMP102 CONFIGURATION register                                              */
#define TMP102_LOW_REG                        0x02   /**< Base address of TMP102  T-LOW        register                                              */
#define TMP102_HIGH_REG                       0x03   /**< Base address of TMP102  T-HIGH       register                                              */

#define TMP102_CONFIG_REG_DEFAULT_MSB         0x60   /**< Default value of Most Significant Byte of Configuration register                           */
#define TMP102_CONFIG_REG_DEFAULT_LSB         0xA0   /**< Default value of Least Significant Byte of Configuration register                          */
#define TMP102_ENABLE_SD_BIT_IN_MSB           0x01   /**< Enable SD (Shut-Down) bit (used only with Most Significant Byte of Configuration Register) */ 																						                                          
#define TMP102_ENABLE_OS_BIT_IN_MSB           0x80   /**< Enable OS (One-Shot) bit (used only with Most Significant Byte of Configuration Register)  */
																								                                            

/**< Functions   */
/*Public Functions*/
bool      config_tmp102_shutdown_mode  (void);                     /**< Enable shut-down mode               */  
uint16_t  get_tmp102_oneshot_temp      (void);                     /**< Get temperature using one-shot mode */          

/*Private Functions*/
int16_t   read_register_content        (uint8_t);                  /**< Read contents of a data register    */
bool      write_to_register            (uint8_t,uint8_t,uint8_t);  /**< Write data to a data register       */


/*------------------------------------------------------------------------------------------*/
/* TMP006 DRIVER																  		    */
/*------------------------------------------------------------------------------------------*/
/**
*@Brief   TMP006 Temperature Sensor Driver
 *@Featurs Provides functions for
 *             1.Configure TMP006 in power down mode
 *             2.Configure TMP006 in continuous conversion mode         
 *             3.Get the content of V-object and T-ambient register one time
*/

/**< Macros       */
#define TMP006_ADDRESS                 0x80     /**< Slave address of TMP006 (ADR1 & ADR0 connectd to ground) */
                                       
#define TMP006_V_OBJ_REG               0x00     /**< Base address of V-OBJECT register */
#define TMP006_T_AMBIENT_REG           0x01     /**< Base address of T-AMBIENT register */
#define TMP006_CONFIG_REG              0x02     /**< Base address of Configuration register */
#define TMP006_MANUFACT_ID_REG         0xFE     /**< Base address of Manufacturer ID register */
#define TMP006_DEVICE_ID_REG           0xFF     /**< Base address of Device ID register */

#define TMP006_ENABLE_POWER_DOWN       0x04     /**< Enable power down mode in TMP006 (no conversion) */
#define TMP006_ENABLE_CONVERSION       0x74     /**< Enable continious conversion mode in TMP006  */
#define TMP006_CONFIG_REG_LSB          0x00     /**< Default value of Least Significant Byte(LSB) in Configuration register */

// Constants for calculating object temperature
#define TMP006_B0                      -0.0000294
#define TMP006_B1                      -0.00000057
#define TMP006_B2                       0.00000000463
#define TMP006_C2                      13.4
#define TMP006_TREF                   298.15
#define TMP006_A2                      -0.00001678
#define TMP006_A1                       0.00175
#define TMP006_S0                       6.4       /**< 10^-14  */

/**< Functions      */
/*Public Functions  */
bool      TMP006_enable_powerdown_mode(void);     /**< Enable power down mode */
uint32_t  TMP006_get_onetime_data(void);          /**< Enable continuous conversion mode then read contents of V-object & T-ambient  
                                                       registers one time and reconfigure TMP006 in power down mode */																									 
float     TMP006_readDieTempC(void);              /**< Calculate sensor temperature based on raw reading */
float     TMP006_ObjTempC_conversion(void);       /**< Calculate object temperature based on raw sensor temp and thermopile voltage */
void      do_thermopile_measurement(uint8_t*, float*);

/*Private Functions */
bool      TMP006_enable_continuous_conversion(void);                          /**< Enable continuous conversion mode */
bool      TMP006_write_to_reg(uint8_t base_Address,uint8_t MSB,uint8_t LSB);  /**< Write data to the register of TMP006*/
uint16_t  TMP006_read_register(uint8_t base_address);                         /**< Read the data from the registers of TMP006 */
float     stof(char*);                                                        /**< Convert string to float */


/*Private Functions */
bool      TMP006_enable_continuous_conversion(void);        /**< Enable continuous conversion mode */
bool      TMP006_write_to_reg(uint8_t base_Address,uint8_t MSB,uint8_t LSB);  /**< Write data to the register of TMP006*/
uint16_t  TMP006_read_register(uint8_t base_address);       /**< Read the data from the registers of TMP006 */



/*------------------------------------------------------------------------------------------*/
/* ISL29023 DRIVER																  		    */
/*------------------------------------------------------------------------------------------*/
/**
*@Brief   ISL29023 Light Sensor Driver
*@Featurs Provides functions for
*             1.Configure the Full Scale Range value of LUX in ISL29023and enable power down mode                
*             2.One time Ambient Light Sensing mode for power saving 
*/

/**< Macros       */
#define ISL29023_ADDRESS                        0x88  /**< Slave address ISL29023 */

#define ISL29023_COMMAND_REG_1                  0x00  /**< Base address of Command Register 1 */
#define ISL29023_COMMAND_REG_2                  0x01  /**< Base address of Command Register 2 */
#define ISL29023_DATA_REG_LSB                   0x02  /**< Base address of Data Register LSB (Least Significant Byte) */
#define ISL29023_DATA_REG_MSB                   0x03  /**< Base address of Data Register MSB (Most Significant Byte) */
#define ISL29023_INT_LT_LSB                     0x04  /**< Base address of Interrupt Register low threshold LSB */
#define ISL29023_INT_LT_MSB                     0x05  /**< Base address of Interrupt Register high threshold MSB */
#define ISL29023_INT_HT_LSB                     0x06  /**< Base address of Interrupt Register low threshold LSB */
#define ISL29023_INT_HT_MSB                     0x07  /**< Base address of Interrupt Register high threshold MSB */

#define ISL29023_ENABLE_POWER_DOWN              0x00  /**< Configure power Down mode */
#define ISL29023_ENABLE_ALS_ONCE                0x20  /**< Enable Ambient Light Sensing Once mode */
#define ISL29023_USE_64K_LUX_FSR                0x03  /**< Configure FSR of LUX as 64000 */
#define ISL29023_USE_16K_LUX_FSR                0x02  /**< Configure FSR of LUX as 16000 */
#define ISL29023_USE_4K_LUX_FSR                 0x01  /**< Configure FSR of LUX as 4000 */
#define ISL29023_USE_1K_LUX_FSR                 0x00  /**< Configure FSR of LUX as 1000 */
 
/**< Functions   */
/*Public Functions*/
bool          ISL29023_config_FSR_and_powerdown(void); /**< Configure the FSR reading of ISL29023 and enable power down mode*/
uint16_t      ISL29023_get_one_time_ALS(void);         /**< Get the content of data registers using ALS once mode*/

/*Private Functions */
uint8_t       ISL29023_read_register(uint8_t);         /**< Read data of register of ISL29023 */
bool          ISL29023_write_to_reg(uint8_t,uint8_t);  /**< Write data to the register of ISL29023*/

/*------------------------------------------------------------------------------------------*/
/* MMA7660FC DRIVER																  		    */
/*------------------------------------------------------------------------------------------*/
/**
 *@Brief   MMA7660FC Accelerometer Driver
 *@Featurs Provides API's for      
 *             1.Initializing the MMA7660FC by disabling tap detection and using 1 sample per second conversion 
 *                and then configure MMA7660FC for shut-down mode
 *             2.Reading data for X ,Y and Z registers one time and configure shut-down mode in MMA7660FC  for
 *                power saving            
*/

/**< Macros       */
#define MMA7660FC_ADDRESS                       0x98   /**< Slave address of MMA7660FC */

#define MMA7660_XOUT_REG                        0x00   /**< Base address of Xout register */
#define MMA7660_YOUT_REG                        0x01   /**< Base address of Yout register */
#define MMA7660_ZOUT_REG                        0x02   /**< Base address of Zout register */
#define MMA7660_TILT_REG                        0x03   /**< Base address of Tilt Status register */
#define MMA7660_SRST_REG                        0x04   /**< Base address of Sampling Rate Status register */
#define MMA7660_SPCNT_REG                       0x05   /**< Base address of Sleep Count register */
#define MMA7660_INTSU_REG                       0x06   /**< Base address of Interrupt Setup register */
#define MMA7660_MODE_REG                        0x07   /**< Base address of Mode Register register */
#define MMA7660_SR_REG                          0x08   /**< Base address of Sample Rate register */
#define MMA7660_PDET_REG                        0x09   /**< Base address of Tap/Pulse Detection register */

#define MMA7660_DISABLE_TAP_DETECTION           0xE0   /**< Disable TAP dectection in MMA7660FC*/
#define MMA7660_ENABLE_ACTIVE_MODE              0x01   /**< Enable active mode in MMA7660FC(continious conversion)  */
#define MMA7660_ENABLE_STANDBY_MODE             0xFE   /**< Enable standby mode in MMA7660FC (no conversion) */
#define MMA7660_USE_1_SAMPLE_PER_SECOND         0x07   /**< Changes sampling rate to 1 sample per second */
#define MMA7660_ALERT_BIT_SET_CHECK             0x40   /**< Verify weather alert bit is set in any of Xout,Yout or Zout register */

#define MMA7660_SLEEP_COUNT_5S                  0x14   /**< SamplinG rate of AMSR is 4samples/s so 5*4=20 (0x14)*/
#define MMA7660_SHAKE_ORIENT_INTERRUPT          0xE3   /**< Use Shake interupt on 3 axis and Front/Back & Up/Down/Right/Left interrupt*/
#define MMA7660_AWE_ASE_SET                     0x18   /**< Set the Auto-wake and Auto-sleep modes*/
#define MMA7660_AMSR_4SAMPLES                   0x05   /**< AMSR is set to 4 samples per second*/
#define MMA7660_AWSR_1SAMPLES                   0x18   /**< AWSR is set to 1 samples per second*/

/**< Functions      */
/*Public Functions*/
bool        MMA7660_config_standby_and_initialize(void);              /**< Configure MMA7660FC in standby mode and enable Auto-sleep & Auto-wake mode   */   
bool        MMA7660_read_xyz_reg_one_time(uint32_t * ptr_to_Reg_val); /**< Read contents of X-out,Y-out and Z-out registers 
                                                                          , after reading activates standby mode            */ 
void        MMA7660_ClearInterrupts(void);                            /**< Reading the TILT register clears the INTSU interrupt register*/																									
/*Private Functions*/
bool        MMA7660_enable_active_mode(void);                         /**< Enable active mode for continuous  conversion     */
bool        MMA7660_enable_standby_mode(void);                        /**< Enable standby mode (power down)                  */                 
uint8_t     MMA7660_read_register(uint8_t base_address);              /**< Read the data from the registers of MMA7660FC     */
bool        MMA7660_write_to_reg(uint8_t base_Address,uint8_t data);  /**< Write data to the registers of MMA7660FC          */


/*------------------------------------------------------------------------------------------*/
/* HTU21D DRIVER																  		    */
/*------------------------------------------------------------------------------------------*/
/**
 *@Brief    HTU21D Humidity and Temperature Sensor Driver
 *@Features Provides functions for
 *             1.Soft reset of HTU21D
 *             2.Measure Temperature (HOLD MASTER / NO HOLD MASTER mode)
 *             3.Measure Humidity (HOLD MASTER / NO HOLD MASTER mode)
 *             4.Read User Register
 *             5.Write data to User Register
*/

/**< Macros       */
#define HTU21D_ADDRESS                          0x80   /**< Slave address of HTU21D */
#define HTU21D_CRC_CHECK_ENABLE                 0x00   /**< 0x00 - Disabled 0x01 - Enabled */
#define USE_HOLD_MASTER_MODE_ENABLE             0x01   /**< use Hold master mode while reading, if set to 0x00 use No hold master mode*/
#define FEATURE_ENABLED                         0x01   /**< Default value for enabled feature*/

/* sensor command  */
typedef enum
{
	TRIG_T_MEASUREMENT_HM    = 0xE3,                    /**< command trig. temp meas. hold master        */ 
	TRIG_RH_MEASUREMENT_HM   = 0xE5,                    /**< command trig. humidity meas. hold master    */
	TRIG_T_MEASUREMENT_POLL  = 0xF3,                    /**< command trig. temp meas. no hold master     */ 
	TRIG_RH_MEASUREMENT_POLL = 0xF5,                    /**< command trig. humidity meas. no hold master */
	USER_REG_W               = 0xE6,                    /**< command writing user register               */
	USER_REG_R               = 0xE7,                    /**< command reading user register               */
	SOFT_RESET               = 0xFE                     /**< command soft reset                       */
}etHTU21Command;

typedef enum
{ 
	HTU21_RES_12_14BIT       = 0x02, /**< RH=12bit, T=14bit                     */ 
	HTU21_RES_8_12BIT        = 0x03, /**< RH= 8bit, T=12bit                     */
	HTU21_RES_10_13BIT       = 0x82, /**< RH=10bit, T=13bit                     */ 
	HTU21_RES_11_11BIT       = 0x83, /**< RH=11bit, T=11bit                     */ 
	HTU21_RES_MASK           = 0x81  /**< Mask for res. bits (7,0) in user reg. */
}etHTU21Resolution;

typedef enum
{
	HTU21_EOB_ON             = 0x41, /**< end of battery                        */
	HTU21_EOB_MASK           = 0x40, /**< Mask for EOB bit(6) in user reg.      */
} etHTU21Eob;

typedef enum
{
	HTU21_HEATER_ON          = 0x06, /**< heater on                            */
	HTU21_HEATER_OFF         = 0x00, /**< heater off                           */
	HTU21_HEATER_MASK        = 0x04, /**< Mask for Heater bit(2) in user reg.  */
}etHTU21Heater;

/* measurement signal selection */
typedef enum
{
	HUMIDITY,
	TEMP
}etHTU21MeasureType;


/**< Functions   */
/*Public Functions*/
bool eDRV_HTU21_Reset(void);                                                  /**< Function for Soft Reset */
uint16_t eDRV_HTU21_MeasureTemperature(void);                                 /**< Function to measure Temperature(16 bit value, status bit cleared)) using either Hold master / Polling mode default : Hold master mode */ 
uint16_t eDRV_HTU21_MeasureHumidity(void);                                    /**< Function to measure Humidity (16 bit value, status bit cleared) using  either Hold master / Polling mode, default : Hold master mode*/                                                                                
bool HTU21D_WriteToUserRegister(uint8_t data);                                /**< Function to write data to the User register */
uint8_t  HTU21D_ReadUserRegister(void);                                       /**< Function to read data from the User register */

/*Private Functions */
bool HTU21D_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);   /**< CRC error checking*/
uint16_t HTU21D_MeasureHM(etHTU21MeasureType eHTU21MeasureType);              /**< Function to read the temperature/humidity   using HOLD MASTER mode */                                                                                  
uint32_t HTU21D_ReadMeasurementValue(uint8_t Command_for_activity);           /**< Function to assist HOLD MASTER mode for data retrieval */
uint16_t HTU21D_MeasurePOLL(etHTU21MeasureType eHTU21MeasureType);            /**< Function to read the temperature/humidity                                                                                      using NO HOLD MASTER mode(POLLING) */
uint16_t HTU21D_PollMasterTransfer(uint8_t MeasurementType);                  /**< Function to assist NO HOLD MASTER (POLLING)mode for data retrieval */
float f32CalcTemperatureC(uint16_t);                                          /**< Calculates Temperature in Degree Celsius*/
float f32CalcRH(uint16_t);                                                    /**< Calculates relative humidity value */                             


/*------------------------------------------------------------------------------------------*/
/* ADC DRIVER																  		    */
/*------------------------------------------------------------------------------------------*/
/**< Macros       */
#define STOP_ADC                                                  0x01   /**< Defines for controlling ADC*/
#define START_ADC                                                 0x01
#define STOP_RUNNING_CONVERTION                                   0x00 

/**< Functions   */
void     adc_init(void);                                                 /**< Initialize ADC */
uint8_t do_soil_moisture_measurement(void);                              /**< Read soil moisture value from ADC interfaced to soil moisture sensor*/
uint8_t do_probe_temperature_measurement(void);                          /**< Read the probe temperature using ADC*/
uint8_t do_waterl_adc_measurement(void);                                 /**< Read water level using ADC */         

/*------------------------------------------------------------------------------------------*/
/* PWM & TIMER DRIVER														  		    */
/*------------------------------------------------------------------------------------------*/

/**< Macros       */
#define PWM_OUTPUT_PIN_NUMBER    1    /**< Pin number for PWM output.*/
#define TICKS (4UL)                   /**< Ticks */
#define TIMER_PRESCALERS 1U           /**< Prescaler setting for timer, as 1. */

/**< Functions   */
void    timer2_init(void);                                               /**< Initialize timer 2 module*/
void    gpiote1_init(void);                                              /**< Initialize GPIOTE*/
void    ppi_init(void);                                                  /**< Initialize PPI*/
int     one_mhz_start(void);                                             /**< Start 1Mhz wave*/


/********************************************************************************************/ 
/* MODULE END*/
/********************************************************************************************/
#endif

