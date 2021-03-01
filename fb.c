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

int main(void) {
    const char *fb_path = "/dev/fb0";

    Buffer *buf = init_fb(fb_path);

    if (!buf) return 1;

    struct sigaction interrupt_action;
    interrupt_action.sa_handler = &interrupt;
    if (sigaction(SIGINT, &interrupt_action, NULL) < 0) {
        fprintf(stderr, "Failed to register SIGINT handler: %s", strerror(errno));
        destroy_fb(buf);
        return 2;
    }

    draw_circ(buf, 1920, 1080, 300, 10, 0x00FF0000);
    fill_circ(buf, 100, 100, 2, 0x00FF0000);
    swap_buffers(buf);

    while (!interrupted);
    destroy_fb(buf);

    return 0;
}
