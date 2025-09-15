#pragma once
#include "IWindow.hpp"
#include <windows.h>
#include <string>

class Win32Window : public IWindow {
private:
    HWND m_hwnd;
    HINSTANCE m_hinstance;
    std::wstring m_className;
    bool m_shouldClose;
    int m_width, m_height;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool RegisterWindowClass();
    void UnregisterWindowClass();

public:
    Win32Window(int width, int height, const char* title);
    ~Win32Window() override;

    // IWindow interface
    void* GetNativeHandle() const override;
    bool ProcessMessages() override;

    // Additional methods that might be useful
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    bool ShouldClose() const { return m_shouldClose; }
    void SetShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }
};