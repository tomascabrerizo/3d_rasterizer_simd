#include "gui.h"
#include "core/hash.h"
#include <stdlib.h>

#include <assert.h>
#define TC_GUI_ASSER(a) assert((a))

static u32 tc_gui_global_seed = 7;

static inline void *tc_gui_malloc(u64 size)
{
    return malloc(size);
}

static inline void tc_gui_free(void *ptr)
{
    free(ptr);
}

static inline u32 tc_strlen(const char *str)
{
    u32 cout = 0;
    while(*str)
    {
        ++cout;
        ++str;
    }
    return cout;
}

//
// NOTE: tc_gui MATH functions
//

inline bool tc_gui_point_intersect_rect(tc_GuiPoint mouse, tc_GuiRect rect)
{
    bool result = (mouse.x >= rect.min.x && mouse.x <= rect.max.x && mouse.y >= rect.min.y && mouse.y <= rect.max.y) ? true : false;
    return result; 
}

//
// NOTE: tc_gui HELPER functions
//

inline void tc_gui_set_active(tc_GuiContext *ctx, u64 control)
{
    ctx->active = control;
}

inline void tc_gui_set_hot(tc_GuiContext *ctx, u64 control)
{
    ctx->hot = control;
}

inline bool tc_gui_is_active(tc_GuiContext *ctx, u64 control)
{
    bool result = ctx->active == control ? true : false;
    return result;
}

inline bool tc_gui_is_hot(tc_GuiContext *ctx, u64 control)
{
    bool result = ctx->hot == control ? true : false;
    return result;
}

inline bool tc_gui_any_active(tc_GuiContext *ctx)
{
    bool result = ctx->active != 0;
    return result;
}

inline bool tc_gui_any_hot(tc_GuiContext *ctx)
{
    bool result = ctx->hot != 0;
    return result;
}

inline void tc_gui_push_vertices(tc_GuiContext *ctx, f32 *vertices, u32 count)
{
    TC_GUI_ASSER(ctx->vertex_buffer_count + count <= (TC_GUI_TEMP_BUFFER_SIZE * 3));
    memcpy(ctx->vertex_buffer + ctx->vertex_buffer_count, vertices, count*sizeof(*vertices));
    ctx->vertex_buffer_count += count;
}

inline void tc_gui_push_indices(tc_GuiContext *ctx, s32 *indices, u32 count)
{
    TC_GUI_ASSER(ctx->index_buffer_count + count <= TC_GUI_TEMP_BUFFER_SIZE);
    memcpy(ctx->index_buffer + ctx->index_buffer_count, indices, count*sizeof(*indices));
    ctx->index_buffer_count += count;
}

inline u64 tc_gui_hash(const char* name)
{
    // TODO: this function get call a lot, in the future, cash ($) some common results
    u64 result = tc_hash_64((const void *)name, (s32)tc_strlen(name), tc_gui_global_seed);
    return result;
}

inline tc_GuiPoint tc_gui_window_relative_mose(tc_GuiContext *ctx)
{
    tc_GuiWindow *window = ctx->active_window;
    TC_GUI_ASSER(window);
    tc_GuiPoint result = ctx->input.mouse;
    result.x -= window->rect.min.x;
    result.y -= window->rect.min.y;
    return result;
}

inline tc_GuiRect tc_gui_button_get_rect(tc_GuiContext *ctx)
{
    tc_GuiWindow *window = ctx->active_window;
    TC_GUI_ASSER(window);
    tc_GuiRect result = ctx->layout.button_rect;
    // TODO: find a good way to do auto layout for the buttons
    // for now the all are at the corner (0, 0) of its parent window
    result.min = window->rect.min; 
    return result;
}

static tc_GuiWindow *tc_gui_window_get_by_id(tc_GuiContext *ctx, u64 window_id)
{
    tc_GuiWindow *window = ctx->windows; 
    while(window)
    {
        if(window->id == window_id)
        {
            return window;
        }
        window = window->next;
    }
    return window;
}

static inline void tc_gui_window_register(tc_GuiContext *ctx, tc_GuiWindow *window)
{
    window->next = ctx->windows;
    ctx->windows = window;
}

void tc_gui_init(tc_GuiContext *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
}

void tc_gui_free(tc_GuiContext *ctx)
{
    // TODO: release gui context
}

//
// NOTE: tc_gui MAIN functions
//

bool tc_gui_window_begin(tc_GuiContext *ctx, const char *name, s32 x, s32 y, s32 w, s32 h)
{
    u64 window_id = tc_hash_64((const void *)name, (s32)tc_strlen(name), tc_gui_global_seed);
    tc_GuiWindow *window = tc_gui_window_get_by_id(ctx, window_id);
    if(!window)
    {
        window = (tc_GuiWindow *)tc_gui_malloc(sizeof(tc_GuiWindow));
        window->id = window_id;
        window->name = name;
        window->next = 0;
        window->rect.min = {x, y};
        window->rect.max = {x + w, y + h};
        tc_gui_window_register(ctx, window);
        
    }
    ctx->active_window = window;

    // NOTE: prepare window geometry
    // TODO: for now the window geometry is prepare in begin but
    // for auto layout purpose a better idea shoud be reserve vertex and index buffer
    // space but fill them at tc_gui_window_end where all the window layout
    // can be calculated.
    f32 vertices[12] =
    {
        (f32)window->rect.min.x, (f32)window->rect.min.y, 0.0f,
        (f32)window->rect.min.x, (f32)window->rect.max.y, 0.0f,
        (f32)window->rect.max.x, (f32)window->rect.max.y, 0.0f,
        (f32)window->rect.max.x, (f32)window->rect.min.y, 0.0f
    };
    s32 indices[6] = {0, 1, 3, 1, 2, 3};

    tc_gui_push_vertices(ctx, vertices, 12);
    tc_gui_push_indices(ctx, indices, 6);
    
    return false;
}

void tc_gui_window_end(tc_GuiContext *ctx)
{
    ctx->active_window = 0;
}

bool tc_gui_button(tc_GuiContext *ctx, const char *name)
{
    tc_GuiPoint mouse = tc_gui_window_relative_mose(ctx);
    tc_GuiRect rect = tc_gui_button_get_rect(ctx);

    // NOTE: process button
    if(tc_gui_point_intersect_rect(mouse, rect))
    {
        u64 button = tc_gui_hash(name);
        tc_gui_set_hot(ctx, button);
    }

    // NOTE: prepare button geometry
    f32 vertices[12] =
    {
        (f32)rect.min.x, (f32)rect.min.y, 0.0f,
        (f32)rect.min.x, (f32)rect.max.y, 0.0f,
        (f32)rect.max.x, (f32)rect.max.y, 0.0f,
        (f32)rect.max.x, (f32)rect.min.y, 0.0f
    };
    s32 indices[6] = {0, 1, 3, 1, 2, 3};

    tc_gui_push_vertices(ctx, vertices, 12);
    tc_gui_push_indices(ctx, indices, 6);

    return false;
}
