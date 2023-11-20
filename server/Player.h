#pragma once

class Player
{
public:
    Player();
    ~Player();

    // �÷��̾� ��Ȱ��ȭ
    void DisablePlayer();
    // �÷��̾� Ȱ��ȭ
    void SetPlayer(int id);
    void SetPanel(int color, int module);

    void SetPos(float x, float y);

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

};

