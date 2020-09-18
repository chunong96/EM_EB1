#include "stm32f4xx.h"
#include "GLCD.h"
////////������ : 2015134022 ������/////������ : 2020.05.29.///////////
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
	LCD_Init();	// LCD ��� �ʱ�ȭ
	DelayMS(10);
	_GPIO_Init();	// GPIO (LED & SW) �ʱ�ȭ
	_EXTI_Init();	// EXTI �ʱ�ȭ

	DisplayInitScreen();	// LCD �ʱ�ȭ��

	GPIOG->ODR &= ~0x00FF;		// �ʱⰪ: LED0~7 Off
	//GPIOG->ODR |= 0x0001;		// GPIOG->ODR.0 'H'(LED ON)

	while (1)
	{
		/*
	   1 - 3 - 4 - 5 - 6 - 7 - 0 ��й�ȣ ��Ż��

	   SW0     SW1    <> SW2[x]  SW3     SW4     SW5     SW6     SW7
	   EXTI8   EXTI9  <> EXTI10  EXTI11  EXTI3   EXTI4   EXTI5   EXTI6
	   0100    0200   <> 0400    0800    1000    2000    4000    8000
	   0       1      <>         3       4       5       6       7
	   [7��]  [1]��   <>        [2��]   [3��]   [4��]   [5��]   [6��]
	   */
	}
}

/* GLCD �ʱ�ȭ�� ���� �Լ� */
void DisplayInitScreen(void)
{
	LCD_Clear(RGB_WHITE);		// ȭ�� Ŭ����
	LCD_SetFont(&Gulim8);		// ��Ʈ : ���� 8
	LCD_SetBackColor(RGB_WHITE);	// ���ڹ��� : WHITE
	LCD_SetTextColor(RGB_BLUE);	// ���ڻ� : BLUE
	LCD_DisplayText(0, 0, "2015134022 SJH");  	// �й� �̸�


	LCD_SetTextColor(RGB_BLACK);	// ���ڻ� : BLACK
	LCD_DisplayText(1, 0, "R 0 1 3 4 5 6 7");  	// ROOM NO.ǥ��

	LCD_DisplayChar(2, 18, 'S'); //���� ��Ȳ ǥ�� : START


	//////////////���� �׵θ� && �Ͼ� �� ���簢�� 7��////////////////////////////
	LCD_SetPenColor(GET_RGB(0, 0, 0));
	LCD_SetBrushColor(GET_RGB(0xFF, 0xFF, 0xFF));
	for (i = 0; i < 7; i++)
	{
		//void LCD_DrawFillRect(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
		//LCD_DrawRectangle(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
		LCD_DrawFillRect(16 + (16 * i), 30, 8, 8);
		LCD_DrawRectangle(16 + (16 * i), 30, 8, 8);
		//  ����  : 0  1  3  4  5  6  7
		//  �׸�i : 0  1  2  3  4  5  6
	}


	LCD_SetBrushColor(GET_RGB(255, 0, 0));  //���� ������ �׸���� ���������� ����
}

/* GPIO (GPIOG(LED), GPIOH(Switch), GPIOF(Buzzer)) �ʱ� ����	*/
void _GPIO_Init(void)
{
	// LED (GPIO G) ����
	RCC->AHB1ENR |= 0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER |= 0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER &= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
	GPIOG->OSPEEDR |= 0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 

	// SW (GPIO H) ���� 
	RCC->AHB1ENR |= 0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER &= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR &= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) ���� 
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



/* EXTI (EXTI8(GPIOH.8, SW0), EXTI9(GPIOH.9, SW1)) �ʱ� ����  */
//SW2 GPIOH.10 EXTI10�� ���������� ������ ����.

/***** 1345670   *****/
void _EXTI_Init(void)
{//����ġ�� ������ ���� ����ġ�� ���ͷ�Ʈ �ҽ�����
	//PH15 PH8 �� ���� GPIO����� �Ἥ Ű��ĵ ��� Ǯ������ �޾Ƶ���
	//������ ���� EXTI ���ͷ�Ʈ�� �߻����Ѽ� �� Ű�� �ԷµǾ������� �˾Ƴ���.
	//����� Ű�� ���� �ȵ��ķ� �˼� �ִ�.

	RCC->AHB1ENR |= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->APB2ENR |= 0x00004000;	// Enable System Configuration Controller Clock         //�ý����ǱԸ�带 �Է����� ����
	//������ �ּ�ó���ϸ� SYSCFG->EXTICR[2] |= 0x0077; �� �۵��� ���Ѵ�.
	GPIOH->MODER &= ~0xFFFF0000;	// GPIOH PIN8~PIN15 Input mode (reset state)		//�Է��̶�� Ȯ���ϰ� ����.		 

	//SYSCFG->EXTICR[2] |= 0x0077; 	// EXTI8,9�� ���� �ҽ� �Է��� GPIOH�� ����      //�̰� �ſ� �߿���. ���ͷ�Ʈ PDF 40������ ����.
	// EXTI8 <- PH8, EXTI9 <- PH9               //PH�ϱ� 0111 0111�̶� 77�̴�.
	// reset value: 0x0000	

	/*********EXternal InTerrupt Configuration Register*************/
	SYSCFG->EXTICR[2] |= 0x7777; 	// EXTI8, EXTI9, EXTI10, EXTI11
	SYSCFG->EXTICR[3] |= 0x7777; 	// EXT12, EXTI13, EXTI14, EXTI15 

	/*********(Falling | Rising) trigger selection register*************/
	//EXTI->FTSR |= 0x000100;		// EXTI8: Falling Trigger Enable
	//|= (1<<8); //0b00000001 00000000 
	EXTI->FTSR |= 0x0000FF00;		//EXTI8~15 ���� Ʈ���� �̳��̺�
	//EXTI->FTSR |= 0x00000100;             // EXTI8: Falling Trigger Enable
	//EXTI->RTSR |= 0x00000200;		// EXTI9: Rising Trigger  Enable

	/*********Interrupt mask register*******************************/
	//EXTI->IMR  |= 0x0000FF00;  	// EXTI8~15���� ��ü ���ͷ�Ʈ mask (Interrupt Enable) ����
	//0100 + 0200 + 0400 + 0800 + 1000 + 2000 + 4000 + 8000 = FF00
	EXTI->IMR |= 0x00000200;       //EXTI8(SW0) ���ͷ�Ʈ�� ����ŷ
	//1345670 ��ȣ �� 1�� ���� �� �ְԲ� �ؾ��Ѵ�.

	/*
	SW0     SW1    <> SW2[x]  SW3     SW4     SW5     SW6     SW7
	EXTI8   EXTI9  <> EXTI10  EXTI11  EXTI3   EXTI4   EXTI5   EXTI6
	0100    0200   <> 0400    0800    1000    2000    4000    8000
	0       1      <>         3       4       5       6       7
	[7��]  [1]��   <>        [2��]   [3��]   [4��]   [5��]   [6��]
	*/

	/***CMSIS_NVIC  Nested Vectored Interrupt Controller (NVIC)**/
	//NVIC->ISER[]  Interrupt Set Enable Register
	//NVIC�� 32��Ʈ MCU�Ƿ� ISER[]�� 32���� ǥ�������ϴ�. RF-250p����
	NVIC->ISER[0] |= (1 << 23);        //0x0000 009C //0b 0000 0000 0000 0000  0000 0000 1001 1100
	// Enable EXTI Line[9:5] interrupts

	NVIC->ISER[1] |= (1 << (40 - 32)); //0x0000 00E0
	// Enable EXTI Line[15:10] interrupts


	//SW3 GPIOH.11 EXTI11 ����
	// SYSCFG->EXTICR[2] |= 0x7000; //EXTI11	
	//EXTI->FTSR |= (1<<11); // 0x000800;		 
	//EXTI->RTSR |= 0x000200;		
	//EXTI->IMR  |= 0x000800;  

	//SW4 GPIOH.12 EXTI12 ����
	//SYSCFG->EXTICR[3] |= 0x0007; //EXTI12	
	//SYSCFG->EXTICR[3] |= 0x7000; //EXTI15
	//EXTI->FTSR |= (1<<12); // 0x001000;		 
	//EXTI->RTSR |= (1<<12); // 0x001000;		 
	//EXTI->IMR  |= 0x008000;   //(IMR ���ִ°� ��� ���۹�ȣ ����ŷ) SW7
}

/********* EXTI5~9 ���ͷ�Ʈ �ڵ鷯(ISR: Interrupt Service Routine) ****************/
void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR & 0x0100)                   // EXTI8 Interrupt Pending register (EXTI_PR)
	{//SW0     
		EXTI->PR |= 0x0100; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
		//��� �������� ��Ʈ�� Ư���ϰ� 1�� RESET(����°�)�� 0�� SET(�������) Ư���� ������.

		LCD_DisplayChar(2, 18, '\x43'); //���� ��Ȳ ǥ�� : CLEAR

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0001; //GPIO_Pin_0  LED0 ON

		Escape_success(0); //0��° ��ȣ�ڽ� Ż�⼺��

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 ��ü ���ͷ�Ʈ ����ŷ ����
		//EXTI->IMR  |= 0x00000800;       //EXTI11(SW3) ���ͷ�Ʈ�� ����ŷ

		DelayMS(400); //1�ʰ� ���
		Mi_BEEP();
		DelayMS(200); //0.5�ʰ� ���
		So_BEEP();
		DelayMS(200); //0.5�ʰ� ���
		Do_BEEP();

		DelayMS(1200); //3�ʰ� ���
		BEEP();
		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		DisplayInitScreen(); //ȭ�� �ʱ�ȭ
		_EXTI_Init();         //���ͷ�Ʈ �ʱ�ȭ
	}

	else if (EXTI->PR & 0x0200) 		// EXTI9 Interrupt Pending register (EXTI_PR)
	{//SW1 
		EXTI->PR |= 0x0200; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)

		LCD_DisplayChar(2, 18, '\x57'); //���� ��Ȳ ǥ�� : WORK

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0002; //GPIO_Pin_1  LED1 ON

		Escape_success(1); //1��° ��ȣ�ڽ� Ż�⼺��

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 ��ü ���ͷ�Ʈ ����ŷ ����
		EXTI->IMR |= 0x00000800;       //EXTI11(SW3) ���ͷ�Ʈ�� ����ŷ
	}


}

/*
SW0     SW1    <> SW2[x]  SW3     SW4     SW5     SW6     SW7
EXTI8   EXTI9  <> EXTI10  EXTI11  EXTI3   EXTI4   EXTI5   EXTI6
0100    0200   <> 0400    0800    1000    2000    4000    8000
0       1      <>         3       4       5       6       7
[7��]  [1]��   <>        [2��]   [3��]   [4��]   [5��]   [6��]
*/


/********* EXTI10~15 ���ͷ�Ʈ �ڵ鷯(ISR: Interrupt Service Routine) ****************/
void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR & 0x0400) 		// EXTI10 Interrupt Pending register (EXTI_PR)
	{//SW2  ������ ȸ�� ����X
		EXTI->PR |= 0x0400; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
		//GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		//GPIOG->ODR |= 0x0004; //GPIO_Pin_2  LED2 ON
	}

	else if (EXTI->PR & 0x0800) 		// EXTI11 Interrupt Pending register (EXTI_PR)
	{//SW3  
		EXTI->PR |= 0x0800; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0008; //GPIO_Pin_3  LED3 ON

		Escape_success(2); //2��° ��ȣ�ڽ� Ż�⼺��

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 ��ü ���ͷ�Ʈ ����ŷ ����
		EXTI->IMR |= 0x00001000;       //EXTI12(SW4) ���ͷ�Ʈ�� ����ŷ        
	}

	else if (EXTI->PR & 0x1000)              // EXTI12 Interrupt Pending register (EXTI_PR)
	{//SW4  
		EXTI->PR |= 0x1000; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0010; //GPIO_Pin_4  LED4 ON

		Escape_success(3); //3��° ��ȣ�ڽ� Ż�⼺��

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 ��ü ���ͷ�Ʈ ����ŷ ����
		EXTI->IMR |= 0x00002000;       //EXTI12(SW4) ���ͷ�Ʈ�� ����ŷ
	}

	else if (EXTI->PR & 0x2000)               // EXTI13 Interrupt Pending register (EXTI_PR)
	{//SW5       
		EXTI->PR |= 0x2000; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0020; //GPIO_Pin_5  LED5 ON

		Escape_success(4); //4��° ��ȣ�ڽ� Ż�⼺��

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 ��ü ���ͷ�Ʈ ����ŷ ����
		EXTI->IMR |= 0x00004000;       //EXTI12(SW4) ���ͷ�Ʈ�� ����ŷ
	}

	else if (EXTI->PR & 0x4000)              // EXTI14 Interrupt Pending register (EXTI_PR)
	{//SW6      
		EXTI->PR |= 0x4000; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0040; //GPIO_Pin_6  LED6 ON

		Escape_success(5); //5��° ��ȣ�ڽ� Ż�⼺��

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 ��ü ���ͷ�Ʈ ����ŷ ����
		EXTI->IMR |= 0x00008000;       //EXTI12(SW4) ���ͷ�Ʈ�� ����ŷ
	}

	else if (EXTI->PR & 0x8000)              // EXTI15 Interrupt Pending register (EXTI_PR)
	{//SW7      
		EXTI->PR |= 0x8000; 		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)

		GPIOG->ODR &= ~0xFFFF; //GPIO_Pin_All ��� LED OFF
		GPIOG->ODR |= 0x0080; //GPIO_Pin_7  LED7 ON

		Escape_success(6); //6��° ��ȣ�ڽ� Ż�⼺��

		EXTI->IMR &= ~0x0000FF00;       //EXTI8~15 ��ü ���ͷ�Ʈ ����ŷ ����
		EXTI->IMR |= 0x00000100;       //EXTI8(SW0) ���ͷ�Ʈ�� ����ŷ               
	}
}

/* Switch�� �ԷµǾ������� ���ο� � switch�� �ԷµǾ������� ������ return�ϴ� �Լ�  */
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