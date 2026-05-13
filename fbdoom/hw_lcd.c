#include "hw_lcd.h"
#include "hw_serial.h"

// lcd.c 프로토콜: cmd=0x24, d1=0x30(고정), d2=값
// 캐릭터 LCD라 숫자 하나씩만 보낼 수 있음
// HP 십의자리 → 일의자리 순서로 두 번 전송
void HW_LCD_UpdateHP(int hp) {
    if (hp < 0)   hp = 0;
    if (hp > 99)  hp = 99;

    unsigned char tens = 0x30 + (hp / 10);
    unsigned char ones = 0x30 + (hp % 10);

    HW_Serial_Send(0x24, 0x30, tens);
    HW_Serial_Send(0x24, 0x30, ones);
}
