#include "main.h"

#define CHECK_FLASH 0
#define BYTE_FROM_FLASH(addr) (*((char *)addr))

void check_summ(void)
{
	int i;
	int cs;
	int cs_rec;

//Разрешили тактирование на PORTA и PORTB
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

	BAT_ENABLE_PORT->OTYPER = 0;

	BAT_ENABLE_PORT->MODER |= ((BAT_ENABLE_PIN)*(BAT_ENABLE_PIN));
	BAT_ENABLE_PORT->OTYPER |= BAT_ENABLE_PIN;
	BAT_ENABLE_PORT->BSRR=(BAT_ENABLE_PIN << 16);

//Напрямую инициализировали LED_ERROR и LED_OK на вывод			
	LED_ERROR_PORT->MODER = 0;
	LED_ERROR_PORT->MODER |= ((LED_ERROR_PIN)*(LED_ERROR_PIN));
	LED_OK_PORT->MODER    |= ((LED_OK_PIN)*(LED_OK_PIN));

//Найдем контрольную сумму
	cs = 0;

	for(i=APPLICATION_ADDRESS; i<CHECKSUM_ADDRESS; i++)
		{
			cs=cs+BYTE_FROM_FLASH(i);
		}
//И сравним ее с записанной
	cs_rec=BYTE_FROM_FLASH(CHECKSUM_ADDRESS);
	cs_rec |= BYTE_FROM_FLASH(CHECKSUM_ADDRESS+1)<<8;
	cs_rec |= BYTE_FROM_FLASH(CHECKSUM_ADDRESS+2)<<16;
	cs_rec |= BYTE_FROM_FLASH(CHECKSUM_ADDRESS+3)<<24;

#if CHECK_FLASH != 0
	if(( cs != cs_rec )&&(cs_rec != 0x34333231))
		{

//Моргаем и не работаем
			while(1)
				{
					for(i=0; i<1000000; i++)
						{
							LED_ERROR_PORT->BSRR=LED_ERROR_PIN;
							LED_OK_PORT->BSRR=(LED_OK_PIN << 16);
						}

					for(i=0; i<1000000; i++)
						{
							LED_ERROR_PORT->BSRR=(LED_ERROR_PIN << 16);
							LED_OK_PORT->BSRR=LED_OK_PIN;
						}
				}
		}
#endif
}
