#pragma once

class IWindow {
public:
    virtual ~IWindow() = default;
    virtual void* GetNativeHandle() const = 0;
    virtual bool ProcessMessages() = 0;
};