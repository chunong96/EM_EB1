/*********************1_HW6_2015134022_송진한*********************/
#include "stm32f4xx.h"

#define SW0_PUSH        0xFE00  //PH8
#define SW1_PUSH        0xFD00  //PH9
#define SW2_PUSH        0xFB00  //PH10
#define SW3_PUSH        0xF700  //PH11
#define SW4_PUSH        0xEF00  //PH12
#define SW5_PUSH        0xDF00  //PH13
#define SW6_PUSH        0xBF00  //PH14
#define SW7_PUSH        0x7F00  //PH15

/**************외부함수 선언***************************/
void _GPIO_Init(void);
uint16_t KEY_Scan(void);
void BEEP(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);


/**************전역변수 선언***************************/
uint16_t Coin_Flag = 0; //동전이 들어간 상태인지 구별하는 플래그 (동전이 있어야만 커피선택 가능)
uint16_t Make_Flag = 0; //자판기가 작동 상태인지 구별하는 플래그 (작동중엔 동전 삽입 불가)

int i=0;

/**************내부함수 선언***************************/
void Cup_Blink(int Cup_Signal); //컵 LED7 PG7 점멸함수
void Sugar_Blink(int Sugar_Signal); //설탕 LED6 PG6 점멸함수
void Cream_Blink(int Cream_Signal); //크림 LED5 PG5 점멸함수
void Water_Coffee_Blink(int Water_Signal); //물_커피 LED4 PG4 점멸함수

void Mix_Coffee_make(void);     //믹스커피 레시피 함수
void Sugar_Coffee_make(void);   //설탕커피 레시피 함수
void Black_Coffee_make(void);   //블랙커피 레시피 함수

void Finish_Beep(void);         //제작완료시 동작 함수



/********************************메인 함수******************************/
int main(void)
{
	///////메인함수 초기화/////////////////////////////////////////////////
	DelayMS(100);
	_GPIO_Init(); //GPIO인터럽트 초기화 //커스텀버전이라 _GPIO_Init()
	BEEP(); //GPIOF->ODR의 0x0200 레지스터 점멸

	GPIOG->ODR &= ~0x00FF;	// 초기값: LED0~7 OFF


	///////메인함수 무한루프/////////////////////////////////////////////////
	while(1)
	{
		switch(KEY_Scan()) //스위치의 진동에 의한 채터링Chattering 현상을 방지하기 위한 키스캔 함수
		{
			///CoinSW0/////////////////////////////////////////////////////
		case SW0_PUSH : 	//0xFE00 SW0   //0b 1111 1110 0000 0000
			if(Make_Flag == 0) //자판기가 동작중이 아닐 때
			{//BSRR은 특정 비트 1개만 바꿀 때 사용하는 레지스터이다. BRx는 강제로 0을 만든다. BSx는 강제로 1을 만든다.
				GPIOG->BSRRL = 0x0001;    //  LED0 점등
				//GPIOG->BSRRH = 0x0001;  //  LED0 소등

				BEEP();
				Coin_Flag = 1; //동전 삽입된 상태
			}
			break;


			///블랙커피 선택SW1/////////////////////////////////////////////////////
		case SW1_PUSH : 	//0xFD00 SW1   //0b 1111 1101 0000 0000                        
			if(Coin_Flag == 1) //동전 삽입된 상태여야만 동작
			{
				BEEP();
				GPIOG->ODR |= 0x0002;   //  LED1 점등
				Make_Flag = 1; //자판기 동작 중
				DelayMS(300);
				Black_Coffee_make();//블랙커피 레시피 함수
				Finish_Beep();
			}
			break;


			///설탕커피 선택SW2/////////////////////////////////////////////////////
		case SW2_PUSH : 	//0xFB00 SW2   //0b 1111 1011 0000 0000
			if(Coin_Flag == 1) //동전 삽입된 상태여야만 동작
			{
				BEEP();
				GPIOG->ODR |= 0x0004;   //  LED2 점등
				Make_Flag = 1; //자판기 동작 중
				DelayMS(300);
				Sugar_Coffee_make();//설탕커피 레시피 함수
				Finish_Beep();
			}
			break;

			///믹스커피 선택SW3/////////////////////////////////////////////////////
		case SW3_PUSH : 	//0xF700 SW3    //0b 1111 0111 0000 0000
			if(Coin_Flag == 1) //동전 삽입된 상태여야만 동작
			{
				BEEP();
				GPIOG->ODR |= 0x0008;   //  LED3 점등
				Make_Flag = 1; //자판기 동작 중
				DelayMS(300);
				Mix_Coffee_make();//믹스커피 레시피 함수
				Finish_Beep();
			}
			break;
		}
	}

}


/**************************HW6 사용자 지정함수들 시작*********************************/

/***************커피 출력 종료 함수*************/
void Finish_Beep(void)
{
	Coin_Flag = 0;
	GPIOG->ODR &= ~0x00FF;	//  LED0~7 소등

	for(i=0; i<3; i++)
	{
		BEEP();
		DelayMS(150);
	}

	DelayMS(300); //종료 후 1초
	Make_Flag = 0; //모든 동작 종료(대기상태)
}


/*****************커피제작 레시피 함수들*******************/
void Mix_Coffee_make(void)
{
	Cup_Blink(1); //종이컵 하나
	Sugar_Blink(2); //설탕 2펌핑
	Cream_Blink(2); //크림 2펌핑
	Water_Coffee_Blink(3); //물에 탄 커피 3펌핑
}

void Sugar_Coffee_make(void)
{
	Cup_Blink(1); //종이컵 하나 
	Sugar_Blink(2); //설탕 2펌핑
	Water_Coffee_Blink(3); //물에 탄 커피 3펌핑
}

void Black_Coffee_make(void)
{
	Cup_Blink(1); //종이컵 하나
	Water_Coffee_Blink(3); //물에 탄 커피 3펌핑
}

/*****************커피제작 시 점멸횟수 지정 함수들*******************/
void Cup_Blink(int Cup_Signal)
{
	for(i=0; i<Cup_Signal; i++)
	{
		GPIOG->ODR |= 0x0080;   //  LED7 점등
		DelayMS(150);
		GPIOG->ODR &= ~0x0080;   //  LED7 소등
		DelayMS(150);
	}
}

void Sugar_Blink(int Sugar_Signal)
{
	for(i=0; i<Sugar_Signal; i++)
	{
		GPIOG->ODR |= 0x0040;   //  LED6 점등
		DelayMS(150);
		GPIOG->ODR &= ~0x0040;   //  LED6 소등
		DelayMS(150);
	}
}

void Cream_Blink(int Cream_Signal)
{
	for(i=0; i<Cream_Signal; i++)
	{
		GPIOG->ODR |= 0x0020;   //  LED5 점등
		DelayMS(150);
		GPIOG->ODR &= ~0x0020;   //  LED5 소등
		DelayMS(150);
	}
}

void Water_Coffee_Blink(int Water_Signal)
{
	for(i=0; i<Water_Signal; i++)
	{
		GPIOG->ODR |= 0x0010;   //  LED4 점등
		DelayMS(150);
		GPIOG->ODR &= ~0x0010;   //  LED4 소등
		DelayMS(150);
	}
}
/**************************HW6 사용자 지정함수들 끝*********************************/


/**************************KEY_Scan 함수 (채터링 현상 방지목적)*********************************/
uint8_t key_flag = 0;

uint16_t KEY_Scan(void)	// input key SW0 - SW7 
{ 
	uint16_t key;
	key = GPIOH->IDR & 0xFF00;	// any key pressed ?


	if(key == 0xFF00)		// if no key, check key off
	{  	if(key_flag == 0)
	return key;
	else
	{	DelayMS(10);
	key_flag = 0;
	return key;
	}
	}
	else				// if key input, check continuous key
	{	if(key_flag != 0)	// if continuous key, treat as no key input
	return 0xFF00;
	else			// if new key,delay for debounce
	{	key_flag = 1;
	DelayMS(10);
	return key;
	}
	}

}



void _GPIO_Init(void)
{
	// LED (GPIO G) 설정
	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 

	// SW (GPIOT H) 설정 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 
	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER 	&= ~0x0200;	// GPIOF 9 : Push-pull  	
	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
}	

void BEEP(void)			/* beep for 30 ms */
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void DelayMS(unsigned short wMS)
{
	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000); 	// 1000us => 1ms
}

void DelayUS(unsigned short wUS)
{
	volatile int Dly = (int)wUS*17;
	for(; Dly; Dly--);
}


/***********************************************************************
// 보충 설명자료
// 다음은 stm32f4xx.h에 있는 RCC관련 주요 선언문임 
#define HSE_STARTUP_TIMEOUT    ((uint16_t)0x05000)   // Time out for HSE start up 
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

#define FLASH_BASE            ((uint32_t)0x08000000) // FLASH(up to 1 MB) base address in the alias region                          
#define CCMDATARAM_BASE       ((uint32_t)0x10000000) // CCM(core coupled memory) data RAM(64 KB) base address in the alias region   
#define SRAM1_BASE            ((uint32_t)0x20000000) // SRAM1(112 KB) base address in the alias region                              

#define SRAM2_BASE            ((uint32_t)0x2001C000) // SRAM2(16 KB) base address in the alias region                               
#define SRAM3_BASE            ((uint32_t)0x20020000) // SRAM3(64 KB) base address in the alias region                               

#define PERIPH_BASE           ((uint32_t)0x40000000) // Peripheral base address in the alias region                                 
#define BKPSRAM_BASE          ((uint32_t)0x40024000) // Backup SRAM(4 KB) base address in the alias region                          

// Peripheral memory map  
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000)

// AHB1 peripherals  
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000)
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400)
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00)

typedef struct
{
__IO uint32_t CR;            // RCC clock control register,                                  Address offset: 0x00  
__IO uint32_t PLLCFGR;       // RCC PLL configuration register,                              Address offset: 0x04  
__IO uint32_t CFGR;          // RCC clock configuration register,                            Address offset: 0x08  
__IO uint32_t CIR;           // RCC clock interrupt register,                                Address offset: 0x0C  
__IO uint32_t AHB1RSTR;      // RCC AHB1 peripheral reset register,                          Address offset: 0x10  
__IO uint32_t AHB2RSTR;      // RCC AHB2 peripheral reset register,                          Address offset: 0x14  
__IO uint32_t AHB3RSTR;      // RCC AHB3 peripheral reset register,                          Address offset: 0x18  
__IO uint32_t APB1RSTR;      // RCC APB1 peripheral reset register,                          Address offset: 0x20  
__IO uint32_t APB2RSTR;      // RCC APB2 peripheral reset register,                          Address offset: 0x24  
__IO uint32_t AHB1ENR;       // RCC AHB1 peripheral clock register,                          Address offset: 0x30  
__IO uint32_t AHB2ENR;       // RCC AHB2 peripheral clock register,                          Address offset: 0x34  
__IO uint32_t AHB3ENR;       // RCC AHB3 peripheral clock register,                          Address offset: 0x38  
__IO uint32_t APB1ENR;       // RCC APB1 peripheral clock enable register,                   Address offset: 0x40  
__IO uint32_t APB2ENR;       // RCC APB2 peripheral clock enable register,                   Address offset: 0x44  
__IO uint32_t AHB1LPENR;     // RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50  
__IO uint32_t AHB2LPENR;     // RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54  
__IO uint32_t AHB3LPENR;     // RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58  
__IO uint32_t APB1LPENR;     // RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60  
__IO uint32_t APB2LPENR;     // RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64  
__IO uint32_t BDCR;          // RCC Backup domain control register,                          Address offset: 0x70  
__IO uint32_t CSR;           // RCC clock control & status register,                         Address offset: 0x74  
__IO uint32_t SSCGR;         // RCC spread spectrum clock generation register,               Address offset: 0x80  
__IO uint32_t PLLI2SCFGR;    // RCC PLLI2S configuration register,                           Address offset: 0x84  
__IO uint32_t PLLSAICFGR;    // RCC PLLSAI configuration register,                           Address offset: 0x88  
__IO uint32_t DCKCFGR;       // RCC Dedicated Clocks configuration register,                 Address offset: 0x8C  
__IO uint32_t CKGATENR;      // RCC Clocks Gated Enable Register,                            Address offset: 0x90   // Only for STM32F412xG, STM32413_423xx and STM32F446xx devices  
__IO uint32_t DCKCFGR2;      // RCC Dedicated Clocks configuration register 2,               Address offset: 0x94   // Only for STM32F410xx, STM32F412xG, STM32413_423xx and STM32F446xx devices  

} RCC_TypeDef;


typedef struct
{
__IO uint32_t ACR;      // FLASH access control register,   Address offset: 0x00  
__IO uint32_t KEYR;     // FLASH key register,              Address offset: 0x04  
__IO uint32_t OPTKEYR;  // FLASH option key register,       Address offset: 0x08  
__IO uint32_t SR;       // FLASH status register,           Address offset: 0x0C  
__IO uint32_t CR;       // FLASH control register,          Address offset: 0x10  
__IO uint32_t OPTCR;    // FLASH option control register ,  Address offset: 0x14  
__IO uint32_t OPTCR1;   // FLASH option control register 1, Address offset: 0x18  
} FLASH_TypeDef;

typedef struct
{
__IO uint32_t MODER;    // GPIO port mode register,               Address offset: 0x00       
__IO uint32_t OTYPER;   // GPIO port output type register,        Address offset: 0x04       
__IO uint32_t OSPEEDR;  // GPIO port output speed register,       Address offset: 0x08       
__IO uint32_t PUPDR;    // GPIO port pull-up/pull-down register,  Address offset: 0x0C       
__IO uint32_t IDR;      // GPIO port input data register,         Address offset: 0x10       
__IO uint32_t ODR;      // GPIO port output data register,        Address offset: 0x14       
__IO uint16_t BSRRL;    // GPIO port bit set/reset low register,  Address offset: 0x18       
__IO uint16_t BSRRH;    // GPIO port bit set/reset high register, Address offset: 0x1A       
__IO uint32_t LCKR;     // GPIO port configuration lock register, Address offset: 0x1C       
__IO uint32_t AFR[2];   // GPIO alternate function registers,     Address offset: 0x20-0x24  
} GPIO_TypeDef;

typedef struct
{
__IO uint32_t IMR;    // EXTI Interrupt mask register, Address offset: 0x00 
__IO uint32_t EMR;    // EXTI Event mask register, Address offset: 0x04 
__IO uint32_t RTSR;   // EXTI Rising trigger selection register,  Address offset: 0x08
__IO uint32_t FTSR;   // EXTI Falling trigger selection register, Address offset: 0x0C
__IO uint32_t SWIER;  // EXTI Software interrupt event register,  Address offset: 0x10 
__IO uint32_t PR;     // EXTI Pending register, Address offset: 0x14 
} EXTI_TypeDef;

typedef struct
{
__IO uint32_t MEMRMP;       // SYSCFG memory remap register, Address offset: 0x00 
__IO uint32_t PMC;          // SYSCFG peripheral mode configuration register, Address offset: 0x04
__IO uint32_t EXTICR[4];    // SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 
__IO uint32_t CMPCR;        // SYSCFG Compensation cell control register,Address offset: 0x20

} SYSCFG_TypeDef;

#define GPIOA 	((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB	((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE  ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF   ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG   ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH   ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI   ((GPIO_TypeDef *) GPIOI_BASE)
#define GPIOJ   ((GPIO_TypeDef *) GPIOJ_BASE)
#define GPIOK   ((GPIO_TypeDef *) GPIOK_BASE)

#define CRC     ((CRC_TypeDef *) CRC_BASE)
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define FLASH   ((FLASH_TypeDef *) FLASH_R_BASE)

#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)

#define FLASH_ACR_PRFTEN             ((uint32_t)0x00000100)
#define FLASH_ACR_ICEN               ((uint32_t)0x00000200)
#define FLASH_ACR_DCEN               ((uint32_t)0x00000400)
#define FLASH_ACR_ICRST              ((uint32_t)0x00000800)
#define FLASH_ACR_DCRST              ((uint32_t)0x00001000)
#define FLASH_ACR_BYTE0_ADDRESS      ((uint32_t)0x40023C00)
#define FLASH_ACR_BYTE2_ADDRESS      ((uint32_t)0x40023C03)

#define FLASH_ACR_LATENCY_5WS        ((uint32_t)0x00000005)

typedef struct {
__IO uint32_t ISER[8];  // Interrupt Set Enable Register    
__IO uint32_t ICER[8];  // Interrupt Clear Enable Register  
__IO uint32_t ISPR[8];  //  Interrupt Set Pending Register   
__IO uint32_t ICPR[8];  //  Interrupt Clear Pending Register
__IO uint32_t IABR[8];  //  Interrupt Active bit Register      
__IO uint8_t  IP[240];  //  Interrupt Priority Register (8Bit) 
__O  uint32_t STIR;  //  Software Trigger Interrupt Register    
}  NVIC_Type;    

// Memory mapping of Cortex-M4 Hardware 
#define SCS_BASE     (0xE000E000)    // System Control Space Base Address 
#define NVIC_BASE   (SCS_BASE +  0x0100)  // NVIC Base Address  
#define NVIC        ((NVIC_Type *)  NVIC_BASE) // NVIC configuration struct                                           

*/ 