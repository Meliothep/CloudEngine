#pragma once
#include "Logger.hpp"

#include <Windows.h>
#include <ctime>
#include <iostream>

class Win32ConsoleStrategy : public LoggerStrategy
{
public:
    explicit Win32ConsoleStrategy(LogLevel logLevel) : LoggerStrategy(logLevel) {}
    
    void Log(LogLevel logLevel, const std::string& message) override {
        if (logLevel < logLevel_) return;

        std::lock_guard<std::mutex> lock(mutex_);

        time_t now = time(0);
        tm timeinfo{};
        localtime_s(&timeinfo, &now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

        std::cout << "[" << timestamp << "] ";

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        switch (logLevel) {
        case LogLevel::DEBUG:    SetConsoleTextAttribute(hConsole, 0x02); std::cout << "DEBUG "; break;
        case LogLevel::INFO:     SetConsoleTextAttribute(hConsole, 0x01); std::cout << "INFO "; break;
        case LogLevel::WARN:   SetConsoleTextAttribute(hConsole, 0x06); std::cout << "WARN "; break;
        case LogLevel::EXCEPT:   SetConsoleTextAttribute(hConsole, 0x04); std::cout << "EXCEPT "; break;
        case LogLevel::CRITICAL: SetConsoleTextAttribute(hConsole, 0x0C); std::cout << "CRITICAL "; break;
        }
        SetConsoleTextAttribute(hConsole, 0x0F);
        std::cout << ": " << message << std::endl;
    }
};
