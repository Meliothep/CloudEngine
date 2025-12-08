#pragma once

class IWindow {
public:
    virtual ~IWindow() = default;
    virtual void* GetNativeHandle() const = 0;
    virtual bool ProcessMessages() = 0;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual bool ShouldClose() const = 0;
    virtual void SetShouldClose(bool shouldClose) = 0;
};