/*********************1_HW6_2015134022_������(���� 2020.05.04)*********************/
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

#define Arrow_Center_X_Left 10
#define Arrow_Center_X_Right 30
#define Arrow_Center_Y 20
#define Arrow_WING_X 20
#define Arrow_TOP_Y 15
#define Arrow_BOTTOM_Y 25

/**************�ܺ��Լ� ����***************************/
void _GPIO_Init(void);
void _EXTI_Init(void);

uint16_t KEY_Scan(void);
void BEEP(void);

void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

/**************�������� ����***************************/
uint8_t	Arrival_6_Flag = 0;         // SW7�Է� ���� ���� ���� ���� (�ʱⰪ�� 0���� ����)
uint16_t blue_pointy = 88, red_pointy = 88; // ����(�Ķ�)�� ������(����)�� ���¹� y��ǥ ���� ���� (�ʱⰪ�� 88�� ����)
uint16_t blue_flag = 1, red_flag = 1;           // ����(�Ķ�)�� ������(����)�� ���� ���� �����ϱ� ���� ���������� (�ʱⰪ�� 1������ ����)
// LCD�� �簢���� �׸� �� �簢���� ���̸� ����ϱ� ���� ���
uint16_t count = 0;             // �� ���� Ƚ���� Ȧ������ ¦������ �����ϴ� ���� (0�̸� �ʱ����, 1�̸� Ȧ��, 2�̸� ¦��)
uint16_t status = 0;            // ���� �����ϴ� ���������Ͱ� �������� ���������� ���¸� �����ϴ� ���� (0�̸� �Ķ���, 1�̸� ���������� ���ڻ� ����)

char status1 = 'L', status2 = 'S', start = '1', end = '1', left_floor = '1', right_floor = '1';    //  LCD ��� ���� ���� ����
// status1�� ���� ���������Ͱ� �������� ���������� ���� (�����̸� 'L', �������̸� 'R')
// status2�� ���� ���������Ͱ� � ������ �����ϰ� �ִ��� ���� (���������̸� 'S', �ö󰡰� ������ 'U', �������� ������ 'D')
// start�� ������ ����� ����, end�� ������ ��ǥ�� ����
// left_floor, right_floor�� ���ʰ� �������� ���� ���� �����ϱ� ���� ������ ���� (start, end�� ���ϱ� ���� ���)

char *mode = "FL";      // LCD ��� ���ڿ� ���� ����
// ���� ��带 ���� (������ ���� "FL", ���� ���� "M", �ߴ� ���� "HL" ����,,, �ʱⰪ�� "FL"�� ����)
uint8_t SW_X;
unsigned char MODE = 'S';
unsigned char Current_F = 1, Target_F = 1, Moving_F = 1;
uint8_t Room;
int i, j;


/**************�����Լ� ����***************************/
void SelectMode(void);          // ������ ��� ���� �Լ�
void Floor_Calculation(void);
void Draw_Arrow(void);

/********************************���� �Լ�******************************/
int main(void)
{
	///////�����Լ� �ʱ�ȭ/////////////////////////////////////////////////
	_GPIO_Init(); // GPIO (LED & SW) �ʱ�ȭ
	_EXTI_Init();   // EXTI(�ܺ� ���ͷ�Ʈ) �ʱ�ȭ
	LCD_Init();	// LCD ��� �ʱ�ȭ
	DelayMS(100);
	Fram_Init();                    // FRAM �ʱ�ȭ H/W �ʱ�ȭ
	// Fram_Status_Config();   // FRAM �ʱ�ȭ S/W �ʱ�ȭ

	GPIOG->ODR |= 0x0002;	// �ʱⰪ: LED1 ON (1��)
	DisplayInitScreen();	// LCD �ʱ�ȭ��


	///////�����Լ� ���ѷ���/////////////////////////////////////////////////
	while (1)
	{
		SelectMode();

	}
}


void SelectMode(void)           // ������ ��� ���� �Լ�
{
	switch (KEY_Scan())
	{
	case SW0_PUSH: 	//SW1�� ������ break���� ���� //0xFD00
		//GPIOG->ODR &= ~0x007E;          // LED0�� ������ ��� LED OFF   
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0001; //GPIO_Pin_0  LED0 ON

		BEEP();     // ���� 1�� �︲

		Target_F = 0;
		LCD_DisplayChar(2, 3, 'B');
		break;

	case SW1_PUSH: 	//SW1�� ������ break���� ���� //0xFD00
		//GPIOG->ODR &= ~0x007E;          // LED0�� ������ ��� LED OFF   
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0002; //GPIO_Pin_1  LED1 ON

		BEEP();     // ���� 1�� �︲

		Target_F = 1;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;

	case SW2_PUSH: 	//SW2�� ������ break���� ���� //0xFB00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0004; //GPIO_Pin_2  LED2 ON


		// DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
		BEEP();     // ���� 1�� �︲

		Target_F = 2;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;

	case SW3_PUSH: 	//SW3�� ������ break���� ���� //0xF700
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0008; //GPIO_Pin_3  LED3 ON


		BEEP();     // ���� 1�� �︲

		Target_F = 3;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;

	case SW4_PUSH: 	//SW4�� ������ break���� ���� //0xEF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0010; //GPIO_Pin_4  LED4 ON

		BEEP();     // ���� 1�� �︲

		Target_F = 4;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;

	case SW5_PUSH: 	//SW5�� ������ break���� ���� //0xDF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0020; //GPIO_Pin_5  LED5 ON


		BEEP();     // ���� 1�� �︲

		Target_F = 5;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;

	case SW6_PUSH: 	//SW6�� ������ break���� ���� //0xBF00
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0040; //GPIO_Pin_6  LED6 ON

		BEEP();     // ���� 1�� �︲

		Target_F = 6;
		LCD_DisplayChar(2, 3, Target_F + 0x30);
		break;
	}
}

void Draw_Arrow(void)
{
	LCD_SetPenColor(GET_RGB(0, 0, 255));
	for (j = -1; j < 2; j++)
	{
		LCD_DrawLine(Arrow_Center_X_Left, Arrow_Center_Y + j, Arrow_Center_X_Right, Arrow_Center_Y + j);
		if (Current_F > Target_F)  ////'<-' <-' <-' <-' <-' <-' <-' 
		{
			LCD_DrawLine(Arrow_Center_X_Left + j, Arrow_Center_Y, Arrow_WING_X + j, Arrow_TOP_Y);
			LCD_DrawLine(Arrow_Center_X_Left + j, Arrow_Center_Y, Arrow_WING_X + j, Arrow_BOTTOM_Y);
		}

		else ////'->' '->' '->' '->' '->' '->' '->' 
		{
			LCD_DrawLine(Arrow_WING_X + j, Arrow_TOP_Y, Arrow_Center_X_Right + j, Arrow_Center_Y);
			LCD_DrawLine(Arrow_WING_X + j, Arrow_BOTTOM_Y, Arrow_Center_X_Right + j, Arrow_Center_Y);
		}
	}
}

void Floor_Calculation(void)
{
	if (Current_F > Target_F)//�������� ��ǥ�� ���� ũ��. [�ϰ�]
	{


		LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
		LCD_DrawFillRect(0, 40, 500, 8 + 1); //���������� ��ġ �ʱ�ȭ



		while (Current_F > Target_F) //�������� ��ǥ������ ������ ũ�ٸ�
		{
			Moving_F = Current_F - 1;
			LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE

			LCD_DrawRectangle(48 + (16 * (Moving_F)+3), 40, 16 * (Current_F - Moving_F), 8);
			LCD_DrawFillRect(48 + (16 * (Moving_F)+3), 40, 16 * (Current_F - Moving_F), 8);
			Current_F--;
			DelayMS(100);
		}

		LCD_SetBrushColor(GET_RGB(0, 0, 255)); //BLUE
		LCD_DrawFillRect(48 + (16 * (Target_F)+1), 40, 8, 8);
		LCD_DrawRectangle(48 + (16 * (Target_F)+1), 40, 8, 8);
		Current_F = Target_F;

	}
	else if (Current_F < Target_F)//�������� ��ǥ������ �۴�. [���]
	{
		////'->' '->' '->' '->' '->' '->' '->' '->' '->' '->' '->' '->' '->' '->' '->' '->' ////////
		LCD_SetPenColor(GET_RGB(255, 0, 0));
		for (j = -3; j < 8; j++)
		{
			LCD_DrawLine(Arrow_Center_X_Left, Arrow_Center_Y + j, Arrow_Center_X_Right, Arrow_Center_Y + j);
			LCD_DrawLine(Arrow_WING_X, Arrow_TOP_Y + j, Arrow_Center_X_Right, Arrow_Center_Y + j);
			LCD_DrawLine(Arrow_WING_X, Arrow_BOTTOM_Y + j, Arrow_Center_X_Right, Arrow_Center_Y + j);
		}
		LCD_SetPenColor(GET_RGB(0, 0, 0));

		LCD_SetBrushColor(GET_RGB(255, 255, 0)); //YELLOW
		LCD_DrawFillRect(0, 40, 500, 8 + 1); //���������� ��ġ �ʱ�ȭ



		while (Current_F < Target_F)
		{
			Moving_F = Current_F + 1;
			LCD_SetBrushColor(GET_RGB(255, 0, 0)); //RED

			LCD_DrawRectangle(48 + (16 * (Current_F)+1), 40, 16 * (Moving_F - Current_F), 8);
			LCD_DrawFillRect(48 + (16 * (Current_F)+1), 40, 16 * (Moving_F - Current_F), 8);
			Current_F++;
			DelayMS(100);
		}

		LCD_SetBrushColor(GET_RGB(255, 0, 0)); //RED
		LCD_DrawFillRect(48 + (16 * (Target_F)+1), 40, 8, 8);
		LCD_DrawRectangle(48 + (16 * (Target_F)+1), 40, 8, 8);
		Current_F = Target_F;
	}
}
void _GPIO_Init(void)
{
	// LED (GPIO G) ����
	RCC->AHB1ENR |= 0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER |= 0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER &= ~0x000000FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
	GPIOG->OSPEEDR |= 0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 

	// SW (GPIO H) ���� 
	RCC->AHB1ENR |= 0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER &= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR &= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) ���� 
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

	//SYSCFG->EXTICR[2] |= 0x0007; 	// EXTI8�� ���� �ҽ� �Է��� GPIOH�� ���� (EXTICR3) (reset value: 0x0000)	
	SYSCFG->EXTICR[3] |= 0x7000; 	// EXTI15�� ���� �ҽ� �Է��� GPIOH�� ���� (EXTICR4) (reset value: 0x0000)

	EXTI->FTSR |= 0x00008000;       // EXTI15 Falling Trigger Enable  (EXTI8:PH8 || EXTI15:PH15)
	EXTI->IMR |= 0x00008000;  	// EXTI15 ���ͷ�Ʈ mask (Interrupt Enable)

	//NVIC->ISER[0] |= ( 1 << 23  );   // Enable Interrupt EXTI8 Vector table Position ����
	NVIC->ISER[1] |= (1 << 8);   // Enable Interrupt EXTI15 Vector table Position ����
}

void EXTI15_10_IRQHandler(void)		// EXTI 10~15 ���ͷ�Ʈ �ڵ鷯
{
	if (EXTI->PR & 0x8000)              // EXTI15 Interrupt Pending register (EXTI_PR)
	{//SW7      
		EXTI->PR |= 0x8000; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0080; //GPIO_Pin_7  LED7 ON

		Floor_Calculation();

	}
}

//int y_point = 2;


void DisplayInitScreen(void)    // LCD ȭ�� ��� �Լ�
{
	LCD_Clear(RGB_YELLOW);		// ȭ�� Ŭ����
	LCD_SetFont(&Gulim8);		// ��Ʈ : ���� 8
	LCD_SetBackColor(RGB_YELLOW);	// ���ڹ��� : WHITE
	LCD_SetTextColor(RGB_BLACK);	// ���ڻ� : BLACK
	LCD_DisplayText(0, 0, "2015134022 SJH");  	// �й� �̸�
	LCD_DisplayText(1, 0, "S    6No:");  	//'��������'��     '6�� ����Ƚ��'

	LCD_SetTextColor(GET_RGB(235, 0, 255));	// ���ڻ� : ViOLET
	LCD_DisplayText(1, 2, "O");  	//'��������' ǥ�� : O
	LCD_DisplayText(1, 8, "0");  	//'6�� ����Ƚ��' ǥ�� : 0
	//LCD_DisplayChar(1, 8, Fram_Read(608) + 0x30);        //FRAM 608���� ����� data(1byte) �о� LCD�� ǥ�� 


	LCD_SetTextColor(RGB_BLACK);	// ���ڻ� : BLACK
	LCD_DisplayText(2, 0, "FL:   B 1 2 3 4 5 6");  	// ROOM NO.ǥ��
	LCD_DisplayChar(2, 3, Current_F + 0x30);


	LCD_SetPenColor(GET_RGB(0, 0, 0));
	LCD_SetBrushColor(GET_RGB(0, 0, 255));
	LCD_SetBrushColor(GET_RGB(255, 0, 0));

	//void LCD_DrawFillRect(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
	//LCD_DrawRectangle(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
	LCD_DrawFillRect(48 + (16 * (Current_F + 1)), 40, 8, 8);
	LCD_DrawRectangle(48 + (16 * (Current_F + 1)), 40, 8, 8);

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
	DelayMS(10);		// Delay 30 ms
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