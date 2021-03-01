#ifndef FB_UTILS_H
#define FB_UTILS_H

#include <string.h>
#include <stdint.h>

struct buffer_t {
    uint32_t *fb;
    uint32_t *bb;
    size_t w;
    size_t h;
    size_t bpp;
    size_t Bpp;
    size_t size;
};
typedef struct buffer_t Buffer;

inline int max(int a, int b) {
    return a > b ? a : b;
}

inline int min(int a, int b) {
    return a < b ? a : b;
}

void *swap_buffers(Buffer *buf);

void set_pixel(Buffer *buf, size_t x, size_t y, uint32_t colour);
uint32_t get_pixel(Buffer *buf, size_t x, size_t y);

void fill_rectangle(Buffer *buf, size_t x0, size_t y0, size_t w, size_t h, uint32_t colour);
void draw_rectangle(Buffer *buf, size_t x0, size_t y0, size_t w, size_t h, size_t border, uint32_t colour);

void fill_arc(Buffer *buf, size_t x, size_t y, size_t r, size_t angle, uint32_t colour);
void draw_arc(Buffer *buf, size_t x, size_t y, size_t r, size_t angle, size_t border, uint32_t colour);

#endif
