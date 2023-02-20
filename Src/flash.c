#include "flash.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usart.h"

char ver_in_flash[8];

void EraseFlash(void)
{
//	int i=0,per=0;
	
	UART4_SendString("Start flash erasing\r\n"); 
	
	__disable_irq();
	HAL_FLASH_Unlock();	
	
	/*for(i=4;i<10;i++)
	{
	}	*/
	
	FLASH_Erase_Sector(FLASH_SECTOR_4, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART4_SendString("FLASH_SECTOR_4 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_5, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART4_SendString("FLASH_SECTOR_5 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_6, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART4_SendString("FLASH_SECTOR_6 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_7, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART4_SendString("FLASH_SECTOR_7 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_8, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);	
	//UART4_SendString("FLASH_SECTOR_8 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_9, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART4_SendString("FLASH_SECTOR_9 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_10, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART4_SendString("FLASH_SECTOR_10 erased\r\n");
	UART4_SendString("Finish flash erasing\r\n");
	
	HAL_FLASH_Lock();
	__enable_irq();
	
}

void ReadCurrentVersion (void)
{
	unsigned int * src;
	unsigned int word=0,word2=0;
	
	src  = (unsigned int *)VERSION_ADDRESS;
	word=*src++;
	word2=*src;
	
	ver_in_flash[0]=word;
	ver_in_flash[1]=word>>8;
	ver_in_flash[2]=word>>16;
	ver_in_flash[3]=word>>24;
	
	ver_in_flash[4]=word2;
	ver_in_flash[5]=word2>>8;
	ver_in_flash[6]=word2>>16;
	ver_in_flash[7]=word2>>24;
}


void Go_To_User_App(void)
{
    uint32_t app_jump_address;
		
    typedef void(*pFunction)(void);
    pFunction Jump_To_Application;

     __disable_irq();
		
    app_jump_address = *( uint32_t*) (APPLICATION_ADDRESS + 4);    
    Jump_To_Application = (pFunction)app_jump_address;            
	
		SCB->VTOR =  APPLICATION_ADDRESS; 
	
		//   Initialize user application's Stack Pointer
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);                                                
    Jump_To_Application();		                       
}

