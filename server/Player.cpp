#include "Player.h"

Player::Player()
{
    ID = -1;
    mState = false;
    mReady = false;
    mPosX = 0 , mPosY = 0;
    mColor = -1;
    mModule = -1;
    mCoolTime = -1;
    mSpeed = -1;
    mEffect = -1;
}

Player::~Player()
{
}

void Player::DisablePlayer()
{
    ID = -1;
    mState = false;
    mReady = false;
    mPosX = 0, mPosY = 0;
    mColor = -1;
    mModule = -1;
    mCoolTime = -1;
    mSpeed = -1;
    mEffect = -1;
}

void Player::SetPlayer(int id)
{
    ID = id;
    mState = true;
}

void Player::SetPanel(int color, int module)
{
    mColor = color;
    mModule = module;
}

void Player::SetPos(float x, float y)
{
    mPosX = x; 
    mPosY = y;
}

bool Player::GetState()
{
    return mState;
}

bool Player::IsReady()
{
    return mReady;
}

