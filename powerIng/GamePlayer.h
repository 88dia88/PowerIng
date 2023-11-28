#pragma once


#pragma pack(1)
struct KeyInput
{
    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;
    bool action = false;
};
#pragma pack()


class GamePlayer
{
public:
    GamePlayer();
    ~GamePlayer();

    // �÷��̾� ��Ȱ��ȭ
    void DisablePlayer();
    // �÷��̾� Ȱ��ȭ
    void SetPlayer(int id);
    void SetPanel(int color, int module);

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
    int mModule;
    float mCoolTime;
    double mSpeed;
    int mEffect;

    KeyInput mKeyInput;
};

