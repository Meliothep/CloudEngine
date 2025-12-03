#include "Windows/Win32Window.hpp"
#include <stdexcept>
#include <codecvt>
#include <locale>
#include <iostream>

Win32Window::Win32Window(int width, int height, LPCWSTR title)
    : m_hwnd(nullptr), m_hinstance(GetModuleHandle(nullptr)), 
      m_shouldClose(false), m_width(width), m_height(height) {
    
    if (!RegisterWindowClass()) {
        throw std::runtime_error("Failed to register window class");
    }

    // Calculate window size including borders
    RECT windowRect = { 0, 0, width, height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // Create the window
    m_hwnd = CreateWindowExW(
        0,                              // Optional window styles
        m_className.c_str(),            // Window class
        title,                          // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,   // Size and position
        windowWidth, windowHeight,      // Width and height
        nullptr,                        // Parent window    
        nullptr,                        // Menu
        m_hinstance,                    // Instance handle
        this                            // Additional application data (this pointer)
    );

    if (m_hwnd == nullptr) {
        DWORD error = GetLastError();
        std::cerr << "CreateWindowExW failed with error: " << error << std::endl;
        UnregisterWindowClass();
        throw std::runtime_error("Failed to create window");
    } else {
        std::cout << "Win32Window created successfully. HWND: " << m_hwnd << std::endl;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
}

Win32Window::~Win32Window() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    UnregisterWindowClass();
}

void* Win32Window::GetNativeHandle() const {
    return m_hwnd;
}

bool Win32Window::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            m_shouldClose = true;
            return false;
        }
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return !m_shouldClose;
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32Window* window = nullptr;

    if (msg == WM_NCCREATE) {
        // Store the this pointer in the window's user data
        CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = static_cast<Win32Window*>(createStruct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        // Retrieve the this pointer from the window's user data
        window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window) {
        switch (msg) {
        case WM_CLOSE:
            window->m_shouldClose = true;
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            // Update internal size when window is resized
            window->m_width = LOWORD(lParam);
            window->m_height = HIWORD(lParam);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool Win32Window::RegisterWindowClass() {
    // Generate a unique class name
    static int classCounter = 0;
    m_className = L"VulkanWindow_" + std::to_wstring(classCounter++);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_hinstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = m_className.c_str();
    wc.lpszMenuName = nullptr;

    return RegisterClassExW(&wc) != 0;
}

void Win32Window::UnregisterWindowClass() {
    if (!m_className.empty()) {
        UnregisterClassW(m_className.c_str(), m_hinstance);
        m_className.clear();
    }
}