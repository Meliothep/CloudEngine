#pragma once
#include "IWindow.hpp"

#include <windows.h>
#include <string>

class Win32Window : public IWindow {

public:
    Win32Window(int width, int height, LPCWSTR title);
    ~Win32Window() override;

    // IWindow interface
    void* GetNativeHandle() const override;
    bool ProcessMessages() override;

    // Additional methods that might be useful
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    bool ShouldClose() const override { return m_shouldClose; }
    void SetShouldClose(bool shouldClose) override { m_shouldClose = shouldClose; }

private:
    HWND m_hwnd;
    HINSTANCE m_hinstance;
    std::wstring m_className;
    bool m_shouldClose;
    int m_width, m_height;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool RegisterWindowClass();
    void UnregisterWindowClass();
};