#include <Windows.h>
#include "platform.h"
#include "window_win32.h"

static u8 win32_DEBUG_key_array[512];
static s32 win32_DEBUG_window_pos_x;
static s32 win32_DEBUG_window_pos_y;
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
        case WM_MOVE:
        {
            win32_DEBUG_window_pos_x = LOWORD(l_param);
            win32_DEBUG_window_pos_y = HIWORD(l_param);
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

    win32_DEBUG_window_pos_x = x;
    win32_DEBUG_window_pos_y = y;
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

void tc_DEBUG_platform_get_mouse_position(u32 *x, u32 *y)
{
    POINT position = {};
    GetCursorPos(&position);
    *x = (u32)position.x;
    *y = (u32)position.y;
}

void tc_DEBUG_platfrom_relative_mode(tc_Window *window)
{
    RECT rect;
    GetWindowRect(window->handle, &rect);
    s32 width = rect.right - rect.left;
    s32 height = rect.bottom - rect.top;
    SetCursorPos(win32_DEBUG_window_pos_x + width/2, win32_DEBUG_window_pos_y + height/2);
    ShowCursor(false);
}
