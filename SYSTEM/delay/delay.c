#include "delay.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"
#endif

static u8  fac_us=0;
static u16 fac_ms=0;
	
	
#if SYSTEM_SUPPORT_OS

#ifdef 	OS_CRITICAL_METHOD
#define delay_osrunning		OSRunning
#define delay_ostickspersec	OS_TICKS_PER_SEC
#define delay_osintnesting 	OSIntNesting
#endif

//֧��UCOSIII
#ifdef 	CPU_CFG_CRITICAL_METHOD
#define delay_osrunning		OSRunning
#define delay_ostickspersec	OSCfg_TickRate_Hz
#define delay_osintnesting 	OSIntNestingCtr
#endif


void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err; 
	OSSchedLock(&err);
#else
	OSSchedLock();
#endif
}

void delay_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err; 
	OSSchedUnlock(&err);
#else
	OSSchedUnlock();
#endif
}

void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err; 
	OSTimeDly(ticks,OS_OPT_TIME_PERIODIC,&err);
#else
	OSTimeDly(ticks);
#endif 
}

void SysTick_Handler(void)
{	
	if(delay_osrunning==1)
	{
		OSIntEnter();
		OSTimeTick();
		OSIntExit();
	}
}
#endif
			   
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS
	u32 reload;
#endif
 	SysTick->CTRL&=~(1<<2);
	fac_us=SYSCLK/8;
#if SYSTEM_SUPPORT_OS
	reload=SYSCLK/8;
	reload*=1000000/delay_ostickspersec;

	fac_ms=1000/delay_ostickspersec;
	SysTick->CTRL|=1<<1;
	SysTick->LOAD=reload;
	SysTick->CTRL|=1<<0;
#else
	fac_ms=(u16)fac_us*1000;
#endif
}								    

#if SYSTEM_SUPPORT_OS
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;
	ticks=nus*fac_us;
	delay_osschedlock();
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;
		}  
	};
	delay_osschedunlock();
}

void delay_ms(u16 nms)
{	
	if(delay_osrunning&&delay_osintnesting==0)
	{		 
		if(nms>=fac_ms)
		{ 
   			delay_ostimedly(nms/fac_ms);
		}
		nms%=fac_ms;
	}
	delay_us((u32)(nms*1000));
}
#else
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us;
	SysTick->VAL=0x00;
	SysTick->CTRL=0x01;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL=0x00;
	SysTick->VAL =0X00;
}

void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;
	SysTick->VAL =0x00;
	SysTick->CTRL=0x01;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL=0x00;
	SysTick->VAL =0X00;
#endif 




























