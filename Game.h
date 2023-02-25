//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include "Floor.h"
#include "Ball.h"
#include "Shadow.h"
#include "Meter.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

private:

    // �{�[���̐F
    enum class BallColor
    {
        Yellow,
        Magenta,
        Cyan,
        NumItems
    };

    // �v���C���[�̃{�[���̔ԍ�
    static const int BN_PLAYER;

    // �{�[���̔��a
    static const float BALL_RADIUS;

    // ���̑傫��
    static const float FLOOR_SIZE;

    // �{�[���������z�u�����G���A�̃T�C�Y
    static const float BALL_PUT_AERA_SIZE;

    // �P�t���[���ӂ�̃J�����̉�]�p�x
    static const float FRAME_ROTATE_ANGLE;

    // �{�[�����
    struct BallInfo
    {
        DirectX::SimpleMath::Vector3 position;  // �ʒu
        float direction;                        // ����
        float speed;                            // ����
        DirectX::SimpleMath::Color color;       // �F
    };

    // �{�[���̐�
    static const int BN_NUM = 20;

    BallInfo m_ballInfo[BN_NUM];

    // �r���[�s��
    DirectX::SimpleMath::Matrix m_view;

    // �ˉe�s��
    DirectX::SimpleMath::Matrix m_proj;

    // ���ʃX�e�[�g
    std::unique_ptr<DirectX::CommonStates> m_states;

    // ��
    std::unique_ptr<Floor> m_floor;

    // �{�[��
    std::unique_ptr<Ball> m_ball[BN_NUM];

    // �{�[���̉e
    std::unique_ptr<Shadow> m_shadow[BN_NUM];

    // �p���[���[�^�[
    std::unique_ptr<Meter> m_meter;

    // �p���[
    float m_power;

private:

    // �Q�[���̃��Z�b�g�֐�
    void GameReset();

    // �Փ˂����{�[���̑��x��ݒ肷��֐�
    void SetBallSpeed(BallInfo* ball_a, BallInfo* ball_b);

    // �Q�[���N���A�`�F�b�N
    bool CheckGameClear();

    // �Q�[���I�[�o�[�`�F�b�N
    bool CheckGameOver();

};
