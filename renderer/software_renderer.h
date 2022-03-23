#ifndef TC_SOFTWARE_RENDERER_H
#define TC_SOFTWARE_RENDERER_H

#include "core/types.h"
#include "math/math.h"

// TODO: IMPORTANT: software_renderer must be independent from
// renderer.h. Dont include this file
#include "renderer.h"

struct tc_Bitmap;

#define MAX_VERTEX_LIST 10
struct tc_VertexList
{
    tc_Vertex vertex[MAX_VERTEX_LIST];
    u32 count;
};

struct tc_BackBuffer
{
    void *pixels;
    f32 *depth;
    u32 height;
    u32 width;
    u32 pitch;
};

enum tc_DrawCommandType
{
    TC_DCMD_VERTEX_ARRAY,
    TC_DCMD_RECT,
};

struct tc_VertexArray
{
    tc_Bitmap *texture;
    m4 transform;
    tc_Vertex* vertices;
    u32 vertex_count;
};

struct tc_DrawCommand
{
    tc_DrawCommandType type;
    union
    {
        tc_VertexArray vertex_array;
    };
};

// TODO: ThreadQueue needs to be refactor
struct ThreadQueue;
struct tc_Renderer
{
    void *platform;
    tc_BackBuffer backbuffer;
    ThreadQueue *thread_queue;

    tc_DrawCommand *command_buffer_darr;
};

tc_Renderer *tc_platform_create_software_renderer(tc_Window *window);
void tc_platform_destroy_software_renderer(tc_Renderer *renderer);

void tc_software_renderer_swap_buffers(tc_Renderer *renderer, tc_Window *window);
void tc_software_renderer_clear(tc_Renderer *renderer, u32 color);

//void tc_software_renderer_draw_rect(tc_Renderer *renderer, u32 x, u32 y, u32 width, u32 height, u32 color);
void tc_software_renderer_draw_triangle(tc_Renderer *renderer, tc_Bitmap *texture, rect2d clipping_rect, tc_Vertex *vertex0, tc_Vertex *vertex1, tc_Vertex *vertex2);

void tc_software_renderer_draw_array(tc_Renderer *renderer, tc_Bitmap *texture, m4 transform, tc_Vertex *vertices, u32 vertices_cout);

void tc_software_renderer_push_draw_command(tc_Renderer *renderer, tc_DrawCommand command);

#endif // TC_SOFTWARE_RENDERER_H
