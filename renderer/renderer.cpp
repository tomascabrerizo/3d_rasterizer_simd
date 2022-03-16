#include "renderer.h"
#include "software_renderer.h"

void tc_platform_set_renderer_api(tc_RendererType type)
{
    switch(type)
    {
        case TC_RENDERER_SOFTWARE:
        {
            tc_platform_renderer_create = tc_platform_create_software_renderer;
            tc_renderer_destroy = tc_platform_destroy_software_renderer;
            tc_renderer_swap_buffers = tc_software_renderer_swap_buffers;
            tc_renderer_clear = tc_software_renderer_clear;
        }break;
        case TC_RENDERER_OPENGL:
        {
        }break;
    }
}

tc_Renderer *tc_renderer_create(tc_Window *window, tc_RendererType type)
{
    tc_platform_set_renderer_api(type);
    return tc_platform_renderer_create(window);
}
