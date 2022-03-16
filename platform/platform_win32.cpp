#include <Windows.h>
#include "platform.h"
#include "window_win32.h"

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
    timeBeginPeriod(1);

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

void tc_platform_sleep(u32 ms)
{
    Sleep(ms);
}

u64 tc_platform_get_cycle_count()
{
    u64 result = __rdtsc();
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
