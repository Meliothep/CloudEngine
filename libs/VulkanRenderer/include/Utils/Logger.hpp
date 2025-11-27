#pragma once
#include <string>
#include <vector>
#include <memory>
#include <format>
#include <mutex>

enum class LogLevel { DEBUG = 0, INFO = 1, WARN = 2, EXCEPT = 3, CRITICAL = 4 };

class LoggerStrategy
{
public:
    explicit LoggerStrategy(LogLevel logLevel) : logLevel_(logLevel) {}
    virtual ~LoggerStrategy() = default;
    virtual void Log(LogLevel logLevel, const std::string& message) = 0;

protected:
    LogLevel logLevel_;
    std::mutex mutex_;
};

class Logger
{
public:
    Logger() = default;
    ~Logger() = default;

    void AddLoggerStrat(std::unique_ptr<LoggerStrategy> strat) {
        mp_strategies.push_back(std::move(strat));
    }

    void RemoveLoggerStrat(LoggerStrategy* strat) {
        auto it = std::remove_if(mp_strategies.begin(), mp_strategies.end(),
            [strat](const std::unique_ptr<LoggerStrategy>& ptr) { return ptr.get() == strat; });
        mp_strategies.erase(it, mp_strategies.end());
    }

    void Log(LogLevel logLevel, const std::string& message) {
        for (auto& strat : mp_strategies) {
            strat->Log(logLevel, message);
        }
    }

    template <typename... T>
    void Log(LogLevel logLevel, std::string message, T&&... args) {
        try {
            Log(logLevel, std::format(message, std::forward<T>(args)...));
        }
        catch (const std::format_error& e) {
            Log(LogLevel::EXCEPT, "Formatting error: " + std::string(e.what()));
        }
    }

private:
    std::vector<std::unique_ptr<LoggerStrategy>> mp_strategies;
};

