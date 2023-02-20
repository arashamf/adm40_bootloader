/*
Индикация:
Светодиод горит долго - идет перепрошивка
2 вспышки - найдено обеспечение той-же версии
3 вспышки - не FAT32 или еще какие-нибудь ошибки
4 вспышки - не найден файл ADM.BIN
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "gfxsimulator.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "usart.h"
#include "flash.h"
#include "main.h"

extern ApplicationTypeDef Appli_state;
FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
FIL MyFile;                   /* File object */
extern USBH_HandleTypeDef hUsbHostFS;

FILINFO fno;
char filename[50];
char ver_in_sd[9];

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);
int get_system_reset_cause(void);

/* USER CODE BEGIN PFP */
#include "ff_gen_drv.h"
/* USER CODE END PFP */

void check_summ(void);

unsigned int ReadFirmwareVersion(void)
{
	static uint32_t bytesread;
	
	if(f_open(&MyFile,filename, FA_READ) != FR_OK){f_close(&MyFile);return 0;	}

	if(f_lseek(&MyFile,0x10000)!= FR_OK){f_close(&MyFile);return 0;	}
	
	if(f_read(&MyFile,ver_in_sd,8,(void *)&bytesread)!= FR_OK){f_close(&MyFile);return 0;	}
		
	f_close(&MyFile);
		
	return 1;		
}

unsigned int scan_files (char* path )
{
    FRESULT res;
    DIR dir;
//~    UINT i;
    static FILINFO fno;
		char buf[200]; 

	
	if(f_mount(&USBDISKFatFs, (TCHAR const*)USBHPath, 0) != FR_OK)
	{					
		Error_Handler();
		return USB_NO_CONNECT;
	}

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */             
            } 
						else {                                        /* It is a file. */
							//sprintf(buf,"%s/%s\r\n", path, fno.fname);
							//UART4_SendString(buf);
						
							if((strstr(fno.fname,"ADM"))&&(strstr(fno.fname,"BIN")))
								{
									sprintf(buf,"Found file %s \r\n", fno.fname);
									memcpy(filename,fno.fname,sizeof(fno.fname));
									UART4_SendString(buf);
										
									if(ReadFirmwareVersion()){
										sprintf(buf,"Version of file %s \r\n", ver_in_sd);
										UART4_SendString(buf);
									}										
									ReadCurrentVersion();
									sprintf(buf,"Current Version %s \r\n", ver_in_flash);
									UART4_SendString(buf);
									
									//check current version and firmware version
									if(memcmp(ver_in_flash,ver_in_sd,8)==0)
									{
										return FOUND_SAME_FIRMWARE;
									}
									else
									{
										return FOUND_NEW_FIRMWARE;
									}
								}			 
            }
        }
        f_closedir(&dir);
    }	
		else {return USB_NO_CONNECT;}	
		
   return FILE_NOT_FOUND;
}

void ExitBootloader(void)
{
	USBH_Stop(&hUsbHostFS);	
	USB_POWER(0);
//Проверим целостность основной прошивки
	check_summ();
	Go_To_User_App();
}


void UpgradeFirmware(void)
{
	char buf[512];
//~	char tmpBuf[40];
//~	unsigned int i=0,x=0,size=0,per=0,per_done=0;
	unsigned int i=0;
	unsigned int x=0;
	unsigned int offset=0;
	static uint32_t bytesread;
	FRESULT res;
			
	if(f_mount(&USBDISKFatFs, (TCHAR const*)USBHPath, 0) != FR_OK)
	{					
		Error_Handler();
	}
	else
	{	
		if(f_open(&MyFile, filename, FA_READ) == FR_OK)
       {
				 UART4_SendString("Found firmware file\r\n");			 		 			 		 				 
				 
				 EraseFlash();
				 				 
				 UART4_SendString("Start flash programming\r\n"); 					
				 HAL_FLASH_Unlock();	
				 
					for(i=0;i<2048;i++)
					{
						res = f_read(&MyFile, buf, sizeof(buf), (void *)&bytesread);						
						if(res!=FR_OK) {UART4_SendString("Error reading file\r\n");}							
						if(bytesread==0){break;}		
												
						for (x=0;x<bytesread;x++)
							{
								HAL_FLASH_Program(TYPEPROGRAM_BYTE, 0x08010000+offset, buf[x]);
								offset=offset+1;
							}		
					}										
					
				HAL_FLASH_Lock();					
				UART4_SendString("Finish flash programming\r\n"); 				 	 
			
				f_close(&MyFile);					
				LED_OK(0);
				NVIC_SystemReset();
			 }
	}	
}

//Моргнем сколько надо раз LED_ERROR(x)
void ind_LEDS(int n)
{
	HAL_Delay(300);
	while(n>0)
		{
			LED_ERROR(1);
			LED_OK(1);
			HAL_Delay(100);
			LED_ERROR(0);
			LED_OK(0);
			HAL_Delay(200);
			n--;
		}
}

int main(void)
{
	int timer=0;
	int res;
	int error_count=0;

  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

	if(get_system_reset_cause()==4){ ExitBootloader(); }	//Если произошел soft reset, то ничего не делаем с флешкой.

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	LED_OK(0);
	LED_ERROR(0);
  MX_FATFS_Init();
  MX_USB_HOST_Init();
//  MX_GFXSIMULATOR_Init();
	MX_UART4_Init();
	USB_POWER(1);
	
  /* Infinite loop */
	while (1)
		{
			MX_USB_HOST_Process();
		
			if(Appli_state==APPLICATION_START)
				{
					timer=0;		

					res=scan_files("0:/");	

					if(res==FILE_NOT_FOUND) 			{	USB_FLASH_FLAG(1); ind_LEDS(4); Appli_state=APPLICATION_IDLE; USB_FLASH_FLAG(0); NVIC_SystemReset();}
					if(res==USB_NO_CONNECT)				{ error_count++; }

					if(res==FOUND_SAME_FIRMWARE)	{	USB_FLASH_FLAG(1); LED_OK(1); UpgradeFirmware(); LED_OK(0); USB_FLASH_FLAG(0); }
					if(res==FOUND_NEW_FIRMWARE)		{	USB_FLASH_FLAG(1); LED_OK(1); UpgradeFirmware(); LED_OK(0); USB_FLASH_FLAG(0); }
					if(error_count>1000)					{	ind_LEDS(3); Appli_state=APPLICATION_IDLE; NVIC_SystemReset();}
				}				 
			else if(Appli_state==APPLICATION_DISCONNECT)
				{
					LED_OK(0); LED_ERROR(0);
				}
	
		timer++;
		if(timer>1000000)
			{
				ExitBootloader();
			}
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

//Определяем причину системного сброса
//0 - не было сброса
//1 - LOW POWER RESET
//2 - сброс по WWWD
//3 - сброс по IWDT
//4 - soft reset
//5 - POWER ON RESET
//6 - EXTERNAL PIN RESET
//7 - BROWNOUT RESET (BOR)
//8 - Неизвестная причина сброса
int get_system_reset_cause(void)
 {
		int reset_cause = 0;
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
		  {
        reset_cause = 1; //LOW POWER RESET
      }
		 else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
      {
        reset_cause = 2; //сброс по WWWD
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
      {
        reset_cause = 3; //сброс по IWDT 
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
      {
        reset_cause = 4; //soft reset по NVIC_SystemReset()
			}
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
      {
        reset_cause = 5; //POWER ON RESET
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
      {
        reset_cause = 6; //EXTERNAL PIN RESET
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
      {
        reset_cause = 7; //BROWNOUT RESET (BOR)
      }
    else
      {
        reset_cause = 8; //Неизвестная причина сброса
      }

    // Clear all the reset flags or else they will remain set during future resets until system power is fully removed.
		if(reset_cause != 3)
			{
				__HAL_RCC_CLEAR_RESET_FLAGS();
			}
			return reset_cause; 
 }

void Error_Handler(void)
{
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
}
#endif /* USE_FULL_ASSERT */
