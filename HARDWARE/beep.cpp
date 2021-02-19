#include "beep.h"

void BEEP_Init(void)
{
	RCC->APB2ENR|=1<<3;
	GPIOB->CRH&=0XFFFFFFF0;
	GPIOB->CRH|=0X00000003;
	BEEP=0;
}
