#include "stm32f4xx.h"

void _GPIO_Init(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

int main(void)
{
    _GPIO_Init();
    GPIOG->ODR &= ~0x00FF;	// 초기값: LED0~7 Off
    
    //GPIO LED0 ON (Test 1)
    GPIOG->ODR |= 0x0003; 	// GPIOG->ODR.0 'H'(LED ON) 0번이랑 1번을 동시에 2개 킨다.
    //0000 0000 0000 0011
    
    //BSRRL 로 하게 되면.. 
    //GPIOG -> BSRRL = 0x0003;
    
    while(1)
    {
        //GPIO SW0 입력으로 LED0 On/Off      //P200참고
	if((GPIOH->IDR & 0x0100) == 0)	// SW1를 눌렸을 때(SW1 = LOW) (GPIOH->IDR.9 = 'L' ?) 
	{
		GPIOG->ODR |= 0x0100;	// LED1 On (GPIOG->ODR.1 'H')
	}
	else
	{
		GPIOG->ODR &= ~0x0100;	// LED1 Off (GPIOG->ODR.1 'L')
	}
      
	//GPIO SW1 입력으로 LED1 On/Off   
	if((GPIOH->IDR & 0x0200) == 0)	// SW1를 눌렸을 때(SW1 = LOW) (GPIOH->IDR.9 = 'L' ?) 
	{
		GPIOG->ODR |= 0x0200;	// LED1 On (GPIOG->ODR.1 'H')
	}
	else
	{
		GPIOG->ODR &= ~0x0200;	// LED1 Off (GPIOG->ODR.1 'L')
	}
	
  	//GPIO SW2입력으로 LED2 1초후 Off 
  	if((GPIOH->IDR & 0x0400) == 0)	// SW2를 눌렸을 때(SW2 = LOW) (GPIOH->IDR.10 = 'L' ?) 
	{
		GPIOG->ODR |=  0x0400;	// LED2 On (GPIOG->ODR.2 'H')
		DelayMS(1000);		// Delay (1000ms  = 1s)
		GPIOG->ODR &= ~0x0400;	// LED2 Off (GPIOG->ODR.2 'L') 
	}
      
        //GPIO SW3입력으로 LED3 1초후 Off 
  	if((GPIOH->IDR & 0x0800) == 0)	// SW3를 눌렸을 때(SW2 = LOW) (GPIOH->IDR.10 = 'L' ?) 
	{
		GPIOG->ODR |=  0x0800;	// LED2 On (GPIOG->ODR.2 'H')
		DelayMS(1000);		// Delay (1000ms  = 1s)
		GPIOG->ODR &= ~0x0800;	// LED2 Off (GPIOG->ODR.2 'L') 
	}
        
        //GPIO SW4입력으로 LED4 1초후 Off 
  	if((GPIOH->IDR & 0x1000) == 0)	// SW4를 눌렸을 때(SW4 = LOW) (GPIOH->IDR.10 = 'L' ?) 
	{
		GPIOG->ODR |=  0x1000;	// LED4 On (GPIOG->ODR.4 'H')
		DelayMS(1000);		// Delay (1000ms  = 1s)
		GPIOG->ODR &= ~0x1000;	// LED4 Off (GPIOG->ODR.4 'L') 
	}
        
        //GPIO SW5입력으로 LED5 1초후 Off 
  	if((GPIOH->IDR & 0x2000) == 0)	// SW5를 눌렸을 때(SW2 = LOW) (GPIOH->IDR.10 = 'L' ?) 
	{
		GPIOG->ODR |=  0x2000;	// LED2 On (GPIOG->ODR.2 'H')
		DelayMS(1000);		// Delay (1000ms  = 1s)
		GPIOG->ODR &= ~0x2000;	// LED2 Off (GPIOG->ODR.2 'L') 
	}
        
        //GPIO SW6입력으로 LED6 1초후 Off 
  	if((GPIOH->IDR & 0x4000) == 0)	// SW6를 눌렸을 때(SW2 = LOW) (GPIOH->IDR.10 = 'L' ?) 
	{
		GPIOG->ODR |=  0x4000;	// LED2 On (GPIOG->ODR.2 'H')
		DelayMS(1000);		// Delay (1000ms  = 1s)
		GPIOG->ODR &= ~0x4000;	// LED2 Off (GPIOG->ODR.2 'L') 
	}
        
        //GPIO SW7입력으로 LED7 1초후 Off 
  	if((GPIOH->IDR & 0x8000) == 0)	// SW7를 눌렸을 때(SW2 = LOW) (GPIOH->IDR.10 = 'L' ?) 
	{
		GPIOG->ODR |=  0x8000;	// LED2 On (GPIOG->ODR.2 'H')
		DelayMS(1000);		// Delay (1000ms  = 1s)
		GPIOG->ODR &= ~0x8000;	// LED2 Off (GPIOG->ODR.2 'L') 
	}
    }
}

void _GPIO_Init(void)
{
	// LED(GPIO G) 설정 출력으로 설정
    	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6)에 Clock Enable	//GPIOG클럭 이네이블								
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)	        //출력으로 설정된 상태					
	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
 	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
    
	// SW(GPIO H) 설정 입력으로 설정
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7)에 Clock Enable	//GPIOH클럭 이네이블						
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)	//기본값이 입력이라 안해도 되지만 굳이 입력으로 해놓은 상태			
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state (풀업,풀다운 저항을 끊어놓은 상태 == 플로팅된 상태)
        
        //GPIOH->MODER 	|= ~0xFFFF0000;	//모더가 출력인 상태
        //GPIOH->PUPDR 	|= 0x55550000;	//PUPDR 레지스터로 풀업으로 바꿀때(풀업저항)
}	

void DelayMS(unsigned short wMS)
{
    register unsigned short i;
    for (i=0; i<wMS; i++)
        DelayUS(1000);         		// 1000us => 1ms
}
void DelayUS(unsigned short wUS)
{
    volatile int Dly = (int)wUS*17;
    for(; Dly; Dly--);
}

/**************************************************************************
// 보충 설명자료
// 다음은 stm32f4xx.h에 있는 RCC관련 주요 선언문임 
#define HSE_STARTUP_TIMEOUT    ((uint16_t)0x05000)   // Time out for HSE start up 
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

#define FLASH_BASE            ((uint32_t)0x08000000) // FLASH(up to 1 MB) base address in the alias region                          
#define CCMDATARAM_BASE       ((uint32_t)0x10000000) // CCM(core coupled memory) data RAM(64 KB) base address in the alias region   
#define SRAM1_BASE            ((uint32_t)0x20000000) // SRAM1(112 KB) base address in the alias region                              

#if defined(STM32F40_41xxx) 
#define SRAM2_BASE            ((uint32_t)0x2001C000) // SRAM2(16 KB) base address in the alias region                               
#define SRAM3_BASE            ((uint32_t)0x20020000) // SRAM3(64 KB) base address in the alias region                               
#endif   

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
  uint32_t      RESERVED0;     // Reserved, 0x1C                                                                     
  __IO uint32_t APB1RSTR;      // RCC APB1 peripheral reset register,                          Address offset: 0x20  
  __IO uint32_t APB2RSTR;      // RCC APB2 peripheral reset register,                          Address offset: 0x24  
  uint32_t      RESERVED1[2];  // Reserved, 0x28-0x2C                                                                
  __IO uint32_t AHB1ENR;       // RCC AHB1 peripheral clock register,                          Address offset: 0x30  
  __IO uint32_t AHB2ENR;       // RCC AHB2 peripheral clock register,                          Address offset: 0x34  
  __IO uint32_t AHB3ENR;       // RCC AHB3 peripheral clock register,                          Address offset: 0x38  
  uint32_t      RESERVED2;     // Reserved, 0x3C                                                                     
  __IO uint32_t APB1ENR;       // RCC APB1 peripheral clock enable register,                   Address offset: 0x40  
  __IO uint32_t APB2ENR;       // RCC APB2 peripheral clock enable register,                   Address offset: 0x44  
  uint32_t      RESERVED3[2];  // Reserved, 0x48-0x4C                                                                
  __IO uint32_t AHB1LPENR;     // RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50  
  __IO uint32_t AHB2LPENR;     // RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54  
  __IO uint32_t AHB3LPENR;     // RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58  
  uint32_t      RESERVED4;     // Reserved, 0x5C                                                                     
  __IO uint32_t APB1LPENR;     // RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60  
  __IO uint32_t APB2LPENR;     // RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64  
  uint32_t      RESERVED5[2];  // Reserved, 0x68-0x6C                                                                
  __IO uint32_t BDCR;          // RCC Backup domain control register,                          Address offset: 0x70  
  __IO uint32_t CSR;           // RCC clock control & status register,                         Address offset: 0x74  
  uint32_t      RESERVED6[2];  // Reserved, 0x78-0x7C                                                                
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
  __IO uint32_t MEMRMP;   // SYSCFG memory remap register, Address offset: 0x00 
  __IO uint32_t PMC;          // SYSCFG peripheral mode configuration register, Address offset: 0x04
  __IO uint32_t EXTICR[4];    // SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 
  uint32_t      RESERVED[2];  // Reserved, 0x18-0x1C     
  __IO uint32_t CMPCR;        // SYSCFG Compensation cell control register,Address offset: 0x20

} SYSCFG_TypeDef;

#define GPIOA 	((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB	((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE  	((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF   ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG   ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH   ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI   ((GPIO_TypeDef *) GPIOI_BASE)
#define GPIOJ   ((GPIO_TypeDef *) GPIOJ_BASE)
#define GPIOK   ((GPIO_TypeDef *) GPIOK_BASE)

#define CRC             ((CRC_TypeDef *) CRC_BASE)
#define RCC             ((RCC_TypeDef *) RCC_BASE)
#define FLASH           ((FLASH_TypeDef *) FLASH_R_BASE)

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
  __IO uint32_t ISER[8];  // Offset: 0x000 Interrupt Set Enable Register    
       uint32_t RESERVED0[24];                                   
  __IO uint32_t ICER[8];  // Offset: 0x080 Interrupt Clear Enable Register  
       uint32_t RSERVED1[24];                                    
  __IO uint32_t ISPR[8];  // Offset: 0x100 Interrupt Set Pending Register   
       uint32_t RESERVED2[24];                                   
  __IO uint32_t ICPR[8];  // Offset: 0x180 Interrupt Clear Pending Register
       uint32_t RESERVED3[24];                                   
  __IO uint32_t IABR[8];  // Offset: 0x200 Interrupt Active bit Register      
       uint32_t RESERVED4[56];                                   
  __IO uint8_t  IP[240];  // Offset: 0x300 Interrupt Priority Register (8Bit) 
       uint32_t RESERVED5[644];                                  
  __O  uint32_t STIR;  // Offset: 0xE00 Software Trigger Interrupt Register    
}  NVIC_Type;      
// Memory mapping of Cortex-M4 Hardware 
#define SCS_BASE     (0xE000E000)    // System Control Space Base Address 
#define NVIC_BASE   (SCS_BASE +  0x0100)  // NVIC Base Address  
#define NVIC        ((NVIC_Type *)  NVIC_BASE) // NVIC configuration struct                                           

*/ 