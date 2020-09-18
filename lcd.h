/*********************************************************************/
/****************** lcd.h **************************************/
/*********************************************************************/
#include <mega128.h>
#include <delay.h>
#define LCD_WDATA PORTA // LCD ������ ��Ʈ ����
#define LCD_WINST PORTA
#define LCD_CTRL PORTG // LCD ���� ��ȣ ����
#define LCD_EN 0 // Enable ��ȣ
#define LCD_RW 1 // ������(1)/��ɾ�(0)
#define LCD_RS 2 // �б�(1)/����(0)
#define Byte unsigned char
#define On 1 // �Ҹ��� ��� ����
#define Off 0
#define RIGHT 1
#define LEFT 0
void LCD_Data(Byte ch);
void LCD_Comm(Byte ch);
void LCD_delay(Byte ms);
void LCD_CHAR(Byte c);
void LCD_Str(Byte *str);
void LCD_Pos(unsigned char col, unsigned char row);
void LCD_Clear(void);
void PortInit(void);
void LCD_Init(void);
void LCD_Shift(char p);
void Cursor_Home(void);
void Cursor_shift(Byte p);