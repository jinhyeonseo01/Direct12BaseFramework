#pragma once
#include <chrono>

class Timer
{
private:
    bool _first = true;
    std::chrono::time_point<std::chrono::steady_clock> resume;
    std::chrono::time_point<std::chrono::steady_clock> pause;

public:
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::duration<long long, std::nano> lastPauseElapsed;
    std::chrono::duration<long long, std::nano> elapsed;

    bool isPause = true;
    bool isStop = true;

    Timer();
    virtual ~Timer();

    void Start();
    void Pause();
    void Stop();

    bool IsStop();
    bool IsPause();
    bool IsPlaying();
    bool IsStart();

    std::chrono::duration<long long, std::nano> GetTime();
    std::chrono::duration<long long, std::nano> GetLastBetweenTime();
};
