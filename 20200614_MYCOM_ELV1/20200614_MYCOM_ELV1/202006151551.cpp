/*********************1_HW6_2015134022_송진한(수정 2020.05.04)*********************/
#include "stm32f4xx.h"
#include "GLCD.h"
#include "FRAM.h"

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
void _EXTI_Init(void);

uint16_t KEY_Scan(void);
void BEEP(void);

void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

/**************전역변수 선언***************************/
uint8_t	SW7_Flag = 0;         // SW7입력 정보 저장 변수 선언 (초기값은 0으로 설정)
uint16_t blue_pointy = 88, red_pointy = 88; // 왼쪽(파랑)과 오른쪽(빨강)의 상태바 y좌표 저장 변수 (초기값은 88로 설정)
uint16_t blue_flag = 1, red_flag = 1;           // 왼쪽(파랑)과 오른쪽(빨강)의 현재 층을 저장하기 위한 정수형변수 (초기값은 1층으로 설정)
// LCD에 사각형을 그릴 때 사각형의 높이를 계산하기 위해 사용
uint16_t count = 0;             // 층 선택 횟수가 홀수인지 짝수인지 저장하는 변수 (0이면 초기상태, 1이면 홀수, 2이면 짝수)
uint16_t status = 0;            // 현재 동작하는 엘리베이터가 왼쪽인지 오른쪽인지 상태를 저장하는 변수 (0이면 파란색, 1이면 빨간색으로 글자색 설정)

char status1 = 'L', status2 = 'S', start = '1', end = '1', left_floor = '1', right_floor = '1';    //  LCD 출력 문자 저장 변수
// status1은 현재 엘리베이터가 왼쪽인지 오른쪽인지 저장 (왼쪽이면 'L', 오른쪽이면 'R')
// status2는 현재 엘리베이터가 어떤 동작을 수행하고 있는지 저장 (정지상태이면 'S', 올라가고 있으면 'U', 내려가고 있으면 'D')
// start는 선택한 출발층 저장, end는 선택한 목표층 저장
// left_floor, right_floor는 왼쪽과 오른쪽의 현재 층을 저장하기 위한 문자형 변수 (start, end와 비교하기 위해 사용)

char *mode = "FL";      // LCD 출력 문자열 저장 변수
// 현재 모드를 저장 (층선택 모드는 "FL", 실행 모드는 "M", 중단 모드는 "HL" 저장,,, 초기값은 "FL"로 설정)

unsigned char MODE = 'S';
unsigned char Current_F = '1', Target_F = '1';
uint8_t Room;
uint8_t i;


/**************내부함수 선언***************************/
void SelectMode(void);          // 층선택 모드 실행 함수



/********************************메인 함수******************************/
int main(void)
{
	///////메인함수 초기화/////////////////////////////////////////////////
	_GPIO_Init(); // GPIO (LED & SW) 초기화
	_EXTI_Init();   // EXTI(외부 인터럽트) 초기화
	LCD_Init();	// LCD 모듈 초기화
	DelayMS(100);
	Fram_Init();                    // FRAM 초기화 H/W 초기화
	// Fram_Status_Config();   // FRAM 초기화 S/W 초기화


	GPIOG->ODR |= 0x0002;	// 초기값: LED1 ON (1층)
	DisplayInitScreen();	// LCD 초기화면


	///////메인함수 무한루프/////////////////////////////////////////////////
	while (1)
	{
		SelectMode();

	}
}

void SelectMode(void)           // 층선택 모드 실행 함수
{
	switch (KEY_Scan())
	{
	case SW0_PUSH: 	//SW1이 눌리면 break까지 실행 //0xFD00
		//GPIOG->ODR &= ~0x007E;          // LED0를 제외한 모든 LED OFF   
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0001; //GPIO_Pin_0  LED0 ON

		// DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
		LCD_DrawFillRect(0, 40, 500, 8 + 1);

		LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE
		LCD_DrawFillRect(32 + (16 * 1), 40, 8, 8);
		LCD_DrawRectangle(32 + (16 * 1), 40, 8, 8);

		break;

	case SW1_PUSH: 	//SW1이 눌리면 break까지 실행 //0xFD00
		//GPIOG->ODR &= ~0x007E;          // LED0를 제외한 모든 LED OFF   
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0002; //GPIO_Pin_1  LED1 ON

		//  DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
		LCD_DrawFillRect(0, 40, 500, 8 + 1);

		LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE
		LCD_DrawFillRect(32 + (16 * 2), 40, 8, 8);
		LCD_DrawRectangle(32 + (16 * 2), 40, 8, 8);

		break;

	case SW2_PUSH: 	//SW2이 눌리면 break까지 실행 //0xFB00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0004; //GPIO_Pin_2  LED2 ON

		LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
		LCD_DrawFillRect(0, 40, 500, 8 + 1);

		LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE
		LCD_DrawFillRect(32 + (16 * 3), 40, 8, 8);
		LCD_DrawRectangle(32 + (16 * 3), 40, 8, 8);
		// DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		LCD_DrawFillRect(32 + (16 * 3), 40, 8, 8);
		LCD_DrawRectangle(32 + (16 * 3), 40, 8, 8);
		break;

	case SW3_PUSH: 	//SW3이 눌리면 break까지 실행 //0xF700
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0008; //GPIO_Pin_3  LED3 ON


		DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		break;

	case SW4_PUSH: 	//SW4이 눌리면 break까지 실행 //0xEF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0010; //GPIO_Pin_4  LED4 ON


		DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		break;

	case SW5_PUSH: 	//SW5이 눌리면 break까지 실행 //0xDF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0020; //GPIO_Pin_5  LED5 ON



		DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		break;

	case SW6_PUSH: 	//SW6이 눌리면 break까지 실행 //0xBF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0080; //GPIO_Pin_6  LED6 ON


		DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		break;
	}
}
void Floor_Calculation(void)
{
	if (Current_F
		LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
	LCD_DrawFillRect(0, 40, 500, 8 + 1);

	LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE
	LCD_DrawFillRect(32 + (16 * 1), 40, 8, 8);
	LCD_DrawRectangle(32 + (16 * 1), 40, 8, 8);
}
void _GPIO_Init(void)
{
	// LED (GPIO G) 설정
	RCC->AHB1ENR |= 0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER |= 0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER &= ~0x000000FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
	GPIOG->OSPEEDR |= 0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 

	// SW (GPIO H) 설정 
	RCC->AHB1ENR |= 0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER &= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR &= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 
	RCC->AHB1ENR |= 0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER |= 0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER &= ~0x00000200;	// GPIOF 9 : Push-pull  	
	GPIOF->OSPEEDR |= 0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
}

void _EXTI_Init(void)
{
	RCC->AHB1ENR |= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->APB2ENR |= 0x00004000;	// Enable System Configuration Controller Clock

	GPIOH->MODER &= 0x0000FFFF;	// GPIOH PIN8~PIN15 Input mode (reset state)			

	//SYSCFG->EXTICR[2] |= 0x0007; 	// EXTI8에 대한 소스 입력은 GPIOH로 설정 (EXTICR3) (reset value: 0x0000)	
	SYSCFG->EXTICR[3] |= 0x7000; 	// EXTI15에 대한 소스 입력은 GPIOH로 설정 (EXTICR4) (reset value: 0x0000)

	EXTI->FTSR |= 0x00008000;       // EXTI15 Falling Trigger Enable  (EXTI8:PH8 || EXTI15:PH15)
	EXTI->IMR |= 0x0000800;  	// EXTI15 인터럽트 mask (Interrupt Enable)

	//NVIC->ISER[0] |= ( 1 << 23  );   // Enable Interrupt EXTI8 Vector table Position 참조
	NVIC->ISER[1] |= (1 << 8);   // Enable Interrupt EXTI15 Vector table Position 참조
}

void EXTI9_5_IRQHandler(void)		// EXTI 5~9 인터럽트 핸들러
{
	if (SW7_Flag)
	{
		//        if(EXTI->PR & 0x0100) 		// EXTI8 Interrupt Pending?
		//        {
		//            EXTI->PR |= 0x0100; 		// Pending bit Clear
		//            mode = "HL";      // MODE를 중단 모드 "HL"로 변경
		//            DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		//            GPIOG->ODR &= ~0x0081;      // LED0, LED7 OFF, 나머지 LED는 상태 유지
		//            BEEP();
		//                     
		//            DelayMS(500);     // 0.5초 정지
		//            mode = "EX";      // MODE를 실행 모드 "EX"로 변경
		//        }
	}
}

void EXTI15_10_IRQHandler(void)		// EXTI 10~15 인터럽트 핸들러
{
	if (EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?
	{
		EXTI->PR |= 0x8000; 		// Pending bit Clear            bit 자리에 1을 넣어야 clear 된다.
		SW7_Flag = 1;	                // SW7이 눌리면 SW7_Flag에 1저장 (SW7이 눌렸다는 정보 저장)
		BEEP();     // 부저 1번 울림
	}
}

//int y_point = 2;


void DisplayInitScreen(void)    // LCD 화면 출력 함수
{
	LCD_Clear(RGB_YELLOW);		// 화면 클리어
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_YELLOW);	// 글자배경색 : WHITE
	LCD_SetTextColor(RGB_BLACK);	// 글자색 : BLACK
	LCD_DisplayText(0, 0, "2015134022 SJH");  	// 학번 이름
	LCD_DisplayText(1, 0, "S    6No:");  	//'정지상태'와     '6층 도달횟수'

	LCD_SetTextColor(GET_RGB(235, 0, 255));	// 글자색 : ViOLET
	LCD_DisplayText(1, 2, "O");  	//'정지상태' 표시 : O
	LCD_DisplayText(1, 8, "0");  	//'6층 도달횟수' 표시 : 0
	//LCD_DisplayChar(1, 8, Fram_Read(608) + 0x30);        //FRAM 608번지 저장된 data(1byte) 읽어 LCD에 표시 


	LCD_SetTextColor(RGB_BLACK);	// 글자색 : BLACK
	LCD_DisplayText(2, 0, "FL:   B 1 2 3 4 5 6");  	// ROOM NO.표시
	LCD_DisplayChar(2, 3, Current_F + 0x30);

	//////////////검정 테두리 && 하얀 색 정사각형 7개////////////////////////////
	LCD_SetPenColor(GET_RGB(0, 0, 0));
	LCD_SetBrushColor(GET_RGB(0, 0, 255));
	LCD_SetBrushColor(GET_RGB(255, 0, 0));  //이후 생성된 네모들은 빨간색으로 지정

	//LCD_DrawFillRect(48 + (16 * 3), 40, 8, 8);
	//LCD_DrawRectangle(48 + (16 * 2), 40, 8, 8);
	for (i = 4; i < 5; i++)
	{
		//void LCD_DrawFillRect(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
		//LCD_DrawRectangle(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
		LCD_DrawFillRect(48 + (16 * i), 40, 8, 8);
		LCD_DrawRectangle(48 + (16 * i), 40, 8, 8);
		//  네모i : B  1  2  3  4  5  6
	}


	LCD_SetBrushColor(GET_RGB(255, 0, 0));  //이후 생성된 네모들은 빨간색으로 지정




}

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

void BEEP(void)			/* beep for 30 ms */
{
	GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
	DelayMS(70);		// Delay 30 ms
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