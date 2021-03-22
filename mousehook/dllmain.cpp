/**
 * 32bit:
 * - i686-w64-mingw32-g++ -c -DUNICODE -D_UNICODE -o mouse.o dllmain.cpp
 * - i686-w64-mingw32-g++ -s -o mouse.dll -s -shared mouse.o -Wl,--subsystem,windows
 */
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport)
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        // quit message loop
        PostThreadMessage(GetCurrentThreadId(), WM_QUIT, wParam, NULL);
        return TRUE;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
