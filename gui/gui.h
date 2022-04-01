#ifndef TC_GUI_H
#define TC_GUI_H

#include "core/types.h"

struct tc_GuiPoint
{
    s32 x, y;
};

struct tc_GuiRect
{
    tc_GuiPoint min;
    tc_GuiPoint max;
};

struct tc_GuiWindow
{
    u64 id; 
    const char *name;
    tc_GuiRect rect;
    tc_GuiWindow *next;
};

struct tc_GuiLayout
{
    tc_GuiRect button_rect;
};

struct tc_GuiInput
{
    tc_GuiPoint mouse;
};

#define TC_GUI_TEMP_BUFFER_SIZE 256

struct tc_GuiContext
{
    // NOTE: widget manipulation
    u64 active;
    u64 hot;
    
    // NOTE: window states
    tc_GuiWindow *windows; 
    tc_GuiWindow *active_window;
    
    // NOTE: active layout of the gui
    tc_GuiLayout layout;
    // NOTE: input for the gui
    tc_GuiInput input;

    // NOTE: render buffer
    f32 vertex_buffer[TC_GUI_TEMP_BUFFER_SIZE * 3];
    u32 vertex_buffer_count;
    s32 index_buffer[TC_GUI_TEMP_BUFFER_SIZE];
    u32 index_buffer_count;
};

void tc_gui_init(tc_GuiContext *ctx);
void tc_gui_free(tc_GuiContext *ctx);

bool tc_gui_window_begin(tc_GuiContext *ctx, const char *name, s32 x, s32 y, s32 w, s32 h);
void tc_gui_window_end(tc_GuiContext *ctx);

bool tc_gui_button(tc_GuiContext *ctx, const char *name);

#endif // TC_GUI_H
