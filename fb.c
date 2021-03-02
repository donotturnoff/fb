#include <linux/fb.h>
#include <linux/string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include "fb_utils.h"

int interrupted;

void interrupt(int signum) {
    interrupted = 1;
}

//https://stackoverflow.com/a/1157217
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int main(void) {
    const char *fb_path = "/dev/fb0";
    const char *tty_path = "/dev/tty2";

    Buffer *buf = init_fb(fb_path, tty_path);

    if (!buf) return 1;

    struct sigaction interrupt_action;
    interrupt_action.sa_handler = &interrupt;
    if (sigaction(SIGINT, &interrupt_action, NULL) < 0) {
        fprintf(stderr, "Failed to register SIGINT handler: %s", strerror(errno));
        destroy_fb(buf);
        return 2;
    }

    clock_t start, stop, elapsed;

    int clocks_per_ms = CLOCKS_PER_SEC/1000;

    int dir = 1;
    int x = 100;
    while (!interrupted) {
        start = clock();
        repaint(buf);
        clear_buffer(buf);
        fill_circ(buf, x, 500, 100, 0x00FF0000);
        if (x >= 1920 - 100 || x < 100) dir *= -1;
        x += 5 * dir;
        stop = clock();
        elapsed = (stop - start)/clocks_per_ms;
        msleep(10 - elapsed);
    }
    destroy_fb(buf);

    return 0;
}
