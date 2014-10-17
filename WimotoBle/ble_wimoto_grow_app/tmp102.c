/** @file
* This file contains the source code for Tmp102 temperature sensor driver.
* 
* Date   : 10/19/2013
* Author : Hariprasad CR
*/


#include "wimoto_sensors.h"



/********************************************************************************************/
/* PRIVATE FUNCTIONS																		*/
/********************************************************************************************/

/**
*@brief    This function reads the 16 bit data from the register of TMP102 
*          temperature sensor.
*@features 1. The argument of this function is
*              (1). Base Address  of the register , from which data has to 
*                   be read.
*          2. The function returns the content of the register read. 
**/
int16_t read_register_content(uint8_t base_Address)
{
    uint8_t temporary_variable2 = 0;                    
    int16_t temporary_variable1 = 0;
    /* Data buffer to receive the data from temperature register                        */
    uint8_t data_buffer[2];    

    /* Write the base address of temperature register to pointer register               */
    if (twi_master_transfer(TMP102_ADDRESS ,(uint8_t*)&base_Address,1,TWI_ISSUE_STOP))
    {
        /* Receive the 16 bit data from the temperature register and store in an array  */				
        if (twi_master_transfer(TMP102_ADDRESS |TWI_READ_BIT ,data_buffer,2,TWI_ISSUE_STOP))
        {
            /* The two 8 bit array is converted into a 16 bit variable                  */						
            temporary_variable1 =    0;
            temporary_variable1 = (((data_buffer[0]) << 8) & 0xFF00); 
            temporary_variable2 =    data_buffer[1];
            temporary_variable1 =   (temporary_variable1 | temporary_variable2); 	

        }

    }

    return temporary_variable1;
}


/**
*@brief    This function writes the 16 bit data to the register of TMP102  temperature
*          sensor. 
*@features 1. The arguments of this function are 
*              (1). Base Address of the  register , to which data is to be  written.
*              (2). Most Significant Byte of the 16 bit data.
*              (3). Least Significant Byte of the 16 bit data.
*          2. The return value of the function is a boolean condition which  indicates
*             the attempt was successful(true) or not(false).
**/
bool write_to_register(uint8_t base_Address,uint8_t MSB,uint8_t LSB)
{

    uint8_t data_buffer[3];
    data_buffer[0] = base_Address; /* Base address of the register into which data is written  */
    data_buffer[1] = MSB;          /* Most Significant Byte                                    */
    data_buffer[2] = LSB;          /* Least Significant Byte                                   */

    /* Perform write operation and if write was successful return true otherwise false         */
    if (twi_master_transfer(TMP102_ADDRESS ,(uint8_t*)data_buffer,3,TWI_ISSUE_STOP))
    {
        return (true);
    }
    else
    {	
        return (false);
    }

}

/********************************************************************************************/
/* PUBLIC FUNCTIONS																			*/
/********************************************************************************************/

/**
*@brief    This function enables shut-down mode in TMP102, so that temperature can
*          be measured on-demand basis using one-shot mode. 
*@features 1. Performs a write operation to the configuration register setting the 
*             SD (SHUT-DOWN) bit as '1' (setting TMP102 in SHUT-DOWN MODE).
*          2. Reads the configuration register to verify that SD bit is set or not.
*          3. If the operation was successful , function returns a TRUE value
*             otherwise a FALSE value.   
*/
bool config_tmp102_shutdown_mode (void)
{

    uint8_t data_buffer[3];
    int16_t tmp102_config_reg_val;
    bool function_state =  false;

    data_buffer[0]      =  TMP102_CONFIG_REG;     /* Select configuration register           */

    /* Set SD bit as '1' in the Most Significant Byte of configuration register              */ 
    data_buffer[1]      = (TMP102_CONFIG_REG_DEFAULT_MSB | TMP102_ENABLE_SD_BIT_IN_MSB); 

    data_buffer[2]      =  TMP102_CONFIG_REG_DEFAULT_LSB;  /* Default Least Significant Byte */



    /* Write operation to the configuration register for setting SD bit                      */
    if (twi_master_transfer(TMP102_ADDRESS ,(uint8_t*)data_buffer,3,TWI_ISSUE_STOP))
    {
        /* Reads the configuration register content and store it in a variable               */   
        tmp102_config_reg_val=read_register_content(TMP102_CONFIG_REG);

        /* Checks whether the SD bit (9th bit) is set or not                                 */	
        if (tmp102_config_reg_val & 0x0100)
        {
            function_state =true;
        }

    }

    else
    {
        function_state=false;
    }

    return function_state;


}


/**
* @brief   This function returns the  temperature value to the calling function  using 
*          one-shot  mode.  By writing '1' to the OS bit in  configuration  register 
*          whenever temperature is needed.
*@features 1. Performs a write operation to the configuration register setting the 
*             OS (ONE-SHOT) bit as '1' (setting TMP102 in ONE-SHOT MODE).
*          2. Reads the configuration register 
*									if OS = 0 , conversion not completed
*										 OS = 1 , conversion complete 
*							verify that conversion is complete.
*          3. Read temperature register value , since it is in 12 bit format (lower 4   
*             bits of Most Significant Byte is 0) the value is right shifted 4 times. 
*          4. Returns the temperature value.
*/
uint16_t get_tmp102_oneshot_temp (void)
{

		int16_t tmp102_configuration_reg_val, tmp102_temperature_reg_val;
    uint8_t data_buffer[3];
     
    data_buffer[0]      = TMP102_CONFIG_REG ;  /* Select configuration register             */

    /* Set OS bit as '1' in the Most Significant Byte of configuration register             */ 
    data_buffer[1]      = (TMP102_CONFIG_REG_DEFAULT_MSB | TMP102_ENABLE_SD_BIT_IN_MSB | TMP102_ENABLE_OS_BIT_IN_MSB); 

    data_buffer[2]      = TMP102_CONFIG_REG_DEFAULT_LSB; /* Default Least Significant Byte  */



    /* Start one-shot conversion by writing OS =1                                           */
    if (twi_master_transfer(TMP102_ADDRESS ,(uint8_t*)data_buffer,3,TWI_ISSUE_STOP))
    {
        nrf_delay_ms (30);    /*26 ms delay required for  one shot temperature conversion*/ 
        tmp102_configuration_reg_val = read_register_content (TMP102_CONFIG_REG);
        tmp102_configuration_reg_val = (tmp102_configuration_reg_val & 0x8000);


        /* Check if OS=1 or not. If OS=1, the conversion is complete                    */
        if ((tmp102_configuration_reg_val) != 0)
        {
            tmp102_temperature_reg_val = read_register_content (TMP102_TEMP_REG);	

            /* Remove four '0' bits in the Least Significant Byte                       */
            tmp102_temperature_reg_val = ((tmp102_temperature_reg_val >> 4) & 0x0FFF);

            /* Can be used to identify temperature while debugging                      */
            /*temperature_in_celsius = (float)tmp102_temperature_reg_val * 0.0625;*/
        }

    }

    return tmp102_temperature_reg_val ;


}


