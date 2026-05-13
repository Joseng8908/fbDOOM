#ifndef HW_SERIAL_H
#define HW_SERIAL_H

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define SERIAL_DEV "/dev/ttymxc1"

// UART fd 싱글톤 - FND/LCD/버튼 전부 공유
static int g_serial_fd = -1;

static inline int HW_Serial_Init(void) {
    struct termios tio;
    g_serial_fd = open(SERIAL_DEV, O_RDWR | O_NONBLOCK);
    if (g_serial_fd < 0) return -1;

    bzero(&tio, sizeof(tio));
    // CREAD: 수신도 활성화 (버튼 읽기용)
    tio.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;
    tio.c_lflag = 0;
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN]  = 0; // nonblock이라 0으로
    tcsetattr(g_serial_fd, TCSANOW, &tio);
    return g_serial_fd;
}

// 5바이트 프로토콜: { 0x12, cmd, d1, d2, 0x13 }
static inline void HW_Serial_Send(unsigned char cmd,
                                   unsigned char d1,
                                   unsigned char d2) {
    if (g_serial_fd < 0) return;
    unsigned char msg[5] = { 0x12, cmd, d1, d2, 0x13 };
    write(g_serial_fd, msg, 5);
}

static inline void HW_Serial_Close(void) {
    if (g_serial_fd >= 0) {
        close(g_serial_fd);
        g_serial_fd = -1;
    }
}

#endif // HW_SERIAL_H
