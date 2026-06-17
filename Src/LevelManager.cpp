#include "LevelManager.h"

LevelManager::LevelManager()
{
    currentLevel = 1;
    state = GameState::Playing;
    gameComplete = false;
}

void LevelManager::LoadLevel(int level)
{
    currentLevel = level;
    state = GameState::Playing;
}

int LevelManager::GetCurrentLevel() const
{
    return currentLevel;
}

void LevelManager::NextLevel()
{
    if (currentLevel < 4)
    {
        currentLevel++;
        state = GameState::Playing;
    }
    else
    {
        state = GameState::Victory;
        gameComplete = true;
    }
}

bool LevelManager::IsGameComplete() const
{
    return gameComplete;
}

GameState LevelManager::GetState() const
{
    return state;
}

void LevelManager::SetState(GameState newState)
{
    state = newState;
}

void LevelManager::Reset()
{
    currentLevel = 1;
    state = GameState::Playing;
    gameComplete = false;
}

int LevelManager::GetEnemyCountForLevel(int level) const
{
    switch (level)
    {
    case 1: return 4;
    case 2: return 5;
    case 3: return 6;
    case 4: return 1; // Boss
    default: return 0;
    }
}

std::string LevelManager::GetLevelName(int level) const
{
    switch (level)
    {
    case 1: return "Standard Battlefield";
    case 2: return "Increased Threat";
    case 3: return "Heavy Resistance";
    case 4: return "Boss Battle";
    default: return "Unknown";
    }
}