/** 
*@file     This file contains the source code for TMP006 temperature sensor driver.
*
*Date      : 10/24/2013
*Author    : Hariprasad C R - GadgEon Systems
*/

#include "wimoto_sensors.h"

/********************************************************************************************/
/* PRIVATE FUNCTIONS																		*/
/********************************************************************************************/

/**
*@brief   Function to Read the contents of registers of TMP006
*@details 1.Read Content of register whose base address is specified as the parameter
*         2.Return the 16 bit data after reading , if not read return 0
*/
uint16_t TMP006_read_register(uint8_t base_address)
{
    uint16_t temporary_variable = 0;
    uint8_t data_buffer[2];
    data_buffer[0] = 0x00;    /* Initializing data buffers */
    data_buffer[1] = 0x00;		/* Initializing data buffers */

    if (twi_master_transfer (TMP006_ADDRESS ,(uint8_t*)&base_address ,1 ,TWI_ISSUE_STOP))
    {

        if (twi_master_transfer (TMP006_ADDRESS | TWI_READ_BIT ,data_buffer ,2 ,TWI_ISSUE_STOP))
        {
            temporary_variable = ((data_buffer[0] << 8) | data_buffer[1]); 
            return temporary_variable;	
        }

    }

    return 0;

}

/**
*@brief   Function to Write data to the registers of TMP006
*@details 1.The argument of the function is
*           (1)Base address of the register from which the data is to be read
*           (2)Most Significant Byte (MSB) to write
*           (3)Least Significant Byte(LSB) to write
*         2.Function returns a true value on the success of the function
*/

bool TMP006_write_to_reg(uint8_t base_Address ,uint8_t MSB ,uint8_t LSB)
{
    uint8_t data_buffer[3];
    data_buffer[0] = base_Address; 
    data_buffer[1] = MSB;          /* Most Significant Byte (MSB) to write */
    data_buffer[2] = LSB;          /* Least Significant Byte(LSB) to write */

    if (twi_master_transfer (TMP006_ADDRESS ,(uint8_t*)data_buffer ,3 ,TWI_ISSUE_STOP))
    {
        return (true);
    }

    return false;

}

/**
*@brief   Function to enable continuous conversion mode
*@details 1.Enable the continuous conversion mode by setting MOD(3-0) bits in the Configuration Register
*         2.Function returns a true value on the success of the function 
*/
bool TMP006_enable_continuous_conversion()
{
    if (false == TMP006_write_to_reg (TMP006_CONFIG_REG ,TMP006_ENABLE_CONVERSION , TMP006_CONFIG_REG_LSB))
    {
        return 0;
    }
    return true;
}


/********************************************************************************************/
/* PUBLIC FUNCTIONS																			*/
/********************************************************************************************/

/**
*@brief   Function to enable power-down mode
*@details 1.Enable the power-down mode by clearing MOD(3-0) bits in the Configuration Register
*         2.Function returns a true value on the success of the function 
*/
bool TMP006_enable_powerdown_mode()
{
    if (false == TMP006_write_to_reg (TMP006_CONFIG_REG ,TMP006_ENABLE_POWER_DOWN ,TMP006_CONFIG_REG_LSB))
    {
        return 0;
    }

    return true;

}


/**
*@brief   Function to read the value of V-object & T-ambient registers one time
*@details 1.Initially the function enables continuous conversion mode
*         2.So TMP006 starts conversion and data will be available in V-object & T-ambient registers,
*            these two 16 bit data are read and combined together to form a 32 bit value
*         3.Switches TMP006 back to power down mode for power considerations
*         4.Returns the 32 bit value 
*/

uint32_t TMP006_get_onetime_data()
{
    uint16_t ambient_reg_val = 0,vobj_reg_val = 0;
    uint32_t combined_val = 0;

    if (false == TMP006_enable_continuous_conversion()) return 0;

    nrf_delay_ms(1100);
    ambient_reg_val =   TMP006_read_register (TMP006_T_AMBIENT_REG);
    /* bits 0 and 1 of T-ambient register is '0' by default so they have to be ignored */
    ambient_reg_val =  ((ambient_reg_val >> 2) & 0x3FFF);

    vobj_reg_val =   TMP006_read_register (TMP006_V_OBJ_REG);
    /* 31-16 bits of temporary_variable3 represents T-ambient register and 15-0 bits represents V-object register */
    combined_val = ((ambient_reg_val << 16) | vobj_reg_val);

    if (false == TMP006_enable_powerdown_mode()) return 0;

    return(combined_val);

}

/**
*@brief   Function to read the value of V-object register one time
*@details 1.Initially the function enables continuous conversion mode
*         2.So TMP006 starts conversion and data will be available in V-object register,       
*         3.Switches TMP006 back to power down mode for power considerations
*         4.Returns the 16 bit value 
*/
uint16_t TMP006_get_v_object_data()
{
    uint16_t vobj_reg_val = 0;

    if (false == TMP006_enable_continuous_conversion()) return 0;

    vobj_reg_val =   TMP006_read_register (TMP006_V_OBJ_REG);

    if (false == TMP006_enable_powerdown_mode()) return 0;

    return(vobj_reg_val);
}

/**
*@brief   Function to read the value of T-ambient register one time
*@details 1.Initially the function enables continuous conversion mode
*         2.So TMP006 starts conversion and data will be available in T-ambient register,       
*         3.Switches TMP006 back to power down mode for power considerations
*         4.Returns the 16 bit value 
*/
uint16_t TMP006_get_t_ambient_data()
{
    uint16_t ambient_reg_val = 0;

    if (false == TMP006_enable_continuous_conversion()) return 0;

    ambient_reg_val =   TMP006_read_register (TMP006_T_AMBIENT_REG);
    /* bits 0 and 1 of T-ambient register is '0' by default so they have to be ignored */
    ambient_reg_val =  ((ambient_reg_val >> 2) & 0x3FFF);

    if (false == TMP006_enable_powerdown_mode()) return 0;

    return(ambient_reg_val);

}


// Read raw sensor temperature
int16_t readRawDieTemperature()
{
    int16_t raw = TMP006_read_register(TMP006_T_AMBIENT_REG);

    raw >>= 2;
    return raw;
}

// Read raw thermopile voltage
int16_t readRawVoltage()
{
    int16_t raw = TMP006_read_register(TMP006_V_OBJ_REG);
    return raw;
}

// Calculate object temperature based on raw sensor temp and thermopile voltage
float TMP006_ObjTempC_conversion() 
{
    static float previous_val = 0;
    if (false == TMP006_enable_continuous_conversion()) 
    {	
        return previous_val;
    }
    nrf_delay_ms(1100);

    volatile float Tdie = readRawDieTemperature();
    volatile float Vobj = readRawVoltage();
    Vobj *= 156.25; // 156.25 nV per LSB
    Vobj /= 1000000000; // nV -> V
    Tdie *= 0.03125; // convert to celsius
    Tdie += 273.15; // convert to kelvin

    // Equations for calculating temperature found in section 5.1 in the user guide
    volatile float tdie_tref = Tdie - TMP006_TREF;
    volatile float S = (1 + (TMP006_A1*tdie_tref) + (TMP006_A2*tdie_tref*tdie_tref));
    S *= TMP006_S0;
    S /= 10000000;
    S /= 10000000;

    volatile float Vos = (TMP006_B0 + (TMP006_B1*tdie_tref) +(TMP006_B2*tdie_tref*tdie_tref));

    volatile float fVobj = (Vobj - Vos) + (TMP006_C2*(Vobj-Vos)*(Vobj-Vos));

    volatile float Tobj = sqrt(sqrt((Tdie * Tdie * Tdie * Tdie) + (fVobj/S)));

    Tobj -= 273.15; // Kelvin -> *C

    if (false == TMP006_enable_powerdown_mode())
    {	
        return previous_val;
    }

    previous_val= Tobj;

    return Tobj;

}

// Caculate sensor temperature based on raw reading
float TMP006_readDieTempC(void) 
{
    float Tdie = TMP006_get_t_ambient_data();
    Tdie *= 0.03125; // convert to Celsius
    return Tdie;
}
/*-----------------------------------------------------------------------------
*  Function:	convert
*  Brief:		converting int value to char
*  Parameter:	None
*  Return:		None
-----------------------------------------------------------------------------*/
void convert (unsigned long int value, unsigned char * dest)
{
    unsigned char digit = 0;
    unsigned char flag = 0;

    while (value>= 1000000000)
    {
        digit++;
        value -=1000000000;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value>= 100000000)
    {
        digit++;
        value -=100000000;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value>= 10000000)
    {
        digit++;
        value -=10000000;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value>= 1000000)
    {
        digit++;
        value -=1000000;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value>= 100000)
    {
        digit++;
        value -=100000;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value>= 10000)
    {
        digit++;
        value -=10000;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value>= 1000)
    {
        digit++;
        value -=1000;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value >= 100)
    {
        digit++;
        value -=100;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    while (value >= 10)
    {
        digit++;
        value -=10;
    }
    if(digit || flag)
    {
        *dest = digit+48;
        digit=0;
        dest++;
        flag=1;
    }
    *dest= value+48;
    dest++;
    *dest=0;
    return;
}


/*-----------------------------------------------------------------------------
*  Function:	float_to_str
*  Brief:		converting float value to character
*  Parameter:	None
*  Return:		None
-----------------------------------------------------------------------------*/
void float_to_str(double source , char *dest)
{
    unsigned char int_part[8],dec_part[8];
    int value,i,j;
    value = (int)source;
    convert(value,int_part);
    for(i=0;int_part[i]!='\0';i++)
    {
        *dest = int_part[i];
        dest++;
    }
    *dest = '.';
    dest++;
    source = (source - (double)value)*1000;
    value = (int)source;
    convert(value,dec_part);
    for(j=0,i=i+1;dec_part[j]!='\0';j++,i++)
    {
        *dest = dec_part[j];
        dest++;
    }
    *dest ='\0';
    return;
}


void do_thermopile_measurement(uint8_t *temp_str, float *fTemp)
{
    static float obj_temp;
    obj_temp= TMP006_ObjTempC_conversion();
    *fTemp = obj_temp;
    float_to_str(obj_temp,(char *)temp_str);
    return;
}

/*-----------------------------------------------------------------------------
*  Function:	stof
*  Brief:		converting character value to float
*  Parameter:	char pointer
*  Return:		float
-----------------------------------------------------------------------------*/
float stof(char* s)
{
    float rez = 0, fact = 1;
    if (*s == '-'){
        s++;
        fact = -1;
    };
    for (int point_seen = 0; *s; s++){
        if (*s == '.'){
            point_seen = 1; 
            continue;
        };
        int d = *s - '0';
        if (d >= 0 && d <= 9){
            if (point_seen) fact /= 10.0f;
            rez = rez * 10.0f + (float)d;
        };
    };
    return rez * fact;
}


