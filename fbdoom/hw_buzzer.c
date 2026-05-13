#include "hw_buzzer.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <stdlib.h>

#define BUZZER_DEV "/dev/buzzer"

// buzzer.c / piano.c에서 확인한 ioctl 명령
#define IOCTL_START_BUZZER  _IOW('b', 0x07, int)
#define IOCTL_END_BUZZER    _IOW('b', 0x09, int)
#define IOCTL_SET_TONE      _IOW('b', 0x0b, int)
#define IOCTL_SET_VOLUME    _IOW('b', 0x0c, int)

static int buzzer_available = 0;

void HW_Buzzer_Init(void) {
    // 열렸다 닫았다 하는 구조라 존재 여부만 확인
    int fd = open(BUZZER_DEV, O_RDONLY);
    if (fd >= 0) {
        buzzer_available = 1;
        close(fd);
    }
}

// Hz → 나노초 주기 변환 (buzzer.c 방식 그대로)
static long freqToTone(double freq) {
    return (long)((1.0 / freq) * 1000000000);
}

typedef struct {
    long tone;
    int  volume;
    int  us;      // usleep 시간
} BeepArg;

// 게임 루프 블로킹 방지 - fire and forget
static void* beep_thread(void *arg) {
    BeepArg *a = (BeepArg*)arg;
    int fd = open(BUZZER_DEV, O_RDONLY);
    if (fd >= 0) {
        ioctl(fd, IOCTL_SET_VOLUME, a->volume);
        ioctl(fd, IOCTL_SET_TONE,   a->tone);
        ioctl(fd, IOCTL_START_BUZZER, 0);
        usleep(a->us);
        ioctl(fd, IOCTL_END_BUZZER, 0);
        close(fd);
    }
    free(a);
    return NULL;
}

// freq_hz: 주파수 (Hz), ms: 지속시간 (밀리초)
void HW_Buzzer_Beep(double freq_hz, int ms) {
    if (!buzzer_available) return;

    BeepArg *a = malloc(sizeof(BeepArg));
    if (!a) return;
    a->tone   = freqToTone(freq_hz);
    a->volume = 10000;
    a->us     = ms * 1000;

    pthread_t tid;
    pthread_create(&tid, NULL, beep_thread, a);
    pthread_detach(tid);
}

void HW_Buzzer_Close(void) {
    buzzer_available = 0;
}
