#include "stm32f4xx.h"
#include "GLCD.h"

void _GPIO_Init(void);   //GPIO 초기설정 함수
void _EXTI_Init(void);   //인터럽트 초기설정 함수

uint16_t KEY_Scan(void);
void DisplayInitScreen(void);  //초기화면 구성 함수
void BEEP(void);  //비프음 울리는 함수
void BEEP3(void); // 비프음 3번 울리는 함수
void Floor(int floor);  // 층 선택을 위한 함수
void L_U(void);    //LCD에 L:U  표기 함수
void L_D(void);    //LCD에 L:D 표기 함수
void L_S(void);    //LCD에 L:S  표기 함수
void R_U(void);   //LCD에 R:U  표기 함수
void R_D(void);   //LCD에 R:D  표기 함수
void R_S(void);   //LCD에 R:S  표기 함수
void blue_clear(void); // blue elevator bar 그림 clear 함수
void red_clear(void);  // red elevator bar 그림 clear 함수
void to_ride_up_blue(void);  //blue elevator가 사람을 태우기 위해 올라가는 함수
void to_ride_down_blue(void);  //blue elevator가 사람을 태우기 위해 내려가는 함수
void after_ride_up_blue(void);   //blue elevator가 사람을 태운 후 목표층으로 올라가는 함수
void after_ride_down_blue(void);  //blue elevator가 사람을 태운 후 목표층으로 내려가는 함수
void to_ride_up_red(void);     //red elevator가 사람을 태우기 위해 올라가는 함수
void to_ride_down_red(void);  //red elevator가 사람을 태우기 위해 내려가는 함수
void after_ride_up_red(void);  //red elevator가 사람을 태운 후 목표층으로 올라가는 함수
void after_ride_down_red(void);  //red elevator가 사람을 태운 후 목표층으로 내려가는 함수

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

uint8_t  SW7_Flag;   //sw7을 인터럽트로 사용하기 위한 변수

int current_floor, goal_floor;  //출발층과 목표층을 저장할 변수
int elevator_blue=1, elevator_red=1; // 엘르베이터가 있는 층을 저장할 변수
int separate=0;  // 출발층 입력과 목표층 입력을 구분하기 위한 변수
int mode=0; // 모드  전환을 위한 변수
int y_blue, height_blue; // LCD에 그림을 그리기 위한 시작점의 y값과 높이 변수 (blue elevator)
int y_red, height_red; // LCD에 그림을 그리기 위한 시작점의 y값과 높이 변수 (red elevator)

int main(void)
{
    DelayMS(100);
    _GPIO_Init();
    LCD_Init();
    _EXTI_Init();

    GPIOG->ODR = 0xFFFFFF01;	// 초기값: LED0 On, LED1~7 Off 
    
    DisplayInitScreen();   //LCD 초기화면 
  
    while(1)
    {
	if(SW7_Flag)   // SW7이 눌렸을 때 EXTI  -> 실행 모드
	{	  
		GPIOG->ODR |= 0x0080;  //SW7 LED ON
                GPIOG->ODR &= ~0x0001; //SW0 LED OFF
                BEEP();    // SW가 눌린것에 대한 비프음
		SW7_Flag = 0;  // 다음 인터럽트가 발생할 수 있게 하기위해 SW7_Flag를 다시 0으로 초기화
                mode=3;  // 실행모드중에만 중단모드로 전환할 수 있게 하기 위해 mode를 3으로 설정
                
                LCD_SetTextColor(RGB_BLUE);  //LCD에 실행모드를 표현하기 위해 파란색으로 FL->EX
                LCD_DisplayText(1,15," EX");
                
                if(abs(current_floor-elevator_blue)>abs(current_floor-elevator_red))  // red elevator가 가까울 때
                {
                         if(current_floor > elevator_red)  //출발층이 red elevator가 있는 층보다 높아서 사람을 태우기 위해 올라감.
                         {
                               R_U();  // LCD에 오른쪽 elevator up 표시 함수
                               to_ride_up_red();  //red elevator가 사람을 태우기 위해 올라가는 함수
                               R_S();  // 출발층에 도착하여 사람을 태우기 위해 잠깐 멈춤. R:U -> R:S
                               
                               if( current_floor > goal_floor)  //사람을 태운 후 출발층이 목표층보다 높이 있을 때
                               {
                                    R_D();   // R:S -> R:D
                                    after_ride_down_red(); //사람을 태운 후 red elevator가 내려가는 함수
                               }
                               else if(current_floor < goal_floor)  // 사람을 태운 후 목표층이 현재층보다 높이 있을 때
                               {
                                    R_U();  //R:S -> R:U
                                    after_ride_up_red(); //사람을 태운 후 red elevator가 올라가는 함수 
                               }               
                         }
                          else if(current_floor < elevator_red)  //출발층이 red elevator가 있는 층보다 낮아서 사람을 태우기 위해 내려감.
                         {
                               R_D();  // LCD에 오른쪽 elevator down 표시 함수
                               to_ride_down_red(); //red elevator가 사람을 태우기 위해 내려가는 함수
                               R_S();  // 출발층에 도착하여 사람을 태우기 위해 잠깐 멈춤. R:U -> R:S
                               
                               if( current_floor > goal_floor)  // 위 if문과 주석 동일
                               {
                                    R_D();
                                    after_ride_down_red();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    R_U();
                                    after_ride_up_red(); 
                               }                      
                         }
                         
                         else if(current_floor == elevator_red) //출발층과 red elevator가 멈춰있는 층이 같을 때 
                        {
                                if( current_floor > goal_floor)  //위 if문과 주석 동일
                               {
                                    R_D();
                                    after_ride_down_red();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    R_U();
                                    after_ride_up_red(); 
                               }                      
                        }
                        
                        R_S();  // elevator가 목표층에 도착하여 멈춤. R:S 
                        elevator_red = goal_floor;  // 다음 실행을 위해 red elevator에 목표층의 위치를 저장함.      
                  }   
      
                else if(abs(current_floor-elevator_blue)<=abs(current_floor-elevator_red)) //거리가 같거나 blue elevator가 가까울 때
                {
                          if(current_floor > elevator_blue)  //출발층이 blue elevator가 있는 층보다 높아서 사람을 태우기 위해 올라감.
                         {
                               L_U();  // LCD에 왼쪽 elevator up 표시 함수
                               to_ride_up_blue();  //blue elevator가 사람을 태우기 위해 올라가는 함수
                               L_S();  // 출발층에 도착하여 사람을 태우기 위해 잠깐 멈춤. L:U -> L:S
                               
                               if( current_floor > goal_floor)   //사람을 태운 후 출발층이 목표층보다 높이 있을 때
                               {
                                    L_D();  // L:S -> L:D
                                    after_ride_down_blue();  //사람을 태운 후 blue elevator가 내려가는 함수
                               }
                               else if(current_floor < goal_floor)  // 사람을 태운 후 목표층이 현재층보다 높이 있을 때
                               {
                                    L_U();  // L:S -> L:U
                                    after_ride_up_blue();   //사람을 태운 후 blue elevator가 올라가는 함수
                               }               
                         }
                                                  
                         else if(current_floor < elevator_blue)   //출발층이 blue elevator가 있는 층보다 낮아서 사람을 태우기 위해 내려감.
                         {
                               L_D();  // LCD에 왼쪽 elevator down 표시 함수
                               to_ride_down_blue(); //blue elevator가 사람을 태우기 위해 내려가는 함수
                               L_S();   // 출발층에 도착하여 사람을 태우기 위해 잠깐 멈춤. L:D -> L:S
                               
                               if( current_floor > goal_floor)  //위 if문과 주석 동일
                               {
                                    L_D();
                                    after_ride_down_blue();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    L_U();
                                    after_ride_up_blue(); 
                               }                      
                         }
                         
                         else if(current_floor == elevator_blue) //출발층과 blue elevator가 멈춰있는 층이 같을 때 
                        {
                               if( current_floor > goal_floor)  //위 if문과 주석 동일
                               {
                                    L_D();
                                    after_ride_down_blue();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    L_U();
                                    after_ride_up_blue(); 
                               }                           
                        }
                       
                        L_S(); // elevator가 목표층에 도착하여 멈춤. L:S
                        elevator_blue = goal_floor;   // 다음 실행을 위해 blue elevator에 목표층의 위치를 저장함.
                  }
                DelayMS(500);  //0.5초 
                LCD_SetTextColor(RGB_BLUE);  //실행모드가 종료되고 층선택모드로 바뀜. (파란색으로 EX->FL)
                LCD_DisplayText(1,15," FL");
                BEEP3();  //목표층에 도착했음을 알리는 비프음3번
                GPIOG->ODR &= ~0x0080; //SW7 LED OFF
                GPIOG->ODR |= 0x0001;    //SW0 LED ON
                mode=0;  //다음 실행을 위해 변수를 초기화 해준다.
        }

        switch(KEY_Scan())  //인터럽트가 아닌 sw1~sw6은 KEY_Scan()함수로
        {
        case 0xFD00 : Floor(1); break;  //sw1이 눌렸을 때 Floor함수에 1을 보내줌.
          
        case 0xFB00 : Floor(2); break;  //sw2이 눌렸을 때 Floor함수에 2을 보내줌.
          
        case 0xF700 :  Floor(3); break; //sw3이 눌렸을 때 Floor함수에 3을 보내줌.
          
        case 0xEF00 : Floor(4);  break; //sw4이 눌렸을 때 Floor함수에 4을 보내줌.
          
        case 0xDF00 : Floor(5);  break; //sw5이 눌렸을 때 Floor함수에 5을 보내줌.
          
        case 0xBF00 : Floor(6);  break; //sw6이 눌렸을 때 Floor함수에 6을 보내줌.
        }
                
    }
}

void to_ride_up_blue(void)  //blue elevator가 사람을 태우기 위해 올라가는 함수
{
      for(int i=elevator_blue, length=14; i<current_floor;i++)   //LCD화면에 bar그림의 높이와 y값를 length만큼 늘렸다 줄임.
      { 
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue-length,10,height_blue+length);  //x값과 폭은 일정 -> y값과 높이값만 변수로 설정
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue-length,10,height_blue+length);
             DelayMS(500);  //0.5초 간격으로 bar그림이 줄어들고 늘어남.
                          
             length+=14;  //length의 값을 14만큼 증가시킴.
       }
        y_blue -= 14*(current_floor - elevator_blue); //bar의 이동이 끝나고 bar의 바뀐 y좌표값을 y_blue변수에 저장.
        height_blue += 14*(current_floor - elevator_blue); //bar의 이동이 끝나고 bar의 바뀐 높이값을 height_blue변수에 저장.
}

void to_ride_down_blue(void)   //blue elevator가 사람을 태우기 위해 내려가는 함수 
{
       for(int i=elevator_blue, length=14; i>current_floor;i--)      //주석 위 함수와 동일
      { 
             blue_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue+length,10,height_blue-length);
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue+length,10,height_blue-length);
             DelayMS(500);
                          
             length+=14;
       }
        y_blue += 14*(elevator_blue-current_floor);
        height_blue -= 14*(elevator_blue-current_floor);
}

void after_ride_up_blue(void)   //blue elevator가 사람을 태운 후 올라가는 함수
{
       for(int i =current_floor,length=14; i<goal_floor;i++)      //주석 위 함수와 동일
      {
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue-length,10,height_blue+length);
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue-length,10,height_blue+length);
             DelayMS(500);
                          
              length+=14;
       }
       y_blue -= 14*(goal_floor-current_floor);
       height_blue += 14*(goal_floor - current_floor);
}
  
void after_ride_down_blue(void)   //blue elevator가 사람을 태운 후 내려가는 함수
{
      for(int i=current_floor,length=14;  i>goal_floor;i--)     //주석 위 함수와 동일
      { 
             blue_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue+length,10,height_blue-length);
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue+length,10,height_blue-length);
             DelayMS(500);
                                          
              length+=14;
         }
          y_blue += 14*(current_floor-goal_floor);
          height_blue -= 14*(current_floor-goal_floor);
 }

  //***************************************************************************************

 void to_ride_up_red(void)  //red elevator가 사람을 태우기 위해 올라가는 함수
 {
       for(int i=elevator_red, length=14; i<current_floor;i++)   //LCD화면에 bar그림의 높이와 y값를 length만큼 늘렸다 줄임.
      { 
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red-length,10,height_red+length);   //x값과 폭은 일정 -> y값과 높이값만 변수로 설정
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red-length,10,height_red+length);
             DelayMS(500);  //0.5초 간격으로 bar그림이 줄어들고 늘어남.
                          
             length+=14;   //a의 값을 14만큼 증가시킴.
       }
        y_red -= 14*(current_floor - elevator_red); //bar의 이동이 끝나고 bar의 바뀐 y좌표값을 y_red변수에 저장.
        height_red += 14*(current_floor - elevator_red);  //bar의 이동이 끝나고 bar의 바뀐 높이값을 height_red변수에 저장.
}

void to_ride_down_red(void)  //red elevator가 사람을 태우기 위해 내려가는 함수
{     
      for(int i=elevator_red, length=14; i>current_floor;i--)      //주석 위 함수와 동일
      {     
             red_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red+length,10,height_red-length);
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red+length,10,height_red-length);
             DelayMS(500);
                          
             length+=14;
       }
        y_red += 14*(elevator_red-current_floor);
        height_red -= 14*(elevator_red-current_floor);
}
                
void after_ride_up_red(void)  //red elevator가 사람을 태운 후 올라가는 함수
{
       for(int i =current_floor,length=14; i<goal_floor;i++)          //주석 위 함수와 동일
      {
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red-length,10,height_red+length);
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red-length,10,height_red+length);
             DelayMS(500);
                          
             length+=14;
       }
       y_red -= 14*(goal_floor-current_floor);
       height_red += 14*(goal_floor - current_floor);
}
                
void after_ride_down_red(void)  //red elevator가 사람을 태운 후 내려가는 함수
{ 
      for(int i=current_floor,length=14;  i>goal_floor;i--)     //주석 위 함수와 동일
      { 
             red_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red+length,10,height_red-length);
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red+length,10,height_red-length);
             DelayMS(500);
                                          
              length+=14;
         }
          y_red += 14*(current_floor-goal_floor);
          height_red -= 14*(current_floor-goal_floor);
 }

//****************************************************************************************
  
void L_U(void)  //왼쪽 elevator가 올라갈때 LCD에 "L:U" 를 표시하기 위한 함수
{
  LCD_SetTextColor(RGB_BLUE);
  LCD_DisplayText(4,6," L");
  LCD_DisplayText(4,8," :");
  LCD_DisplayText(4,10," U");
   DelayMS(500);
}

void L_D(void)  //왼쪽 elevator가 내려갈때 LCD에 "L:D" 를 표시하기 위한 함수
{
   LCD_SetTextColor(RGB_BLUE);
   LCD_DisplayText(4,6," L");
   LCD_DisplayText(4,8," :");
   LCD_DisplayText(4,10," D");
   DelayMS(500);
}

void L_S(void)  //왼쪽 elevator가 멈출때 LCD에 "L:S" 를 표시하기 위한 함수
{
   LCD_SetTextColor(RGB_BLUE);
   LCD_DisplayText(4,6," L");
   LCD_DisplayText(4,8," :");
   LCD_DisplayText(4,10," S");
    DelayMS(500);
}

void R_U(void) //오른쪽 elevator가 올라갈때 LCD에 "R:U" 를 표시하기 위한 함수
{  
    LCD_SetTextColor(RGB_RED);
    LCD_DisplayText(4,6," R");
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," U");
    DelayMS(500);
}

void R_D(void)  //오른쪽 elevator가 내려갈때 LCD에 "R:D" 를 표시하기 위한 함수
{  
    LCD_SetTextColor(RGB_RED);
    LCD_DisplayText(4,6," R");
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," D");
    DelayMS(500);
}

void R_S(void)  //오른쪽 elevator가 멈출때 LCD에 "R:S" 를 표시하기 위한 함수
{
    LCD_SetTextColor(RGB_RED);
    LCD_DisplayText(4,6," R");
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," S");
    DelayMS(500);
}

//*****************************************************************************************
  
void blue_clear(void)  //lblue elevator가 내려갈때 bar의 그림을 clear하기 위한 함수
{
    LCD_SetBrushColor(RGB_YELLOW);
    LCD_DrawFillRect(12,25,12,130);
}

void red_clear(void)   //red elevator가 내려갈때 bar의 그림을 clear하기 위한 함수
{
     LCD_SetBrushColor(RGB_YELLOW);
     LCD_DrawFillRect(123,25,12,130);
}
 
//****************************************************************************************

void Floor(int floor)  //층선택 함수
{         
          BEEP(); //sw가 눌린것에 대한 비프음
          
          if(separate == 0)   //separate변수가 0이면 출발층에 floor값을 저장
          {
            current_floor = floor; 
            separate+=1; // 다음 입력은 목표층으로 저장하기 위해 separate값을 1증가시킴.
            LCD_SetTextColor(RGB_BLACK);  //검정색으로 LCD에 출발층 표시
            LCD_DisplayChar(5,7,floor+0x30); 
            GPIOG->ODR &= ~0x007E;  //출발층과 목표층을 다시 입력받을 때 이전 선택층 LED를 모두 끔,
            mode = 1;  //모드 변홤을 위한 변수 mode를 1로 설정
          }
          
          else   //seperate변수가 0이 아니면 목표층에 floor값을 저장
          {
            goal_floor = floor; 
            GPIOG->ODR &= ~0x007E;  //출발층 led off
            
            if(current_floor != goal_floor) //출발층과 목표층이 다를때만 다음입력으로 다시 출발층을 입력받을수 있고 LCD에 목표층 표시가능.
            {
                 separate-=1;  // 다음 입력은 출발층으로 저장하기 위해 separate값을 1감소시킴.
                 LCD_SetTextColor(RGB_BLACK);  //검정색으로 LCD에 목표층 표시
                 LCD_DisplayChar(5,11,floor+0x30);
                 mode = 2;  //모드 변홤을 위한 변수 mode를 2로 설정
            }  
          }       
   
          if(floor==1)
          {
              GPIOG->ODR |= 0x0002; //LED1 ON
          }
          else if(floor==2)
          {
              GPIOG->ODR |= 0x0004; // LED2 ON
          }
          else if(floor==3)
          {
              GPIOG->ODR |= 0x0008; // LED3 ON
          }
          else if(floor==4)
          {
              GPIOG->ODR |= 0x0010; // LED4 ON
          }
          else if(floor==5)
          {
              GPIOG->ODR |= 0x0020; // LED5 ON
          }
          else if(floor==6)
          {
              GPIOG->ODR |= 0x0040; // LED6 ON
          }
}

void DisplayInitScreen(void)  //LCD초기화면 설정 함수
{
    LCD_Clear(RGB_YELLOW);	// 화면 클리어
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8

    LCD_SetBackColor(RGB_YELLOW);  //노란색 배경에 검정색 글씨로 "Mecha_Elevator"표기.
    LCD_SetTextColor(RGB_BLACK);
    LCD_DisplayText(1,0," Mecha-Elevator");
    
    LCD_SetTextColor(RGB_BLUE);  //파란색 글씨로 "FL" 표기.
    LCD_DisplayText(1,15," FL");
    
    LCD_DisplayText(2,3," 6");  //파란색 글씨로 y값을 바꿔가며 왼쪽 elevator 층 표기.
    LCD_DisplayText(3,3," 5");
    LCD_DisplayText(4,3," 4");
    LCD_DisplayText(5,3," 3");
    LCD_DisplayText(6,3," 2");
    LCD_DisplayText(7,3," 1");
    
    LCD_DisplayText(4,6," L");  //파란색 글씨로 "L:S" 표기.
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," S");
    
    LCD_SetTextColor(RGB_BLACK);  //검정색 글씨로 "1>1" 표기.
    LCD_DisplayChar(5,7,1+0x30);
    LCD_DisplayText(5,8," >");
    LCD_DisplayChar(5,11,1+0x30);
    
    
    LCD_SetTextColor(RGB_RED);  //빨간색 글씨로 y값을 바꿔가며 오른쪽 elevator 층 표기.
    LCD_DisplayText(2,13," 6");
    LCD_DisplayText(3,13," 5");
    LCD_DisplayText(4,13," 4");
    LCD_DisplayText(5,13," 3");
    LCD_DisplayText(6,13," 2");
    LCD_DisplayText(7,13," 1");
    
    LCD_SetPenColor(RGB_GREEN);  //초록색펜으로 왼쪽 elevator와 오른쪽 elevator bar그림의 테두리를 그림
    LCD_DrawRectangle(12,95,10,7);
    LCD_DrawRectangle(123,95,10,7);
    
    LCD_SetBrushColor(RGB_BLUE);  //파란색브러쉬로 왼쪽 elevator bar그림의 사각형 내부를 채움.
    LCD_DrawFillRect(12,95,10,7);
    
    LCD_SetBrushColor(RGB_RED);  //빨간색브러쉬로 오른쪽 elevator bar그림의 사각형 내부를 채움.
    LCD_DrawFillRect(123,95,10,7);
    
    y_blue = 95;  //초기 왼쪽 elevator y값을 95로 저장.
    y_red = 95;   //초기 오른쪽 elevator y값을 95로 저장.
    height_blue = 7;   //초기 왼쪽 elevator 높이를 7로 저장.
    height_red = 7;    //초기 오른쪽 elevator 높이를 7로 저장.
}

void _GPIO_Init(void)  //GPIO핀 초기 설정
{
	// LED (GPIO G) 설정
    	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x000000FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
 	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
        
   
	// SW (GPIO H) 설정 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 
    	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER 	&= ~0x00000200;	// GPIOF 9 : Push-pull  	
 	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
}	

void _EXTI_Init(void)  //SW0과 SW7에 대한 인터럽트 설정
{
    	RCC->AHB1ENR 	|= 0x00000080;	// RCC_AHB1ENR GPIOH Enable H->Switch
	RCC->APB2ENR 	|= 0x00004000;	// Enable System Configuration Controller Clock		 
	
	SYSCFG->EXTICR[2] |=  0x00000007;// EXTI8에 대한 소스 입력은 GPIOH로 설정 
        SYSCFG->EXTICR[3] |= 0x00007000; // EXIT15에 대한 소스 입력은 GPIOH로 설정	
	
	EXTI->FTSR |= 0x00008100;		// Falling Trigger Enable  (EXTI8:PH8)(EXTI15:PH15)
    	EXTI->IMR  |= 0x00008100;  	// EXTI8,15 인터럽트 mask (Interrupt Enable)
		
	NVIC->ISER[0] |= ( 1<<23  );   // Enable Interrupt EXTI8 Vector table Position 참조
        NVIC->ISER[1] |= ( 1<<8  );    // Enable Interrupt EXTI15 Vector table Position 참조
}

void EXTI9_5_IRQHandler(void)	// SW0을 위한 EXTI 5~9 인터럽트 핸들러
{
    if(EXTI->PR & 0x0100)	//SW0이 눌렸고 mode가 2이면 중단모드 실행.
    {
        EXTI->PR |= 0x0100; 		// Pending bit Clear
        if(mode==3)
       {
            GPIOG->ODR &= ~0x0081; //중단모드에서 LED0,7 OFF
            BEEP();  //SW가 눌린것에 대한 비프음
                
            LCD_SetTextColor(RGB_BLUE);  //파란색 글씨로 LCD에 EX->HL 중단모드 표시.
            LCD_DisplayText(1,15," HL");
            GPIOG->ODR &= ~0x0081;  //SW0이 눌리면 SW1~SW6 LED는 그대로, SW0과 SW7 LED는 OFF
                
            for(int i=0; i<=9;i++)  //중단모드가 선택되면 0.5초 간격으로 5초간 비프음 발생.
           {
              BEEP();
              DelayMS(500);
           }
           GPIOG->ODR &= ~0x0001; //led0 off
           GPIOG->ODR |= 0x0080;  // led7 on
       } 
    }
}

void EXTI15_10_IRQHandler(void)  //SW7을 위한 EXTI 15~10 인터럽트 핸들러
{
     if(EXTI->PR & 0x8000)    //SW7이 눌렸으면
    {
        EXTI ->PR |= 0x8000;  // Pending bit Clear
        if(mode==2)  //목표층까지 층 입력을 받은 후에만 실행모드로 전환 가능
        {
            SW7_Flag = 1;	  //main문내에서 sw7에 대한 if문을 실행시키기 위해 1로 설정
        }
    }
}
  

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

void BEEP(void)			/* beep for 30 ms */
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void BEEP3(void)  //비프음 3번 울리는 함수
{
   BEEP();
   DelayMS(50);
   BEEP();
   DelayMS(50);
   BEEP();
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
