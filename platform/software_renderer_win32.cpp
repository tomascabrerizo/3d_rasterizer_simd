#include <Windows.h>
#include <stdio.h>
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

//
// multithreadin training XD!
//

struct ThreadWork 
{
    (*tread_work)(void *attr);
    void *attributes;
};
static ThreadWork work_queue[16];
static volatile s32 remaining_work;

void push_work(u32 value)
{
    ThreadWork *work = work_queue + remaining_work++;
    work->value = value;
}

DWORD thread_do_work(void *parameter)
{
    printf("hello from different thread number %d\n", (int)(u64)parameter);
    while(true)
    {   
        if(remaining_work >= 0)
        {
            s32 work_index = (s32)InterlockedDecrement((LONG volatile *)&remaining_work);
            if(work_index >= 0)
            {
                ThreadWork *work = work_queue + work_index;
                printf("thread %d, do work number %d\n", (int)(u64)parameter, work->value);
                Sleep(2);
            }
        }
    }
    return 0;
}

static HANDLE thread[4];
static DWORD thread_id[4];

void tc_worker_thread_queue_test()
{
    printf("time to lern mutithreading!\n"); 
    
    for(u32 thread_index = 0; thread_index < 4; ++thread_index)
    {
        thread[thread_index] = CreateThread(0, 0, thread_do_work, (void *)(u64)thread_index, 0, &thread_id[thread_index]); 
    }

    push_work(1);
    push_work(2);
    push_work(3);
    push_work(4);
    push_work(5);
    push_work(6);
    push_work(7);
    push_work(8);
    push_work(9);
    push_work(10);
    push_work(11);
    push_work(12);
    push_work(13);
    push_work(14);
    push_work(15);
    push_work(16);
}
