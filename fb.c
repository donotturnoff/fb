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

int interrupted;

void interrupt(int signum) {
    interrupted = 1;
}

void cleanup(int fb_f, size_t fb_size, char *fb, char *bb) {
    if (fb) {
        memset(fb, 0, fb_size);
        munmap(fb, fb_size);
    }
    if (bb) {
        munmap(bb, fb_size);
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
        cleanup(fb_f, 0, NULL, NULL);
        return 2;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fb_f, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        fprintf(stderr, "Failed to retrieve variable screen info: %s", strerror(errno));
        cleanup(fb_f, 0, NULL, NULL);
        return 1;
    }

    size_t fb_w = vinfo.xres;
    size_t fb_h = vinfo.yres;
    size_t fb_bpp = vinfo.bits_per_pixel;
    size_t fb_bytes = fb_bpp / 8;
    size_t fb_size = fb_w * fb_h * fb_bytes;

    char *fb = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_f, (off_t)0);
    if (fb == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap framebuffer: %s", strerror(errno));
        cleanup(fb_f, fb_size, NULL, NULL);
        return 1;
    }

    char *bb = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (bb == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap backbuffer: %s", strerror(errno));
        cleanup(fb_f, fb_size, fb, NULL);
        return 1;
    }

    unsigned int t = 0;
    while (!interrupted) {
        for (size_t y = 0; y < fb_h; y++) {
            for (size_t x = 0; x < fb_w; x++) {
                size_t i = (y * fb_w + x) * 4;
                unsigned int b = 128*sin((((double)t)/5))+127;
                bb[i+0] = b;
                bb[i+1] = b;
                bb[i+2] = b;
                bb[i+3] = 0;
            }
        }
        memcpy(fb, bb, fb_size);
        sleep(1);
        t++;
    }

    cleanup(fb_f, fb_size, fb, bb);

    return 0;
}
