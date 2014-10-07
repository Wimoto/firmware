/**
*@file     This file contains the source code for  HTU21D Humidity and Temperature Sensor driver.
*
*Date      : 10/25/2013
*Author    : Hariprasad C R - GadgEon Systems
*/


#include "wimoto_sensors.h"



/********************************************************************************************/
/* PRIVATE FUNCTIONS																		*/
/********************************************************************************************/

/**
*@brief   Checks for CRC errors
*@details Function takes the 16 bit data and calculates CRC value from it , checks with the
*          received CRC value ,if the values matches,a boolean value true is returned
*          otherwise false
*/
/*------------------------------------------------------------------------------------------*/
bool HTU21D_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
/*------------------------------------------------------------------------------------------*/
{
    uint8_t crc = 0,_bit,byteCtr;
    uint8_t swap_variable;
    swap_variable = data[0];            /*Re-arranging input data for CRC calculation*/
    data[0]       = data[1];
    data[1]       = swap_variable;

    for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[nbrOfBytes-1-byteCtr]);
        for (_bit = 8; _bit > 0; --_bit)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x0131;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }

    if (crc != checksum)
    {
        return false; /*if there is  CRC error it returns a false value */
    }
    else
    {
        return true;
    }

}



/**
*@brief   Function to read the data using HOLD MASTER mode
*@details The argument of the function is
*             (1)Command to trigger what type of measurement
*         Function reads the 16 bit data along with the 8 bit CRC code and checks for CRC errors(if error checking is enabled) , if no error is found the
*            16 bit variable is returned
*/
/*------------------------------------------------------------------------------------------*/
uint16_t HTU21D_MeasureHM(etHTU21MeasureType eHTU21MeasureType)
/*------------------------------------------------------------------------------------------*/
{

    uint8_t CRC_value, reg_val_array[2]={0x00,0x00};
    uint16_t Reg_value = 0;
    uint32_t temporary_value = 0;

    switch(eHTU21MeasureType)
    {
    case HUMIDITY:  temporary_value = HTU21D_ReadMeasurementValue (TRIG_RH_MEASUREMENT_HM); /*Relative humidity measurement trigger*/
        CRC_value = (temporary_value & 0x000000FF);                             /* Last 8 bit is CRC */
        Reg_value = (uint16_t)((temporary_value & 0x00FFFF00) >> 8);            /* RH as 16 bit data(including status bits) */
        reg_val_array[1]=(uint8_t)Reg_value;
        reg_val_array[0]=(uint8_t)(Reg_value >> 8);                             /* If CRC checking is enabled check for CRC errors */

        if (HTU21D_CRC_CHECK_ENABLE == FEATURE_ENABLED)
        {
            if(false == HTU21D_CheckCrc((uint8_t*)reg_val_array,2,CRC_value))   /* Checks for CRC errors */
            {
                return 0;                                                       /* If the returned value is 0 it indicates CRC error */
            }
        }

        break;

    case TEMP   :   temporary_value = HTU21D_ReadMeasurementValue (TRIG_T_MEASUREMENT_HM);  /* temperature measurement trigger */
        CRC_value =  (temporary_value & 0x000000FF);                            /* Last 8 bit is CRC */
        Reg_value = ((temporary_value & 0x00FFFF00) >> 8);                      /* T as 16 bit data(including status bits) */
        reg_val_array[1] = (uint8_t)Reg_value;
        reg_val_array[0] = (uint8_t)(Reg_value >> 8);

        if (HTU21D_CRC_CHECK_ENABLE == FEATURE_ENABLED)                                   /* If CRC checking is enabled check for CRC errors */
        {
            if(false == HTU21D_CheckCrc((uint8_t*)reg_val_array,2,CRC_value))  /* Checks for CRC errors */
            {
                return 0;                                                      /* if the returned value is 0 it indicates CRC error */
            }
        }

        break;

        default      :   return 0;
    }

    return Reg_value;

}

/**
*@brief   Function to assist HTU21D_MeasureHM function
*@details Function reads 16 bit data and 8 bit CRC, after that these two are combined to form a 32 bit variable which is
*           is returned to the called function
*/
/*------------------------------------------------------------------------------------------*/
uint32_t HTU21D_ReadMeasurementValue(uint8_t Command_for_activity)
/*------------------------------------------------------------------------------------------*/
{
    volatile uint32_t temporary_variable1=0;
    uint8_t data_buffer[3];
    data_buffer[0]=0x00;
    data_buffer[1]=0x00;
    data_buffer[2]=0x00;

    if (twi_master_transfer(HTU21D_ADDRESS ,(uint8_t*)&Command_for_activity,1,TWI_DONT_ISSUE_STOP))
    {
        if(Command_for_activity == TRIG_RH_MEASUREMENT_HM)
        nrf_delay_ms(20);
        else
        nrf_delay_ms(50);
        if (twi_master_transfer(HTU21D_ADDRESS | TWI_READ_BIT ,data_buffer ,3 ,TWI_ISSUE_STOP))
        {
            temporary_variable1 = ((data_buffer[0] << 16) | (data_buffer[1]<<8) | data_buffer[2]);
            return temporary_variable1;
        }

    }

    return 0;

}

/**
*@brief   Function to read the data using NO HOLD MASTER mode(POLLING)
*@details The argument of the function is
*             (1)Command to trigger what type of measurement
*         Function returns the 16 bit data
*/
/*------------------------------------------------------------------------------------------*/
uint16_t HTU21D_MeasurePOLL(etHTU21MeasureType eHTU21MeasureType)
/*------------------------------------------------------------------------------------------*/
{
    uint16_t Reg_value = 0;

    switch(eHTU21MeasureType)
    {
    case HUMIDITY: 	Reg_value = HTU21D_PollMasterTransfer(TRIG_RH_MEASUREMENT_POLL);
        break;

    case TEMP    :  Reg_value = HTU21D_PollMasterTransfer(TRIG_T_MEASUREMENT_POLL);
        break;

        default      :  return 0;
    }

    return Reg_value;

}

/**
*@brief   Function to assist HTU21D_MeasurePOLL function
*@details Function reads 16 bit data and 8 bit CRC, after that and checks for CRC errors(if error checking is enabled) , if no error is found the
*            16 bit variable is returned
*/
/*------------------------------------------------------------------------------------------*/
uint16_t HTU21D_PollMasterTransfer(uint8_t MeasurementType)
/*------------------------------------------------------------------------------------------*/
{

    uint8_t CRC_value;
    uint16_t Reg_value = 0;
    uint8_t data_buffer[3];
    uint16_t timeout = 200;  /* 100ms */
    uint8_t reg_val_array[2]={0x00,0x00};

    if(twi_master_transfer(HTU21D_ADDRESS ,(uint8_t*)&MeasurementType ,1 ,TWI_DONT_ISSUE_STOP))
    {
        while((--timeout))
        {
            if(twi_master_transfer(HTU21D_ADDRESS | TWI_READ_BIT ,(uint8_t*)data_buffer ,3 ,TWI_ISSUE_STOP))
            {
                Reg_value = ((data_buffer[0] << 8) | data_buffer[1]);    /* 16 bit reg value */
                break;
            }
        }

        reg_val_array[1] = (uint8_t)Reg_value;
        reg_val_array[0] = (uint8_t)(Reg_value >> 8);
        CRC_value        =  data_buffer[2];                            /* Last 8 bit is CRC   */

        if (HTU21D_CRC_CHECK_ENABLE == FEATURE_ENABLED)                           /* If CRC checking is enabled check for CRC errors */
        {
            if(false == HTU21D_CheckCrc((uint8_t*)reg_val_array ,2 ,CRC_value))   /* checks for CRC errors */
            {
                return 0;                                             /* CRC error occurred */
            }
        }

    }

    return Reg_value;                                                /* if success returns the 16bit register value */

}

/**
*@brief   Function to calculate Relative Humidity from the 16 bit data read (useful while debugging)
*/
/*------------------------------------------------------------------------------------------*/
float f32CalcRH(uint16_t u16sRH)
/*------------------------------------------------------------------------------------------*/
{
    float humidityRH;              /* variable for result  */
    u16sRH &= ~0x0003;             /* clear bits [1..0] (status bits) */

    /*-- calculate relative humidity [%RH] -- */
    humidityRH = -6.0 + (125.0/65536 * (float)u16sRH); /* RH= -6 + 125 * SRH/2^16 */
    return humidityRH;
}

/**
*@brief   Function to calculate Temperature in Degree Celsius (useful while debugging)
*/
/*------------------------------------------------------------------------------------------*/
float f32CalcTemperatureC(uint16_t u16sT)
/*------------------------------------------------------------------------------------------*/
{
    float temperatureC;             /* variable for result */

    u16sT &= ~0x0003;              /* clear bits [1..0] (status bits) */

    /*-- calculate temperature [°C] -- */
    temperatureC= -46.85 + (175.72/65536 *(float)u16sT); /* T= -46.85 + 175.72 * ST/2^16 */
    return temperatureC;
}


/********************************************************************************************/
/* PUBLIC FUNCTIONS																			*/
/********************************************************************************************/

/**
*@brief   Function to measure Humidity and returns 16 bit value , Status bit cleared
*/
/*------------------------------------------------------------------------------------------*/
uint16_t eDRV_HTU21_MeasureHumidity()
/*------------------------------------------------------------------------------------------*/
{
    /* Comment the corresponding line line to choose type of measurement HOLD MASTER/NO HOLD MASTER mode ( default : HOLD MASTER mode)*/
    uint16_t result = 0;

    if(USE_HOLD_MASTER_MODE_ENABLE == FEATURE_ENABLED)
    {
        result = HTU21D_MeasureHM(HUMIDITY);     /*Use hold master mode*/
    }
    else
    {
        result = HTU21D_MeasurePOLL(HUMIDITY);   /*Use no hold master mode*/
    }
    result &= ~0x0003; 	   /* Clearing status bits (0th and 1st bit of 16 bit data)*/
    return (result);
}

/**
*@brief   Function to measure Temperature and returns 16 bit value , Status bit cleared
*/
/*------------------------------------------------------------------------------------------*/
uint16_t eDRV_HTU21_MeasureTemperature()
/*------------------------------------------------------------------------------------------*/
{
    /* Comment the corresponding line line to choose type of measurement HOLD MASTER/NO HOLD MASTER mode ( default : HOLD MASTER mode)*/
    uint16_t result = 0;

    if(USE_HOLD_MASTER_MODE_ENABLE == FEATURE_ENABLED)
    {
        result = HTU21D_MeasureHM(TEMP);     /*Use hold master mode*/
    }
    else
    {
        result = HTU21D_MeasurePOLL(TEMP);   /*Use no hold master mode*/
    }
    result &= ~0x0003;       /* Clearing status bits (0th and 1st bit of 16 bit data)*/
    return (result);
}

/**
*@brief   Function for Soft reset (Power on reset value)
*/
/*------------------------------------------------------------------------------------------*/
bool eDRV_HTU21_Reset(void)
/*------------------------------------------------------------------------------------------*/
{
    uint8_t data_buffer[1];
    data_buffer[0] = SOFT_RESET;

    if (twi_master_transfer(HTU21D_ADDRESS ,(uint8_t*)data_buffer,1,TWI_ISSUE_STOP))
    {
        nrf_delay_ms(15);    /* Delay for soft reset*/
        return (true);
    }

    return false;

}

/**
*@brief   Function to read the user register
*@details Function reads the user register and checks for CRC errors(if error checking is enabled), if no CRC error occurred the 8 bit
*           user register data is returned
*/
/*------------------------------------------------------------------------------------------*/
uint8_t  HTU21D_ReadUserRegister()
/*------------------------------------------------------------------------------------------*/
{
    uint8_t checksum=0,user_reg_val[1],data_buffer[2];
    data_buffer[0]  = 0x00;   /* Initializing data buffers used for reception of data */
    data_buffer[1]  = 0x00;
    user_reg_val[0] = 0;      /* Register to hold User Register data*/

    if (twi_master_transfer(HTU21D_ADDRESS ,(uint8_t*)USER_REG_R ,1 ,TWI_DONT_ISSUE_STOP))
    {

        if (twi_master_transfer(HTU21D_ADDRESS | TWI_READ_BIT ,data_buffer ,2 ,TWI_ISSUE_STOP))
        {
            user_reg_val[0] = data_buffer[0];
            checksum        = data_buffer[1];
        }

    }

    if (HTU21D_CRC_CHECK_ENABLE == FEATURE_ENABLED)   /* If CRC checking is enabled check for CRC errors */
    {
        if(false == HTU21D_CheckCrc(user_reg_val,1,checksum))   /* Checks for CRC errors */
        {
            return 0;                       /* If there is  CRC error it returns a false value*/
        }
    }

    return user_reg_val[0];          /* returns the user register content (if there is no CRC errors) */

}

/**
*@brief   Function to write data to the user register
*@details Function  writes the 8 bit data provided as an argument to the user register .
*          if the write was successful return boolean value true, otherwise false
*/
/*------------------------------------------------------------------------------------------*/
bool HTU21D_WriteToUserRegister(uint8_t data)
/*------------------------------------------------------------------------------------------*/
{
    uint8_t data_buffer[2];

    data_buffer[0] = USER_REG_W;
    data_buffer[1] = data;

    if (twi_master_transfer(HTU21D_ADDRESS ,(uint8_t*)data_buffer ,2 ,TWI_ISSUE_STOP))
    {
        return (true);                       /* If write was successful returns a true value */
    }

    return false;

}



