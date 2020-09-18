#include "stm32f4xx.h"
#include "GLCD.h"
#include "FRAM.h"

void _GPIO_Init(void);
uint16_t KEY_Scan(void);

void BEEP(void);
void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
/////////////spi ���̺귯�� ����� �ٲٸ� �ȴ�.

uint8_t Cur_Sound;
//unsigned char
int main(void)
{
	_GPIO_Init(); 	// GPIO (LED,SW,Buzzer,Joy stick) �ʱ�ȭ
	LCD_Init();	// LCD ��� �ʱ�ȭ
	DelayMS(10);
	BEEP();
	Fram_Init();                    // FRAM �ʱ�ȭ H/W �ʱ�ȭ
	Fram_Status_Config();   // FRAM �ʱ�ȭ S/W �ʱ�ȭ

	DisplayInitScreen();    // LCD �ʱ�ȭ��


	GPIOG->ODR &= ~0x00FF;	// LED �ʱⰪ: LED0~7 Off
	LCD_DisplayChar(1, 10, Fram_Read(50) + 0x30); //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ��

        LCD_SetTextColor(RGB_RED);	// ���ڻ� : BLUE
        if(Fram_Read(601) == 'V')
        { //uint8_t Cur_Sound;
          Cur_Sound = 'V';
          LCD_DisplayChar(0, 10, 'V');        //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ�� 
          //LCD_DisplayChar(0, 10, Fram_Read(601) + 0x30);        //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ�� 
        }
        else if(Fram_Read(601) == 'N')
        {
          Cur_Sound = 'N';
          LCD_DisplayChar(0, 12, 'N');  
         // LCD_DisplayChar(2, 12, Fram_Read(601) + 0x30);        //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ�� 
        }
        else //�Ҹ����
        {
          Cur_Sound = 'S';
          LCD_DisplayChar(0, 12, 'S');  
          LCD_DisplayChar(2, 10, Fram_Read(601) + 0x30);        //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ�� 
        }
        
           // LCD_DisplayChar(2, 10, Fram_Read(601) + 0x30);        //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ�� 
	while (1)
	{
		
		switch (KEY_Scan())	// �Էµ� Switch ���� �з� 		
		{
			//        		case 0xFE00 : 	//SW0 �Է�
			//                        	Fram_Write(50,0);  // FRAM(0~8191) 50������ 0 ���� 
			//	        	break;


		case 0xFD00: 	//SW1 �Է�
		{
			if (Cur_Sound == 'S') //�Ҹ����
			{
				Cur_Sound = 'V';
                                LCD_SetTextColor(RGB_BLUE);	// ���ڻ� : BLUE
                                LCD_DisplayText(0, 10, "S");
                                LCD_SetTextColor(RGB_RED);	// ���ڻ� : BLUE
				LCD_DisplayText(0, 10, "V");

				Fram_Write(601, Cur_Sound);  // FRAM(0~8191) 601������ Cur_Sound�� �� ���� 
			}
			else if (Cur_Sound == 'V') //�������
			{
				Cur_Sound = 'N';
                                LCD_SetTextColor(RGB_BLUE);	// ���ڻ� : BLUE
                                LCD_DisplayText(0, 12, "V");
                                LCD_SetTextColor(RGB_RED);	// ���ڻ� : BLUE
				LCD_DisplayText(0, 12, "N");

				Fram_Write(601, Cur_Sound);  // FRAM(0~8191) 601������ Cur_Sound�� �� ���� 
			}
			else //�������
			{
				Cur_Sound = 'S';
                                LCD_SetTextColor(RGB_BLUE);	// ���ڻ� : BLUE
                                LCD_DisplayText(0, 8, "N");
                                LCD_SetTextColor(RGB_RED);	// ���ڻ� : BLUE
				LCD_DisplayText(0, 8, "S");

				Fram_Write(601, Cur_Sound);  // FRAM(0~8191) 601������ Cur_Sound�� �� ���� 
			}
			//Fram_Write(50,1);  // FRAM(0~8191) 50������ 1 ���� 
		}
		break;
		//S53 V56 N4E
		//        		case 0xFB00 : 	//SW2 �Է�
		//                        	Fram_Write(50,2);  // FRAM(0~8191) 50������ 2 ���� 
		//	        	break;

		case 0xF700: 	//SW3 �Է�
			//LCD_DisplayChar(2, 10, Fram_Read(601));        //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ�� 
                        LCD_DisplayChar(2, 10, Fram_Read(601) + 0x30);        //FRAM 50���� ����� data(1byte) �о� LCD�� ǥ�� 
			//   0x30: ���ڸ�  ASCII code(ȭ��ǥ�ù���)�� ��ȯ�ϱ� ���� �����ִ� ��   

			break;
		}  // switch(KEY_Scan())
	}  // while(1)
}

/* GPIO (GPIOG(LED), GPIOH(Switch), GPIOF(Buzzer), GPIOI(Joy stick)) �ʱ� ����	*/
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

	//Joy Stick SW(PORT I) ����
	RCC->AHB1ENR |= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER &= ~0x000FFC00;	// GPIOI 5~9 : Input mode (reset state)
	GPIOI->PUPDR &= ~0x000FFC00;	// GPIOI 5~9 : Floating input (No Pull-up, pull-down) (reset state)

}

/* GLCD �ʱ�ȭ�� ���� */
void DisplayInitScreen(void)
{
	LCD_Clear(RGB_YELLOW);		// ȭ�� Ŭ����
	LCD_SetFont(&Gulim8);		// ��Ʈ : ���� 8
	LCD_SetBackColor(RGB_YELLOW);	// ���ڹ��� : YELLOW


	//        LCD_DisplayText(0,0,"FRAM test");  // Title
	//        LCD_DisplayText(1,0,"Reset  :");  // subtitle
	//        LCD_DisplayText(2,0,"Current:");  // subtitle
	LCD_SetTextColor(RGB_BLUE);	// ���ڻ� : BLUE
	LCD_DisplayText(0, 0, "SOUND : S V N");  // Title
	LCD_DisplayText(1, 0, "BT: O");  // Title

	LCD_SetTextColor(RGB_RED);	// ���ڻ� : ����
	LCD_DisplayText(2, 4, "O");  // Title
	LCD_DisplayText(2, 8, "S");  // Title
	LCD_DisplayText(2, 9, "V");  // Title
	LCD_DisplayText(2, 10, "N");  // Title

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
