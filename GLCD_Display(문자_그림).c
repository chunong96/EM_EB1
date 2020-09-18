#include "stm32f4xx.h"
#include "GLCD.h"
//임베디드 프로그램은 레지스터들을 기반으로 하는 프로그램이다. 레지스터는 하드웨어 가장 바탕이 되는 모듈이다.
//모든게 레지스터로 관리하게끔 되어있다. 예를들어 GPIOG를 내 용도에 맞게끔 변화를 시켜주어야 한다. 어떤 명령을 주어야 내가 원하는 대로 바뀌는 지 알수 있다.
//GLCD는 GPIO와는 다르다. MCU안에 GPIO가 있지만 GLCD는 MCU의 바깥에 있다. MCU안에 FSMC 모듈이 있고 그 모듈이 GLCD와 같은 외부모듈을 연결해준다.
//FSMC들을 레지스터로 설정해주어야 하는데 그 설정부분은 1학기는 생략하였다. (라이브러리로 미리 만들어놓았다. GLCD회사에서)
//그 라이브러리를 GUI로 미리 만들어 놓았다. 그리고 그 함수들은 void DisplayInitScreen(void);

void _GPIO_Init(void);
uint16_t KEY_Scan(void);

void BEEP(void);
void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

uint8_t	SW0_Flag, SW1_Flag; 	// Switch 입력이 홀수번째인지? 짝수번째인지? 를 알기위한 변수 

int main(void)
{
	_GPIO_Init(); 		// GPIO (LED, SW, Buzzer) 초기화
	LCD_Init();		// LCD 모듈 초기화
	DelayMS(100);
	BEEP();

	GPIOG->ODR &= ~0x00FF;	// LED 초기값: LED0~7 Off
	DisplayInitScreen();	// LCD 초기화면
 
	/* 문자표시용으로 많이 사용하는 함수들 */ 
	//void LCD_DrawHorLine(UINT16 x, UINT16 y, UINT16 width)  
	//void LCD_DrawVerLine(UINT16 x, UINT16 y, UINT16 height)
	//void LCD_DrawRectangle(UINT16 x, UINT16 y, UINT16 width, UINT16 height)  
	//void LCD_DrawFillRect(UINT16 x, UINT16 y, UINT16 width, UINT16 height)  
	//void LCD_DrawLine(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2)
	
        /* GLCD.c에 정의되어있다.
        void LCD_DisplayChar(UINT16 Row, UINT16 Col, char ch)
        {
              LCD_DrawChar(LCD_COL(Col), LCD_ROW(Row), ch);
        }

          void LCD_DisplayText(UINT16 Row, UINT16 Col, char *str)
          {
                LCD_DrawText(LCD_COL(Col), LCD_ROW(Row), str);
          }
          */
	while(1)
	{
          LCD_SetTextColor(RGB_BLACK);	// 글자색 : Black
          LCD_DisplayText(8, 9, "A");
          LCD_DisplayText(9, 10, "B");
          LCD_DisplayText(10, 10, "C");
          LCD_DisplayText(11, 17, "D");
          
          
        
		switch(KEY_Scan())	// 입력된 Switch 정보 분류 
        	{
        		case 0xFE00 : 	//SW0
				if (SW0_Flag==0) {
					GPIOG->ODR |= 0x0001; 		// LED0 ON		
                 			LCD_DisplayText(1,5,"ON ");	// Text(String) "ON " 표시
					SW0_Flag = 1;
				}
				else {
					GPIOG->ODR &= ~0x0001; 		// LED0 OFF
					LCD_DisplayText(1,5,"OFF");	// Text(String) "OFF" 표시
					SW0_Flag = 0;
				}
	        	break;
        		case 0xFD00 : 	//SW1
				if (SW1_Flag==0) {
					GPIOG->BSRRL = 0x0002; 		// LED1 ON		
                 			LCD_DisplayChar(2,5,'O');	// Charater 'O' 표시
					SW1_Flag = 1;
				}
				else {
					GPIOG->BSRRH = 0x0002; 		// LED1 OFF
					LCD_DisplayChar(2,5,0x58);  	// Charater 'X' 표시
									// 'X'=0x58 (ASCII code)
					SW1_Flag = 0;
				}	
	        	break;
         	}  // switch(KEY_Scan())
	}  // while(1)
}

/* GPIO (GPIOG(LED), GPIOH(Switch), GPIOF(Buzzer)) 초기 설정	*/
void _GPIO_Init(void)
{
      	// LED (GPIO G) 설정
    	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
 	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
    
	// SW (GPIO H) 설정 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 
    	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER 	&= ~0x0200;	// GPIOF 9 : Push-pull  	
 	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 

}	

/* GLCD 초기화면 설정 함수 */
void DisplayInitScreen(void)
{
	LCD_Clear(RGB_WHITE);		// 화면 클리어
	//LCD_SetFont(&Gulim7);		// 폰트 : 굴림 8
        LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_GREEN);	// 글자배경색 : Green
	LCD_SetTextColor(RGB_BLACK);	// 글자색 : Black
	LCD_DisplayText(0,0,"GLCD Test");  	// Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
          
	LCD_DisplayText(1,0,"LED0 OFF");
	LCD_DrawChar(2,0, 48);
        
        
        uint8_t ch, y;
          for(ch=0; ch<8; ch++)
          {
            for(y=0; y<6; y++)
            {
              //LCD_DrawChar(y,1,ch+0x30);
              LCD_DisplayChar(y,1,ch+0x30);
            }
          }
          /*
          uint8_t d, y;
          y=1;
          for(d=7; d>=0; d--)
          {
            LCD_DisplayChar(y++,6,ch+0x30);
          }
          
          LCD_SetPen
            for(ch=0; ch<8; ch++)
            {
              LCD_DrawRectangle(20, 12*(ch+1), 8, 8);
            }
*/
          
        /*
        uint8_t ch;
        ch=0;
        LCD_DisplayChar(2,0,ch+0x30); //0을 출력
        LCD_DrawChar(2,0, 48); //0을 출력
        */

        /*
        void LCD_DisplayChar(UINT16 Row, UINT16 Col, char ch)
        {
              LCD_DrawChar(LCD_COL(Col), LCD_ROW(Row), ch);
        }

        void LCD_DisplayText(UINT16 Row, UINT16 Col, char *str)
        {
              LCD_DrawText(LCD_COL(Col), LCD_ROW(Row), str);
        }
        */
        
	/* 그리기용으로 많이 사용하는 함수들 */
  	//void LCD_SetPenColor(UINT32 Color); //펜색(그리기용)지정
 	//void LCD_DrawHorLine(UINT16 x, UINT16 y, UINT16 width)  // 수평선 그리기
	//void LCD_DrawVerLine(UINT16 x, UINT16 y, UINT16 height) // 수직선 그리기
 	//void LCD_DrawRectangle(UINT16 x, UINT16 y, UINT16 width, UINT16 height) // 사각형 그리기  
	//void LCD_DrawFillRect(UINT16 x, UINT16 y, UINT16 width, UINT16 height)  // 사각형(fille) 그리기
	//void LCD_DrawLine(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2)  // 수평선 그리기
    	
	/* 그리기 테스트용 문장들 */
        
 	LCD_SetPenColor(RGB_RED); 
	LCD_DrawHorLine(0, 60, 100); //빨간 가로선
	LCD_SetPenColor(RGB_BLUE);
	LCD_DrawVerLine(0, 60, 110); //(0, 60)에 110픽셀만큼 그었다.
	LCD_SetPenColor(RGB_RED);
	LCD_DrawRectangle(10, 70, 30, 20);
	LCD_SetBrushColor(RGB_GRAY);
	LCD_DrawFillRect(50, 70, 40, 30);
	LCD_SetPenColor(RGB_BLACK);  
	LCD_DrawLine(0, 60, 100, 100);
        LCD_SetBrushColor(GET_RGB(0,  255, 125));
	LCD_DrawFillRect(80, 80, 85, 85);
        LCD_SetBrushColor(RGB_YELLOW);
	LCD_DrawFillRect(160, 160, 170, 170);
        
}

/* Switch가 입력되었는지를 여부와 어떤 switch가 입력되었는지의 정보를 return하는 함수  */ 
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

/* Buzzer: Beep for 30 ms */
void BEEP(void)			
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
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
