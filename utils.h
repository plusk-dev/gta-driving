#pragma once
#include <windows.h>
#include <unordered_map>

void PressKey(CHAR hexKeyCode, std::unordered_map<CHAR, bool>& keyMappings);
void ReleaseKey(CHAR hexKeyCode, std::unordered_map<CHAR, bool>& keyMappings);
HMODULE LoadvJoyDLLFile(const char* dllFilePath);