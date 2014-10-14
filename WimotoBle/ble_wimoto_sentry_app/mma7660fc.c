/** 
*@file     This file contains the source code for MMA7660FC accelerometer driver.
*
* Author : Hariprasad CR
* Date   : 23.October.2013
*/

#include <stdio.h>
#include "wimoto_sensors.h"
#include "simple_uart.h"


/********************************************************************************************/
/* PRIVATE FUNCTIONS																		*/
/********************************************************************************************/


/**
*@brief   Function to enable active mode
*@details 1.Enable the active mode by setting MODE bit in the Mode Register
*         2.Function returns a true value on the success of the function 
*/

bool MMA7660_enable_active_mode()
{
    volatile uint8_t temporary_variable,mode_reg_val;

    mode_reg_val = MMA7660_read_register (MMA7660_MODE_REG);
    MMA7660_write_to_reg (MMA7660_MODE_REG , (mode_reg_val | MMA7660_ENABLE_ACTIVE_MODE));          /* Setting Mode Register for active mode */
    nrf_delay_ms(30);                                                     /*switching time from standby to active mode */
    temporary_variable = MMA7660_read_register (MMA7660_MODE_REG);

    if (temporary_variable & (mode_reg_val | MMA7660_ENABLE_ACTIVE_MODE)) 
    {		
				

        return true;
    }	

    return false;		

}

/**
*@brief   Function to enable standby mode
*@details 1.Enable the standby mode by clearing MODE bit in the Mode Register
*         2.Function returns a true value on the success of the function 
*/
bool MMA7660_enable_standby_mode()
{	
		char output[10];
		
    volatile uint8_t temporary_variable;
    MMA7660_write_to_reg (MMA7660_MODE_REG , MMA7660_ENABLE_STANDBY_MODE);   /* Clear MODE bit in mode register */
    temporary_variable=MMA7660_read_register (MMA7660_MODE_REG);
		
	
		
    if (temporary_variable) 
    {	

        return false;	 
    }

    return true;	

}

/**
*@brief   Function to read the data from the registers of MMA7660FC
*@details 1.The argument of the function is
*           (1)Base address of the register from which the data is to be read
*         2.Returns the content of read register after reading , if not returns 0
*/
uint8_t MMA7660_read_register(uint8_t base_address)
{
    uint8_t data_buffer[1];
    data_buffer[0] = 0xFF;             /* Initializing data buffer */

    if (twi_master_transfer (MMA7660FC_ADDRESS ,(uint8_t*)&base_address ,1 ,TWI_DONT_ISSUE_STOP))
    {

        if (twi_master_transfer (MMA7660FC_ADDRESS | TWI_READ_BIT ,data_buffer ,1 ,TWI_ISSUE_STOP))
        {
            return data_buffer[0];	
        }

    }

    return 0;

}

/**
*@brief   Function to Write data to the registers of MMA7660FC
*@details 1.The argument of the function is
*           (1)Base address of the register from which the data is to be read
*           (2)8 bit data to write
*         2.Function returns a true value on the success of the function
*/

bool MMA7660_write_to_reg(uint8_t base_Address,uint8_t data)
{
    volatile uint8_t data_buffer[2];
    data_buffer[0] = base_Address; 
    data_buffer[1] = data; 

    if (twi_master_transfer (MMA7660FC_ADDRESS ,(uint8_t*)data_buffer ,2 ,TWI_ISSUE_STOP))
    {
        return (true);
    }

    return false;

}



/********************************************************************************************/
/* PUBLIC FUNCTIONS																			*/
/********************************************************************************************/
/**
*@brief   Function to configure standby MMA7660FC and configure for Auto-wake & Auto-sleep mode  
*@details 1.Function initially enables standby mode so the MMA7660FC calculates orientation and makes data
*            available in X-out,Y-out and Z-out registers 
*         2.Configure MMA7660FC for shake & orientation interrupt 
*         3.Enable Auto-wake & Auto-sleep mode and 
*         4.Enable the Active mode 
*/
bool MMA7660_config_standby_and_initialize()
{
    volatile uint8_t temporary_variable1,temporary_variable2;

    uint8_t Mode_Reg_Val;

    // Enable standby mode for writing 
    //if (false == MMA7660_enable_standby_mode()) {simple_uart_putstring((const uint8_t *)"standby messed up \r\r"); return false;}
    // Sleep count value is 20 i.e.; waits for 5 seconds while sleeping 
    if (false == MMA7660_write_to_reg (MMA7660_SPCNT_REG,MMA7660_SLEEP_COUNT_5S)) {simple_uart_putstring((const uint8_t *)"sleep messed up \r\r"); return false;}	
    // Enable shake and orientation interrupt
    if (false == MMA7660_write_to_reg (MMA7660_INTSU_REG,MMA7660_SHAKE_ORIENT_INTERRUPT)) {simple_uart_putstring((const uint8_t *)"shake messed up \r\r"); return false;}
    // Enable the Auto-wake and Auto-sleep mode
    Mode_Reg_Val = MMA7660_read_register (MMA7660_MODE_REG);
    if (false == MMA7660_write_to_reg (MMA7660_MODE_REG,(Mode_Reg_Val | MMA7660_AWE_ASE_SET))) {simple_uart_putstring((const uint8_t *)"autowake-autosleep messed up \r\r"); return false;}
    // Configure 1 sample/s while sleeping and 4 sample/s in wake 
    if (false == MMA7660_write_to_reg (MMA7660_SR_REG,(MMA7660_AMSR_4SAMPLES | MMA7660_AWSR_1SAMPLES))) {simple_uart_putstring((const uint8_t *)"setting sample speed messed up \r\r"); return false;}
    // Disable tap detection in MMA7660
    if (false == MMA7660_write_to_reg (MMA7660_PDET_REG,MMA7660_DISABLE_TAP_DETECTION)) {simple_uart_putstring((const uint8_t *)"tap det messed up \r\r"); return false;}                
    // Enable Active mode
    Mode_Reg_Val = MMA7660_read_register (MMA7660_MODE_REG);
    if (false == MMA7660_write_to_reg (MMA7660_MODE_REG,(Mode_Reg_Val | MMA7660_ENABLE_ACTIVE_MODE))) {simple_uart_putstring((const uint8_t *)"active mode messed up \r\r"); return false;}
		
		simple_uart_putstring((const uint8_t *)"Everything initialized fine.\r\r\r");
    return true;

}

/**
*@brief   Function to read the orientation data from the registers of MMA7660FC
*@details 1.Function initially enables active mode so the MMA7660FC calculates orientation and makes data
*            available in X-out,Y-out and Z-out registers 
*         2.Puts back MMA7660FC into standby mode for power conservation
*         3.Function returns a contents of X-out,Y-out and Z-out registers as a 32 bit value
*               (24-16) bits  X-out register
*               (16-8) bits Y-out register
*               (8-0) bits Z-out register
*/
bool MMA7660_read_xyz_reg_one_time(uint32_t * ptr_to_Reg_val)
{
    uint8_t X_buff = 0,Y_buff = 0,Z_buff = 0;
    bool state1 = true ,state2 = true ,state3 = true;

    while(state1)                                        /*after enabling active mode read x,y,z registers */
    {
        X_buff = MMA7660_read_register (MMA7660_XOUT_REG);

        if(X_buff & MMA7660_ALERT_BIT_SET_CHECK) 
        state1 = true;                            /* If alert bit set re=read the register */
        else 
        state1 = false;

    }

    while(state2)
    {
        Y_buff = MMA7660_read_register(MMA7660_YOUT_REG);

        if(Y_buff & MMA7660_ALERT_BIT_SET_CHECK) 
        state2 = true;                           /* If alert bit set re=read the register */
        else 
        state2 = false;

    }

    while(state3)
    {
        Z_buff = MMA7660_read_register (MMA7660_ZOUT_REG);

        if(Z_buff & MMA7660_ALERT_BIT_SET_CHECK) 
        state3 = true;                           /* If alert bit set re=read the register */       
        else  
        state3 = false;

    }

    * ptr_to_Reg_val= ((X_buff << 16) | ( Y_buff <<8) | Z_buff);    /* 32 bit result is returned */

    return true;

}


/**
*@brief   Reading TILT register contents clears all the interrupts
*
*/
void MMA7660_ClearInterrupts(void)
{
    MMA7660_read_register(MMA7660_TILT_REG);
}



