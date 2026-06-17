#pragma once

#include <string>

enum class GameState
{
    Playing,
    LevelComplete,
    Victory,
    GameOver
};

class LevelManager
{  
public:
    LevelManager();

    // Level management
    void LoadLevel(int level);
    int GetCurrentLevel() const;
    void NextLevel();
    bool IsGameComplete() const;

    // State management
    GameState GetState() const;
    void SetState(GameState state);
    void Reset();

    // Level information
    int GetEnemyCountForLevel(int level) const;
    std::string GetLevelName(int level) const;

private:
    int currentLevel;
    GameState state;
    bool gameComplete;
};