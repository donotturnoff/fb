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

void fill_rectangle(Buffer *buf, int x0, int y0, unsigned int w, unsigned int h, uint32_t colour) {
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

void draw_rectangle(Buffer *buf, int x0, int y0, unsigned int w, unsigned int h, unsigned int border, uint32_t colour) {
    int buf_w = buf->w;
    int buf_h = buf->h;
    if (x0 >= buf_w || y0 >= buf_h) return;

    size_t x_ = x0;
    size_t y_ = y0;
    size_t w_ = w;
    size_t h_ = min(h, border);
    fill_rectangle(buf, x_, y_, w_, h_, colour);

    x_ = x0;
    y_ = max(y0, y0+h-border);
    w_ = w;
    h_ = min(h, border);
    fill_rectangle(buf, x_, y_, w_, h_, colour);

    x_ = x0;
    y_ = y0+border;
    w_ = border;
    h_ = h-min(h, 2*border);
    fill_rectangle(buf, x_, y_, w_, h_, colour);

    x_ = x0+w-border;
    y_ = y0+border;
    w_ = border;
    h_ = h-min(h, 2*border);
    fill_rectangle(buf, x_, y_, w_, h_, colour);
}

void fill_arc(Buffer *buf, int x, int y, unsigned int r, unsigned int angle, uint32_t colour) {

}

void draw_arc(Buffer *buf, int x, int y, unsigned int r, unsigned int angle, unsigned int border, uint32_t colour) {

}

