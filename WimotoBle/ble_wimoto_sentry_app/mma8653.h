/**
*@File     MMA8653FC accelerometer Driver
*
*@Featurs Provides API's for      
*             1.Initializing the MMA8653FC for auto wake/sleep mode and configure for motion interrupt through INT1 pin
*             2.Reading data for X ,Y and Z registers when an motion occurs,
*                power saving            
*Date      : 26.November.2013
*Author    : Hariprasad C R
*/


/* MODULE  */
#ifndef _MMA8653_H_
#define _MMA8653_H_

/**< Include Files */
#include "twi_master.h"
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

/*------------------------------------------------------------------------------------------*/
/* MMA8653 DRIVER																  		    */
/*------------------------------------------------------------------------------------------*/
/**
*@File     MMA8653FC accelerometer Driver
*
*@Featurs Provides API's for      
*             1.Initializing the MMA8653 for auto wake/sleep mode and configure for motion interrupt through INT1 pin
*             2.Reading data for X ,Y and Z registers when an motion occurs,
*                power saving            
*/

/**< Macros       */
#define MMA8653_ADDRESS                             0x3A        /**< Slave address of MMA8653FC */

#define MMA8653_STATUS                              0x00        /**< Base address of Data Status register */
#define MMA8653_OUT_X_MSB                           0x01        /**< Base address of X_MSB register */
#define MMA8653_OUT_X_LSB                           0x02        /**< Base address of X_LSB register */
#define MMA8653_OUT_Y_MSB                           0x03        /**< Base address of Y_MSB register */
#define MMA8653_OUT_Y_LSB                           0x04        /**< Base address of Y_LSB register */
#define MMA8653_OUT_Z_MSB                           0x05        /**< Base address of Z_MSB register */
#define MMA8653_OUT_Z_LSB                           0x06        /**< Base address of Z_LSB register */
#define MMA8653_SYSMOD                              0x0B        /**< Base address of System Mode register */
#define MMA8653_INT_SOURCE                          0x0C        /**< Base address of System Interrupt Status register */
#define MMA8653_WHO_AM_I                            0x0D        /**< Base address of Device ID register */
#define MMA8653_XYZ_DATA_CFG                        0x0E        /**< Base address of Data configuration registers */
#define MMA8653_PL_STATUS                           0x10        /**< Base address of Portrait/Landscape Status register */
#define MMA8653_PL_CFG                              0x11        /**< Base address of Portrait/Landscape Configuration register */
#define MMA8653_PL_COUNT                            0x12        /**< Base address of Portrait/Landscape Debounce register */
#define MMA8653_PL_BF_ZCOMP                         0x13        /**< Base address of Back/Front and Z Compensation register */
#define MMA8653_PL_THS_REG                          0x14        /**< Base address of Portrait/Landscape Threshold and Hysteresis register */
#define MMA8653_FF_MT_CFG                           0x15        /**< Base address of Freefall/Motion Configuration register */
#define MMA8653_FF_MT_SRC                           0x16        /**< Base address of Freefall/Motion Source register */
#define MMA8653_FF_MT_THS                           0x17        /**< Base address of Freefall and Motion Threshold register */
#define MMA8653_FF_MT_COUNT                         0x18        /**< Base address of Debounce register*/
#define MMA8653_ASLP_COUNT                          0x29        /**< Base address of Auto-WAKE/SLEEP Detection register */
#define MMA8653_CTRL_REG1                           0x2A        /**< Base address of System Control 1 register */
#define MMA8653_CTRL_REG2                           0x2B        /**< Base address of System Control 2 register */
#define MMA8653_CTRL_REG3                           0x2C        /**< Base address of Interrupt Control register */
#define MMA8653_CTRL_REG4                           0x2D        /**< Base address of Interrupt Enable register */
#define MMA8653_CTRL_REG5                           0x2E        /**< Base address of Interrupt Configuration register */
#define MMA8653_OFF_X                               0x2F        /**< Base address of Offset Correction X register */
#define MMA8653_OFF_Y                               0x30        /**< Base address of Offset Correction Y register */
#define MMA8653_OFF_Z                               0x31        /**< Base address of Offset Correction Z register */

#define MMA8653_CHECKING_FASTREAD                   0x02        /**< Check whether fast read is enabled or not */
#define MMA8653_CHECKING_STATUS                     0x08        /**< Check the status register for a new data availability */

#define MMA8653_ENABLE_STANDBY_MODE                 0x00        /**< Enable standby mode */
#define MMA8653_ENABLE_ACTIVE_MODE                  0x01        /**< Enable active mode */
#define MMA8653_FAST_READ_ENABLE                    0x02        /**< Enable fast read mode */
#define MMA8653_AUTO_SLEEP_ENABLE                   0x04        /**< Enable auto sleep function */
 
#define MMA8653_FSR_2G                              0x00        /**< Use FSR of ±2g */
#define MMA8653_FSR_4G                              0x01        /**< Use FSR of ±4g */
#define MMA8653_FSR_8G                              0x02        /**< Use FSR of ±8g */

#define MMA8653_DR_800HZ                            0x00        /**< Set Output Data Rate as 800Hz in normal conversion/in wake up */
#define MMA8653_DR_400HZ                            0x08        /**< Set Output Data Rate as 400Hz in normal conversion/in wake up */
#define MMA8653_DR_200HZ                            0x10        /**< Set Output Data Rate as 200Hz in normal conversion/in wake up */
#define MMA8653_DR_100HZ                            0x18        /**< Set Output Data Rate as 100Hz in normal conversion/in wake up */
#define MMA8653_DR_50HZ                             0x20        /**< Set Output Data Rate as 50Hz in normal conversion/in wake up */
#define MMA8653_DR_12HZ                             0x28        /**< Set Output Data Rate as 12.5Hz in normal conversion/in wake up */
#define MMA8653_DR_6HZ                              0x30        /**< Set Output Data Rate as 6.25Hz in normal conversion/in wake up*/
#define MMA8653_DR_1HZ                              0x38        /**< Set Output Data Rate as 1.56Hz in normal conversion/in wake up */
 
#define MMA8653_ASLP_RATE_50HZ                      0x00        /**< Set Output Data Rate as 50Hz in sleep mode */
#define MMA8653_ASLP_RATE_12HZ                      0x40        /**< Set Output Data Rate as 12.5Hz in sleep mode */
#define MMA8653_ASLP_RATE_6HZ                       0x80        /**< Set Output Data Rate as 6.25Hz in sleep mode*/
#define MMA8653_ASLP_RATE_1HZ                       0xC0        /**< Set Output Data Rate as 1.56Hz in sleep mode */
                                                                
#define MMA8653_MODS_MODE_NORMAL                    0x00        /**< Set Normal mode power scheme in WAKE Mode */
#define MMA8653_MODS_MODE_LNLP                      0x01        /**< Set Low Noise Low Power mode power scheme in WAKE Mode */
#define MMA8653_MODS_MODE_HIGH_RESOLUTION           0x02        /**< Set High Resolution mode power scheme in WAKE Mode*/
#define MMA8653_MODS_MODE_LOW_POWER                 0x03        /**< Set Low Power mode power scheme in WAKE Mode */
#define MMA8653_SMODS_MODE_NORMAL                   0x00        /**< Set Normal mode power scheme in SLEEP Mode */
#define MMA8653_SMODS_MODE_LNLP                     0x08        /**< Set Low Noise Low Power mode power scheme in SLEEP Mode */
#define MMA8653_SMODS_MODE_HIGH_RESOLUTION          0x10        /**< Set High Resolution mode power scheme in SLEEP Mode */
#define MMA8653_SMODS_MODE_LOW_POWER                0x18        /**< Set Low Power mode power scheme in SLEEP Mode */

#define MMA8653_CR3_ORIENTATION_ENABLE              0x20        /**< Orientation function interrupt can wake up system */
#define MMA8653_CR3_MOTION_ENABLE                   0x08        /**< Freefall/Motion function interrupt can wake up system */
#define MMA8653_INTERRUPT_PIN_CONFIG                0x01        /**< Selects polarity of the interrupt as ACTIVE low and Open-Drain 
                                                                     selection on interrupt pad */
#define MMA8653_AUTO_SLEEP_WAKEUP_INTERRUPT_ENABLE  0x80        /**< Auto-SLEEP/WAKE Interrupt Enable */
#define MMA8653_ORIENTATION_INTERRUPT_ENABLE        0x10        /**< Orientation (Landscape/Portrait) Interrupt Enable */
#define MMA8653_MOTION_INTERRUPT_ENABLE             0x04        /**< Freefall/Motion Interrupt Enable */
#define MMA8653_DATA_READY_INTERRUPT_ENABLE         0x01        /**< Data Ready Interrupt Enable */

#define MMA8653_AUTO_SLEEP_WAKE_INT1                0x80        /**< Auto-SLEEP/WAKE Interrupt is routed to INT1 pin */
#define MMA8653_ORIENTATION_INT1                    0x10        /**< Orientation Interrupt is routed to INT1 pin*/
#define MMA8653_MOTION_FREEFALL_INT1                0x04        /**< Freefall/motion Interrupt is routed to INT1 pin*/
#define MMA8653_DATA_READY_INT1                     0x01        /**< Data Ready Interrupt is routed to INT1 pin*/
#define MMA8653_USE_INT2                            0x00        /**< Interrupt is routed to INT2 pin (default) */

#define MMA8653_30S_TIMEOUT                         0x2F        /**< Time period of inactivity to switch between Wake and Sleep is 30Seconds*/
#define MMA8653_3S_TIMEOUT													0x05				/**< Time period of inactivity to switch between Wake and Sleep is 3.2seconds (5 * 640ms)*/

#define MMA8653_FREEFALL_DETECTION                  0x00        /**< Motion detect flag selection */
#define MMA8653_MOTION_DETECTION                    0x40        /**< Freefall detect flag selection */
#define MMA8653_ZEFE                                0x20        /**< Z axis is taken into consideration for motion detection */
#define MMA8653_YEFE                                0x10        /**< Y axis is taken into consideration for motion detection  */
#define MMA8653_XEFE                                0x08        /**< X axis is taken into consideration for motion detection  */	
                                                                
#define MMA8653_MOTION_THRESHOLD_5G                 0x08        /**< Threshold for the motion detection, 8 * 0.063 = .504g */
#define MMA8653_MOTION_THRESHOLD_1									0x01				/**< Threshold for the motion detection, 1 * 0.063 = 0.063g */
#define MMA8653_DBCNTM_SET                          0x80        /**< Debounce counter is cleared to 0 whenever the inertial event of interest 
is no longer true*/                                             
#define MMA8653_DEBOUNCE_COUNT_5                    0x05        /**< Minimum number of debounce sample counts that continuously match detection*/
#define MMA8653_DEBOUNCE_COUNT_1										0x01				/**< Number of sample counts before event is triggered */
 
 
/**< Functions   */
/*Public Functions*/
bool MMA8653_ReadXYZdata(uint32_t * ptr_to_Reg_val);                      /**< Read the X,Y,Z registers for coordinates*/
bool MMMA8653_Init(void);                                                 /**< Initialize MMA8653 accelerometer*/

/*Private Functions*/                                                      
bool MMA8653_read_register (uint8_t base_address,uint8_t * ptr_to_strng); /**< Read data from any of the registers of MMA8653 */
bool MMA8653_write_to_reg(uint8_t base_Address,uint8_t data);             /**< Write to any of the registers of MMA8653 */
bool MMA8653_StandbyMode_Enable (void);                                   /**< Enable stand by mode for configuring MMA8653 */
bool MMA8653_ActiveMode_Enable (void);                                    /**< Enable active mode */
bool MMA8653_FullScaleRange_Set (uint8_t range_val);                      /**< Set Full Scale Reading 2/4/8g */
bool MMA8653_FastRead_Enable (void);                                      /**< 8 Bit data format is chosen */
bool MMA8653_DATA_RATE_Set (uint8_t DR_val);                              /**< Set the ODR in wake mode */
bool MMA8653_ASLP_RATE_Set (uint8_t ASLP_val);                            /**< Set the ODR in sleep mode */
bool MMA8653_OversamplingMode_Enable (uint8_t OS_val);                    /**< Configure for oversampling */
bool MMA8653_AutoSleep_Enable (void);                                     /**< Enable auto sleep mode */
bool MMA8653_InterruptControlReg_config	 (uint8_t wake_type);             /**< Configure for the wake up interrupt */
bool MMA8653_InterruptEnableReg_config (uint8_t write_to_Reg_Val);        /**< Enable the selected interrupt */	
bool MMA8653_InterruptConfigurationReg_config (uint8_t write_to_Reg_Val); /**< Map the interrupts to the any of two pins INT1/INT2*/
bool MMA8653_ASLPCounter_val(uint8_t write_to_Reg_Val);                   /**< Time-out counter for inactivity */
bool MMA8653_FreefallMotionConfigReg_Set (uint8_t config_val);            /**< Configure free-fall motion configuration register*/
bool MMA8653_FreefallMotionThresholdReg_Set (uint8_t Reg_val);            /**< Function to set free-fall motion threshold */
bool MMA8653_DebounceRegister_Set (uint8_t Count_val);                    /**< Function to set the number of denounce counts for the event trigger*/


/* MODULE END */
#endif