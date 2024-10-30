#include "stdafx.h"
#include "Timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
    isPause = false;
    isStop = false;

    resume = std::chrono::steady_clock::now();
    if (_first)
    {
        _first = false;
        start = resume;
        pause = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration<long long, std::nano>::zero();
        lastPauseElapsed = std::chrono::duration<long long, std::nano>::zero();
    }
}

void Timer::Pause()
{
    isPause = true;
    pause = std::chrono::steady_clock::now();
    elapsed += (lastPauseElapsed = (pause - resume));
}

void Timer::Stop()
{
    isStop = true;
    isPause = true;
    _first = true;
    pause = std::chrono::steady_clock::now();
    elapsed += (lastPauseElapsed = (pause - resume));
}

bool Timer::IsStop()
{
    return isStop;
}

bool Timer::IsPause()
{
    return isPause;
}

bool Timer::IsPlaying()
{
    return (!isStop) && (!isPause);
}

bool Timer::IsStart()
{
    return !isStop;
}

std::chrono::duration<long long, std::nano> Timer::GetTime()
{
    return ((!isPause) ? ((std::chrono::steady_clock::now() - resume) + elapsed) : elapsed);
}

std::chrono::duration<long long, std::nano> Timer::GetLastBetweenTime()
{
    return ((!isPause) ? std::chrono::steady_clock::now() - resume : lastPauseElapsed);
}
