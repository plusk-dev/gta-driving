#include <unordered_map>
#include <iostream>
#include "utils.h"

HMODULE LoadvJoyDLLFile(const char* dllFilePath) {
    HMODULE vJoyDLL = LoadLibraryA(dllFilePath);
    if (vJoyDLL == NULL) {
        throw std::invalid_argument("Could not load DLL file.");
    }
    else {
        return vJoyDLL;
    }
}

void PressKey(CHAR keyParam, std::unordered_map<CHAR, bool>& keyMappings) {
    SHORT key;
    UINT mappedkey;
    INPUT input = { 0 };
    key = VkKeyScan(keyParam);
    mappedkey = MapVirtualKey(LOBYTE(key), 0);
    input.type = INPUT_KEYBOARD;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    input.ki.wScan = mappedkey;
    SendInput(1, &input, sizeof(input));
    !keyMappings[keyParam] ? keyMappings[keyParam] = true : true;
}

void ReleaseKey(CHAR keyParam, std::unordered_map<CHAR, bool>& keyMappings) {
    if (keyMappings[keyParam] == true) {
        SHORT key;
        UINT mappedkey;
        INPUT input = { 0 };
        key = VkKeyScan(keyParam);
        mappedkey = MapVirtualKey(LOBYTE(key), 0);
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_SCANCODE;
        input.ki.wScan = mappedkey;
        input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(input));
        keyMappings[keyParam] = false;
    }
}
