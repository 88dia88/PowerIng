#pragma once
#include <string>




class GamePlayer
{
public:
    GamePlayer();
    ~GamePlayer();

    // �÷��̾� ��Ȱ��ȭ
    void DisablePlayer();
    // �÷��̾� Ȱ��ȭ
    void SetPlayer(int id);
    void SetPanel(int color, int module[5]);

    void SetPos(float x, float y);
    void SetKeyInput(KeyInput keyInput);
    void SetKeyInput(bool up, bool down, bool right, bool left, bool action);
    void SetActionKeyDwon(bool actionKey);
    void SetPlayerData(GamePlayer player);
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
    int mModule[5];
    float mCoolTime;
    double mSpeed;
    int mEffect;

    KeyInput mKeyInput;
};