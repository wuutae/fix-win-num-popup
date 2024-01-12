#pragma once
#include <windows.h>

using namespace std;

struct AppStatus {
    bool isProcessing{};  // Flag if WinNumProc is processing
    DWORD timestamp{};  // Timestamp when Win key + number key is pressed

    // Required minimum duration in ms for holding down Win key when Win + number key is pressed to prevent a bug
    // This is to prevent a blank popup from being displayed and fixed above the taskbar
    DWORD winHoldDuration = 250;  // 250ms recommended; need more duration for more windows
};

void WinNumProc(int vkCode, DWORD remainDuration);
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
