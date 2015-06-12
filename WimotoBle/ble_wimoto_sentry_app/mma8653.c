/** 
*@file     This file contains the source code for MMA8653  accelerometer driver.
*
* Author : Hariprasad CR
* Date   : 11/25/2013
*/
#include "wimoto_sensors.h"
#include "mma8653.h"

/**
*@brief   Function to read the data from the registers of MMA8653FC
*@details 1.The argument of the function is
*           (1)Base address of the register from which the data is to be read
*         2.Returns the content of read register after reading , if not returns 0
*/
bool MMA8653_read_register (uint8_t base_address,uint8_t * ptr_to_strng)
{
    uint8_t data_buffer[1];
    data_buffer[0] = 0xFF;             /* Initializing data buffer */	
   
    if (twi_master_transfer (MMA8653_ADDRESS ,(uint8_t*)&base_address ,1 ,TWI_DONT_ISSUE_STOP))
    {
        if (twi_master_transfer ( MMA8653_ADDRESS | TWI_READ_BIT ,data_buffer ,1 ,TWI_ISSUE_STOP))
        {
            *ptr_to_strng = data_buffer[0];	
			       return true;
        }			
    }	
		
    return false;		
		
}

/**
*@brief   Function to Write data to the registers of MMA8653FC
*@details 1.The argument of the function is
*           (1)Base address of the register from which the data is to be read
*           (2)8 bit data to write
*         2.Function returns a true value on the success of the function
*/
bool MMA8653_write_to_reg(uint8_t base_Address, uint8_t data)
{
    uint8_t data_buffer[2];
    data_buffer[0] = base_Address; 
    data_buffer[1] = data; 
    
	  if (twi_master_transfer (MMA8653_ADDRESS ,(uint8_t*)data_buffer ,2 ,TWI_ISSUE_STOP))
        return true;
    else
        return false;	
}

/**
*@brief   Function to enable standby mode
*@details 1.Enable the standby mode by clearing ACTIVE bit in the System Control 1 register
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_StandbyMode_Enable (void)
{
    uint8_t read_Reg_Val;
    
	  MMA8653_write_to_reg (MMA8653_CTRL_REG1 , MMA8653_ENABLE_STANDBY_MODE);   /* Clear ACTIVE bit in mode register */
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false ;  
    
	  if (read_Reg_Val == MMA8653_ENABLE_STANDBY_MODE) 
        return true;
    else
        return false;
		
}

/**
*@brief   Function to enable active mode
*@details 1.Enable the active mode by setting ACTIVE bit in the System Control 1 register 
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_ActiveMode_Enable (void)
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;
	
    if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | MMA8653_ENABLE_ACTIVE_MODE;
    MMA8653_write_to_reg (MMA8653_CTRL_REG1 , write_to_Reg_Val);   /* Clear MODE bit in mode register */
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false;  

	  if ((read_Reg_Val & MMA8653_ENABLE_ACTIVE_MODE)) 
        return true;
    else
        return false;
		
}

/**
*@brief   Function to set the Full Scale Range
*@details 1.Set the bits FS1,FS0 for the corresponding dynamic range
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_FullScaleRange_Set (uint8_t range_val)
{
    uint8_t read_Reg_Val;
    
	  MMA8653_write_to_reg (MMA8653_XYZ_DATA_CFG , range_val);
	
    if (false == MMA8653_read_register (MMA8653_XYZ_DATA_CFG, &read_Reg_Val)) return false;  
    
	  if (read_Reg_Val == range_val) 
        return true;
    else
        return false;
		
}

/**
*@brief   Function to set freefall motion threshold
*@details 1.Set the bits THS[6-0] for the corresponding threshold ,ie; content of register * 0.063 g/LSB gives 
*            the threshold value
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_FreefallMotionThresholdReg_Set (uint8_t Reg_val)
{
    uint8_t read_Reg_Val;
    
	  MMA8653_write_to_reg (MMA8653_FF_MT_THS , Reg_val);
	
    if (false == MMA8653_read_register (MMA8653_FF_MT_THS, &read_Reg_Val)) return false;  
    
	  if (read_Reg_Val == Reg_val) 
        return true;
    else
        return false;
		
}

/**
*@brief   Function to set the number of debounce sample counts for the event trigger
*@details 1.Set the bits D[7:0] for the debounce sample counts that continuously match the detection condition
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_DebounceRegister_Set (uint8_t Count_val)
{
    uint8_t read_Reg_Val;
    
	  MMA8653_write_to_reg (MMA8653_FF_MT_COUNT , Count_val);
	
    if (false == MMA8653_read_register (MMA8653_FF_MT_COUNT, &read_Reg_Val)) return false;  
    
	  if (read_Reg_Val == Count_val) 
        return true;
    else
        return false;
		
}

/**
*@brief   Function to set the number of debounce sample counts for the event trigger on Orientation Detection
*@details 1.Set the bits D[7:0] for the debounce sample counts that continuously match the detection condition
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_OrientationDebounceRegister_Set (uint8_t Count_val)
{
    uint8_t read_Reg_Val;
    
	  MMA8653_write_to_reg (MMA8653_PL_COUNT , Count_val);
	
    if (false == MMA8653_read_register (MMA8653_PL_COUNT, &read_Reg_Val)) return false;  
    
	  if (read_Reg_Val == Count_val) 
        return true;
    else
        return false;
		
}

/**
*@brief   Function to enable the Fast Read Mode (8 bit data's are obtained from the X,Y,Z registers)
*@details 1.Set the bit F_READ in System Control 1 register
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_FastRead_Enable (void)
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;

	  if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | MMA8653_FAST_READ_ENABLE;
    MMA8653_write_to_reg (MMA8653_CTRL_REG1 , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false; 
	
    if ((read_Reg_Val & MMA8653_CHECKING_FASTREAD)) 
        return true;
    else
        return false;
		
}

/**
*@brief   Function to set the Data Rate 
*@details 1.Set the bits [DR2 DR1 DR0] in System Control 1 register for corresponding frequency 
*            (used for normal operation or in waked up state)
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_DATA_RATE_Set (uint8_t DR_val)         /*expected value is DR[2 1 0] = [1:0:0] for 50Hz */ 
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;
	
    if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | DR_val;
    MMA8653_write_to_reg (MMA8653_CTRL_REG1 , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false; 
	
    if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;		
		
}

/**
*@brief   Configures the Auto-WAKE sample frequency when the device is in SLEEP Mode
*@details 1.Set the bits [ASLP_RATE1 ASLP_RATE0] in System Control 1 register for corresponding frequency 
*            (the sampling frequency while the device is in sleep)           
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_ASLP_RATE_Set (uint8_t ASLP_val)           /*Preferable value is ASLP[1 0] = [1:1] for 1.56Hz */
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;

	  if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | ASLP_val;
    MMA8653_write_to_reg (MMA8653_CTRL_REG1 , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG1, &read_Reg_Val)) return false;
   
  	if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;		
		
}

/**
*@brief   Configures The Oversampling modes in both WAKE Mode MOD[1:0] and also in the SLEEP Mode SMOD[1:0].
*@details 1.Set the bits [MODS1 MODS0] in System Control 2 register for power scheme selection in WAKE Mode 
*         2.Set the bits [SMODS1 SMODS0] in System Control 2 register for power scheme selection in SLEEP Mode          
*         3.Function returns a true value on the success of the function 
*/
bool MMA8653_OversamplingMode_Enable (uint8_t OS_val)  /*Expected Val (MMA8653_MODS_MODE_LOW_POWER | MMA8653_SMODS_MODE_LOW_POWER) */
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;

	  if (false == MMA8653_read_register (MMA8653_CTRL_REG2, &read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | OS_val;
    MMA8653_write_to_reg (MMA8653_CTRL_REG2 , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG2, &read_Reg_Val)) return false;

	  if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;	
		
}	

/**
*@brief   Enables Auto-SLEEP Enable in MMA8653
*@details 1.Set the bits [SLPE] in System Control 2 register           
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_AutoSleep_Enable (void)         
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG2, &read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | MMA8653_AUTO_SLEEP_ENABLE;
    MMA8653_write_to_reg (MMA8653_CTRL_REG2 , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG2, &read_Reg_Val)) return false;
    
	  if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;		
		
}	

/**
*@brief   Enables Wake from Freefall/Motion interrupt which used to control the Auto-WAKE/SLEEP mode
*@details 1.Set the bit [WAKE_FF_MT] in Interrupt Control register/Control register 3  
*         2.Clear the bit [IPOL] for ACTIVE low Interrupt polarity 
*         3.Set the bit [PP_OD] Open Drain on interrupt pad       
*         4.Function returns a true value on the success of the function 
*/
bool MMA8653_InterruptControlReg_config	 (uint8_t reg_Val)   /*Expecting type of interrupt to poke auto sleep/wake up function
				                                                       WAKE_FF_MT = 1*/       
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;	
	
    if (false == MMA8653_read_register (MMA8653_CTRL_REG3, &read_Reg_Val)) return false;	
	
    write_to_Reg_Val = read_Reg_Val | reg_Val;
    MMA8653_write_to_reg (MMA8653_CTRL_REG3 , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG3, &read_Reg_Val)) return false;	 
	
    if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;		
		
}

/**
*@brief   Enables the interrupts (Auto-WAKE/SLEEP, Orientation Detection, Freefall/Motion, and Data Ready).
*@details 1.Set the bits corresponding to the interrupt which is to be enabled in 
*			Interrupt Enable register/Control register 4        
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_InterruptEnableReg_config (uint8_t write_to_Reg_Val)   /*Expecting MMA8653_MOTION_INTERRUPT_ENABLE*/
{
    uint8_t read_Reg_Val;	
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG4, &read_Reg_Val)) return false;
	
	  write_to_Reg_Val = read_Reg_Val | write_to_Reg_Val;
    MMA8653_write_to_reg (MMA8653_CTRL_REG4 , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG4, &read_Reg_Val)) return false; 
	
    if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;

}

/**
*@brief   Maps the desired interrupts to INT2 or INT1 pins.
*@details 1.By default all interrupts are mapped to INT2 pin. To map to INT1 pin set the bits for the 
*            corresponding interrupts in Interrupt Configuration register/Control register 4        
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_InterruptConfigurationReg_config (uint8_t write_to_Reg_Val) /*Expecting MMA8653_MOTION_FREEFALL_INT1*/
{
    uint8_t read_Reg_Val;	
    
	  if (false == MMA8653_read_register (MMA8653_CTRL_REG5, &read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | write_to_Reg_Val;
    MMA8653_write_to_reg (MMA8653_CTRL_REG5 , write_to_Reg_Val);
    
    if (false == MMA8653_read_register (MMA8653_CTRL_REG5, &read_Reg_Val)) return false; 

	  if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;

}	

/**
*@brief   Sets the minimum time period of inactivity required to switch the part between Wake and Sleep status.
*           in Auto-WAKE/SLEEP Detection register 
*@details 1.D7–D0 defines the minimum duration time needed to change the current ODR value from DR to ASLP_RATE. 
*			 The time step and maximum value depend on the ODR chosen       
*         2.Function returns a true value on the success of the function 
*/
bool MMA8653_ASLPCounter_val(uint8_t write_to_Reg_Val)  /* (47 * .640s) = 30s time-out for changing data rate from DR to ASLP */
{
    uint8_t read_Reg_Val;
	
    MMA8653_write_to_reg (MMA8653_ASLP_COUNT , write_to_Reg_Val);
    if (false == MMA8653_read_register (MMA8653_ASLP_COUNT, &read_Reg_Val)) return false;  
	
    if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;
		
}

/**
*@brief   Freefall/Motion configuration register for setting up the conditions of the freefall or motion function in
*           Freefall/Motion Configuration register.
*@details 1.Set the bit [OAE] for Motion detect flag selection
*         2.Set the bit [ZEFE YEFE XEFE] which determine  axes are taken into consideration for motion detection           
*         3.Function returns a true value on the success of the function 
*/
bool MMA8653_FreefallMotionConfigReg_Set (uint8_t config_val)          /*Expected Val MMA8653_MOTION_DETECTION*/
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;

	  if (false == MMA8653_read_register (MMA8653_FF_MT_CFG,&read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | config_val;
    MMA8653_write_to_reg (MMA8653_FF_MT_CFG , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_FF_MT_CFG,&read_Reg_Val)) return false;
    
	  if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;		

}	

/**
*@brief   Portrait/orientation reg config
*@details 1. Set DBCNTM value to 1 so that counter clears to zero whenever motion stops.
					2. Set PL_EN to 1 to enable portrait/landscape detection.
*         3.Function returns a true value on the success of the function 
*/
bool MMA8653_OrientationConfigReg_Set (uint8_t config_val)         
{
    uint8_t read_Reg_Val;
    uint8_t write_to_Reg_Val;

	  if (false == MMA8653_read_register (MMA8653_PL_CFG,&read_Reg_Val)) return false;
	
    write_to_Reg_Val = read_Reg_Val | config_val;
    MMA8653_write_to_reg (MMA8653_PL_CFG , write_to_Reg_Val);
    
	  if (false == MMA8653_read_register (MMA8653_PL_CFG,&read_Reg_Val)) return false;
    
	  if (read_Reg_Val == write_to_Reg_Val) 
        return true;
    else
        return false;		

}	


/**
*@brief   Initialization of MMA8653 accelerometer
*@details MMA8653 accelerometer is configured to produce 8 bit data from the X_MSB,Y_MSB,Z_MSB register
*          and also configured for generating an interrupt if a movement occured. The threshold value for
*          the movement is set so that movement's greater than the threshold will only trigger the interrupt
*          through iNT1 pin.Auto wake/sleep mode is used inorder to reduce power consumption
*/
bool MMMA8653_Init(void)

{		
		//reset device 
		//if (false == MMA8653_write_to_reg(MMA8653_CTRL_REG2, 0x40)) return false;
	  /* Enable stand by mode for configuring MMA8653*/
    if ( false == MMA8653_StandbyMode_Enable () ) return false; 
	
    /* 8 Bit data format is chosen*/	
    if ( false == MMA8653_FastRead_Enable () ) return false; 
	
		/* 1 Count corresponds to 15.6mg */
    if ( false == MMA8653_FullScaleRange_Set (MMA8653_FSR_2G) ) return false; 
		//MOTION DETECTION CODE-----------------------------------------------------------------------
		/* Enable motion detection and X,Y and Z axis are taken for motion detection*/
    if ( false == MMA8653_FreefallMotionConfigReg_Set ((  MMA8653_MOTION_DETECTION | MMA8653_ZEFE | MMA8653_YEFE | MMA8653_XEFE)) ) return false; 
	
	  /* Sets the threshold for the motion detection */
	  if ( false == MMA8653_FreefallMotionThresholdReg_Set( 0x14) ) return false;
	
	  /* Set the number of debounce sample counts for the event trigger */
	  if ( false == MMA8653_DebounceRegister_Set(0x02) ) return false;
	
		//ORIENTATION DETECTION CODE------------------------------------------------------------------
		/* Activate orientation interrupt */
		if ( false == MMA8653_OrientationConfigReg_Set (0xC0)) return false;
		
		if ( false == MMA8653_OrientationDebounceRegister_Set(0x05)) return false;
		

	  /* Set the data rate for 50hz when the device is woke up by an interrupt*/
    if ( false == MMA8653_DATA_RATE_Set(MMA8653_DR_50HZ) ) return false;	 
	
	  /* Set the data rate for 1.56Hz when device is put to sleep mode*/
    if ( false == MMA8653_ASLP_RATE_Set(MMA8653_ASLP_RATE_12HZ) ) return false;
	
    /* Time-out required to put back the device into Sleep mode*/	
    if ( false == MMA8653_ASLPCounter_val(MMA8653_3S_TIMEOUT) ) return false;  
	
	  /*Enable low power mode	for wakeup and sleep functions*/
    if ( false == MMA8653_OversamplingMode_Enable((MMA8653_MODS_MODE_LOW_POWER | MMA8653_SMODS_MODE_LOW_POWER)) ) return false;
		
	  /* Enable auto sleep mode*/
    if ( false == MMA8653_AutoSleep_Enable() ) return false;  				//extraneous?
		
	  /* Enable Motion/Free-fall interrupt for waking up from sleep*/
    if ( false == MMA8653_InterruptControlReg_config((MMA8653_CR3_ORIENTATION_ENABLE | MMA8653_CR3_MOTION_ENABLE | MMA8653_INTERRUPT_PIN_CONFIG )) ) return false; 
		
		//MMA8653_write_to_reg (MMA8653_CTRL_REG4 , 0x00);
		
	  /*Free-fall/Motion Interrupt Enable  */
    if ( false == MMA8653_InterruptEnableReg_config(MMA8653_ORIENTATION_INTERRUPT_ENABLE | MMA8653_MOTION_INTERRUPT_ENABLE ) ) return false;  
		
		/* Use INT1 pin for routing the interrupt */
    if ( false == MMA8653_InterruptConfigurationReg_config( MMA8653_ORIENTATION_INT1 | MMA8653_MOTION_FREEFALL_INT1) ) return false; 
		
		/*Configure Active mode for continuous monitoring*/
    if ( false == MMA8653_ActiveMode_Enable() ) return false; 
		
		return true;
}

/* Data is assumed to be in 8 bit format*/
/**
*@brief    Function to read the X,Y,Z data from  OUT_X(Y,Z)_MSB, registers
*
*@details  Check the ZYXDR bit in status register for X, Y, Z-axis new data ready, if the new data is ready
*           read the X,Y,Z axis data
*/
bool MMA8653_ReadXYZdata(uint32_t * ptr_to_Reg_val)
{
    uint8_t base_address   = MMA8653_OUT_X_MSB; /* Base address of the X_MSB register(first register of coordinates*/
    uint8_t data_buffer[3] = {0xff,0xFF,0xFF};  /* Initializing data buffer */ 
    uint8_t read_Reg_Val;

     if(false == MMA8653_read_register(MMA8653_STATUS, &read_Reg_Val)) return false;

    if (read_Reg_Val & MMA8653_CHECKING_STATUS)
    {	
        if (twi_master_transfer (MMA8653_ADDRESS ,(uint8_t*)&base_address ,1 ,TWI_DONT_ISSUE_STOP))
        {
            twi_master_transfer ( MMA8653_ADDRESS | TWI_READ_BIT ,data_buffer ,3 ,TWI_ISSUE_STOP);
					  
					 *ptr_to_Reg_val = ((data_buffer[0] << 24) | (data_buffer[1] << 16) | data_buffer[2]); 
					
					  return true;
        }

    } 
		
    return false;				
		
}

