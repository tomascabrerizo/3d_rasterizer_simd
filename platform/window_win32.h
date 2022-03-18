#ifndef TC_WINDOW_WIN32_H
#define TC_WINDOW_WIN32_H

// TODO: this need to be reformat to work as the renderer
// tc_Window should have void * to platform
struct tc_Window
{
    HWND handle;
    DWORD width;
    DWORD height;
};

#endif // TC_WINDOW_WIN32_H
