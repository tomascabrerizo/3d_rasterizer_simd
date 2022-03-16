#include "renderer.cpp"

#include <Windows.h>

#define ASSERT(x) do { if(!(x)) { *((int *)0) = 0; } } while(0)

struct Win32BackBuffer
{
    HDC dc;
    HBITMAP bitmap;
    BITMAPINFO info;
    void *memory;
    DWORD width;
    DWORD height;
};

static bool global_running;
static Win32BackBuffer global_backbuffer;

void win32_create_backbuffer(Win32BackBuffer *buffer, HDC dc)
{
    buffer->width = BACKBUFFER_WIDTH;
    buffer->height = BACKBUFFER_HEIGHT;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = (LONG)buffer->width;
    buffer->info.bmiHeader.biHeight = -(INT)(buffer->height);
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;
    buffer->bitmap = CreateDIBSection(dc, &buffer->info, DIB_RGB_COLORS, &buffer->memory, 0, 0);
    buffer->dc = CreateCompatibleDC(dc);
    SelectObject(buffer->dc, buffer->bitmap);
}

void win32_destroy_backbuffer(Win32BackBuffer *buffer)
{
    DeleteObject(buffer->bitmap);
}

Bitmap win32_load_tiled_bitmap(char *path)
{
    HANDLE file = CreateFileA((LPCSTR)path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); 
    ASSERT(file != INVALID_HANDLE_VALUE);
   
    LARGE_INTEGER file_size;
    GetFileSizeEx(file, &file_size);
    void *file_buffer = VirtualAlloc(0, (DWORD)file_size.QuadPart, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    ReadFile(file, file_buffer, (DWORD)file_size.QuadPart, 0, 0);

    BYTE *header = (BYTE *)file_buffer;
    DWORD pixel_offset = *(DWORD *)(header + 10);
    DWORD *pixels = (DWORD *)(header + pixel_offset);
    (void)pixels;
    
    BYTE *info_header = header + 14;
    DWORD width = *(DWORD *)(info_header + 4);
    DWORD height = *(DWORD *)(info_header + 8);
    WORD bpp = *(WORD *)(info_header + 14);

    ASSERT(bpp == 32);

    Bitmap bitmap = {};
    bitmap.real_width = (width + (BACKBUFFER_ALIGN_W-1)) & ~(BACKBUFFER_ALIGN_W-1);
    bitmap.real_height = (height + (BACKBUFFER_ALIGN_H-1)) & ~(BACKBUFFER_ALIGN_H-1);
    bitmap.size = bitmap.real_width * bitmap.real_height * BACKBUFFER_PIXEL_S;
    bitmap.width = (uint32_t)width;
    bitmap.height = (uint32_t)height;
    bitmap.memory = VirtualAlloc(0, (DWORD)bitmap.size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    bitmap.pitch = (int32_t)(bitmap.real_width * BACKBUFFER_PIXEL_S);

    // TODO: not implemented

    VirtualFree(file_buffer, 0, MEM_RELEASE);
    CloseHandle(file);

    return bitmap;
}

void win32_delete_bitmap(Bitmap* bitmap)
{
    VirtualFree(bitmap->memory, 0, MEM_RELEASE);
    bitmap->memory = 0;
    bitmap->size = 0;
}

inline float min_f2(float f0, float f1)
{
    return f0 < f1 ? f0 : f1;
}

inline float min_f3(float f0, float f1, float f2)
{
    return min_f2(min_f2(f0, f1), f2);
}

inline float max_f2(float f0, float f1)
{
    return f0 > f1 ? f0 : f1;
}

inline float max_f3(float f0, float f1, float f2)
{
    return max_f2(max_f2(f0, f1), f2);
}

inline float edge_test(float x0, float y0, float x1, float y1, float px, float py)
{
    float dx = x1 - x0; 
    float dy = y1 - y0; 
    return (px - x0) * dy - (py - y0) * dx;
}

void prush_triangle(Bitmap *buffer, float *vertices, int color)
{
    float x0 = vertices[0];
    float y0 = vertices[1];
    float x1 = vertices[2];
    float y1 = vertices[3];
    float x2 = vertices[4];
    float y2 = vertices[5];

    int min_x = (int)min_f3(x0, x1, x2);
    int max_x = (int)(max_f3(x0, x1, x2) + 1);
    int min_y = (int)min_f3(y0, y1, y2);
    int max_y = (int)(max_f3(y0, y1, y2) + 1);

    char *row = (char *)buffer->memory + (min_y * buffer->pitch) + (min_x * 4); 
    for(int y = min_y; y <= max_y; ++y)
    {
        int *pixel = (int *)row;
        for(int x = min_x; x <= max_x; ++x)
        {
            float edge0 = edge_test(x0, y0, x1, y1, (float )x, (float)y);
            float edge1 = edge_test(x1, y1, x2, y2, (float )x, (float)y);
            float edge2 = edge_test(x2, y2, x0, y0, (float )x, (float)y);
            int test = (int)edge0|(int)edge1|(int)edge2;
            (void)test;
            if(test >= 0)
            {
                *pixel = color;
            }
            pixel++;
        }
        row += buffer->pitch;
    }
}

LRESULT CALLBACK win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch(message)
    {
        case WM_CREATE:
        {
            HDC dc = GetDC(window);
            win32_create_backbuffer(&global_backbuffer, dc);
            ReleaseDC(window, dc);
        }break;
        case WM_CLOSE:
        case WM_DESTROY:
        {
            win32_destroy_backbuffer(&global_backbuffer);
            global_running = false;
        }break;
        default:
        {
            result = DefWindowProcA(window, message, w_param, l_param);
        }break;
    }
    return result;
}

void win32_copy_to_backbuffer(Win32BackBuffer *win32_buffer, Bitmap *bitmap)
{
    DWORD win32_pitch = (win32_buffer->width * BACKBUFFER_PIXEL_S);
    
    BYTE *pixel_row = (BYTE *)bitmap->memory;
    BYTE *win32_row = (BYTE *)win32_buffer->memory;
    for(DWORD y = 0; y < win32_buffer->height; ++y)
    {
        DWORD *pixel = (DWORD *)pixel_row;
        DWORD *win32_pixel = (DWORD *)win32_row;
        for(DWORD x = 0; x < win32_buffer->width; ++x)
        {
            *win32_pixel++ = *pixel++;
        }
        win32_row += win32_pitch;
        pixel_row += bitmap->pitch;
    }
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev_instace, PSTR cmd_line, int cmd_show)
{
    WNDCLASSA window_class = {};
    window_class.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    window_class.lpfnWndProc = win32_window_proc;
    window_class.hInstance = hinstance;
    window_class.lpszClassName = "win32_class_name";

    RegisterClassA(&window_class);

    HWND window = CreateWindowExA(0, window_class.lpszClassName, "window",
                                  WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
                                  CW_USEDEFAULT, CW_USEDEFAULT, 
                                  WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hinstance, 0);
    
    Bitmap test_texture = win32_load_tiled_bitmap("test.bmp");

    // Aling renderer specific backbuffer for AVX
    // this backbuffer should be allocated by the renderer
    DWORD backbuffer_width_align = (BACKBUFFER_WIDTH + (BACKBUFFER_ALIGN_W-1)) & ~(BACKBUFFER_ALIGN_W-1);
    DWORD backbuffer_height_align = (BACKBUFFER_HEIGHT + (BACKBUFFER_ALIGN_H-1)) & ~(BACKBUFFER_ALIGN_H-1);
    DWORD real_backbuffer_size = (backbuffer_width_align * backbuffer_height_align) * BACKBUFFER_PIXEL_S;
    void *backbuffer_memory = VirtualAlloc(0, real_backbuffer_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    INT backbuffer_pitch = (INT)backbuffer_width_align * BACKBUFFER_PIXEL_S;

    Bitmap backbuffer = {};
    backbuffer.memory = backbuffer_memory;
    backbuffer.width = BACKBUFFER_WIDTH;
    backbuffer.height = BACKBUFFER_HEIGHT;
    backbuffer.real_width = backbuffer_width_align;
    backbuffer.real_height = backbuffer_height_align;
    backbuffer.size = real_backbuffer_size;
    backbuffer.pitch = backbuffer_pitch;

    float tringle[] = 
    {
        100, 100, 120, 300, 200, 250
    };
    
    global_running = true;
    while(global_running)
    {
        MSG message;
        while(PeekMessageA(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message); 
            DispatchMessageA(&message); 
        }
    
        prush_triangle(&backbuffer, tringle, 0xFF00FF00);

        win32_copy_to_backbuffer(&global_backbuffer, &backbuffer);

        HDC dc = GetDC(window);
        BitBlt(dc, 0, 0, (int)global_backbuffer.width, (int)global_backbuffer.height, global_backbuffer.dc, 0, 0, SRCCOPY);
        ReleaseDC(window, dc);
    }

    win32_delete_bitmap(&backbuffer);
}
