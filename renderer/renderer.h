#ifndef TC_RENDERER_H
#define TC_RENDERER_H

struct tc_Renderer;
struct tc_Window;

enum tc_RendererType
{
    TC_RENDERER_SOFTWARE,
    TC_RENDERER_OPENGL,
};

struct tc_Vertex
{
    v4 position;
    v4 color;
    v2 coord;
};

tc_Renderer *tc_renderer_create(tc_Window *window, tc_RendererType type);
tc_Renderer *(*tc_platform_renderer_create)(tc_Window *window);
void (*tc_renderer_destroy)(tc_Renderer *renderer);
void (*tc_renderer_swap_buffers)(tc_Renderer *renderer, tc_Window *window);
void (*tc_renderer_clear)(tc_Renderer *renderer, u32 color);

void (*tc_renderer_draw_array)(tc_Renderer *renderer, tc_Bitmap *texture, m4 transform, tc_Vertex *vertices, u32 vertices_cout);

void tc_platform_set_renderer_api(tc_RendererType type);

#endif // TC_RENDERER_H

