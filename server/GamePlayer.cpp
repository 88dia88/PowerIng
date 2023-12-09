#include "GamePlayer.h"


GamePlayer::GamePlayer()
{
    ID = -1;
    mState = false;
    mReady = false;
    mPosX = 0, mPosY = 0;
    mColor = -1;
    mCoolTime = -1;
    mSpeed = -1;
    mEffect = -1;
}

GamePlayer::~GamePlayer()
{
}

void GamePlayer::DisablePlayer()
{
    ID = -1;
    mState = false;
    mReady = false;
    mPosX = 0, mPosY = 0;
    mColor = -1;
    //mModule;
    mCoolTime = -1;
    mSpeed = -1;
    mEffect = -1;
}

void GamePlayer::SetPlayer(int id)
{
    ID = id;
    mState = true;
}

void GamePlayer::SetPanel(int color, int module[5])
{
    mColor = color;
    memcpy(&mModule, module, sizeof(int) * 5);
}

void GamePlayer::SetPos(float x, float y)
{
    mPosX = x;
    mPosY = y;
}

void GamePlayer::SetKeyInput(bool up, bool down, bool right, bool left, bool action)
{
    mKeyInput.up = up;
    mKeyInput.down = down;
    mKeyInput.right = right;
    mKeyInput.left = left;
    mKeyInput.action = action;
}

void GamePlayer::SetKeyInput(KeyInput keyInput)
{
    mKeyInput = keyInput;
}

void GamePlayer::SetActionKeyDwon(bool actionKey)
{
    mKeyInput.action = actionKey;
}

void GamePlayer::SetPlayerData(GamePlayer player)
{
    mReady = player.mReady;

    mColor = player.mColor;
    memcpy(&mModule, player.mModule, sizeof(int) * 5);
}

KeyInput GamePlayer::GetKeyInput()
{
    return mKeyInput;
}

bool GamePlayer::GetState()
{
    return mState;
}

bool GamePlayer::IsReady()
{
    return mReady;
}

