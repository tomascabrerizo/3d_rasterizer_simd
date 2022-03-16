#include "platform/platform.cpp"
#include "renderer/software_renderer.cpp"
#include "renderer/renderer.cpp"

int main()
{
    tc_Window *window = tc_platform_create_window("test", 100, 100, 1240, 720);
    tc_Renderer *renderer = tc_renderer_create(window, TC_RENDERER_SOFTWARE);
    
    bool should_close = false;
    while(!should_close)
    {
        tc_Event event;
        while(tc_platform_pop_event(window, &event))
        {
            switch(event.type)
            {
                case TC_EVENT_CLOSE_WINDOW:
                {
                    should_close = event.window.should_close;
                }
            }
        }
        
        tc_renderer_clear(renderer, 0xFF222222);
        tc_rasterizer_test(renderer);

        tc_renderer_swap_buffers(renderer, window);

        tc_platform_sleep(26);
    }
    
    tc_renderer_destroy(renderer);
    tc_platform_destroy_window(window);
}
