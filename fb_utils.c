#include "fb_utils.h"

extern inline int max(int a, int b);
extern inline int min(int a, int b);

Buffer *init_fb(const char *fb_path) {
    int fb_f = open(fb_path, O_RDWR);
    if (fb_f < 0) {
        fprintf(stderr, "Failed to open framebuffer %s: %s", fb_path, strerror(errno));
        return NULL;
    }

    Buffer *buf = malloc(sizeof(Buffer));
    if (!buf) {
        fprintf(stderr, "Failed to allocate memory for buffer: %s", strerror(errno));
        close(fb_f);
        return NULL;
    }
    buf->f = fb_f;

    struct fb_var_screeninfo vinfo;
    if (ioctl(fb_f, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        fprintf(stderr, "Failed to retrieve variable screen info: %s", strerror(errno));
        destroy_fb(buf);
        return NULL;
    }

    size_t fb_w = vinfo.xres;
    size_t fb_h = vinfo.yres;
    size_t fb_bpp = vinfo.bits_per_pixel;
    size_t fb_bytes = fb_bpp / 8;
    size_t fb_size = fb_w * fb_h * fb_bytes;

    buf->w = fb_w;
    buf->h = fb_h;
    buf->bpp = fb_bpp;
    buf->Bpp = fb_bytes;
    buf->size = fb_size;
    buf->fb = NULL;
    buf->bb = NULL;

    uint32_t *fb_buf = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_f, (off_t)0);
    if (fb_buf == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap framebuffer: %s", strerror(errno));
        destroy_fb(buf);
        return NULL;
    }
    buf->fb = fb_buf;

    uint32_t *bb_buf = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (bb_buf == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap backbuffer: %s", strerror(errno));
        destroy_fb(buf);
        return NULL;
    }
    buf->bb = bb_buf;

    return buf;
}

void destroy_fb(Buffer *buf) {
    if (buf) {
        size_t size = buf->size;
        if (buf->fb) {
            memset(buf->fb, 0, size);
            munmap(buf->fb, size);
        }
        if (buf->bb) {
            munmap(buf->bb, size);
        }
        if (buf->f > 0) {
            close(buf->f);
        }
        free(buf);
    }
}

void repaint(Buffer *buf) {
    memcpy(buf->fb, buf->bb, buf->size);
}

void clear_buffer(Buffer *buf) {
    memset(buf->bb, 0, buf->size);
}

void set_pixel(Buffer *buf, int x, int y, uint32_t colour) {
    uint32_t *bb = buf->bb;
    int buf_w = buf->w;
    int buf_h = buf->h;
    if (x < 0 || x >= buf_w || y < 0 || y >= buf_h) return;
    bb[y*buf_w + x] = colour;
}

uint32_t get_pixel(Buffer *buf, int x, int y) {
    uint32_t *bb = buf->bb;
    int buf_w = buf->w;
    int buf_h = buf->h;
    if (x < 0 || x >= buf_w || y < 0 || y >= buf_h) return 0;
    return bb[y*buf_w + x];
}

void fill_rect(Buffer *buf, int x0, int y0, unsigned int w, unsigned int h, uint32_t colour) {
    uint32_t *bb = buf->bb;
    int buf_w = buf->w;
    int buf_h = buf->h;
    if (x0 >= buf_w || y0 >= buf_h) return;

    for (int y = max(y0, 0); y < min(y0+h, buf_h); y++) {
        for (int x = max(x0, 0); x < min(x0+w, buf_w); x++) {
            bb[y*buf_w+x] = colour;
        }
    }
}

void draw_rect(Buffer *buf, int x0, int y0, unsigned int w, unsigned int h, unsigned int border, uint32_t colour) {
    int buf_w = buf->w;
    int buf_h = buf->h;
    if (x0 >= buf_w || y0 >= buf_h) return;

    size_t x_ = x0;
    size_t y_ = y0;
    size_t w_ = w;
    size_t h_ = min(h, border);
    fill_rect(buf, x_, y_, w_, h_, colour);

    x_ = x0;
    y_ = max(y0, y0+h-border);
    w_ = w;
    h_ = min(h, border);
    fill_rect(buf, x_, y_, w_, h_, colour);

    x_ = x0;
    y_ = y0+border;
    w_ = border;
    h_ = h-min(h, 2*border);
    fill_rect(buf, x_, y_, w_, h_, colour);

    x_ = x0+w-border;
    y_ = y0+border;
    w_ = border;
    h_ = h-min(h, 2*border);
    fill_rect(buf, x_, y_, w_, h_, colour);
}

void fill_circ(Buffer *buf, int x0, int y0, unsigned int r, uint32_t colour) {
    uint32_t *bb = buf->bb;
    int buf_w = buf->w;
    int buf_h = buf->h;
    int r2 = r*r;

    for (int y = max(y0-r, 0); y < min(y0+r, buf_h); y++) {
        int yd2 = (y-y0)*(y-y0);
        for (int x = min(buf_w-1, max(x0, 0)); x < min(x0+r, buf_w); x++) {
            int d2 = (x-x0)*(x-x0)+yd2;
            if (d2 > r2) break;
            bb[y*buf_w+x] = colour;
        }
        for (int x = min(buf_w-1, max(x0-1, 0)); x >= max(x0-r, 0); x--) {
            int d2 = (x-x0)*(x-x0)+yd2;
            if (d2 > r2) break;
            bb[y*buf_w+x] = colour;
        }
    }
}

void draw_circ(Buffer *buf, int x0, int y0, unsigned int r, unsigned int border, uint32_t colour) {
    uint32_t *bb = buf->bb;
    int buf_w = buf->w;
    int buf_h = buf->h;

    int inner_r = max(r-border, 0);
    int inner_r2 = inner_r*inner_r;
    int outer_r2 = r*r;

    for (int y = max(y0-r, 0); y < min(y0+r, buf_h); y++) {
        int yd2 = (y-y0)*(y-y0);
        for (int x = min(buf_w-1, max(x0, 0)); x < min(x0+r, buf_w); x++) {
            int d2 = (x-x0)*(x-x0)+yd2;
            if (d2 < inner_r2) continue;
            if (d2 > outer_r2) break;
            bb[y*buf_w+x] = colour;
        }
        for (int x = min(buf_w-1, max(x0-1, 0)); x >= max(x0-r, 0); x--) {
            int d2 = (x-x0)*(x-x0)+yd2;
            if (d2 < inner_r2) continue;
            if (d2 > outer_r2) break;
            bb[y*buf_w+x] = colour;
        }
    }

}
