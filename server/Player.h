#pragma once
#include "Packit.h"

class Player
{
public:
    Player();
    ~Player();

    // 플레이어 비활성화
    void DisablePlayer();
    // 플레이어 활성화
    void SetPlayer(int id);
    void SetPanel(int color, int module);

    void SetPos(float x, float y);
    void SetKeyInput(KeyInput keyInput);
    void SetKeyInput(bool up, bool down, bool right, bool left, bool action);
    void SetActionKeyDwon(bool actionKey);
    KeyInput GetKeyInput();

    bool GetState();
    bool IsReady();
private:
    // Client Data
    int ID = -1;
    bool mState = false; 

    // Lobby Data
    bool mReady = false;

    // Panel Data
    float mPosX, mPosY;
    int mColor;
    int mModule;
    float mCoolTime;
    double mSpeed;
    int mEffect;

    KeyInput mKeyInput;
};

