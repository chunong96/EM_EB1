#include <mega128.h>
#include "lcd.h" 

void PortInit(void)
{
    DDRA = 0xFF; // PORTA�� �������
    DDRG = 0x0F; // PORTG�� ���� 4��Ʈ�� �������
}

void LCD_Data(Byte ch)
{
    LCD_CTRL |= (1 << LCD_RS); // RS=1, R/W=0���� ������ ���� ����Ŭ
    LCD_CTRL &= ~(1 << LCD_RW);
    LCD_CTRL |= (1 << LCD_EN); // LCD Enable
    delay_us(50); // �ð� ����
    LCD_WDATA = ch; // ������ ���
    delay_us(50); // �ð� ����
    LCD_CTRL &= ~(1 << LCD_EN); // LCD Disable
}

void LCD_Comm(Byte ch)
{
    LCD_CTRL &= ~(1 << LCD_RS); // RS=R/W=0���� ��ɾ� ���� ����Ŭ
    LCD_CTRL &= ~(1 << LCD_RW);
    LCD_CTRL |= (1 << LCD_EN); // LCD Enable
    delay_us(50); // �ð� ����
    LCD_WINST = ch; // ��ɾ� ����
    delay_us(50); // �ð� ����
    LCD_CTRL &= ~(1 << LCD_EN); // LCD Disable
}

void LCD_delay(Byte ms)
{
    delay_ms(ms);
}

void LCD_CHAR(Byte c) // �� ���� ���
{
    LCD_Data(c);
    delay_ms(1);
}

void LCD_Str(Byte *str) // ���ڿ� ���
{
    while(*str != 0) {
        LCD_CHAR(*str);
        str++;
    }
}

void LCD_Pos(unsigned char col, unsigned char row) // LCD ������ ����
{
    LCD_Comm(0x80|(row+col*0x40)); // row = ������, col = ���ڿ�
} 

void Cursor_Home(void)
{
    LCD_Comm(0x02); // Cursor Home
    LCD_delay(2); // 2ms ����
}

void LCD_Clear(void) // ȭ�� Ŭ���� (1)
{
    LCD_Comm(0x01);
    LCD_delay(2);
}

void LCD_Shift(char p) // ���÷��� ����Ʈ (5)
{
    // ǥ�� ȭ�� ��ü�� ���������� �̵�
    if(p == RIGHT) {
        LCD_Comm(0x1C);
        LCD_delay(1); // �ð� ����
    }
    // ǥ�� ȭ�� ��ü�� �������� �̵�
    else if(p == LEFT) {
        LCD_Comm(0x18);
        LCD_delay(1);
    }
}

void Cursor_shift(Byte p)
{
    if(p == RIGHT) {
        LCD_Comm(0x14);
        LCD_delay(1);
    }
    else if(p == LEFT) {
        LCD_Comm(0x10);
        LCD_delay(1);
    }
}

void LCD_Init(void) 	// LCD �ʱ�ȭ
{
     PortInit();
    
     LCD_Comm(0x30);	// �ʱ�ȭ Set, 
     delay_us(4100);    // 4.1ms ����
     LCD_Comm(0x30);   	// �ʱ�ȭ Set, 
     delay_us(100); 	// 100us ����
     LCD_Comm(0x30); 	// �ʱ�ȭ Set, 
     delay_us(100); 	// 100us ����
     LCD_Comm(0x38); 	// �ʱ�ȭ Set, ������ ���� 8Bit, ǥ�ö��� 2�� ����� ���� ��� ����
     delay_us(1000); 	// ����� ó�� �ϴµ� �ּ� 40us ������ �߻��ϱ⿡ ������ ����Ͽ� ����
     LCD_Comm(0x0e); 	// Display ON, Cursor On, Blink Off
     delay_us(1000); 	// 40�� �̻��� ��ٸ�
     LCD_Comm(0x01); 	// LCD Clear   
     delay_us(2000); 	// 1.64�� �̻��� ��ٸ�
     LCD_Comm(0x06); 	// Cursor Entry Mode Set, ǥ�� ��ġ +1�� ����
     delay_us(1000); 	// 40�� �̻��� ��ٸ�
}
