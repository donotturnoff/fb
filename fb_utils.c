#include "fb_utils.h"

extern inline int max(int a, int b);
extern inline int min(int a, int b);

void *swap_buffers(Buffer *buf) {
    return memcpy(buf->fb, buf->bb, buf->size);
}

size_t get_offset(Buffer *buf, unsigned int x, unsigned int y) {
    unsigned int w = buf->w;
    return ((y*w)+x);
}

void set_pixel(Buffer *buf, unsigned int x, unsigned int y, uint32_t colour) {
    size_t i = get_offset(buf, x, y);
    buf->bb[i] = colour;
}

uint32_t get_pixel(Buffer *buf, unsigned int x, unsigned int y) {
    size_t i = get_offset(buf, x, y);
    return buf->bb[i];
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
        for (int x = max(x0, 0); x < min(x0+r, buf_h); x++) {
            int d2 = (x-x0)*(x-x0)+yd2;
            if (d2 > r2) break;
            bb[y*buf_w+x] = colour;
        }
        for (int x = max(x0-1, 0); x >= max(x0-r, 0); x--) {
            int d2 = (x-x0)*(x-x0)+yd2;
            if (d2 > r2) break;
            bb[y*buf_w+x] = colour;
        }
    }
}

void draw_circ(Buffer *buf, int x, int y, unsigned int r, unsigned int border, uint32_t colour) {

}

