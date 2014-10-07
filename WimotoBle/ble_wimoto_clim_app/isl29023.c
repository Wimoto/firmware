/** 
*@file     This file contains the source code for ISL29023 light sensor driver.
*
* Date   : 10/23/2013
* Author : Hariprasad C R - GadgEon Systems
*/


#include "wimoto_sensors.h"

/********************************************************************************************/
/* PRIVATE FUNCTIONS																		*/
/********************************************************************************************/

/**
*@brief   Function to Read the contents of registers of ISL29023
*@details 1.Read Content of register whose base address is specified as the parameter
*         2.Return the 8 bit data after reading , if not read return 0
*/
uint8_t ISL29023_read_register(uint8_t base_address)
{
    uint8_t data_buffer[1];
    data_buffer[0] = 0xFF;    /* Initializing data buffer */

    if (twi_master_transfer (ISL29023_ADDRESS ,(uint8_t*)&base_address,1,TWI_ISSUE_STOP))
    {

        if (twi_master_transfer (ISL29023_ADDRESS | TWI_READ_BIT ,data_buffer ,1 ,TWI_ISSUE_STOP))
        {
            return  data_buffer[0];	
        }

    }

    return 0;

}

/**
*@brief   Function to write data to the registers of ISL29023
*@details 1.The arguments of the function are
*            (1)Base address of the register to which data is to be written
*            (2)8 bit data to be written
*         2.Returns a true value on the success of the function otherwise a false value
*/
bool ISL29023_write_to_reg(uint8_t base_Address,uint8_t data)
{
    uint8_t data_buffer[2];
    data_buffer[0] = base_Address;        /* Base address of the register to which the data is to be written */
    data_buffer[1] = data;                /* 8 Bit data which is to be written                               */ 

    if (twi_master_transfer (ISL29023_ADDRESS ,(uint8_t*)data_buffer ,2 ,TWI_ISSUE_STOP))
    {
        return (true);
    }

    return false;

}



/********************************************************************************************/
/* PUBLIC FUNCTIONS																			*/
/********************************************************************************************/

/**
*@brief   Function to change Full Scale Reading of LUX and enable Power down mode of ISL29023 
*@details 1.FSR value is configured as 64000 for Full Scale Reading considering the lighting conditions
*           where the device will be used 
*         2.16 bit data register mode is enabled by default condition and used it  
*         3.Power down mode is enabled for power saving considerations (on demand based data)
*         4.Function returns a true value on the success of the function            
*/
bool ISL29023_config_FSR_and_powerdown(void)
{
    uint8_t test_cmd_reg1 = 0 ,test_cmd_reg2 = 0;;

    ISL29023_write_to_reg (ISL29023_COMMAND_REG_2 , ISL29023_USE_64K_LUX_FSR);
    test_cmd_reg2 = ISL29023_read_register (ISL29023_COMMAND_REG_2);

    if (test_cmd_reg2 & ISL29023_USE_64K_LUX_FSR) 
    {
        ISL29023_write_to_reg (ISL29023_COMMAND_REG_1 , ISL29023_ENABLE_POWER_DOWN);
        test_cmd_reg1 = ISL29023_read_register (ISL29023_COMMAND_REG_1);

        if (!test_cmd_reg1) 
        {
            return true;
        }

    }				

    return false;

}

/**
*@brief   Function to enable One time Ambient Light Sensing mode
*@details 1.it will start ADC conversion with light sensing. It will go to the power-down mode 
*             automatically  after one conversion is finished and keep the conversion data available 
*             in data registers
*         2.Returns the 16 bit value in the data register after the light sensing and conversion
*              has been finished
*/
uint16_t ISL29023_get_one_time_ALS()
{
    uint8_t data_reg_LSB = 0 ,data_reg_MSB = 0 ,reg_content;
    uint32_t combined_val;

    ISL29023_write_to_reg (ISL29023_COMMAND_REG_1 , ISL29023_ENABLE_ALS_ONCE);
    reg_content = ISL29023_read_register (ISL29023_COMMAND_REG_1);

    if (reg_content & ISL29023_ENABLE_ALS_ONCE) 
    {  
        nrf_delay_ms(90);                                     /* Integration time required for 16bit conversion */		
        data_reg_LSB =   ISL29023_read_register (ISL29023_DATA_REG_LSB); /* Least Significant Byte of Data Register        */
        data_reg_MSB =   ISL29023_read_register (ISL29023_DATA_REG_MSB); /* Most Significant Byte of Data Register         */ 
        combined_val = ((data_reg_MSB << 8) | data_reg_LSB);
        return combined_val;                                        /* Return two 8 bit data as a 16 bit data         */  
    } 

    return 0;		                                             /* On failure return 0                            */

}
