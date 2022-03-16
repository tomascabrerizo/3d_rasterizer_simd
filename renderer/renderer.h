#ifndef TC_RENDERER_H
#define TC_RENDERER_H

struct tc_Renderer;
struct tc_Window;

enum tc_RendererType
{
    TC_RENDERER_SOFTWARE,
    TC_RENDERER_OPENGL,
};

tc_Renderer *tc_renderer_create(tc_Window *window, tc_RendererType type);
tc_Renderer *(*tc_platform_renderer_create)(tc_Window *window);
void (*tc_renderer_destroy)(tc_Renderer *renderer);
void (*tc_renderer_swap_buffers)(tc_Renderer *renderer, tc_Window *window);
void (*tc_renderer_clear)(tc_Renderer *renderer, u32 color);

void tc_platform_set_renderer_api(tc_RendererType type);

#endif // TC_RENDERER_H

