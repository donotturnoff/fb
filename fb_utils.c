#include "fb_utils.h"

extern inline int max(int a, int b);
extern inline int min(int a, int b);

void *swap_buffers(Buffer *buf) {
    return memcpy(buf->fb, buf->bb, buf->size);
}

size_t get_offset(Buffer *buf, size_t x, size_t y) {
    size_t w = buf->w;
    return ((y*w)+x);
}

void set_pixel(Buffer *buf, size_t x, size_t y, uint32_t colour) {
    size_t i = get_offset(buf, x, y);
    buf->bb[i] = colour;
}

uint32_t get_pixel(Buffer *buf, size_t x, size_t y) {
    size_t i = get_offset(buf, x, y);
    return buf->bb[i];
}

void fill_rectangle(Buffer *buf, size_t x0, size_t y0, size_t w, size_t h, uint32_t colour) {
    uint32_t *bb = buf->bb;
    uint32_t buf_w = buf->w;
    uint32_t buf_h = buf->h;
    for (size_t y = y0; y < min(y0+h, buf_h); y++) {
        for (size_t x = x0; x < min(x0+w, buf_w); x++) {
            bb[y*buf_w+x] = colour;
        }
    }
}

void draw_rectangle(Buffer *buf, size_t x0, size_t y0, size_t w, size_t h, size_t border, uint32_t colour) {

}

void fill_arc(Buffer *buf, size_t x, size_t y, size_t r, size_t angle, uint32_t colour) {

}

void draw_arc(Buffer *buf, size_t x, size_t y, size_t r, size_t angle, size_t border, uint32_t colour) {

}

