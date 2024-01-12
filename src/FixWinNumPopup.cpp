#include "FixWinNumPopup.h"
#include <thread>

AppStatus status;

// Process Win + number key, preventing blank popup
void WinNumProc(int vkCode, DWORD remainDuration) {
    status.isProcessing = true;
    Sleep(remainDuration);

    // Send Win key up
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));

    status.isProcessing = false;
}

// Keyboard hook procedure
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) return CallNextHookEx(nullptr, nCode, wParam, lParam);

    auto* pKeyboardStruct = (KBDLLHOOKSTRUCT*)lParam;

    if ((pKeyboardStruct->vkCode == VK_LWIN or pKeyboardStruct->vkCode == VK_RWIN) and // Win key
        (wParam == WM_KEYUP or wParam == WM_SYSKEYUP)) {  // Key up
        DWORD winHoldDuration = GetTickCount() - status.timestamp;  // Duration of Win key down after Win + number key is pressed
        // Handle bug that occurs when a Win key is released quickly after pressing Win + number key
        if (winHoldDuration < status.winHoldDuration and not status.isProcessing) {
            thread t(WinNumProc, pKeyboardStruct->vkCode, status.winHoldDuration - winHoldDuration);
            t.detach();
            return 1;
        }
    }

    // Handle Win + number key
    if (
    (pKeyboardStruct->vkCode >= 0x30 && pKeyboardStruct->vkCode <= 0x39) &&  // Number Key
    (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&  // Keydown
    (GetAsyncKeyState(VK_LWIN) or GetAsyncKeyState(VK_RWIN))) {  // Win key is pressed
        if (status.isProcessing) {
            return 1;  // Ignore if WinNumProc is already processing
        }
        status.timestamp = GetTickCount();  // Record timestamp of Win + number key trigger
    }

    // Call next hook procedure
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

int main() {
    // Prevent multiple instances
    HANDLE Mutex;
    const char ProgMutex[] = "fix-win-num-popup";
    if ((Mutex = OpenMutex(MUTEX_ALL_ACCESS, false, ProgMutex)) == nullptr)
        Mutex = CreateMutex(nullptr, true, ProgMutex);
    else {
        MessageBox(nullptr, "Already processing.", "Error", MB_OK);
        return 0;
    }

    // Set keyboard hook
    HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, nullptr, 0);
    if (hKeyboardHook == nullptr) return 1;

    // Message loop
    MSG msg;
    BOOL bRet;
    while( (bRet = GetMessage( &msg, nullptr, 0, 0 )) != 0) {
        if (bRet == -1) return 1;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook keyboard hook
    UnhookWindowsHookEx(hKeyboardHook);

    return 0;
}