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

void set_pixel(Buffer *buf, unsigned int x, unsigned int y, uint32_t colour);
uint32_t get_pixel(Buffer *buf, unsigned int x, unsigned int y);

void fill_rect(Buffer *buf, int x0, int y0, unsigned int w, unsigned int h, uint32_t colour);
void draw_rect(Buffer *buf, int x0, int y0, unsigned int w, unsigned int h, unsigned int border, uint32_t colour);

void fill_circ(Buffer *buf, int x0, int y0, unsigned int r, uint32_t colour);
void draw_circ(Buffer *buf, int x0, int y0, unsigned int r, unsigned int border, uint32_t colour);

#endif
