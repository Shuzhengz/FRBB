#include "sys.h"
#include "usart.h"		
#include "delay.h"
#include "beep.h" 

int main(void)
{
	u8 t=0; 
	Stm32_Clock_Init(9);
	delay_init(72);
	uart_init(72,115200);
	BEEP_Init();
  	while(1)
	{
		printf("t:%d\r\n",t);
		delay_ms(500);
		t++;
		BEEP=0;
		delay_ms(300);
		BEEP=1;
		delay_ms(300);
	}
} 
