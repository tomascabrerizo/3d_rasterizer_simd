#include <Windows.h>
#include <stdio.h>
#include "window_win32.h"
//#include "platform/software_renderer_win32.cpp"

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

struct ThreadWork 
{
    void (*work)(void *attr);
    void *attributes;
};

#define WIN32_THREAD_COUNT 7
struct ThreadQueue
{
    HANDLE threads[WIN32_THREAD_COUNT];
    DWORD thread_id[WIN32_THREAD_COUNT];
    HANDLE semaphore;
    ThreadWork works[64];
    volatile s32 work_count;
    volatile s32 next_work_to_do;
    volatile s32 work_done;
};

static ThreadQueue global_win32_thread_queue;

void thread_queue_begin(ThreadQueue *queue)
{
    queue->work_count = 0;
    queue->next_work_to_do = 0;
    queue->work_done = 0;
}

void thread_queue_push_work(ThreadQueue *queue, ThreadWork work)
{
    s32 work_index = queue->work_count;
    ThreadWork *thread_work = queue->works + work_index;
    *thread_work = work;
    
    // NOTE: ensure that the work was written before incrementing work_count
    MemoryBarrier();
    
    queue->work_count++;
    ReleaseSemaphore(queue->semaphore, 1, 0);
}

void thread_queue_end(ThreadQueue *queue)
{
    while(queue->work_done != queue->work_count)
    {
        s32 original_work_to_do = queue->next_work_to_do;
        if(original_work_to_do < queue->work_count)
        {
            s32 work_index = InterlockedCompareExchange((LONG volatile *)&queue->next_work_to_do, original_work_to_do + 1, original_work_to_do);
            if(work_index == original_work_to_do)
            {
                ThreadWork *work = queue->works + work_index;
                work->work(work->attributes);
                InterlockedIncrement((LONG volatile *)&queue->work_done);
            }
        }
    }
}

DWORD thread_do_work(void *parameter)
{
    ThreadQueue *queue = (ThreadQueue *)parameter;
    while(true)
    {   
        s32 original_work_to_do = queue->next_work_to_do;
        if(original_work_to_do < queue->work_count)
        {
            s32 work_index = InterlockedCompareExchange((LONG volatile *)&queue->next_work_to_do, original_work_to_do + 1, original_work_to_do);
            if(work_index == original_work_to_do)
            {
                ThreadWork *work = queue->works + work_index;
                work->work(work->attributes);
                InterlockedIncrement((LONG volatile *)&queue->work_done);
            }
        }
        else
        {
            WaitForSingleObjectEx(queue->semaphore, INFINITE, FALSE);
        }
    }
    return 0;
}

void thread_queue_create(ThreadQueue *thread_queue)
{
    *thread_queue = {};
    thread_queue->semaphore = CreateSemaphoreA(0, 0, WIN32_THREAD_COUNT, 0);

    for(u32 thread_index = 0; thread_index < WIN32_THREAD_COUNT; ++thread_index)
    {
        thread_queue->threads[thread_index] = CreateThread(0, 0, thread_do_work, (void *)thread_queue, 0, &thread_queue->thread_id[thread_index]); 
    }
}

tc_Renderer *tc_platform_create_software_renderer(tc_Window *window)
{
    thread_queue_create(&global_win32_thread_queue);

    tc_Renderer *renderer = (tc_Renderer *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tc_Renderer));
    renderer->platform = (tc_BackBufferWin32 *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tc_BackBufferWin32));
    renderer->thread_queue = &global_win32_thread_queue;
    renderer->command_buffer_darr = 0;

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

    buffer->depth = (f32 *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffer->width*buffer->height*sizeof(f32));

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

void win32_sofware_renderer_swap_buffers(tc_Renderer *renderer, tc_Window *window)
{
    tc_BackBufferWin32 *win32_buffer = (tc_BackBufferWin32 *)renderer->platform;
    HDC dc = GetDC(window->handle);
    BitBlt(dc, 0, 0, (s32)renderer->backbuffer.width, (s32)renderer->backbuffer.height, win32_buffer->dc, 0, 0, SRCCOPY);
    ReleaseDC(window->handle, dc);
}
