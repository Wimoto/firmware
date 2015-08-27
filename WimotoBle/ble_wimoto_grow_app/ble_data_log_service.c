/** @file
*  @brief Data logger Service file.
*
* This file contains the source code for datalogger service
* Author : Shafy
* Date   : 11/16/2013
* Change log:
* Sherin           12/10/2013     Added write events for value fields
* Hariprasad       12/11/2013     Added 128bit Vendor specific  custom UUID's for the service and all characteristics 
* Shafy S          12/19/2013     Added a tx complete event check in reset_data_log() to avoid tx buffer overflow
* sruthi.k.s       01/10/2014     migrated to soft device 7.0.0 and SDK 6.1.0
* Shafy            11/10/2014     Changes in handling flash completion event and in reset data log function
*/

#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "wimoto.h"
#include "nrf_soc.h"
#include "ble.h"
#include "ble_flash.h"
#include "ble_data_log_service.h"
#include "app_error.h"

extern bool         ENABLE_DATA_LOG;              /* Flag to enable data logger */
extern bool					ENABLE_DLOG_TIMER;						/* Flag to start the data logger timer */
extern bool         READ_DATA;                    /* flag to start reading data from flash*/
extern bool         START_DATA_READ;              /* flag to start data logging*/
extern bool         TX_COMPLETE;                  /* flag to indicate transmission complete*/

extern volatile bool m_radio_event;               /* TRUE if radio is active (or about to become active), FALSE otherwise. */

uint32_t            read_pg;                      /* flash page number of the cyclic buffer from which read operation should be done*/
uint32_t            write_pg;                     /* flash page number to which data is being written*/

static uint32_t *write_addr;                      /* write_address of the word to which data is being written*/
static uint32_t pg_end;                           /* last page in the buffer*/ 

bool   DLOGS_CONNECTED_STATE=false;               /* Indicates whether the data logger service is connected or not*/

bool   done_read = false;                         /* flag to indicate whether data logger reading is over*/ 
bool   flash_comp_evt = false;			              /*flag to indicate flash operation completion event*/
extern ble_date_time_t m_time_stamp;              /* time stamp structure*/ 
extern uint8_t	 var_receive_uuid;  							/*variable for receiving uuid*/
extern uint32_t buf[4];														/*buffer for flash write operation*/
static bool reread = false;												/*flag to indicate whether or not a data log has to be resent*/
/**@brief Function for handling the Connect event.
*
* @param[in]   ble_dlogs     Data logger service structure.
* @param[in]   p_ble_evt     Event received from the BLE stack.
*/
static void on_connect(ble_dlogs_t * ble_dlogs, ble_evt_t * p_ble_evt)
{
    ble_dlogs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    DLOGS_CONNECTED_STATE = true;  /*Set the flag to true so that state remains in connectable mode until disconnect*/
}

/**@brief Function for handling the Disconnect event.
*
* @param[in]   ble_dlogs       Data logger service structure.
* @param[in]   p_ble_evt       Event received from the BLE stack.
*/
static void on_disconnect(ble_dlogs_t * ble_dlogs, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    DLOGS_CONNECTED_STATE= false; 
    ble_dlogs->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the write event.
*
* @param[in]   p_device    Data logger Service structure.
* @param[in]   p_evt       Data logger Service write event type.
*/

static void write_evt_handler (ble_dlogs_t * ble_dlogs, ble_dlogs_write_evt_t * p_evt)
{	  
    switch (p_evt->evt_type)
    {
    case BLE_DLOGS_ENABLE_WRITE:
        if(ble_dlogs->data_logger_enable != 0x00)				/*if the user has set the data logger enable characteristics set the flag to true*/
        {
            ENABLE_DATA_LOG =true;
						ENABLE_DLOG_TIMER = true; 									/*Set flag to initiate the data log timer*/
        }
        else
        {
            ENABLE_DATA_LOG =false;
        }
        break;
        
    case BLE_DLOGS_READ_SWITCH_WRITE:
        if(ble_dlogs->read_data_switch != 0x00)				  /*if the user has set the data logger enable characteristics set the flag to true*/
        {
            READ_DATA =true;
        }
        else
        {
            READ_DATA = false;
        }
        break;   
        
    default:
        break;
    }
}

/**@brief Function for handling the Write event.
*
* @param[in]   ble_dlogs       Data logger service structure.
* @param[in]   p_ble_evt       Event received from the BLE stack.
*/
static void on_write(ble_dlogs_t * ble_dlogs, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (ble_dlogs->is_notification_supported)
    {
        //write event for data logger enable char cccd.   
        if (
                (p_evt_write->handle == ble_dlogs->data_logger_enable_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (ble_dlogs->evt_handler != NULL)
            {
                ble_dlogs_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_DLOGS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_DLOGS_EVT_NOTIFICATION_DISABLED;
                }

                ble_dlogs->evt_handler(ble_dlogs, &evt);
            }
            
        }
        
        //write event for data char cccd. 
        
        if (
                (p_evt_write->handle == ble_dlogs->data_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (ble_dlogs->evt_handler != NULL)
            {
                ble_dlogs_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_DLOGS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_DLOGS_EVT_NOTIFICATION_DISABLED;
                }

                ble_dlogs->evt_handler(ble_dlogs, &evt);
            }
        }
        
        //write event for  read data switch char cccd
        
        if (
                (p_evt_write->handle == ble_dlogs->read_data_handles.cccd_handle)
                &&
                (p_evt_write->len == 2)
                )
        {
            // CCCD written, call application event handler
            if (ble_dlogs->evt_handler != NULL)
            {
                ble_dlogs_evt_t evt;
                
                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_DLOGS_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_DLOGS_EVT_NOTIFICATION_DISABLED;
                }

                ble_dlogs->evt_handler(ble_dlogs, &evt);
            }
        } 
    }
    
    /*Write event for data logger enable char value*/
    
    if (
            (p_evt_write->handle == ble_dlogs->data_logger_enable_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            &&
            (ble_dlogs->write_evt_handler != NULL)
            )
    {  
        ble_dlogs_write_evt_t evt;
        evt.evt_type           = BLE_DLOGS_ENABLE_WRITE;
        
        // update the service stucture
        ble_dlogs->data_logger_enable =   p_evt_write->data[0];
        
        // call application event handler
        ble_dlogs->write_evt_handler(ble_dlogs, &evt);
    }

    /*Write event for data logger read char value*/
    
    if (
            (p_evt_write->handle == ble_dlogs->read_data_handles.value_handle) 
            && 
            (p_evt_write->len == 1)
            &&
            (ble_dlogs->write_evt_handler != NULL)
            )
    {  
        ble_dlogs_write_evt_t evt;
        evt.evt_type           = BLE_DLOGS_READ_SWITCH_WRITE;
        
        // update the service structure
        ble_dlogs->read_data_switch =   p_evt_write->data[0];
        
        // call application event handler
        ble_dlogs->write_evt_handler(ble_dlogs, &evt);
    }		
}

void ble_dlogs_on_ble_evt(ble_dlogs_t * ble_dlogs, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(ble_dlogs, p_ble_evt);
        break;
        
    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(ble_dlogs, p_ble_evt);
        break;
        
    case BLE_GATTS_EVT_WRITE:
        on_write(ble_dlogs, p_ble_evt);
        break;
        
    default:
        break;
    }
    
}

/**@brief Function for adding the data logger enable characteristics.
*
* @param[in]   ble_dlogs        Data logger service structure.
* @param[in]   ble_dlogs_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t data_logger_enable_char_add(ble_dlogs_t * ble_dlogs, const ble_dlogs_init_t * ble_dlogs_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      data_logger_enable = 0x00;
    
    if (ble_dlogs->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = ble_dlogs_init->dlogs_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read         	= 1;
		char_md.char_props.write					=	1;                            /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                            /*add fix for characteristic write issue*/
    char_md.char_props.notify       	= (ble_dlogs->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf               	= NULL;
    char_md.p_user_desc_md          	= NULL;
		char_md.p_cccd_md               	= (ble_dlogs->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md              		= NULL;

    // Adding custom UUID
    ble_uuid.type = ble_dlogs->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_DLOGS_DLOGS_EN_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = ble_dlogs_init->dlogs_char_attr_md.read_perm;
    attr_md.write_perm = ble_dlogs_init->dlogs_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(data_logger_enable);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(data_logger_enable);
    attr_char_value.p_value      = &data_logger_enable;

    err_code = sd_ble_gatts_characteristic_add(ble_dlogs->service_handle, &char_md,
    &attr_char_value,
    &ble_dlogs->data_logger_enable_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    return NRF_SUCCESS;
    
}

/**@brief Function for adding the data characteristics.
*
* @param[in]   ble_dlogs        Data logger service structure.
* @param[in]   ble_dlogs_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t data_char_add(ble_dlogs_t * ble_dlogs, const ble_dlogs_init_t * ble_dlogs_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      data[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    if (ble_dlogs->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = ble_dlogs_init->dlogs_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
    char_md.char_props.notify   = (ble_dlogs->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = (ble_dlogs->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md           = NULL;

    // Adding custom UUID
    ble_uuid.type = ble_dlogs->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_DLOGS_DATA_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = ble_dlogs_init->dlogs_char_attr_md.read_perm;
    attr_md.write_perm = ble_dlogs_init->dlogs_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(data);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(data);
    attr_char_value.p_value      = data;

    err_code = sd_ble_gatts_characteristic_add(ble_dlogs->service_handle, &char_md,
    &attr_char_value,
    &ble_dlogs->data_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the read data switch characteristics.
*
* @param[in]   ble_dlogs        Data logger service structure.
* @param[in]   ble_dlogs_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

static uint32_t read_data_switch_char_add(ble_dlogs_t * ble_dlogs, const ble_dlogs_init_t * ble_dlogs_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t      read_data_switch   = 0x00; 

    // Add read data characteristic
    if (ble_dlogs->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = ble_dlogs_init->dlogs_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     			= 1;
		char_md.char_props.write					=	1;                                /*add fix for characteristic write issue*/
		char_md.char_props.write_wo_resp 	= 1;                                /*add fix for characteristic write issue*/
    char_md.char_props.notify   			= (ble_dlogs->is_notification_supported) ? 1 : 0;
    char_md.p_char_pf           			= NULL;
    char_md.p_user_desc_md      			= NULL;
    char_md.p_cccd_md           			= (ble_dlogs->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md          	 			= NULL;

    // Adding custom UUID
    ble_uuid.type = ble_dlogs->uuid_type;
    ble_uuid.uuid = GROW_PROFILE_DLOGS_READ_DATA_UUID;    

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = ble_dlogs_init->dlogs_char_attr_md.read_perm;
    attr_md.write_perm = ble_dlogs_init->dlogs_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(read_data_switch);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(read_data_switch);
    attr_char_value.p_value      = &read_data_switch;

    err_code = sd_ble_gatts_characteristic_add(ble_dlogs->service_handle, &char_md,
    &attr_char_value,
    &ble_dlogs->read_data_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for initializing the data logger service.
*
* @param[in]   ble_dlogs        Data logger service structure.
* @param[in]   ble_dlogs_init   Information needed to initialize the service.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t ble_dlogs_init(ble_dlogs_t * ble_dlogs, const ble_dlogs_init_t * ble_dlogs_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Add service
    ble_uuid.type = var_receive_uuid ;
		ble_dlogs->uuid_type= var_receive_uuid;
    ble_uuid.uuid = GROW_PROFILE_DLOGS_SERVICE_UUID;

    // Initialize service structure
    ble_dlogs->evt_handler               = ble_dlogs_init->evt_handler;
    ble_dlogs->write_evt_handler         = write_evt_handler;
    ble_dlogs->conn_handle               = BLE_CONN_HANDLE_INVALID;
    ble_dlogs->is_notification_supported = ble_dlogs_init->support_notification;
    ble_dlogs->data_logger_enable        = ble_dlogs_init->data_logger_enable;
    ble_dlogs->read_data_switch  	       = ble_dlogs_init->read_data_switch;  

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_dlogs->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code =  data_logger_enable_char_add(ble_dlogs, ble_dlogs_init); /* Add data logger enable characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code =  data_char_add(ble_dlogs, ble_dlogs_init);               /* Add data characteristic*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code =  read_data_switch_char_add(ble_dlogs, ble_dlogs_init);  /* Add read data characteristics for downloading data*/
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    return NRF_SUCCESS;
    
}

/**@brief Function write sensor data to flash.
*
* @param[in]   data             Data buffer.
* 
*/
void write_data_flash(uint32_t *data)
{		
		static uint32_t i=0;
    static uint32_t pg_size;         												 /*size of a page*/
    static bool first_write=true;    												 /*flag indicates whether a write is done for the first time in the flash*/
    static unsigned char write_cycle=0;
		buf[0]=*data;
		buf[1]=*(data+1);
		buf[2]=*(data+2);
		buf[3]=*(data+3);
		if(first_write)                                         /* for the first write cycle set the start address and erase the page*/
    {
        pg_size   = NRF_FICR->CODEPAGESIZE;
        write_pg  = DATA_LOGGER_BUFFER_START_PAGE;          /* th e first page to be written for logging data*/
        pg_end  	= DATA_LOGGER_BUFFER_END_PAGE;						/* the last page for writing data*/
        read_pg 	= DATA_LOGGER_BUFFER_START_PAGE; 
        write_addr = (uint32_t *)(pg_size * write_pg);
				
				flash_comp_evt = false;
				sd_flash_page_erase(write_pg);
				
				while(flash_comp_evt!=true)	                       /*wait for the completeion of flash erase*/
				{
							sd_app_evt_wait();
				}
        first_write = false;
    }
    
    if(write_pg <= pg_end)								
    {		
        
        if(i < pg_size)                                    /* stay in same page if the page size(1024 bytes) is not exceeded*/
        {		
						flash_comp_evt = false;
						sd_flash_write(write_addr,buf,4);      /* write four words*/
						while(flash_comp_evt!=true)
						{
								sd_app_evt_wait();
						}
						i += 16;                                       /* for each block write 16 bytes are written to flash*/
						write_addr+=4;                                 /* increment the address by four words*/
				}
        else if((++write_pg) <= pg_end)                    /* increment the page number when the current page size is exceeded*/
        {
						flash_comp_evt = false;
            sd_flash_page_erase(write_pg);         /* Erase the page before writing*/
						while(flash_comp_evt!=true)	                  /*wait for the completeion of flash erase*/
						{
								sd_app_evt_wait();
								
						}
            write_addr = (uint32_t *)(pg_size * write_pg);
						flash_comp_evt = false;
            sd_flash_write(write_addr,buf,4);
						while(flash_comp_evt != true)
						{
								sd_app_evt_wait();
							
						}
						i = 16;															
						write_addr+=4;
            if((write_cycle!=0x00) & (write_pg != pg_end))  /* If cyclic buffer has been written fully atleast once and the last page is not reached, the page to be read next is next page after the current write page */
            {
                read_pg = write_pg + 1;
            }
            else                                           /* If write is still in cycle 0, the read page the buffer start page*/
            {
                read_pg = DATA_LOGGER_BUFFER_START_PAGE;
            }
        }
        
    }
    if(write_pg > pg_end)
    {
        write_pg  = DATA_LOGGER_BUFFER_START_PAGE;        /* when the last page is reached, go back to the first page*/
        read_pg = write_pg + 1;
				flash_comp_evt = false;
        sd_flash_page_erase(write_pg);
				while(flash_comp_evt!=true)
				{
						sd_app_evt_wait();
				}
				
        write_addr = (uint32_t *)(pg_size * write_pg);
				flash_comp_evt = false;
        sd_flash_write(write_addr,buf,4);
				while(flash_comp_evt != true)
				{
						sd_app_evt_wait();
				}
				i = 16;															
				write_addr+=4;      
				write_cycle=0x01;                               /* if write operation reached the end of the buffer, change the cycle to 1*/
         
    }	
}

/**@brief Function to send data to the connected BLE central device.
*
* @param[in]   ble_dlogs        Data logger service structure.
* 
*/
void send_data(ble_dlogs_t * ble_dlogs)
{
    uint32_t err_code;
    static send_state state=READ; 
    bool exit_loop=false;
    uint32_t data[4]={0x00,0x00,0x00,0x00};                             /* array to read data from the flash*/


    while(true)
    {
        switch(state)
        {
        case READ:
            read_data_flash(ble_dlogs,data);														/* read data from the flash*/
            if(done_read)                                               /* If all the data has been read set the next state to read complete*/
            {
                state=READ_COMPLETE;
								break;
            }
						
						err_code=send_data_to_central(ble_dlogs,data);              /* trasmit data to the connected central device*/
						
            if(err_code == BLE_ERROR_NO_TX_BUFFERS)
						{
							reread = true;
							state  = TXMIT;
							break;
						}else if (err_code != NRF_SUCCESS)
						{
							APP_ERROR_HANDLER(err_code);
						}
            
            break;

        case TXMIT: 																										/* Enter this case when TX Buffer has become full */	
            
            while(!TX_COMPLETE)                                        	/* Wait for TX complete event*/
            {
                uint32_t err_code = sd_app_evt_wait();
                APP_ERROR_CHECK(err_code);
            }
            state=READ;                                                	/* Set next state to READ*/															
            break;

        case READ_COMPLETE:                                            	/* If the read is completed, exit the loop*/			
            
            exit_loop=true;
            state=READ;
            break;

        default:
            break;
        }

        if(exit_loop)
        {
            break;
        }
    }
}

/**@brief Function readin data to flash and sending to the connected BLE central device.
*
* @param[in]   ble_dlogs        Data logger service structure.
* @param[in]   data             Data buffer.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t read_data_flash(ble_dlogs_t * ble_dlogs, uint32_t * data)
{
    static uint32_t pg_size;
    static uint32_t *read_addr;
    static uint32_t *buffer_end_addr;
		static uint32_t *prev_addr;
		static uint32_t prev_read_page;
    static bool first_read=true;
    int i;
    pg_size = NRF_FICR->CODEPAGESIZE;
    buffer_end_addr = (uint32_t *)(pg_size * (pg_end + 1));
	
		if(reread == true)											/*Check if there was a TX FULL error and data needs to be resent*/																						
		{
			read_addr = prev_addr;								/*Roll back the read address and read page to what they were previously*/
			read_pg = prev_read_page;
			reread = false;
		}
		
		prev_addr = read_addr;									/*Store current address and page for future use incase of TX full error*/
		prev_read_page = read_pg;

    if(first_read)
    {																				/*in the first read operation, set the address to be read as the first word of read page set by the write routine. i.e the oldest data*/	
        read_addr = (uint32_t *)(pg_size * read_pg);	
        first_read=false;
    }

    if(read_pg <= write_pg)
    {
        if(read_addr < write_addr)					/*if read page is below write page, send data up-to the last written address*/
        {
            for(i=0;i<4;i++)
            {
                *data = *read_addr;
                read_addr++;
                data++;
            }
        }
        else if(read_addr == write_addr)    /*If the read pointer has reached the current position of write pointer, set done_read*/
        {
            done_read=true;
        }
    }
    else
    {																		  	/*if read page is above write page, read up-to the last memory location*/
        if(read_addr <  buffer_end_addr)
        {
            for(i=0;i<4;i++)
            {
                *data = *read_addr;
                read_addr++;
                data++;
            }
        }
        else
        {																	/*After reading till the last memory, start reading from the first page*/				
            read_pg   = DATA_LOGGER_BUFFER_START_PAGE;
            read_addr = (uint32_t *)(pg_size * read_pg);
            for(i=0;i<4;i++)
            {
                *data = *read_addr;
                read_addr++;
                data++;
            }
        }
    }
    
    return NRF_SUCCESS;
}

/**@brief Function to send the data to the connected central device.
*
* @param[in]   ble_dlogs  Data logger service structure.
* @param[in]   data       Data buffer.
* @return      NRF_SUCCESS on success, otherwise an error code.
*/

uint32_t send_data_to_central(ble_dlogs_t * ble_dlogs,uint32_t *data)
{
    uint8_t  buffer[16];
    uint16_t len = sizeof(buffer);
    uint32_t err_code;

    buffer[0] =  (data[0]&0xFF000000)>>24;				/*pack the data and time stamp to uint8 array*/
    buffer[1]  = (data[0]&0x00FF0000)>>16;
    buffer[2]  = (data[0]&0x0000FF00)>>8;
    buffer[3]  = (data[0]&0x000000FF);

    buffer[4]   = (data[1]&0xFF000000)>>24;
    buffer[5]   = (data[1]&0x00FF0000)>>16;
    buffer[6]   = (data[1]&0x0000FF00)>>8;
    buffer[7]   = (data[1]&0x000000FF);

    buffer[8]   = (data[2]&0xFF000000)>>24;
    buffer[9]   = (data[2]&0x00FF0000)>>16;
    buffer[10]  = (data[2]&0x0000FF00)>>8;
    buffer[11]  = (data[2]&0x000000FF);

    buffer[12]  = (data[3]&0xFF000000)>>24;
    buffer[13]  = (data[3]&0x00FF0000)>>16;
    buffer[14]  = (data[3]&0x0000FF00)>>8;
    buffer[15]  = (data[3]&0x000000FF);

    // Send the updated value of data if connected and notifying		
    if ((ble_dlogs->conn_handle != BLE_CONN_HANDLE_INVALID) && ble_dlogs->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;
        
        memset(&hvx_params, 0, sizeof(hvx_params));
        
        
        hvx_params.handle   = ble_dlogs->data_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len;
        hvx_params.p_data   = buffer;
        
        err_code = sd_ble_gatts_hvx(ble_dlogs->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
        
    }
    TX_COMPLETE=false;
    return NRF_SUCCESS;
}


/**@brief Function to reset the datalogger enable and read data switch characteristics.
*
* @param[in]   ble_dlogs        Data logger service structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t reset_data_log(ble_dlogs_t * ble_dlogs)		 
{
    uint32_t err_code;
    uint16_t len = sizeof(uint8_t);

    uint8_t data_logger_enable=0x00;
    uint8_t read_data_switch=0x00;
    
    // Update the service structure
    ble_dlogs->data_logger_enable =data_logger_enable;
    ble_dlogs->read_data_switch   =read_data_switch;	
    
    

    // Send the updated value of data logger enable if connected and notifying
    if ((ble_dlogs->conn_handle != BLE_CONN_HANDLE_INVALID) && ble_dlogs->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;
        
        memset(&hvx_params, 0, sizeof(hvx_params));
        
        
        hvx_params.handle   = ble_dlogs->data_logger_enable_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len;
        hvx_params.p_data   = &data_logger_enable;
        
				TX_COMPLETE = false;																	/*Set the TX Complete flag to false before updating characteristics value*/
        while(1)
				{
        err_code = sd_ble_gatts_hvx(ble_dlogs->conn_handle, &hvx_params);									/*Make sure TX BUFFERS are not full*/
				if (err_code != BLE_ERROR_NO_TX_BUFFERS)
					{
						break;
					}
				}
				
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }
    if ((err_code != NRF_SUCCESS) &&															 
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
        )
    {
        return err_code;
        
    }
    
    // Send the updated value of read data switch if connected and notifying						
    if ((ble_dlogs->conn_handle != BLE_CONN_HANDLE_INVALID) && ble_dlogs->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;
        
        memset(&hvx_params, 0, sizeof(hvx_params));
        
        
        hvx_params.handle   = ble_dlogs->read_data_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len;
        hvx_params.p_data   = &read_data_switch;
        
        while(1)
				{
        err_code = sd_ble_gatts_hvx(ble_dlogs->conn_handle, &hvx_params);
				if (err_code != BLE_ERROR_NO_TX_BUFFERS)																					/*Make sure that the TX BUFFERS are not full*/
					{	
						break;
					}
				}
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }
    if ((err_code != NRF_SUCCESS) &&															 
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != BLE_ERROR_NO_TX_BUFFERS) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
        )
    {
        return err_code;
        
    }
		sd_ble_gatts_value_set(ble_dlogs->data_logger_enable_handles.value_handle, 0, &len, 0);  /*clear the value of data logger enable characteristics*/
		sd_ble_gatts_value_set(ble_dlogs->read_data_handles.value_handle, 0, &len, 0);  /*clear the value of read data enable characteristics*/
    done_read=false;
    return NRF_SUCCESS;
    
}
/**
 * @brief Handles Flash Access Result Events 
 *
 * @param[in] sys_evt System event to be handled.
 */
void data_log_sys_event_handler(uint32_t sys_evt)
{
		if (sys_evt == NRF_EVT_FLASH_OPERATION_SUCCESS)
		{ 
				flash_comp_evt = true;
		}
} 
