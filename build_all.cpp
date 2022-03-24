#include "platform/platform.cpp"
#include "renderer/software_renderer.cpp"
#include "renderer/renderer.cpp"

#include <stdio.h>

int main()
{
    u32 target_fps = 60;
    u32 target_ms = (u32)(1000.0f / (f32)target_fps);
    u32 last_ms = tc_platfrom_get_ms();
    bool limit_fps = false;

    tc_Window *window = tc_platform_create_window("test", 100, 100, 1240, 720);
    tc_Renderer *renderer = tc_renderer_create(window, TC_RENDERER_SOFTWARE);

    f32 width = (f32)renderer->backbuffer.width;
    f32 height = (f32)renderer->backbuffer.height;
    
    m4 projection = m4_perspective(f32_rad(70), width/height, 100.0f, 1.0f);
    //m4 translation1 = m4_translate(_v3(-2,  1, -3.2f));
    //m4 translation2 = m4_translate(_v3( 0,  1, -3.2f));
    //m4 translation3 = m4_translate(_v3( 2,  1, -3.2f));
    //m4 translation4 = m4_translate(_v3(-2, -1, -3.2f));
    //m4 translation5 = m4_translate(_v3( 0, -1, -3.2f));
    //m4 translation6 = m4_translate(_v3( 2, -1, -3.2f));

    tc_Bitmap texture = tc_DEBUG_platform_load_bmp_file("test.bmp");

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
        

        static f32 angle = 0;
        m4 rotation = m4_rotate_y(f32_rad(angle)) * m4_rotate_z(f32_rad(angle));
        angle += 1.0f;
        f32 offset = f32_sin(f32_rad(angle * 0.1f)) * 4;
        if(offset > 0.2)
        {
            int break_here = 0;
            (void)break_here;
        }

        m4 translation1 = m4_translate(_v3(-2,  1, -3.2f)) * m4_translate(_v3(0, 0, offset));
        m4 translation2 = m4_translate(_v3( 0,  1, -3.2f)) * m4_translate(_v3(0, 0, offset));
        m4 translation3 = m4_translate(_v3( 2,  1, -3.2f)) * m4_translate(_v3(0, 0, offset));
        m4 translation4 = m4_translate(_v3(-2, -1, -3.2f)) * m4_translate(_v3(0, 0, offset));
        m4 translation5 = m4_translate(_v3( 0, -1, -3.2f)) * m4_translate(_v3(0, 0, offset));
        m4 translation6 = m4_translate(_v3( 2, -1, -3.2f)) * m4_translate(_v3(0, 0, offset));
        
        tc_renderer_clear(renderer, 0xFF222222);

        // TODO: maybe be able to set somthing like uniforms???
        tc_renderer_draw_array(renderer, &texture, projection * translation1 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * translation2 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * translation3 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * translation4 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * translation5 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * translation6 * rotation, cube, 36);

        tc_renderer_swap_buffers(renderer, window);

        u32 current_ms = tc_platfrom_get_ms();
        u32 frame_ms = current_ms - last_ms;
        if(limit_fps)
        {
            if(frame_ms < target_ms)
            {
                tc_platform_sleep(target_ms - frame_ms);
            }
            current_ms = tc_platfrom_get_ms();
            frame_ms = current_ms - last_ms;
        }
        last_ms = current_ms;
        
        printf("ms: %d\n", frame_ms);
    }
    
    tc_renderer_destroy(renderer);
    tc_platform_destroy_window(window);
}
