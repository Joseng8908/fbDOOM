#include "hw_key.h"
#include "hw_serial.h"
#include "d_event.h"
#include "doomkeys.h"

// button.c 프로토콜:
//   read_buf[1] == 0x22  → 버튼 이벤트
//   read_buf[2] - 0x30   → 버튼 번호 (1~5)
//   read_buf[3] - 0x30   → 상태 (1=press, 0=release)

// M4 버튼 5개 매핑
// 보드 실물 보고 번호 바꿔도 됨
static int MapButton(int btn_num) {
    switch (btn_num) {
        case 1: return KEY_UPARROW;    // 전진
        case 2: return KEY_DOWNARROW;  // 후진
        case 3: return KEY_LEFTARROW;  // 좌회전
        case 4: return KEY_RIGHTARROW; // 우회전
        case 5: return KEY_FIRE;       // 발사
        default: return -1;
    }
}

void HW_Key_Poll(void) {
    if (g_serial_fd < 0) return;

    unsigned char buf[5];
    // nonblock read - 읽을 거 없으면 즉시 리턴
    int n = read(g_serial_fd, buf, sizeof(buf));
    if (n < 5) return;

    // 버튼 이벤트 확인
    if (buf[0] != 0x12) return;
    if (buf[1] != 0x22) return;
    if (buf[4] != 0x13) return;

    int btn_num = buf[2] - 0x30;  // 버튼 번호
    int btn_state = buf[3] - 0x30; // 1=press, 0=release

    int dkey = MapButton(btn_num);
    if (dkey < 0) return;

    event_t ev;
    ev.type  = (btn_state == 1) ? ev_keydown : ev_keyup;
    ev.data1 = dkey;
    ev.data2 = 0;
    ev.data3 = 0;
    D_PostEvent(&ev);
}
