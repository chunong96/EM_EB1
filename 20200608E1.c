/*
******************************************************************************
* @제  목    : 1_PR1_2015134022_송진한
* @제출일    : 2020. 06. 14.
******************************************************************************
* @기능설명
SW0~ SW6 : 엘리베이터 층 설정  (SW누르면 FL에 가고자 하는 Target층 출력)
SW7 : EXTI15 와 연결되어있고 Target층으로 Current층부터 출발해서 현황을 출력
(단, Target층과 Current 층이 같을 시는 BEEP만 울리고 무동작)

Target층으로 도착시 BEEP 3번 울리고 Target층이 Current 층이 되어 대기

FRAM을 사용하여 6층에 도달한 횟수가 608번지에 저장됨.
(단 횟수가 9초과 시 0으로 돌아옴)
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "GLCD.h"
#include "FRAM.h"

/* 개인 define ---------------------------------------------------------------*/
#define SW0_PUSH        0xFE00  //PH8
#define SW1_PUSH        0xFD00  //PH9
#define SW2_PUSH        0xFB00  //PH10
#define SW3_PUSH        0xF700  //PH11
#define SW4_PUSH        0xEF00  //PH12
#define SW5_PUSH        0xDF00  //PH13
#define SW6_PUSH        0xBF00  //PH14
#define SW7_PUSH        0x7F00  //PH15

#define Arrow_Center_X_Left  12 //화살표 중앙선 좌측 X좌표
#define Arrow_Center_X_Right 28 //화살표 중앙선 우측 X좌표
#define Arrow_Center_Y       20 //화살표 중앙선      Y좌표
#define Arrow_WING_X         20 //화살표 삼각형 날개 X좌표
#define Arrow_TOP_Y          15 //화살표 삼각형 상측 Y좌표
#define Arrow_BOTTOM_Y       25 //화살표 삼각형 하측 Y좌표

/*외부함수 선언---------------------------------------------------------------*/
void _GPIO_Init(void);
void _EXTI_Init(void);

uint16_t KEY_Scan(void);
void BEEP(void);
void Do_BEEP(void);
void Mi_BEEP(void);
void So_BEEP(void);

void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

/*전역변수 선언---------------------------------------------------------------*/
uint8_t	Arrival_6_Flag = 0;         // 6층 도달 횟수 저장 변수 선언 (초기값은 0으로 설정)
uint8_t Down_Flag = 0;              //최근에 동작한 입력이 하강일 때 파란 네모를 유지하기 위한 Flag

unsigned char Current_F = 1, Target_F = 1, Moving_F = 1;
//////////////현재층/////////목표층////////움직일 때의 현재층(임시저장용)

int i, j; //반복에 사용한 변수

/*내부함수 선언---------------------------------------------------------------*/
void SelectMode(void);          // 층 선택 함수
void Floor_Calculation(void);   // 층 연산 동작 함수
void Draw_Arrow(void);          // 두꺼운 화살표 그리는 함수

/********************************메인 함수 시작************************ *******/
int main(void)
{
	///////메인함수 초기화/////////////////////////////////////////////////
	_GPIO_Init(); // GPIO (LED & SW) 초기화
	_EXTI_Init();   // EXTI(외부 인터럽트) 초기화
	LCD_Init();	// LCD 모듈 초기화
	DelayMS(400);
	Fram_Init();                // FRAM 초기화 H/W 초기화
	Fram_Status_Config();   // FRAM 초기화 S/W 초기화

	GPIOG->ODR |= 0x0008;	// 초기값: LED1 ON (3층)
	DisplayInitScreen();	// LCD 초기화면

	if (Fram_Read(608) >= 1 || Fram_Read(608) <= 9) //FRAM을 읽어와서 Arrival_6_Flag에 담는다.
	{
		Arrival_6_Flag = Fram_Read(608);
	}
	else //오작동 시 FRAM 608 번지에 0을 저장해주는 보험
	{
		Fram_Write(608, Arrival_6_Flag);
	}

	///////메인함수 무한루프////////////////////////////////////////////////
	while (1)
	{
		SelectMode(); //층 선택 함수(키 스캔_스위치)
	}
}
/********************************메인 함수 끝**********************************/

/*사용자 정의 함수 시작-------------------------------------------------------*/
void SelectMode(void)           // 층선택 모드 실행 함수
{
	switch (KEY_Scan())
	{
	case SW0_PUSH: 	//SW0이 눌리면 break까지 실행 //0xFE00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0001; //GPIO_Pin_0  LED0 ON

		BEEP();     // 부저 1번 울림

		Target_F = 0;
		LCD_DisplayChar(2, 3, 'B');
		break;


	case SW1_PUSH: 	//SW1이 눌리면 break까지 실행 //0xFD00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0002; //GPIO_Pin_1  LED1 ON

		BEEP();     // 부저 1번 울림

		Target_F = 1;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;


	case SW2_PUSH: 	//SW2이 눌리면 break까지 실행 //0xFB00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0004; //GPIO_Pin_2  LED2 ON

		// DisplayInitScreen();      // 변경된 정보로 LCD 초기화 후 재출력
		BEEP();     // 부저 1번 울림

		Target_F = 2;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;


	case SW3_PUSH: 	//SW3이 눌리면 break까지 실행 //0xF700
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0008; //GPIO_Pin_3  LED3 ON

		BEEP();     // 부저 1번 울림

		Target_F = 3;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;


	case SW4_PUSH: 	//SW4이 눌리면 break까지 실행 //0xEF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0010; //GPIO_Pin_4  LED4 ON

		BEEP();     // 부저 1번 울림

		Target_F = 4;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;


	case SW5_PUSH: 	//SW5이 눌리면 break까지 실행 //0xDF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0020; //GPIO_Pin_5  LED5 ON

		BEEP();     // 부저 1번 울림

		Target_F = 5;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;


	case SW6_PUSH: 	//SW6이 눌리면 break까지 실행 //0xBF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0040; //GPIO_Pin_6  LED6 ON

		BEEP();     // 부저 1번 울림

		Target_F = 6;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;
	}
}

void Draw_Arrow(void) //두꺼운 화살표를 그려주는 함수
{
	LCD_DisplayText(1, 0, "M"); //현재 동작 : MOVE 중
	LCD_SetBrushColor(GET_RGB(255, 255, 0)); //노란 붓 지정
	LCD_DrawFillRect(10, 12, 20, 14); //화살표 자리 청소시켜주는 네모

	for (j = -1; j < 2; j++) //화살표 두께 : 3
	{
		if (Current_F > Target_F)  ////'<-' <-' <-' <-' <-' <-' <-' 
		{
			LCD_SetPenColor(GET_RGB(0, 15, 255)); //파랑
			LCD_DrawLine(Arrow_Center_X_Left, Arrow_Center_Y + j, Arrow_Center_X_Right, Arrow_Center_Y + j); // 'ㅡ'
			LCD_DrawLine(Arrow_Center_X_Left + j, Arrow_Center_Y, Arrow_WING_X + j, Arrow_TOP_Y); //  '<'의 윗 부분
			LCD_DrawLine(Arrow_Center_X_Left + j, Arrow_Center_Y, Arrow_WING_X + j, Arrow_BOTTOM_Y); //  '<'의 아랫 부분
		}

		else ////'->' '->' '->' '->' '->' '->' '->' 
		{
			LCD_SetPenColor(GET_RGB(255, 0, 0)); //빨강빨강
			LCD_DrawLine(Arrow_Center_X_Left, Arrow_Center_Y + j, Arrow_Center_X_Right, Arrow_Center_Y + j); // 'ㅡ'
			LCD_DrawLine(Arrow_WING_X + j, Arrow_TOP_Y, Arrow_Center_X_Right + j, Arrow_Center_Y);  //  '<'의 윗 부분
			LCD_DrawLine(Arrow_WING_X + j, Arrow_BOTTOM_Y, Arrow_Center_X_Right + j, Arrow_Center_Y);  //  '<'의 아랫 부분
		}
	}
}

void Floor_Calculation(void)  // 층 연산 동작 함수
{
	if (Current_F != Target_F) //현재층과 목표층이 다를 때
	{
		if (Current_F > Target_F)//현재층이 목표층 보다 크다. [하강]
		{
			Draw_Arrow(); //두꺼운 화살표를 그려주는 함수

			LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
			LCD_DrawFillRect(0, 40, 500, 8 + 1); //엘리베이터 위치 초기화

			LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE
			LCD_DrawFillRect(32 + (16 * (Current_F + 1)), 40, 8, 8);
			//LCD_DrawRectangle(32 + (16 * (Current_F + 1)), 40, 8, 8);
			while (Current_F > Target_F) //현재층이 목표층보다 아직도 크다면
			{
				Moving_F = Current_F - 1; //엘리베이터가 아래쪽으로 이동 중인 층 상태 저장
				LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE

				//LCD_DrawRectangle(48 + (16 * (Moving_F)), 40, 16 * (Current_F - Moving_F), 8);
				LCD_DrawFillRect(48 + (16 * (Moving_F)), 40, 16 * (Current_F - Moving_F), 8);
				Current_F--;
				DelayMS(150);
			}

			Down_Flag = 1;

			Current_F = Target_F; //전에 눌렀던 목표층이 현재층이 됨.
		}

		else if (Current_F < Target_F)//현재층이 목표층보다 작다. [상승]
		{
			Draw_Arrow(); //두꺼운 화살표를 그려주는 함수
			LCD_SetPenColor(GET_RGB(0, 0, 0));

			LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
			LCD_DrawFillRect(0, 40, 500, 8 + 1); //엘리베이터 위치 초기화

			while (Current_F < Target_F)
			{
				Moving_F = Current_F + 1; //엘리베이터가 위 쪽으로 이동 중인 층 상태 저장
				LCD_SetBrushColor(GET_RGB(255, 0, 0)); //RED

				//LCD_DrawRectangle(48 + (16 * (Current_F)), 40, 16 * (Moving_F - Current_F), 8);
				LCD_DrawFillRect(48 + (16 * (Current_F)), 40, 16 * (Moving_F - Current_F), 8);
				Current_F++;
				DelayMS(150);
			}

			LCD_SetBrushColor(GET_RGB(255, 0, 0)); //RED
			LCD_DrawFillRect(32 + (16 * (Current_F + 1)), 40, 8, 8);
			//LCD_DrawRectangle(32 + (16 * (Current_F + 1)), 40, 8, 8);

			Current_F = Target_F;  //전에 눌렀던 목표층이 현재층이 됨.

			//6층 도달한 횟수를 FRAM 608번지에 입력 시작-----------//
			if (Current_F == 6)
			{
				Arrival_6_Flag++;

				if (Arrival_6_Flag == 10) // 도달 횟수 9 초과 시 0으로 돌아감
				{
					Arrival_6_Flag = 0;
				}

				Fram_Write(608, Arrival_6_Flag); //FRAM 608 번지에 Arrival_6_Flag 입력 
			}
			//6층 도달한 횟수를 FRAM 608번지에 입력 끝--------------//
		}

		//도착 3음------------------//
		Mi_BEEP();
		DelayMS(200); //0.5초간 대기
		So_BEEP();
		DelayMS(200); //0.5초간 대기
		Do_BEEP();
	}

	DisplayInitScreen(); //층 선택 모드로 초기화
}
/*사용자 정의 함수 끝---------------------------------------------------------*/


/*초기화 함수 시작------------------------------------------------------------*/
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

	EXTI->FTSR |= 0x00008000;       // EXTI15 Falling Trigger Enable  (EXTI15:PH15)
	EXTI->IMR |= 0x00008000;  	// EXTI15 인터럽트 mask (Interrupt Enable)

	//NVIC->ISER[0] |= ( 1 << 23  );   // Enable Interrupt EXTI8 Vector table Position 참조
	NVIC->ISER[1] |= (1 << 8);   // Enable Interrupt EXTI15 Vector table Position 참조
}

void EXTI15_10_IRQHandler(void)		// EXTI 10~15 인터럽트 핸들러
{
	if (EXTI->PR & 0x8000)              // EXTI15 Interrupt Pending register (EXTI_PR)
	{//SW7      
		EXTI->PR |= 0x8000; 		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)

		//GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All 모든 LED OFF
		GPIOG->ODR |= 0x0080; //GPIO_Pin_7  LED7 ON

		if (Current_F == Target_F) //현재 층과 목표 층이 같다면 BEEP
		{
			BEEP();
		}

		Floor_Calculation(); // 층 연산 동작 함수
	}
}

void DisplayInitScreen(void)    // LCD 화면 출력 함수
{
	LCD_Clear(RGB_YELLOW);		// 화면 클리어
	GPIOG->ODR &= ~0x0080;          //GPIO_Pin_7 LED7 OFF
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_YELLOW);	// 글자배경색 : WHITE
	LCD_SetTextColor(RGB_BLACK);	// 글자색 : BLACK
	LCD_DisplayText(0, 0, "2015134022 SJH");  	// 학번 :2015134022 이름: 송진한
	LCD_DisplayText(1, 0, "S    6No:");  	//'정지상태'와     '6층 도달횟수'

	LCD_SetTextColor(GET_RGB(235, 0, 255));	// 글자색 : ViOLET

	LCD_SetBrushColor(GET_RGB(255, 255, 0)); // 붓 색 : 노랑
	LCD_DrawFillRect(10, 12, 20, 14);//화살표 지우개
	LCD_DisplayText(1, 2, "O");  	//'정지상태' 표시 : O

	//LCD_DisplayChar(1, 9, Arrival_6_Flag + 0x30);  	//'6층 도달횟수' 표시 : 0
	LCD_DisplayChar(1, 9, Fram_Read(608) + 0x30);        //FRAM 608번지 저장된 data(1byte) 읽어 LCD에 표시 



	LCD_SetTextColor(RGB_BLACK);	// 글자색 : BLACK
	LCD_DisplayText(2, 0, "FL:   B 1 2 3 4 5 6");  	// ROOM NO.표시

	if (Current_F == 0)
		LCD_DisplayChar(2, 3, 'B');
	else
		LCD_DisplayChar(2, 3, Current_F + 0x30);


	LCD_SetPenColor(GET_RGB(0, 0, 0));
	if (Down_Flag == 1) //최근 동작이 하강이였을 때 파랑 네모 출력
	{
		LCD_SetBrushColor(GET_RGB(0, 0, 255));
		Down_Flag = 0;
	}
	else ////최근 동작이 상승이였을 때 빨간 네모 출력
	{
		LCD_SetBrushColor(GET_RGB(255, 0, 0));
	}
	//void LCD_DrawFillRect(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
	//LCD_DrawRectangle(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
	LCD_DrawFillRect(32 + (16 * (Current_F + 1)), 40, 8, 8);
	LCD_DrawRectangle(32 + (16 * (Current_F + 1)), 40, 8, 8);

}
/*초기화 함수 끝--------------------------------------------------------------*/



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
	DelayMS(30);		// Delay 30 ms
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
///////////////////////////////////////////////////////////////////////////////

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