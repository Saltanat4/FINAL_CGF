#pragma once

class GameContext
{
public:
    void RequestRestart()
    {
        restartRequested = true;
    }

    bool RestartRequested() const
    {
        return restartRequested;
    }

    bool ConsumeRestart()
    {
        bool result = restartRequested;
        restartRequested = false;
        return result;
    }

private:
    bool restartRequested = false;
};