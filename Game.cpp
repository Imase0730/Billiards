//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

//----- �萔�錾 -----//

// �v���C���[�̃{�[���̔ԍ�
const int Game::BN_PLAYER = 0;

// �{�[���̔��a
const float Game::BALL_RADIUS = 1.0f;

// ���̑傫��
const float Game::FLOOR_SIZE = 50.0f;

// �{�[���������z�u�����G���A�̃T�C�Y
const float Game::BALL_PUT_AERA_SIZE = 40.0f;

// �P�t���[���ӂ�̃J�����̉�]�p�x
const float Game::FRAME_ROTATE_ANGLE = 1.0f;

Game::Game() noexcept(false)
    : m_power{}
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
    GameReset();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

    // �L�[����
    auto kb = Keyboard::Get().GetState();
    static Keyboard::KeyboardStateTracker tracker;
    tracker.Update(kb);

    // �p���[�̐ݒ�
    m_power += 0.01f;
    if (m_power > 1.0f) m_power = -1.0f;
    m_meter->SetPower(fabsf(m_power));

    // �J�����̐ݒ�
    static SimpleMath::Vector3 fromPt = SimpleMath::Vector3::Zero;
    SimpleMath::Vector3 lookAt = m_ballInfo[BN_PLAYER].position;

    if (m_ballInfo[BN_PLAYER].speed < 0.01f)
    {
        SimpleMath::Vector3 v(0.0f, 3.0f, -10.0f);
        SimpleMath::Matrix m = SimpleMath::Matrix::CreateRotationY(m_ballInfo[BN_PLAYER].direction);
        v = SimpleMath::Vector3::Transform(v, m);
        fromPt = lookAt + v;
    }
    m_view = SimpleMath::Matrix::CreateLookAt(fromPt, lookAt, SimpleMath::Vector3::UnitY);

    // �X�y�[�X�L�[�������ꂽ
    if (tracker.pressed.Space)
    {
        // �{�[����ł��o��
        m_ballInfo[BN_PLAYER].speed += fabsf(m_power) * 1.0f;
    }

    // ���E�L�[�ŃJ�����̌�������]����
    if (kb.Left)
    {
        m_ballInfo[BN_PLAYER].direction += XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }
    if (kb.Right)
    {
        m_ballInfo[BN_PLAYER].direction -= XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }

    // �{�[���̈ړ�
    for (int i = 0; i < BN_NUM; i++)
    {
        SimpleMath::Matrix matrix = SimpleMath::Matrix::CreateRotationY(m_ballInfo[i].direction);
        SimpleMath::Vector3 vec = SimpleMath::Vector3(0.0f, 0.0f, m_ballInfo[i].speed);
        vec = SimpleMath::Vector3::Transform(vec, matrix);
        m_ballInfo[i].position += vec;
        m_ballInfo[i].speed *= 0.985f;

        // �ړ������{�[������]������
        float length = vec.Length();
        if (length)
        {
            // ��]�������߂�
            float tmp = -vec.x;
            vec.x = vec.z;
            vec.z = tmp;
            // ��]��������
            m_ballInfo[i].rotate *= SimpleMath::Quaternion::CreateFromAxisAngle(vec, vec.Length() * BALL_RADIUS);
        }
    }

    // �{�[���̏Փ˔���
    for (int i = 0; i < BN_NUM - 1; i++)
    {
        for (int j = i + 1; j < BN_NUM; j++)
        {
            // �~�Ɖ~�̏Փ˔���
            SimpleMath::Vector3 v = m_ballInfo[i].position - m_ballInfo[j].position;
            if (v.LengthSquared() < (BALL_RADIUS * 2.0f) * (BALL_RADIUS * 2.0f))
            {
                // �Փ˂����̂Ń{�[���̑��x��ݒ肷��
                SetBallSpeed(&m_ballInfo[i], &m_ballInfo[j]);
            }
        }
    }

    // �Q�[���N���A�`�F�b�N
    if (CheckGameClear())
    {
        MessageBox(nullptr, L"GameClear", L"���߂łƂ�!", MB_OK);
        GameReset();
    }

    // �Q�[���I�[�o�[�`�F�b�N
    if (CheckGameOver())
    {
        MessageBox(nullptr, L"GameOver", L"�c�O", MB_OK);
        GameReset();
    }
    
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    DirectX::SimpleMath::Matrix world;

    // ���̕`��
    m_floor->Draw(context, m_states.get(), world, m_view, m_proj);

    // �{�[���̉e�̕`��
    for (int i = 0; i < BN_NUM; i++)
    {
        world = SimpleMath::Matrix::CreateTranslation(m_ballInfo[i].position);
        m_shadow[i]->Draw(context, m_states.get(), world, m_view, m_proj);
    }

    // �{�[���̕`��
    for (int i = 0; i < BN_NUM; i++)
    {
        world = SimpleMath::Matrix::CreateFromQuaternion(m_ballInfo[i].rotate) * SimpleMath::Matrix::CreateTranslation(m_ballInfo[i].position);
        m_ball[i]->Draw(context, *m_states.get(), world, m_view, m_proj);
    }

    // �p���[���[�^�[�̕`��
    m_meter->Draw(context, m_states.get());

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).

    // ���ʃX�e�[�g�̍쐬
    m_states = std::make_unique<CommonStates>(device);

    auto context = m_deviceResources->GetD3DDeviceContext();

    // �G�t�F�N�g�t�@�N�g���[�̍쐬
    m_effectFactory = std::make_unique<EffectFactory>(device);
    m_effectFactory->SetDirectory(L"Resources");

    // �{�[���̃��f���f�[�^�̃��[�h
    m_ballModel = Model::CreateFromCMO(device, L"Resources/ball.cmo", *m_effectFactory.get());

    // ���C�g�̐ݒ�
    m_ballModel->UpdateEffects([&](IEffect* effect) 
        {
            auto lights = dynamic_cast<IEffectLights*>(effect);
            if (lights)
            {
                lights->SetPerPixelLighting(true);
            }
        }
    );

    // ���̍쐬
    m_floor = std::make_unique<Floor>(device, context, FLOOR_SIZE);

    // �{�[���̍쐬
    for (int i = 0; i < BN_NUM; i++)
    {
        m_ball[i] = std::make_unique<Ball>(context, BALL_RADIUS, *m_ballModel.get());
    }

    // �{�[���̉e�̍쐬
    for (int i = 0; i < BN_NUM; i++)
    {
        m_shadow[i] = std::make_unique<Shadow>(device, context, BALL_RADIUS);
    }

    // �p���[���[�^�[�̍쐬
    int width, height;
    GetDefaultSize(width, height);
    m_meter = std::make_unique<Meter>(device, context, width, height);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // �ˉe�s��̐ݒ�
    int width, height;
    GetDefaultSize(width, height);
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(45.0f),
        static_cast<float>(width) / static_cast<float>(height), 1.0f, 1000.0f);
}

void Game::GameReset()
{
    // �v���C���[�̃{�[����񏉊���
    m_ballInfo[BN_PLAYER].position.x = 0.0f;
    m_ballInfo[BN_PLAYER].position.y = 0.0f;
    m_ballInfo[BN_PLAYER].position.z = 0.0f;
    m_ballInfo[BN_PLAYER].direction = 0.0f;
    m_ballInfo[BN_PLAYER].speed = 0.0f;
    m_ballInfo[BN_PLAYER].color = Colors::White;
    m_ballInfo[BN_PLAYER].rotate = GetRandomRotate();

    // �v���C���[�̃{�[���ȊO�̃{�[�����̏�����
    for (size_t i = 1; i < BN_NUM; i++)
    {
        // �����_���Ń{�[���̈ʒu��ݒ�
        m_ballInfo[i].position.x = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].position.y = 0.0f;
        m_ballInfo[i].position.z = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].direction = 0.0f;
        m_ballInfo[i].speed = 0.0f;
        m_ballInfo[i].rotate = GetRandomRotate();

        // �����_���Ń{�[���̐F��ݒ�
        switch (static_cast<BallColor>(rand() % static_cast<int>(BallColor::NumItems)))
        {
        case BallColor::Yellow:
            m_ballInfo[i].color = Colors::Yellow;
            break;
        case BallColor::Magenta:
            m_ballInfo[i].color = Colors::Magenta;
            break;
        default:
            m_ballInfo[i].color = Colors::Cyan;
            break;
        }

    }
}

void Game::SetBallSpeed(BallInfo* ball_a, BallInfo* ball_b)
{
    // �Փ˂����{�[���𔽎˂�����i�������v�Z�͂��Ă��܂���j
    SimpleMath::Vector3 v = ball_b->position - ball_a->position;

    ball_a->direction = atan2f(v.x, v.z) + XM_PI;
    ball_b->direction = ball_a->direction + XM_PI;

    v.Normalize();
    ball_b->position = ball_a->position + (v * 2.01f);
    ball_b->speed = ball_a->speed;
}

bool Game::CheckGameClear()
{
    // �S�Ẵ{�[�������̊O���֏o�����H
    for (int i = 1; i < BN_NUM; i++)
    {
        float x = m_ballInfo[i].position.x + (FLOOR_SIZE / 2.0f);
        float z = m_ballInfo[i].position.z + (FLOOR_SIZE / 2.0f);
        if (x >= 0.0f && x <= FLOOR_SIZE && z >= 0.0f && z <= FLOOR_SIZE)
        {
            return false;
        }
    }
    return true;
}

bool Game::CheckGameOver()
{
    // �v���C���[�̃{�[�������̊O���֏o�����H
    float x = m_ballInfo[BN_PLAYER].position.x + (FLOOR_SIZE / 2.0f);
    float z = m_ballInfo[BN_PLAYER].position.z + (FLOOR_SIZE / 2.0f);
    if (x < 0.0f || x > FLOOR_SIZE || z < 0.0f || z > FLOOR_SIZE)
    {
        return true;
    }
    return false;
}

DirectX::SimpleMath::Quaternion Game::GetRandomRotate()
{
    float yaw   = XMConvertToRadians(static_cast<float>(rand() % 360 - 180));
    float pitch = XMConvertToRadians(static_cast<float>(rand() % 360 - 180));
    float roll  = XMConvertToRadians(static_cast<float>(rand() % 360 - 180));

    return SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_floor.reset();
    for (int i = 0; i < BN_NUM; i++)
    {
        m_ball[i].reset();
        m_shadow[i].reset();
    }
    m_meter.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
