#pragma once

#include <cstdint>
#include <vector>

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

    bool HasAllKeys() const
    {
        return keysCollected >= keysInLevel;
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

    int GetCurrentLevel() const { return currentLevel; }
    void SetCurrentLevel(int level) { currentLevel = level; }

    static std::string LevelPath(int level)
    {
        return "levels/level" + std::to_string(level) + ".json";
    }

    void RequestNextLevel() { nextLevelRequested = true; }

private:
    bool restartRequested = false;

    int keysCollected = 0;
    int keysInLevel = 0;

    bool nextLevelRequested = false;

    int currentLevel = 4;

    std::vector<std::uint32_t> destroyRequests;
};