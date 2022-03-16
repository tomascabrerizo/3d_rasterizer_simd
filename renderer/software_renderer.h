#ifndef TC_SOFTWARE_RENDERER_H
#define TC_SOFTWARE_RENDERER_H

#include "core/types.h"
#include "math/math.h"

struct tc_BackBuffer
{
    void *pixels;
    f32 *depth;
    u32 height;
    u32 width;
    u32 pitch;
};

struct tc_Bitmap
{
    void *pixels;
    u32 width;
    u32 height;
    s32 pitch;
};

struct tc_Renderer
{
    void *platform;
    tc_BackBuffer backbuffer;
};

struct tc_Vertex
{
    v3 position;
    v4 color;
    v2 coord;
};

tc_Renderer *tc_platform_create_software_renderer(tc_Window *window);
void tc_platform_destroy_software_renderer(tc_Renderer *renderer);
void tc_software_renderer_swap_buffers(tc_Renderer *renderer, tc_Window *window);
void tc_software_renderer_clear(tc_Renderer *renderer, u32 color);

void tc_software_renderer_draw_rect(tc_Renderer *renderer, u32 x, u32 y, u32 width, u32 height, u32 color);
void tc_software_renderer_draw_triangle(tc_Renderer *renderer, tc_Bitmap *texture, tc_Vertex *vertex0, tc_Vertex *vertex1, tc_Vertex *vertex2);

#endif // TC_SOFTWARE_RENDERER_H
