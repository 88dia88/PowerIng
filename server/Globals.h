#pragma once

const int MAX_NUM_CLIENTS = 3;

enum GameState
{
    GAME_STATE_LOBBY,
    GAME_STATE_READY,
    GAME_STATE_GAME,
    GAME_STATE_END,
    Count
};
