#pragma once

#include <cstdint>
#include <vector>

class GameContext
{
public:
    // Перезапуск уровня
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

    void AddKeyToLevel()
    {
        ++keysInLevel;
    }

    void CollectKey()
    {
        ++keysCollected;
    }

    int GetKeysCollected() const
    {
        return keysCollected;
    }

    int GetKeysInLevel() const
    {
        return keysInLevel;
    }

    void RequestDestroy(std::uint32_t objectId)
    {
        destroyRequests.push_back(objectId);
    }

    const std::vector<std::uint32_t>& GetDestroyRequests() const
    {
        return destroyRequests;
    }

    void ClearDestroyRequests()
    {
        destroyRequests.clear();
    }

    void ResetLevelProgress()
    {
        keysCollected = 0;
        keysInLevel = 0;
        destroyRequests.clear();
    }

    // Дверь и переход между уровнями
    bool HasAllKeys() const
    {
        return keysCollected >= keysInLevel;
    }

    void RequestNextLevel()
    {
        nextLevelRequested = true;
    }

    bool NextLevelRequested() const
    {
        return nextLevelRequested;
    }

    bool ConsumeNextLevel()
    {
        bool result = nextLevelRequested;
        nextLevelRequested = false;
        return result;
    }

private:
    bool restartRequested = false;

    int keysCollected = 0;
    int keysInLevel = 0;

    bool nextLevelRequested = false;

    std::vector<std::uint32_t> destroyRequests;
};