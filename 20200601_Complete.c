#include "stm32f4xx.h"
#include "GLCD.h"
#include "FRAM.h"

void _GPIO_Init(void);
uint16_t KEY_Scan(void);

void BEEP(void);
void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
/////////////spi 라이브러리 펑션을 바꾸면 된다.

uint8_t Cur_Sound;
//unsigned char
int main(void)
{
	_GPIO_Init(); 	// GPIO (LED,SW,Buzzer,Joy stick) 초기화
	LCD_Init();	// LCD 모듈 초기화
	DelayMS(10);
	BEEP();
	Fram_Init();                    // FRAM 초기화 H/W 초기화
	Fram_Status_Config();   // FRAM 초기화 S/W 초기화

	DisplayInitScreen();    // LCD 초기화면


	GPIOG->ODR &= ~0x00FF;	// LED 초기값: LED0~7 Off
	LCD_DisplayChar(1, 10, Fram_Read(50) + 0x30); //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시

        LCD_SetTextColor(RGB_RED);	// 글자색 : BLUE
        if(Fram_Read(601) == 'V')
        { //uint8_t Cur_Sound;
          Cur_Sound = 'V';
          LCD_DisplayChar(0, 10, 'V');        //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시 
          //LCD_DisplayChar(0, 10, Fram_Read(601) + 0x30);        //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시 
        }
        else if(Fram_Read(601) == 'N')
        {
          Cur_Sound = 'N';
          LCD_DisplayChar(0, 12, 'N');  
         // LCD_DisplayChar(2, 12, Fram_Read(601) + 0x30);        //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시 
        }
        else //소리모드
        {
          Cur_Sound = 'S';
          LCD_DisplayChar(0, 12, 'S');  
          LCD_DisplayChar(2, 10, Fram_Read(601) + 0x30);        //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시 
        }
        
           // LCD_DisplayChar(2, 10, Fram_Read(601) + 0x30);        //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시 
	while (1)
	{
		
		switch (KEY_Scan())	// 입력된 Switch 정보 분류 		
		{
			//        		case 0xFE00 : 	//SW0 입력
			//                        	Fram_Write(50,0);  // FRAM(0~8191) 50번지에 0 저장 
			//	        	break;


		case 0xFD00: 	//SW1 입력
		{
			if (Cur_Sound == 'S') //소리모드
			{
				Cur_Sound = 'V';
                                LCD_SetTextColor(RGB_BLUE);	// 글자색 : BLUE
                                LCD_DisplayText(0, 10, "S");
                                LCD_SetTextColor(RGB_RED);	// 글자색 : BLUE
				LCD_DisplayText(0, 10, "V");

				Fram_Write(601, Cur_Sound);  // FRAM(0~8191) 601번지에 Cur_Sound의 값 저장 
			}
			else if (Cur_Sound == 'V') //진동모드
			{
				Cur_Sound = 'N';
                                LCD_SetTextColor(RGB_BLUE);	// 글자색 : BLUE
                                LCD_DisplayText(0, 12, "V");
                                LCD_SetTextColor(RGB_RED);	// 글자색 : BLUE
				LCD_DisplayText(0, 12, "N");

				Fram_Write(601, Cur_Sound);  // FRAM(0~8191) 601번지에 Cur_Sound의 값 저장 
			}
			else //무음모드
			{
				Cur_Sound = 'S';
                                LCD_SetTextColor(RGB_BLUE);	// 글자색 : BLUE
                                LCD_DisplayText(0, 8, "N");
                                LCD_SetTextColor(RGB_RED);	// 글자색 : BLUE
				LCD_DisplayText(0, 8, "S");

				Fram_Write(601, Cur_Sound);  // FRAM(0~8191) 601번지에 Cur_Sound의 값 저장 
			}
			//Fram_Write(50,1);  // FRAM(0~8191) 50번지에 1 저장 
		}
		break;
		//S53 V56 N4E
		//        		case 0xFB00 : 	//SW2 입력
		//                        	Fram_Write(50,2);  // FRAM(0~8191) 50번지에 2 저장 
		//	        	break;

		case 0xF700: 	//SW3 입력
			//LCD_DisplayChar(2, 10, Fram_Read(601));        //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시 
                        LCD_DisplayChar(2, 10, Fram_Read(601) + 0x30);        //FRAM 50번지 저장된 data(1byte) 읽어 LCD에 표시 
			//   0x30: 숫자를  ASCII code(화면표시문자)로 변환하기 위해 더해주는 수   

			break;
		}  // switch(KEY_Scan())
	}  // while(1)
}

/* GPIO (GPIOG(LED), GPIOH(Switch), GPIOF(Buzzer), GPIOI(Joy stick)) 초기 설정	*/
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

	//Joy Stick SW(PORT I) 설정
	RCC->AHB1ENR |= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER &= ~0x000FFC00;	// GPIOI 5~9 : Input mode (reset state)
	GPIOI->PUPDR &= ~0x000FFC00;	// GPIOI 5~9 : Floating input (No Pull-up, pull-down) (reset state)

}

/* GLCD 초기화면 설정 */
void DisplayInitScreen(void)
{
	LCD_Clear(RGB_YELLOW);		// 화면 클리어
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_YELLOW);	// 글자배경색 : YELLOW


	//        LCD_DisplayText(0,0,"FRAM test");  // Title
	//        LCD_DisplayText(1,0,"Reset  :");  // subtitle
	//        LCD_DisplayText(2,0,"Current:");  // subtitle
	LCD_SetTextColor(RGB_BLUE);	// 글자색 : BLUE
	LCD_DisplayText(0, 0, "SOUND : S V N");  // Title
	LCD_DisplayText(1, 0, "BT: O");  // Title

	LCD_SetTextColor(RGB_RED);	// 글자색 : 빨강
	LCD_DisplayText(2, 4, "O");  // Title
	LCD_DisplayText(2, 8, "S");  // Title
	LCD_DisplayText(2, 9, "V");  // Title
	LCD_DisplayText(2, 10, "N");  // Title

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
