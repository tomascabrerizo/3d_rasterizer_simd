#include "Windows.h"
#include "window_win32.h"

struct tc_BackBufferWin32
{
    HDC dc;
    HBITMAP bitmap;
    BITMAPINFO info;
    DWORD width;
    DWORD height;
    void *memory;
    float *depth;
};

tc_Renderer *tc_platform_create_software_renderer(tc_Window *window)
{
    tc_Renderer *renderer = (tc_Renderer *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tc_Renderer));
    renderer->platform = (tc_BackBufferWin32 *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tc_BackBufferWin32));

    tc_BackBufferWin32 *buffer = (tc_BackBufferWin32 *)renderer->platform;
    
    buffer->width = window->width;
    buffer->height = window->height;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = (LONG)buffer->width;
    buffer->info.bmiHeader.biHeight = -(INT)(buffer->height);
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;
    
    HDC window_dc = GetDC(window->handle);
    buffer->bitmap = CreateDIBSection(window_dc, &buffer->info, DIB_RGB_COLORS, &buffer->memory, 0, 0);
    buffer->dc = CreateCompatibleDC(window_dc);
    ReleaseDC(window->handle, window_dc);
    
    SelectObject(buffer->dc, buffer->bitmap);

    buffer->depth = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffer->width*buffer->height*sizeof(float));

    renderer->backbuffer.pixels = buffer->memory;
    renderer->backbuffer.depth = buffer->depth;
    renderer->backbuffer.width = buffer->width;
    renderer->backbuffer.height = buffer->height;
    renderer->backbuffer.pitch = buffer->width * 4;

    return renderer;
}

void tc_platform_destroy_software_renderer(tc_Renderer *renderer)
{
    tc_BackBufferWin32 *win32_buffer = (tc_BackBufferWin32 *)renderer->platform;
    DeleteObject(win32_buffer->bitmap);
    HeapFree(GetProcessHeap(), 0, win32_buffer->depth);
    HeapFree(GetProcessHeap(), 0, renderer->platform);
    HeapFree(GetProcessHeap(), 0, renderer);
}

void tc_software_renderer_swap_buffers(tc_Renderer *renderer, tc_Window *window)
{
    tc_BackBufferWin32 *win32_buffer = (tc_BackBufferWin32 *)renderer->platform;
    HDC dc = GetDC(window->handle);
    BitBlt(dc, 0, 0, (int)renderer->backbuffer.width, (int)renderer->backbuffer.height, win32_buffer->dc, 0, 0, SRCCOPY);
    ReleaseDC(window->handle, dc);
}
