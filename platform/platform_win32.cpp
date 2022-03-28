#include <Windows.h>
#include <Windowsx.h>
#include <stdio.h>
#include "platform.h"
#include "window_win32.h"

static s32 win32_mouse_x;
static s32 win32_mouse_y;

static u8 win32_DEBUG_key_array[512];
LRESULT CALLBACK win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    tc_Event *event = (tc_Event *)GetWindowLongPtr(window, 0);
    LRESULT result = 0;
    switch(message)
    {
        case WM_CREATE:
        {
        }break;
        case WM_CLOSE:
        case WM_DESTROY:
        {
            event->type = TC_EVENT_CLOSE_WINDOW; 
            event->window.should_close = true;
        }break;
        case WM_KEYDOWN:
        {
            win32_DEBUG_key_array[(u8)w_param] = 1;
        }break;
        case WM_KEYUP:
        {
            win32_DEBUG_key_array[(u8)w_param] = 0;
        }break;
        case WM_MOUSEMOVE:
        {
            win32_mouse_x = (s32)GET_X_LPARAM(l_param);
            win32_mouse_y = (s32)GET_Y_LPARAM(l_param);
        }break;
        case WM_MOVE:
        {
        }break;
        default:
        {
            result = DefWindowProcA(window, message, w_param, l_param);
        }break;
    }

    return result;
}

bool tc_platform_pop_event(tc_Window *window, tc_Event *event)
{
    event->type = TC_EVENT_DEFAULT;
    SetWindowLongPtr(window->handle, 0, (LONG_PTR)event);
    MSG message;
    bool is_event = PeekMessageA(&message, window->handle, 0, 0, PM_REMOVE);
    TranslateMessage(&message);
    DispatchMessageA(&message);

    return is_event;
}

tc_Window *tc_platform_create_window(char *name, int x, int y, int width, int height)
{
    tc_Window *window = (tc_Window *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tc_Window));

    HINSTANCE hinstance = GetModuleHandle(0);

    WNDCLASSA window_class = {};
    window_class.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    window_class.lpfnWndProc = win32_window_proc;
    window_class.hInstance = hinstance;
    window_class.lpszClassName = name;
    window_class.cbWndExtra = sizeof(tc_Event *);

    RegisterClassA(&window_class);

    window->width = (DWORD)width;
    window->height = (DWORD)height;
    window->handle = CreateWindowExA(0, window_class.lpszClassName, name,
                                     WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
                                     x, y, width, height, 
                                     0, 0, hinstance, 0);

    return window;
}

void tc_platform_destroy_window(tc_Window *window)
{
    DestroyWindow(window->handle); 
    HeapFree(GetProcessHeap(), 0, window);
}

void *tc_platfrom_virtual_alloc(void *base, size_t size)
{
    if(!base)
    {
        void *memory = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
        return memory;
    }
   
    void *memory = VirtualAlloc(base, size, MEM_COMMIT, PAGE_READWRITE);
    return memory;
}

void tc_platfrom_virtual_free(void *base)
{
    VirtualFree(base, 0, MEM_RELEASE);
}

static bool sleep_period_set;
void tc_platform_sleep(u32 ms)
{
    if(!sleep_period_set)
    {
        timeBeginPeriod(1);
        sleep_period_set = true;
    }

    Sleep(ms);
}

u64 tc_platform_get_cycle_count()
{
    u64 result = __rdtsc();
    return result;
}


static LARGE_INTEGER performance_freq;
static bool performance_freq_set;
u32 tc_platfrom_get_ms()
{
    if(!performance_freq_set)
    {
        QueryPerformanceFrequency(&performance_freq);
        performance_freq_set = true;
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    f64 seconds = (f64)counter.QuadPart / (f64)performance_freq.QuadPart;

    u32 result = (u32)(seconds * 1000);
    return result;
}

tc_DebugFile tc_DEBUG_platform_read_file(char *path)
{
    tc_DebugFile result = {};

    HANDLE file = CreateFileA((LPCSTR)path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); 
    ASSERT(file != INVALID_HANDLE_VALUE);
   
    LARGE_INTEGER file_size;
    GetFileSizeEx(file, &file_size);
    result.size = (u64)file_size.QuadPart;
    result.memory = VirtualAlloc(0, (DWORD)file_size.QuadPart, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    ReadFile(file, result.memory, (DWORD)file_size.QuadPart, 0, 0);
    CloseHandle(file);
    
    return result;
}

void tc_DEBUG_platform_free_file(tc_DebugFile *file)
{
    if(file && file->memory)
    {
        VirtualFree(file->memory, 0, MEM_RELEASE);
        file->memory = 0;
        file->size = 0;
    }
}

tc_Bitmap tc_DEBUG_platform_load_bmp_file(char *path)
{
    tc_DebugFile file = tc_DEBUG_platform_read_file(path);

    u8 *header = (u8 *)file.memory;
    u32 pixel_offset = *(u32 *)(header + 10);
    
    u8 *info_header = header + 14;
    u32 width = *(u32 *)(info_header + 4);
    u32 height = *(u32 *)(info_header + 8);
    u16 bpp = *(u16 *)(info_header + 14);

    ASSERT(bpp == 32);

    tc_Bitmap bitmap = {};
    bitmap.width = (u32)width;
    bitmap.height = (u32)height;
    bitmap.pixels = (void *)(header + pixel_offset);
    bitmap.pitch = (s32)(bitmap.width * 4);
    
    return bitmap;
}

bool tc_DEBUG_platform_key_down(u8 key)
{
    return win32_DEBUG_key_array[key] == 1;
}

void tc_DEBUG_platform_get_mouse_position(s32 *x, s32 *y)
{
    POINT position = {};
    GetCursorPos(&position);
    *x = (s32)position.x;
    *y = (s32)position.y;
}

void tc_DEBUG_platfrom_relative_mode(tc_Window *window, s32 *x, s32 *y)
{
    ShowCursor(false);

    POINT position;
    GetCursorPos(&position);
    static s32 last_mouse_x = position.x;
    static s32 last_mouse_y = position.y;
    
    *x = position.x - last_mouse_x;
    *y = last_mouse_y - position.y;

    RECT rect;
    GetWindowRect(window->handle, &rect);
    
    if(position.x > rect.right)
    {
        SetCursorPos(rect.left, position.y);
        position.x = rect.left;
    }
    else if(position.x < rect.left)
    {
        SetCursorPos(rect.right, position.y);
        position.x = rect.right;
    }
    
    if(position.y > rect.bottom)
    {
        SetCursorPos(position.x, rect.top);
        position.y = rect.top;
    }
    else if(position.y < rect.top)
    {
        SetCursorPos(position.x, rect.bottom);
        position.y = rect.bottom;
    }
    
    last_mouse_x = position.x;
    last_mouse_y = position.y;
}
