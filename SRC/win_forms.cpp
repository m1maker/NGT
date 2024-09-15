#include<string>
#include<windows.h>
class win_forms {
public:
    HWND create_window(std::string window_title, int x, int y, int width, int height)
    {

        WNDCLASSEX wcex = { 0 };
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.lpfnWndProc = DefWindowProc;
        wcex.hInstance = GetModuleHandle(NULL);
        wcex.lpszClassName = lpszClassName;
        RegisterClassEx(&wcex);

        return CreateWindow(
            lpszClassName,
            lpszWindowName,
            WS_OVERLAPPEDWINDOW,
            x,
            y,
            width,
            height,
            hwndParent,
            hMenu,
            GetModuleHandle(NULL),
            NULL
        );
    }
};