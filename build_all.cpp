#include "platform/platform.cpp"
#include "renderer/software_renderer.cpp"
#include "renderer/renderer.cpp"
#include "gui/gui.cpp"

#include <stdio.h>

int main()
{
    u32 target_fps = 60;
    u32 target_ms = (u32)(1000.0f / (f32)target_fps);
    u32 last_ms = tc_platfrom_get_ms();
    bool limit_fps = true;
    f32 dt = 1.0f / (f32)target_fps;

    tc_Window *window = tc_platform_create_window("test", 100, 100, 1240, 720);
    tc_Renderer *renderer = tc_renderer_create(window, TC_RENDERER_SOFTWARE);

    f32 width = (f32)renderer->backbuffer.width;
    f32 height = (f32)renderer->backbuffer.height;
    
    tc_Bitmap texture = tc_DEBUG_platform_load_bmp_file("test.bmp");

    m4 projection = m4_perspective(f32_rad(70), width/height, 8.0f, 1.0f);

    m4 translation1 = m4_translate(_v3(-2,  1, -3.2f));
    m4 translation2 = m4_translate(_v3( 0,  1, -3.2f));
    m4 translation3 = m4_translate(_v3( 2,  1, -3.2f));
    m4 translation4 = m4_translate(_v3(-2, -1, -3.2f));
    m4 translation5 = m4_translate(_v3( 0, -1, -3.2f));
    m4 translation6 = m4_translate(_v3( 2, -1, -3.2f));
    
    v3 world_up = _v3(0.0f, 1.0f, 0.0f);
    v3 camera_pos = _v3(0.0f, 0.0f, 0.0f);
    v3 camera_dir = _v3(0.0f, 0.0f, -1.0f);
    f32 yaw =  -90.0f;
    f32 pitch = 0.0f;
    

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
        
        if(tc_DEBUG_platform_key_down('M'))
        {
            should_close = true;
        }

        s32 rel_x;
        s32 rel_y;
        tc_DEBUG_platfrom_relative_mode(window, &rel_x, &rel_y);
        
        f32 sensitivity = 0.1f;
        f32 mouse_offset_x = (f32)rel_x * sensitivity;
        f32 mouse_offset_y = (f32)rel_y * sensitivity;

        yaw += mouse_offset_x;
        pitch += mouse_offset_y;
         
        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
        
        camera_dir.x = f32_cos(f32_rad(yaw)) * f32_cos(f32_rad(pitch));
        camera_dir.y = f32_sin(f32_rad(pitch));
        camera_dir.z = f32_sin(f32_rad(yaw)) * f32_cos(f32_rad(pitch));
        camera_dir = v3_normalize(camera_dir);

        v3 right = v3_normalize(v3_cross(camera_dir, world_up));
        v3 up = v3_normalize(v3_cross(right, camera_dir));
        
        f32 speed = 5.0f * dt;
        if(tc_DEBUG_platform_key_down('W'))
        {
            camera_pos = camera_pos + camera_dir * speed;
        }
        if(tc_DEBUG_platform_key_down('S'))
        {
            camera_pos = camera_pos - camera_dir * speed;
        }
        if(tc_DEBUG_platform_key_down('A'))
        {
            camera_pos = camera_pos - right * speed;
        }
        if(tc_DEBUG_platform_key_down('D'))
        {
            camera_pos = camera_pos + right * speed;
        }
        if(tc_DEBUG_platform_key_down('Q'))
        {
            camera_pos = camera_pos + world_up * speed;
        }
        if(tc_DEBUG_platform_key_down('E'))
        {
            camera_pos = camera_pos - world_up * speed;
        }
        
        m4 view = m4_look_at(right, up, v3_normalize(-camera_dir), camera_pos);

        static f32 angle = 0;
        m4 rotation = m4_rotate_y(f32_rad(angle)) * m4_rotate_z(f32_rad(angle));
        angle += 1.0f;
        
        tc_renderer_clear(renderer, 0xFF222222);

        // TODO: maybe be able to set somthing like uniforms???
        tc_renderer_draw_array(renderer, &texture, projection * view * translation1 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * view * translation2 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * view * translation3 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * view * translation4 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * view * translation5 * rotation, cube, 36);
        tc_renderer_draw_array(renderer, &texture, projection * view * translation6 * rotation, cube, 36);

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
