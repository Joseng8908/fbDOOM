#include "hw_fnd.h"
#include "hw_serial.h"

// fnd.c 프로토콜: cmd=0x23, d1=십의자리, d2=일의자리
// value: 0~99 (Doom HP는 0~200이지만 FND 2자리라 99로 클램프)
void HW_FND_Update(int value) {
    if (value < 0)  value = 0;
    if (value > 99) value = 99;

    unsigned char d1 = 0x30 + (value / 10);
    unsigned char d2 = 0x30 + (value % 10);
    HW_Serial_Send(0x23, d1, d2);
}
