#include "stm32f4xx.h"
#include "GLCD.h"
////////제출자 : 2015134022 송진한/////제출일 : 2020.05.29.///////////
void _GPIO_Init(void);
void _EXTI_Init(void);

void DisplayInitScreen(void);
uint16_t KEY_Scan(void);
void BEEP(void);
void Do_BEEP(void);
void Mi_BEEP(void);
void So_BEEP(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

void Escape_success(uint8_t Room);

uint8_t Room;
uint8_t i;

int main(void)
{
	LCD_Init();	// LCD 모듈 초기화
	DelayMS(10);
	_GPIO_Init();	// GPIO (LED & SW) 초기화
	_EXTI_Init();	// EXTI 초기화

	DisplayInitScreen();	// LCD 초기화면

	GPIOG->ODR &= ~0x00FF;		// 초기값: LED0~7 Off
	//GPIOG->ODR |= 0x0001;		// GPIOG->ODR.0 'H'(LED ON)

	while (1)
	{
		/*
	   1 - 3 - 4 - 5 - 6 - 7 - 0 비밀번호 대탈출

	   SW0     SW1    <> SW2[x]  SW3     SW4     SW5     SW6     SW7
	   EXTI8   EXTI9  <> EXTI10  EXTI11  EXTI3   EXTI4   EXTI5   EXTI6
	   0100    0200   <> 0400    0800    1000    2000    4000    8000
	   0       1      <>         3       4       5       6       7
	   [7번]  [1]번   <>        [2번]   [3번]   [4번]   [5번]   [6번]
	   */
	}
}

/* GLCD 초기화면 설정 함수 */
void DisplayInitScreen(void)
{
	LCD_Clear(RGB_WHITE);		// 화면 클리어
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_WHITE);	// 글자배경색 : WHITE
	LCD_SetTextColor(RGB_BLUE);	// 글자색 : BLUE
	LCD_DisplayText(0, 0, "2015134022 SJH");  	// 학번 이름


	LCD_SetTextColor(RGB_BLACK);	// 글자색 : BLACK
	LCD_DisplayText(1, 0, "R 0 1 3 4 5 6 7");  	// ROOM NO.표시

	LCD_DisplayChar(2, 18, 'S'); //동작 상황 표시 : START


	//////////////검정 테두리 && 하얀 색 정사각형 7개////////////////////////////
	LCD_SetPenColor(GET_RGB(0, 0, 0));
	LCD_SetBrushColor(GET_RGB(0xFF, 0xFF, 0xFF));
	for (i = 0; i < 7; i++)
	{
		//void LCD_DrawFillRect(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
		//LCD_DrawRectangle(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
		LCD_DrawFillRect(16 + (16 * i), 30, 8, 8);
		LCD_DrawRectangle(16 + (16 * i), 30, 8, 8);
		//  숫자  : 0  1  3  4  5  6  7
		//  네모i : 0  1  2  3  4  5  6
	}


	LCD_SetBrushColor(GET_RGB(255, 0, 0));  //이후 생성된 네모들은 빨간색으로 지정
}

/* GPIO (GPIOG(LED), GPIOH(Switch), GPIOF(Buzzer)) 초기 설정	*/
void _GPIO_Init(void)
{
	// LED (GPIO G) 설정
	RCC->AHB1ENR |= 0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER |= 0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER &= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
	GPIOG->OSPEEDR |= 0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 

	// SW (GPIO H) 설정 
	RCC->AHB1ENR |= 0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER &= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR &= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 
	RCC->AHB1ENR |= 0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER |= 0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER &= ~0x0200;	// GPIOF 9 : Push-pull  	
	GPIOF->OSPEEDR |= 0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
}

void Escape_success(uint8_t Room)
{
	LCD_DrawFillRect(16 + (16 * Room), 30, 8, 8);
	LCD_DrawRectangle(16 + (16 * Room), 30, 8, 8);
	BEEP();
}



/* EXTI (EXTI8(GPIOH.8, SW0), EXTI9(GPIOH.9, SW1)) 초기 설정  */
//SW2 GPIOH.10 EXTI10은 물리적으로 오류가 난다.

/***** 1345670   *****/
void _EXTI_Init(void)
{//스위치를 누르는 순간 스위치가 인터럽트 소스역할
	//PH15 PH8 과 같은 GPIO기능을 써서 키스캔 펑션 풀링으로 받아들임
	//이제는 순수 EXTI 인터럽트를 발생시켜서 그 키가 입력되었는지를 알아낸다.
	//결론은 키가 들어갔냐 안들어갔냐로 알수 있다.

	RCC->AHB1ENR |= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->APB2ENR |= 0x00004000;	// Enable System Configuration Controller Clock         //시스컨피규모드를 입력으로 해줌
	//위에를 주석처리하면 SYSCFG->EXTICR[2] |= 0x0077; 이 작동을 안한다.
	GPIOH->MODER &= ~0xFFFF0000;	// GPIOH PIN8~PIN15 Input mode (reset state)		//입력이라고 확실하게 해줌.		 

	//SYSCFG->EXTICR[2] |= 0x0077; 	// EXTI8,9에 대한 소스 입력은 GPIOH로 설정      //이게 매우 중요함. 인터럽트 PDF 40페이지 참고.
	// EXTI8 <- PH8, EXTI9 <- PH9               //PH니까 0111 0111이라서 77이다.
	// reset value: 0x0000	

	/*********EXternal InTerrupt Configuration Register*************/
	SYSCFG->EXTICR[2] |= 0x7777; 	// EXTI8, EXTI9, EXTI10, EXTI11
	SYSCFG->EXTICR[3] |= 0x7777; 	// EXT12, EXTI13, EXTI14, EXTI15 

	/*********(Falling | Rising) trigger selection register*************/
	//EXTI->FTSR |= 0x000100;		// EXTI8: Falling Trigger Enable
	//|= (1<<8); //0b00000001 00000000 
	EXTI->FTSR |= 0x0000FF00;		//EXTI8~15 폴링 트리거 이네이블
	//EXTI->FTSR |= 0x00000100;             // EXTI8: Falling Trigger Enable
	//EXTI->RTSR |= 0x00000200;		// EXTI9: Rising Trigger  Enable

	/*********Interrupt mask register*******************************/
	//EXTI->IMR  |= 0x0000FF00;  	// EXTI8~15까지 전체 인터럽트 mask (Interrupt Enable) 설정
	//0100 + 0200 + 0400 + 0800 + 1000 + 2000 + 4000 + 8000 = FF00
	EXTI->IMR |= 0x00000200;       //EXTI8(SW0) 인터럽트만 마스킹
	//1345670 암호 중 1만 눌릴 수 있게끔 해야한다.

	/*
	SW0     SW1    <> SW2[x]  SW3     SW4     SW5     SW6     SW7
	EXTI8   EXTI9  <> EXTI10  EXTI11  EXTI3   EXTI4   EXTI5   EXTI6
	0100    0200   <> 0400    0800    1000    2000    4000    8000
	0       1      <>         3       4       5       6       7
	[7번]  [1]번   <>        [2번]   [3번]   [4번]   [5번]   [6번]
	*/

	/***CMSIS_NVIC  Nested Vectored Interrupt Controller (NVIC)**/
	//NVIC->ISER[]  Interrupt Set Enable Register
	//NVIC은 32비트 MCU므로 ISER[]이 32개씩 표현가능하다. RF-250p참고
	NVIC->ISER[0] |= (1 << 23);        //0x0000 009C //0b 0000 0000 0000 0000  0000 0000 1001 1100
	// Enable EXTI Line[9:5] interrupts

	NVIC->ISER[1] |= (1 << (40 - 32)); //0x0000 00E0
	// Enable EXTI Line[15:10] interrupts


	//SW3 GPIOH.11 EXTI11 설정
	// SYSCFG->EXTICR[2] |= 0x7000; //EXTI11	
	//EXTI->FTSR |= (1<<11); // 0x000800;		 
	//EXTI->RTSR |= 0x000200;		
	//EXTI->IMR  |= 0x000800;  

	//SW4 GPIOH.12 EXTI12 설정
	//SYSCFG->EXTICR[3] |= 0x0007; //EXTI12	
	//SYSCFG->EXTICR[3] |= 0x7000; //EXTI15
	//EXTI->FTSR |= (1<<12); // 0x001000;		 
	//EXTI->RTSR |= (1<<12); // 0x001000;		 
	//EXTI->IMR  |= 0x008000;   //(IMR 해주는건 비번 시작번호 마스킹) SW7
}

/********* EXTI5~9 인터럽트 핸들러(ISR: Interrupt Service Routine) ****************/
void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR & 0x0100)                   // EXTI8 Interrupt Pending register (EXTI_PR)
	{//SW0     
		EXTI->PR |= 0x0100; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
		//펜딩 레지스터 비트만 특이하게 1이 RESET(지우는거)고 0이 SET(안지우는) 특성을 가졌다.

		LCD_DisplayChar(2, 18, '\x43'); //동작 상황 표시 : CLEAR

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0001; //GPIO_Pin_0  LED0 ON

		Escape_success(0); //0번째 암호박스 탈출성공

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 전체 인터럽트 마스킹 해제
		//EXTI->IMR  |= 0x00000800;       //EXTI11(SW3) 인터럽트만 마스킹

		DelayMS(400); //1초간 대기
		Mi_BEEP();
		DelayMS(200); //0.5초간 대기
		So_BEEP();
		DelayMS(200); //0.5초간 대기
		Do_BEEP();

		DelayMS(1200); //3초간 대기
		BEEP();
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		DisplayInitScreen(); //화면 초기화
		_EXTI_Init();         //인터럽트 초기화
	}

	else if (EXTI->PR & 0x0200) 		// EXTI9 Interrupt Pending register (EXTI_PR)
	{//SW1 
		EXTI->PR |= 0x0200; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)

		LCD_DisplayChar(2, 18, '\x57'); //동작 상황 표시 : WORK

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0002; //GPIO_Pin_1  LED1 ON

		Escape_success(1); //1번째 암호박스 탈출성공

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 전체 인터럽트 마스킹 해제
		EXTI->IMR |= 0x00000800;       //EXTI11(SW3) 인터럽트만 마스킹
	}


}

/*
SW0     SW1    <> SW2[x]  SW3     SW4     SW5     SW6     SW7
EXTI8   EXTI9  <> EXTI10  EXTI11  EXTI3   EXTI4   EXTI5   EXTI6
0100    0200   <> 0400    0800    1000    2000    4000    8000
0       1      <>         3       4       5       6       7
[7번]  [1]번   <>        [2번]   [3번]   [4번]   [5번]   [6번]
*/


/********* EXTI10~15 인터럽트 핸들러(ISR: Interrupt Service Routine) ****************/
void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR & 0x0400) 		// EXTI10 Interrupt Pending register (EXTI_PR)
	{//SW2  물리적 회로 연결X
		EXTI->PR |= 0x0400; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
		//GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		//GPIOG->ODR |= 0x0004; //GPIO_Pin_2  LED2 ON
	}

	else if (EXTI->PR & 0x0800) 		// EXTI11 Interrupt Pending register (EXTI_PR)
	{//SW3  
		EXTI->PR |= 0x0800; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0008; //GPIO_Pin_3  LED3 ON

		Escape_success(2); //2번째 암호박스 탈출성공

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 전체 인터럽트 마스킹 해제
		EXTI->IMR |= 0x00001000;       //EXTI12(SW4) 인터럽트만 마스킹        
	}

	else if (EXTI->PR & 0x1000)              // EXTI12 Interrupt Pending register (EXTI_PR)
	{//SW4  
		EXTI->PR |= 0x1000; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0010; //GPIO_Pin_4  LED4 ON

		Escape_success(3); //3번째 암호박스 탈출성공

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 전체 인터럽트 마스킹 해제
		EXTI->IMR |= 0x00002000;       //EXTI12(SW4) 인터럽트만 마스킹
	}

	else if (EXTI->PR & 0x2000)               // EXTI13 Interrupt Pending register (EXTI_PR)
	{//SW5       
		EXTI->PR |= 0x2000; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0020; //GPIO_Pin_5  LED5 ON

		Escape_success(4); //4번째 암호박스 탈출성공

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 전체 인터럽트 마스킹 해제
		EXTI->IMR |= 0x00004000;       //EXTI12(SW4) 인터럽트만 마스킹
	}

	else if (EXTI->PR & 0x4000)              // EXTI14 Interrupt Pending register (EXTI_PR)
	{//SW6      
		EXTI->PR |= 0x4000; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0040; //GPIO_Pin_6  LED6 ON

		Escape_success(5); //5번째 암호박스 탈출성공

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 전체 인터럽트 마스킹 해제
		EXTI->IMR |= 0x00008000;       //EXTI12(SW4) 인터럽트만 마스킹
	}

	else if (EXTI->PR & 0x8000)              // EXTI15 Interrupt Pending register (EXTI_PR)
	{//SW7      
		EXTI->PR |= 0x8000; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0080; //GPIO_Pin_7  LED7 ON

		Escape_success(6); //6번째 암호박스 탈출성공

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 전체 인터럽트 마스킹 해제
		EXTI->IMR |= 0x00000100;       //EXTI8(SW0) 인터럽트만 마스킹               
	}
}

/* Switch가 입력되었는지를 여부와 어떤 switch가 입력되었는지의 정보를 return하는 함수  */
uint8_t key_flag = 0;
uint16_t KEY_Scan(void)	// input key SW0 - SW7 
{
	uint16_t key;
	key = GPIOH->IDR & 0xFF00;	// any key pressed ?
	if (key == 0xFF00)		// if no key, check key off
	{
		if (key_flag == 0)
			return key;
		else
		{
			DelayMS(10);
			key_flag = 0;
			return key;
		}
	}
	else				// if key input, check continuous key
	{
		if (key_flag != 0)	// if continuous key, treat as no key input
			return 0xFF00;
		else			// if new key,delay for debounce
		{
			key_flag = 1;
			DelayMS(10);
			return key;
		}
	}
}

/* Buzzer: Beep for 30 ms */
void BEEP(void)
{
	GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void DO_BEEP(void)
{
	GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
	DelayMS(100);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void DelayMS(unsigned short wMS)
{
	register unsigned short i;
	for (i = 0; i < wMS; i++)
		DelayUS(1000);         		// 1000us => 1ms
}

void DelayUS(unsigned short wUS)
{
	volatile int Dly = (int)wUS * 17;
	for (; Dly; Dly--);
}



void Mi_BEEP(void) // 329 hz
{
	for (i = 0; i < (35000 / 1515); i++)
	{
		GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
		DelayUS(1515);
		GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
		DelayUS(1515);
	}
}

void So_BEEP(void) // 391 hz
{
	for (i = 0; i < (35000 / 1275); i++)
	{
		GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
		DelayUS(1275);
		GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
		DelayUS(1275);
	}
}

void Do_BEEP(void) // 261/2 hz
{
	for (i = 0; i < (35000 / 954); i++)
	{
		GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
		DelayUS(954);
		GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
		DelayUS(954);
	}
}