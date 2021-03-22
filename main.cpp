/**
 * 32bit:
 * - i686-w64-mingw32-g++ -c -DUNICODE -D_UNICODE -o pintopwnd.o main.cpp
 * - i686-w64-mingw32-g++ -s -static -o pintopwnd.exe pintopwnd.o
 */

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef _MSC_VER 
#define HOOK_DLL L"mousehook.dll"
#else
#define HOOK_DLL L"Mouse.dll"
#endif

using std::cout;
using std::endl;

#define STYLE(x, y) if (x & y) \
{ \
    cout << #x << endl; \
}

void printWndStyles(LONG wndStyles, LONG extWndStyles);
int modWndStyles();


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    HMODULE dll = LoadLibrary(
        HOOK_DLL
    );
    if (dll == NULL)
    {
        MessageBox(NULL, L"Could not load library " HOOK_DLL, L"Dll Loading Failed", MB_ICONERROR);
        return 1;
    }

    FARPROC proc = GetProcAddress(
        dll,
		#ifdef __MINGW32__
		"LowLevelMouseProc@12"
		#else
        "_LowLevelMouseProc@12"
		#endif
    );
    if (proc == NULL)
    {
        MessageBox(NULL, L"Could not find function _LowLevelMouseProc@12 from " HOOK_DLL, L"Function Loading Failed", MB_ICONERROR);
        return 1;
    }

    MessageBox(NULL,
        L"1.  Close this popup. \n"
         "2.  Use <alt>+<tab> to select a window.\n"
         "3a. Left-click on the selected window.\n"
         "3b. Right-click to abort!", L"Global Mouse Hook Failed", MB_ICONINFORMATION);

    HHOOK hook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)proc, dll, 0);
    if (hook == NULL)
    {
        MessageBox(NULL, L"Could not set a global mouse hook.", L"Global Mouse Hook Failed", MB_ICONERROR);
        return 1;
    }

    // fake message loop
    BOOL bRet; MSG msg;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnhookWindowsHookEx(hook);

    switch(msg.wParam)
    {
    case WM_LBUTTONUP: // confirm
        modWndStyles();
        break;
    case WM_RBUTTONUP: // abort
        MessageBox(NULL, L"Operation aborted by the user.", L"Operation aborted!", MB_ICONWARNING);
        return 0;
    }
    MessageBox(NULL, L"Done", L"Done!", 0);
    return 0;
}

int modWndStyles()
{
    DWORD err;
    HWND wnd = GetForegroundWindow();

    if (wnd == NULL)
    {
        return 1;
    }

    // get window styles
    LONG wndStyles = GetWindowLongPtr(wnd, GWL_STYLE);
    if (wndStyles == NULL)
    {
        err = GetLastError();
        cout << "GetWindowLongPtr failed with last error: " << err << endl;
        return 1;
    }

    // get extended window styles
    LONG extWndStyles = GetWindowLongPtr(wnd, GWL_EXSTYLE);
    if (extWndStyles == NULL)
    {
        err = GetLastError();
        cout << "GetWindowLongPtr failed with last error: " << err << endl;
        return 1;
    }

    printWndStyles(wndStyles, extWndStyles);

    wndStyles = wndStyles & ~WS_VSCROLL;          // remove scroll bar
    wndStyles = wndStyles & ~WS_OVERLAPPEDWINDOW; // remove caption etc.

    if (!SetWindowLongPtr(wnd, GWL_STYLE, wndStyles))
    {
        err = GetLastError();
        cout << "SetWindowLongPtr failed with last error: " << err << endl;
        return 1;
    }

    // mandatory *before* setting GWL_EXSTYLE
    if (!SetWindowPos(
        wnd,
        HWND_TOPMOST, // WS_EX_TOPMOST
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW
    ))
    {
        err = GetLastError();
        cout << "SetWindowPos failed with last error: " << err << endl;
        return 1;
    }

    // remove app from taskbar
    extWndStyles |= WS_EX_TOOLWINDOW;
    if (!SetWindowLongPtr(wnd, GWL_EXSTYLE, extWndStyles))
    {
        err = GetLastError();
        cout << "SetWindowLongPtr failed with last error: " << err << endl;
        return 1;
    }

    cout << endl << endl;

    // get window styles
    wndStyles = GetWindowLongPtr(wnd, GWL_STYLE);
    if (wndStyles == NULL)
    {
        err = GetLastError();
        cout << "GetWindowLongPtr failed with last error: " << err << endl;
        return 1;
    }

    // get extended window styles
    extWndStyles = GetWindowLongPtr(wnd, GWL_EXSTYLE);
    if (extWndStyles == NULL)
    {
        err = GetLastError();
        cout << "GetWindowLongPtr failed with last error: " << err << endl;
        return 1;
    }
    printWndStyles(wndStyles, extWndStyles);


    
    return 0;
}

void printWndStyles(LONG wndStyles, LONG extWndStyles)
{
    cout << "Window Styles" << endl;
    STYLE(WS_BORDER, wndStyles)
    STYLE(WS_CAPTION, wndStyles)
    STYLE(WS_CHILD, wndStyles)
    STYLE(WS_CHILDWINDOW, wndStyles)
    STYLE(WS_CLIPCHILDREN, wndStyles)
    STYLE(WS_CLIPSIBLINGS, wndStyles)
    STYLE(WS_DISABLED, wndStyles)
    STYLE(WS_DLGFRAME, wndStyles)
    STYLE(WS_GROUP, wndStyles)
    STYLE(WS_HSCROLL, wndStyles)
    STYLE(WS_ICONIC, wndStyles)
    STYLE(WS_MAXIMIZE, wndStyles)
    STYLE(WS_MAXIMIZEBOX, wndStyles)
    STYLE(WS_MINIMIZE, wndStyles)
    STYLE(WS_MINIMIZEBOX, wndStyles)
    STYLE(WS_OVERLAPPED, wndStyles)
    STYLE(WS_OVERLAPPEDWINDOW, wndStyles)
    STYLE(WS_POPUP, wndStyles)
    STYLE(WS_POPUPWINDOW, wndStyles)
    STYLE(WS_SIZEBOX, wndStyles)
    STYLE(WS_SYSMENU, wndStyles)
    STYLE(WS_TABSTOP, wndStyles)
    STYLE(WS_THICKFRAME, wndStyles)
    STYLE(WS_TILED, wndStyles)
    STYLE(WS_TILEDWINDOW, wndStyles)
    STYLE(WS_VISIBLE, wndStyles)
    STYLE(WS_VSCROLL, wndStyles)

    cout << "Extended Window Styles" << endl;
    STYLE(WS_EX_ACCEPTFILES, extWndStyles)
    STYLE(WS_EX_APPWINDOW, extWndStyles)
    STYLE(WS_EX_CLIENTEDGE, extWndStyles)
    STYLE(WS_EX_COMPOSITED, extWndStyles)
    STYLE(WS_EX_CONTEXTHELP, extWndStyles)
    STYLE(WS_EX_CONTROLPARENT, extWndStyles)
    STYLE(WS_EX_DLGMODALFRAME, extWndStyles)
    STYLE(WS_EX_LAYERED, extWndStyles)
    STYLE(WS_EX_LAYOUTRTL, extWndStyles)
    STYLE(WS_EX_LEFT, extWndStyles)
    STYLE(WS_EX_LEFTSCROLLBAR, extWndStyles)
    STYLE(WS_EX_LTRREADING, extWndStyles)
    STYLE(WS_EX_MDICHILD, extWndStyles)
    STYLE(WS_EX_NOACTIVATE, extWndStyles)
    STYLE(WS_EX_NOINHERITLAYOUT, extWndStyles)
    STYLE(WS_EX_NOPARENTNOTIFY, extWndStyles)
	#ifndef __MINGW32__
    STYLE(WS_EX_NOREDIRECTIONBITMAP, extWndStyles)
	#endif
    STYLE(WS_EX_OVERLAPPEDWINDOW, extWndStyles)
    STYLE(WS_EX_PALETTEWINDOW, extWndStyles)
    STYLE(WS_EX_RIGHT, extWndStyles)
    STYLE(WS_EX_RIGHTSCROLLBAR, extWndStyles)
    STYLE(WS_EX_RTLREADING, extWndStyles)
    STYLE(WS_EX_STATICEDGE, extWndStyles)
    STYLE(WS_EX_TOOLWINDOW, extWndStyles)
    STYLE(WS_EX_TOPMOST, extWndStyles)
    STYLE(WS_EX_TRANSPARENT, extWndStyles)
    STYLE(WS_EX_WINDOWEDGE, extWndStyles)
}
