#ifndef TC_PLATFORM_H
#define TC_PLATFORM_H

#include "core/types.h"

enum tc_Event_Type
{
    TC_EVENT_DEFAULT,
    TC_EVENT_CLOSE_WINDOW,
    TC_EVENT_KEY_DOWN,
    TC_EVENT_KEY_UP,
    TC_EVENT_BUTTON_DOWN,
    TC_EVENT_BUTTON_UP,

    TC_EVENT_COUNT,
};

struct tc_EventWindow
{
    bool should_close;
};

struct tc_EventKeyboard
{
    u32 key;
};

struct tc_EventMouse
{
    u32 button;
};

struct tc_Event
{
    tc_Event_Type type;
    union 
    {
        tc_EventWindow window;
        tc_EventKeyboard keyboard;
        tc_EventMouse mouse;
    };
};

struct tc_Window;
struct tc_DebugFile
{
    void *memory;
    u64 size;
};

struct tc_Bitmap
{
    void *pixels;
    u32 width;
    u32 height;
    s32 pitch;
};

tc_Window *tc_platform_create_window(char *name, int x, int y, int width, int height);
void tc_platform_destroy_window(tc_Window *window);

tc_Event *tc_platform_pop_event(tc_Window *window);

void *tc_platfrom_virtual_alloc(void *base, size_t size);
void tc_platfrom_virtual_free(void *mem);

u32 tc_platfrom_get_ms();
void tc_platform_sleep(u32 ms);
u64 tc_platform_get_cycle_count();

tc_DebugFile tc_DEBUG_platform_read_file(char *path);
void tc_DEBUG_platform_free_file(tc_DebugFile *file);
tc_Bitmap tc_DEBUG_platform_load_bmp_file(char *path);

#endif // TC_PLATFORM_H
