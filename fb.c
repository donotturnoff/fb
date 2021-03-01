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

void cleanup(int fb_f, Buffer *buf) {
    if (buf) {
        size_t size = buf->size;
        if (buf->fb) {
            memset(buf->fb, 0, size);
            munmap(buf->fb, size);
        }
        if (buf->bb) {
            munmap(buf->bb, size);
        }
    }
    if (fb_f > 0) {
        close(fb_f);
    }
}

int main(void) {
    const char *fb_path = "/dev/fb0";
    int fb_f = open(fb_path, O_RDWR);

    if (fb_f < 0) {
        fprintf(stderr, "Failed to open framebuffer %s: %s", fb_path, strerror(errno));
        return 1;
    }

    interrupted = 0;

    struct sigaction interrupt_action;
    interrupt_action.sa_handler = &interrupt;
    if (sigaction(SIGINT, &interrupt_action, NULL) < 0) {
        fprintf(stderr, "Failed to register SIGINT handler: %s", strerror(errno));
        cleanup(fb_f, NULL);
        return 2;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fb_f, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        fprintf(stderr, "Failed to retrieve variable screen info: %s", strerror(errno));
        cleanup(fb_f, NULL);
        return 1;
    }

    size_t fb_w = vinfo.xres;
    size_t fb_h = vinfo.yres;
    size_t fb_bpp = vinfo.bits_per_pixel;
    size_t fb_bytes = fb_bpp / 8;
    size_t fb_size = fb_w * fb_h * fb_bytes;

    Buffer buf;
    buf.w = fb_w;
    buf.h = fb_h;
    buf.bpp = fb_bpp;
    buf.Bpp = fb_bytes;
    buf.size = fb_size;
    buf.fb = NULL;
    buf.bb = NULL;

    uint32_t *fb_buf = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_f, (off_t)0);
    if (fb_buf == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap framebuffer: %s", strerror(errno));
        cleanup(fb_f, NULL);
        return 1;
    }
    buf.fb = fb_buf;

    uint32_t *bb_buf = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (bb_buf == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap backbuffer: %s", strerror(errno));
        cleanup(fb_f, &buf);
        return 1;
    }
    buf.bb = bb_buf;

    fill_rectangle(&buf, 100, 100, 200, 100, 0x00FFFFF00);
    swap_buffers(&buf);

    while (!interrupted);
    cleanup(fb_f, &buf);

    return 0;
}
