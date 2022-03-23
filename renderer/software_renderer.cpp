#include "software_renderer.h"
#include "core/data_structures.h"
#ifdef _TC_WINDOWS_BUILD
#include "platform/software_renderer_win32.cpp"
#else
#endif

#include <emmintrin.h>

void tc_software_renderer_clear(tc_Renderer *renderer, u32 color)
{
    u32 width = renderer->backbuffer.width;
    u32 height = renderer->backbuffer.height;
    tc_software_renderer_draw_rect(renderer, 0, 0, width, height, color);
    for(u32 i = 0; i < height*width; ++i)
    {
        renderer->backbuffer.depth[i] = f32_infinity();
    }
}

void tc_software_renderer_draw_rect(tc_Renderer *renderer, u32 x, u32 y, u32 width, u32 height, u32 color)
{
    u8 *row = (u8 *)renderer->backbuffer.pixels + (y * renderer->backbuffer.pitch);
    for(u32 dy = 0; dy < height; ++dy)
    {
        u32 *pixel = (u32 *)row + x;
        for(u32 dx = 0; dx < width; ++dx)
        {
            *pixel++ = color;
        }
        row += renderer->backbuffer.pitch;
    }
}

tc_Vertex cube[36] = {
    //  - position            - color                   - texture coord
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f }},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f }},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 1.0f }},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f }},
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 1.0f }},
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f }},
                                                                  
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f }}, 
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f }},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 1.0f }},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f }},
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 1.0f }},
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f }},
                                                                  
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 1.0f, 0.0f }}, 
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f }},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 1.0f }},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f }},
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 0.0f }},
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f }},
                                                                  
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 1.0f, 0.0f }}, 
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f }},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 1.0f }},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f }},
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 0.0f }},
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f }},
                                                                  
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f }},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f }},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 0.0f }},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f }},
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 0.0f }},
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 0.0f, 1.0f }},
                                                                  
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f }},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f }},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 0.0f }},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f }},
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 0.0f }},
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 0.0f, 1.0f }}
};

inline v3 perspective_divide(v4 v)
{
    f32 inv_w = 1 / v.w;
    v3 result = {v.x * inv_w, v.y * inv_w, v.w};
    return result;
}

inline float edge_test(float x0, float y0, float x1, float y1, float px, float py)
{
    float dx = x1 - x0; 
    float dy = y1 - y0; 
    return (px - x0) * dy - (py - y0) * dx;
}

inline void sort_vertices(tc_Vertex **min, tc_Vertex **mid, tc_Vertex **max)
{
    v3 edge0 = (*max)->position - (*min)->position;
    v3 edge1 = (*mid)->position - (*min)->position;
    
    if((edge0.x * edge1.y - edge0.y * edge1.x) < 0)
    {
        tc_Vertex *temp = *mid;
        *mid = *max;
        *max = temp;
    }
}

void tc_software_renderer_draw_triangle_slow(tc_Renderer *renderer, tc_Bitmap *texture, tc_Vertex *vertex0, tc_Vertex *vertex1, tc_Vertex *vertex2)
{
    sort_vertices(&vertex0, &vertex1, &vertex2);

    tc_BackBuffer *buffer = &renderer->backbuffer;

    float x0 = vertex0->position.x;
    float y0 = vertex0->position.y;
    float x1 = vertex1->position.x;
    float y1 = vertex1->position.y;
    float x2 = vertex2->position.x;
    float y2 = vertex2->position.y;

    f32 one_over_z_0 = 1.0f / vertex0->position.z;
    f32 one_over_z_1 = 1.0f / vertex1->position.z;
    f32 one_over_z_2 = 1.0f / vertex2->position.z;

    f32 x_coord_over_z_0 = vertex0->coord.x * one_over_z_0;
    f32 y_coord_over_z_0 = vertex0->coord.y * one_over_z_0;
    f32 x_coord_over_z_1 = vertex1->coord.x * one_over_z_1;
    f32 y_coord_over_z_1 = vertex1->coord.y * one_over_z_1;
    f32 x_coord_over_z_2 = vertex2->coord.x * one_over_z_2;
    f32 y_coord_over_z_2 = vertex2->coord.y * one_over_z_2;

    f32 r_color_over_z_0 = vertex0->color.x * one_over_z_0;
    f32 g_color_over_z_0 = vertex0->color.y * one_over_z_0;
    f32 b_color_over_z_0 = vertex0->color.z * one_over_z_0;
    f32 r_color_over_z_1 = vertex1->color.x * one_over_z_1;
    f32 g_color_over_z_1 = vertex1->color.y * one_over_z_1;
    f32 b_color_over_z_1 = vertex1->color.z * one_over_z_1;
    f32 r_color_over_z_2 = vertex2->color.x * one_over_z_2;
    f32 g_color_over_z_2 = vertex2->color.y * one_over_z_2;
    f32 b_color_over_z_2 = vertex2->color.z * one_over_z_2;

    v3 edge0 = _v3(x1 - x0, y1 - y0, 1.0f);
    v3 edge1 = _v3(x2 - x1, y2 - y1, 1.0f);
    v3 edge2 = _v3(x0 - x2, y0 - y2, 1.0f);
    
    int min_x = (int)f32_min_3(x0, x1, x2);
    int max_x = (int)(f32_max_3(x0, x1, x2) + 1);
    int min_y = (int)f32_min_3(y0, y1, y2);
    int max_y = (int)(f32_max_3(y0, y1, y2) + 1);

    if(min_x < 0) min_x = 0;
    if(min_y < 0) min_y = 0;
    if(max_x > (int)(buffer->width-1)) max_x = (int)(buffer->width-1);
    if(max_y > (int)(buffer->height-1)) max_y = (int)(buffer->height-1);

    float inv_area = 1.0f / edge_test(x0, y0, x1, y1, x2, y2);

    char *row = (char *)buffer->pixels + (min_y * buffer->pitch) + (min_x * 4);

    for(int y = min_y; y <= max_y; ++y)
    {
        u32 *pixel = (u32 *)row;
        for(int x = min_x; x <= max_x; ++x)
        {
            f32 w0 = edge_test(x1, y1, x2, y2, (f32)x, (f32)y);
            f32 w1 = edge_test(x2, y2, x0, y0, (f32)x, (f32)y);
            f32 w2 = edge_test(x0, y0, x1, y1, (f32)x, (f32)y);
            
            bool test = true;
            test &= (w0 == 0) ? (((edge0.y == 0 && edge0.x < 0) || (edge0.y < 0))) : (w0 >= 0);
            test &= (w1 == 0) ? (((edge1.y == 0 && edge1.x < 0) || (edge1.y < 0))) : (w1 >= 0);
            test &= (w2 == 0) ? (((edge2.y == 0 && edge2.x < 0) || (edge2.y < 0))) : (w2 >= 0);
            if(test)
            {
                w0 *= inv_area;
                w1 *= inv_area;
                w2 *= inv_area;

                f32 one_over_z = (w0 * one_over_z_0) + (w1 * one_over_z_1) + (w2 * one_over_z_2);
                f32 z = 1.0f / one_over_z;
                
                if(z < buffer->depth[y * buffer->width + x])
                {
                    buffer->depth[y * buffer->width + x] = z;
                    
                    f32 u = (w0 * x_coord_over_z_0 + w1 * x_coord_over_z_1 + w2 * x_coord_over_z_2) * z;
                    f32 v = (w0 * y_coord_over_z_0 + w1 * y_coord_over_z_1 + w2 * y_coord_over_z_2) * z;
                    
                    u32 s = (u32)(u * (f32)texture->width);
                    u32 t = (u32)(v * (f32)texture->height);
                    if(s >= texture->width) s = texture->width - 1;
                    if(t >= texture->height) t = texture->height - 1;

                    u32 sample = *((u32 *)texture->pixels + (t * texture->width + s));
                    
                    u8 sr = (sample >> 16) & 0xFF; 
                    u8 sg = (sample >> 8 ) & 0xFF; 
                    u8 sb = (sample >> 0 ) & 0xFF; 

                    u8 r = (u8)((w0 * r_color_over_z_0 + w1 * r_color_over_z_1 + w2 * r_color_over_z_2) * z * 255.0f);
                    u8 g = (u8)((w0 * g_color_over_z_0 + w1 * g_color_over_z_1 + w2 * g_color_over_z_2) * z * 255.0f);
                    u8 b = (u8)((w0 * b_color_over_z_0 + w1 * b_color_over_z_1 + w2 * b_color_over_z_2) * z * 255.0f);
                    
                    f32 f = 0.5f;
                    u8 fr = (u8)((sr * f) + (r * (1.0f - f)));
                    u8 fg = (u8)((sg * f) + (g * (1.0f - f)));
                    u8 fb = (u8)((sb * f) + (b * (1.0f - f)));
                    
                    u32 color = (255 << 24) | (u32)(fr << 16) | (u32)(fg << 8) | (u32)(fb << 0);
                    *pixel = color; 
                }
            }
            pixel++;
        }
        row += buffer->pitch;
    }
}

struct tc_Edge
{
    __m128 one_step_x;
    __m128 one_step_y;
};

static inline tc_Edge create_edge(v3 v0, v3 v1, v3 p, u32 step_size_x, u32 step_size_y, __m128 *w_row)
{
    tc_Edge result = {};

    f32 A = v1.y - v0.y;
    f32 B = v0.x - v1.x;
    f32 C = v1.x * v0.y - v0.x * v1.y;

    result.one_step_x = _mm_set_ps1(A * (f32)step_size_x);
    result.one_step_y = _mm_set_ps1(B * (f32)step_size_y);
    
    // NOTE: note that the step is 4x1 so we calculate all the x
    // NOTE: _mm_set_ps arguments are y register order
    __m128 x = _mm_add_ps(_mm_set_ps1(p.x), _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f));
    __m128 y = _mm_set_ps1(p.y);

    *w_row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set_ps1(A), x), _mm_mul_ps(_mm_set_ps1(B), y)), _mm_set_ps1(C));

    return result;
}

void tc_software_renderer_draw_triangle_fast(tc_Renderer *renderer, tc_Bitmap *texture, rect2d clipping_rect, tc_Vertex *vertex0, tc_Vertex *vertex1, tc_Vertex *vertex2)
{
    sort_vertices(&vertex0, &vertex1, &vertex2);

    tc_BackBuffer *buffer = &renderer->backbuffer;
    u32 *texture_pixel = (u32 *)texture->pixels;
    
    tc_Vertex v0 = *vertex0;
    tc_Vertex v1 = *vertex1;
    tc_Vertex v2 = *vertex2;

    float x0 = v0.position.x;
    float y0 = v0.position.y;
    float z0 = v0.position.z;
    
    float x1 = v1.position.x;
    float y1 = v1.position.y;
    float z1 = v1.position.z;
    
    float x2 = v2.position.x;
    float y2 = v2.position.y;
    float z2 = v2.position.z;
    
    
    rect2d render_rect;
    render_rect.min.x = f32_min_3(x0, x1, x2);
    render_rect.max.x = (f32_max_3(x0, x1, x2) + 1);
    render_rect.min.y = f32_min_3(y0, y1, y2);
    render_rect.max.y = (f32_max_3(y0, y1, y2) + 1);
    
    render_rect.min.x = (f32)((s32)render_rect.min.x - 3 & ~(3)) + 4;
    render_rect.max.x = (f32)((s32)render_rect.max.x + 3 & ~(3));
    
    render_rect = rect2d_intersection(render_rect, clipping_rect);

    ASSERT(((u32)render_rect.min.x % 4 == 0) && ((u32)render_rect.max.x % 4 == 0));
   
    u32 x_step_size = 4;
    u32 y_step_size = 1;
    
    u32 buffer_width = buffer->width;
    u32 buffer_pitch = buffer->pitch;
    float *buffer_depth = buffer->depth;

    __m128 width_wide = _mm_set_ps1((f32)buffer->width);
    __m128 height_wide = _mm_set_ps1((f32)buffer->height);

    __m128 texture_width_wide = _mm_set_ps1((f32)texture->width);
    __m128 texture_height_wide = _mm_set_ps1((f32)texture->height);
    u32 texture_with = texture->width;
    //u32 texture_height = texture->height;

    __m128 one_over_z_0 = _mm_set_ps1(1.0f / z0);
    __m128 one_over_z_1 = _mm_set_ps1(1.0f / z1);
    __m128 one_over_z_2 = _mm_set_ps1(1.0f / z2);

    __m128 x_coord_over_z_0 = _mm_mul_ps(_mm_set_ps1(v0.coord.x), one_over_z_0);
    __m128 y_coord_over_z_0 = _mm_mul_ps(_mm_set_ps1(v0.coord.y), one_over_z_0);
    __m128 x_coord_over_z_1 = _mm_mul_ps(_mm_set_ps1(v1.coord.x), one_over_z_1);
    __m128 y_coord_over_z_1 = _mm_mul_ps(_mm_set_ps1(v1.coord.y), one_over_z_1);
    __m128 x_coord_over_z_2 = _mm_mul_ps(_mm_set_ps1(v2.coord.x), one_over_z_2);
    __m128 y_coord_over_z_2 = _mm_mul_ps(_mm_set_ps1(v2.coord.y), one_over_z_2);
    
    __m128 r_color_over_z_0 = _mm_mul_ps(_mm_set_ps1(v0.color.x), one_over_z_0);
    __m128 g_color_over_z_0 = _mm_mul_ps(_mm_set_ps1(v0.color.y), one_over_z_0);
    __m128 b_color_over_z_0 = _mm_mul_ps(_mm_set_ps1(v0.color.z), one_over_z_0);
    __m128 r_color_over_z_1 = _mm_mul_ps(_mm_set_ps1(v1.color.x), one_over_z_1);
    
    __m128 g_color_over_z_1 = _mm_mul_ps(_mm_set_ps1(v1.color.y), one_over_z_1);
    __m128 b_color_over_z_1 = _mm_mul_ps(_mm_set_ps1(v1.color.z), one_over_z_1);
    
    __m128 r_color_over_z_2 = _mm_mul_ps(_mm_set_ps1(v2.color.x), one_over_z_2);
    __m128 g_color_over_z_2 = _mm_mul_ps(_mm_set_ps1(v2.color.y), one_over_z_2);
    __m128 b_color_over_z_2 = _mm_mul_ps(_mm_set_ps1(v2.color.z), one_over_z_2);

    __m128 zero_wide = _mm_set_ps1(0.0f);
    __m128 one_wide = _mm_set_ps1(1.0f);
    __m128 xFF_F32_wide = _mm_set_ps1(255);
    __m128i xFF_wide = _mm_set1_epi32(0xFF);

    __m128 w0_row;
    __m128 w1_row;
    __m128 w2_row;

    v3 p = _v3((f32)render_rect.min.x, (f32)render_rect.min.y, 1.0f);
    tc_Edge edge12 = create_edge(vertex1->position, vertex2->position, p, x_step_size, y_step_size, &w0_row);
    tc_Edge edge20 = create_edge(vertex2->position, vertex0->position, p, x_step_size, y_step_size, &w1_row);
    tc_Edge edge01 = create_edge(vertex0->position, vertex1->position, p, x_step_size, y_step_size, &w2_row);
    
    __m128 inv_area = _mm_set_ps1(1.0f / edge_test(x0, y0, x1, y1, x2, y2));
    
    u32 min_x = (u32)render_rect.min.x;
    u32 min_y = (u32)render_rect.min.y;
    u32 max_x = (u32)render_rect.max.x;
    u32 max_y = (u32)render_rect.max.y;

    char *row = (char *)buffer->pixels + (min_y * buffer->pitch) + (min_x * 4);
    for(u32 y = min_y; y < max_y; y += y_step_size)
    {
        u32 *pixel = (u32 *)row;
       
        __m128 w0 = w0_row;
        __m128 w1 = w1_row;
        __m128 w2 = w2_row;

        for(u32 x = min_x; x < max_x; x += x_step_size)
        {
            // NOTE: edge test
            __m128 mask0 = _mm_cmpge_ps(w0, zero_wide);
            __m128 mask1 = _mm_cmpge_ps(w1, zero_wide);
            __m128 mask2 = _mm_cmpge_ps(w2, zero_wide);

            // TODO: fill rules with simd 
            //test &= (w0 == 0) ? (((edge0.y == 0 && edge0.x < 0) || (edge0.y < 0))) : (w0 >= 0);
            //test &= (w1 == 0) ? (((edge1.y == 0 && edge1.x < 0) || (edge1.y < 0))) : (w1 >= 0);
            //test &= (w2 == 0) ? (((edge2.y == 0 && edge2.x < 0) || (edge2.y < 0))) : (w2 >= 0);

            __m128 mask = _mm_and_ps(_mm_and_ps(mask0, mask1), mask2);
            u32 test_mask = (u32)_mm_movemask_ps(mask);
            if(test_mask)
            {
                __m128 nrm_w0 = _mm_mul_ps(w0, inv_area);
                __m128 nrm_w1 = _mm_mul_ps(w1, inv_area);
                __m128 nrm_w2 = _mm_mul_ps(w2, inv_area);

                __m128 one_over_z = _mm_add_ps(_mm_add_ps(_mm_mul_ps(nrm_w0, one_over_z_0), _mm_mul_ps(nrm_w1, one_over_z_1)), _mm_mul_ps(nrm_w2, one_over_z_2));
                __m128 z = _mm_div_ps(one_wide, one_over_z);

                // NOTE: z buffer test
                f32 *z_buffer_bucket = (buffer_depth + y * buffer_width + x);
                __m128 z_test_wide = _mm_loadu_ps(z_buffer_bucket);
                __m128 z_mask = _mm_cmplt_ps(z, z_test_wide); 
                u32 test_z_mask = (u32)_mm_movemask_ps(z_mask);
                if(test_z_mask)
                {
                    z_mask = _mm_and_ps(z_mask, mask); 
                    __m128 store_z = _mm_or_ps(_mm_and_ps(z_mask, z), _mm_andnot_ps(z_mask, z_test_wide)); 
                    _mm_storeu_ps(z_buffer_bucket, store_z);
                    mask = _mm_and_ps(mask, z_mask);

                    // NOTE: calculate texture sample 
                    __m128 u = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(nrm_w0, x_coord_over_z_0), _mm_mul_ps(nrm_w1, x_coord_over_z_1)), _mm_mul_ps(nrm_w2, x_coord_over_z_2)), z);
                    __m128 v = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(nrm_w0, y_coord_over_z_0), _mm_mul_ps(nrm_w1, y_coord_over_z_1)), _mm_mul_ps(nrm_w2, y_coord_over_z_2)), z);
                    __m128 s = _mm_max_ps(_mm_min_ps(_mm_mul_ps(u, texture_width_wide ), _mm_sub_ps(texture_width_wide, one_wide)), zero_wide);
                    __m128 t = _mm_max_ps(_mm_min_ps(_mm_mul_ps(v, texture_height_wide), _mm_sub_ps(texture_height_wide, one_wide)), zero_wide);
                    
                    _MM_SET_ROUNDING_MODE(_MM_ROUND_DOWN);
                    __m128i us =_mm_cvtps_epi32(s);
                    __m128i ut =_mm_cvtps_epi32(t);
                    u32 *s_ptr = (u32 *)&us;
                    u32 *t_ptr = (u32 *)&ut;
                    u32 new_color0 = *(texture_pixel + (t_ptr[0]*texture_with + s_ptr[0]));
                    u32 new_color1 = *(texture_pixel + (t_ptr[1]*texture_with + s_ptr[1]));
                    u32 new_color2 = *(texture_pixel + (t_ptr[2]*texture_with + s_ptr[2]));
                    u32 new_color3 = *(texture_pixel + (t_ptr[3]*texture_with + s_ptr[3]));
                    
                    __m128i sample = _mm_set_epi32((s32)new_color3, (s32)new_color2, (s32)new_color1, (s32)new_color0);
                    __m128i old_sample = _mm_loadu_si128((__m128i *)pixel);
                    
                    //NOTE: vertex color lerp
                    __m128 r_sample = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sample,16), xFF_wide));
                    __m128 g_sample = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sample, 8), xFF_wide));
                    __m128 b_sample = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sample, 0), xFF_wide));
                    
                    __m128 r = _mm_mul_ps(_mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(nrm_w0, r_color_over_z_0), _mm_mul_ps(nrm_w1, r_color_over_z_1)), _mm_mul_ps(nrm_w2, r_color_over_z_2)), z), xFF_F32_wide);
                    __m128 g = _mm_mul_ps(_mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(nrm_w0, g_color_over_z_0), _mm_mul_ps(nrm_w1, g_color_over_z_1)), _mm_mul_ps(nrm_w2, g_color_over_z_2)), z), xFF_F32_wide);
                    __m128 b = _mm_mul_ps(_mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(nrm_w0, b_color_over_z_0), _mm_mul_ps(nrm_w1, b_color_over_z_1)), _mm_mul_ps(nrm_w2, b_color_over_z_2)), z), xFF_F32_wide);
                    
                    __m128 f = _mm_set_ps1(0.5f);
                    __m128i fr = _mm_cvtps_epi32(_mm_add_ps(_mm_mul_ps(r_sample, f), _mm_mul_ps(r, _mm_sub_ps(one_wide, f))));
                    __m128i fg = _mm_cvtps_epi32(_mm_add_ps(_mm_mul_ps(g_sample, f), _mm_mul_ps(g, _mm_sub_ps(one_wide, f))));
                    __m128i fb = _mm_cvtps_epi32(_mm_add_ps(_mm_mul_ps(b_sample, f), _mm_mul_ps(b, _mm_sub_ps(one_wide, f))));
                    
                    sample = _mm_or_si128(_mm_or_si128(_mm_or_si128(_mm_slli_epi32(xFF_wide, 24), _mm_slli_epi32(fr, 16)), _mm_slli_epi32(fg, 8)), _mm_slli_epi32(fb, 0));
                    
                    __m128i write_mask = _mm_castps_si128(mask); 
                    __m128i output_color = _mm_or_si128(_mm_and_si128(write_mask, sample), _mm_andnot_si128(write_mask, old_sample));
                    
                    _mm_storeu_si128((__m128i *)pixel, output_color);
                }
            }
            
            w0 = _mm_add_ps(w0, edge12.one_step_x);
            w1 = _mm_add_ps(w1, edge20.one_step_x);
            w2 = _mm_add_ps(w2, edge01.one_step_x);
            pixel += x_step_size;
        }
        
        w0_row = _mm_add_ps(w0_row, edge12.one_step_y);
        w1_row = _mm_add_ps(w1_row, edge20.one_step_y);
        w2_row = _mm_add_ps(w2_row, edge01.one_step_y);
        row += buffer_pitch;
    }
}

void tc_software_renderer_draw_array(tc_Renderer *renderer, tc_Bitmap *texture, rect2d clipping_rect, m4 transform, tc_Vertex *vertices, u32 count)
{
    for(u32 i = 0; i < count; i+=3)
    {
        tc_Vertex vertex0 = vertices[i + 0];
        tc_Vertex vertex1 = vertices[i + 1];
        tc_Vertex vertex2 = vertices[i + 2];
        
        v4 vertex0_4 = transform * to_v4(vertex0.position);
        v4 vertex1_4 = transform * to_v4(vertex1.position);
        v4 vertex2_4 = transform * to_v4(vertex2.position);

        f32 hw = (f32)renderer->backbuffer.width / 2.0f;
        f32 hh = (f32)renderer->backbuffer.height / 2.0f;

        vertex0.position = perspective_divide(vertex0_4);
        vertex1.position = perspective_divide(vertex1_4);
        vertex2.position = perspective_divide(vertex2_4);

        vertex0.position.x = vertex0.position.x * hw + hw; 
        vertex0.position.y = -vertex0.position.y * hh + hh; 
        vertex1.position.x = vertex1.position.x * hw + hw;
        vertex1.position.y = -vertex1.position.y * hh + hh; 
        vertex2.position.x = vertex2.position.x * hw + hw; 
        vertex2.position.y = -vertex2.position.y * hh + hh; 

        tc_software_renderer_draw_triangle_fast(renderer, texture, clipping_rect, &vertex0, &vertex1, &vertex2);
    }
}

void tc_software_renderer_push_draw_command(tc_Renderer *renderer, tc_DrawCommand command)
{
    array_push(renderer->command_buffer_darr, command);
}

void tc_software_renderer_begin(tc_Renderer *renderer)
{
    array_clear(renderer->command_buffer_darr);
}

inline static void tc_software_renderer_draw_command_buffer(tc_Renderer *renderer, rect2d clipping_rect)
{
    for(u32 command_index = 0; command_index < array_size(renderer->command_buffer_darr); ++command_index)
    {
        tc_DrawCommand *command = renderer->command_buffer_darr + command_index;
        tc_software_renderer_draw_array(renderer, command->texture, clipping_rect, command->transform, command->vertices, command->vertex_count);
    }
}

struct RenderWork
{
    tc_Renderer *renderer;
    rect2d clipping_rect;
};

void render_work_function(void *param)
{
    RenderWork *render_work = (RenderWork *)param;
    tc_software_renderer_draw_command_buffer(render_work->renderer, render_work->clipping_rect);
}

void tc_software_renderer_end(tc_Renderer *renderer)
{
    f32 width = (f32)renderer->backbuffer.width;
    f32 height = (f32)renderer->backbuffer.height;

    const u32 num_tiles_x = 5;
    const u32 num_tiles_y = 4;
    
    v2 tile_dim = _v2(width/(f32)num_tiles_x, height/(f32)num_tiles_y);
    
    thread_queue_begin(renderer->thread_queue);
    
    // TODO: fix the RenderWork allocations
    RenderWork render_work[num_tiles_y][num_tiles_x];
    for(u32 tile_y = 0; tile_y < num_tiles_y; ++tile_y)
    {
        for(u32 tile_x = 0; tile_x < num_tiles_x; ++tile_x)
        {
            render_work[tile_y][tile_x].clipping_rect = rect2d_min_dim(_v2((f32)tile_x * tile_dim.x, (f32)tile_y * tile_dim.y), tile_dim - _v2(0, 0));
            
            render_work[tile_y][tile_x].renderer = renderer;
            ThreadWork work = {render_work_function, (void *)&render_work[tile_y][tile_x]};
            thread_queue_push_work(renderer->thread_queue, work);
            
        }
    }
    thread_queue_end(renderer->thread_queue);
}
