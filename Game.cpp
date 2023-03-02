//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

//----- 定数宣言 -----//

// プレイヤーのボールの番号
const int Game::BN_PLAYER = 0;

// ボールの半径
const float Game::BALL_RADIUS = 1.0f;

// 床の大きさ
const float Game::FLOOR_SIZE = 50.0f;

// ボールが初期配置されるエリアのサイズ
const float Game::BALL_PUT_AERA_SIZE = 40.0f;

// １フレーム辺りのカメラの回転角度
const float Game::FRAME_ROTATE_ANGLE = 1.0f;

// １フレーム辺りのパワーの変化量
const float Game::AMOUNT_OF_CHANGE_OF_POWER = 0.01f;

// 球の最大速度
const float Game::MAX_BALL_SPEED = 1.0f;

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
    InitializeGame();
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

    // キー入力
    auto kb = Keyboard::Get().GetState();
    static Keyboard::KeyboardStateTracker tracker;
    tracker.Update(kb);

    // パワーの設定
    m_power += AMOUNT_OF_CHANGE_OF_POWER;
    if (m_power > 1.0f) m_power = -1.0f;
    m_meter->SetPower(fabsf(m_power));

    // カメラの設定
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

    // スペースキーが押された
    if (tracker.pressed.Space)
    {
        // ボールを打ち出す
        m_ballInfo[BN_PLAYER].speed = fabsf(m_power) * MAX_BALL_SPEED;
    }

    // 左右キーでカメラの向きを回転する
    if (kb.Left)
    {
        m_ballInfo[BN_PLAYER].direction += XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }
    if (kb.Right)
    {
        m_ballInfo[BN_PLAYER].direction -= XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }

    // ボールの移動
    for (int i = 0; i < BN_NUM; i++)
    {
        SimpleMath::Matrix matrix = SimpleMath::Matrix::CreateRotationY(m_ballInfo[i].direction);
        SimpleMath::Vector3 vec = SimpleMath::Vector3(0.0f, 0.0f, m_ballInfo[i].speed);
        vec = SimpleMath::Vector3::Transform(vec, matrix);
        m_ballInfo[i].position += vec;
        m_ballInfo[i].speed *= 0.985f;
    }

    // ボールの衝突判定
    for (int i = 0; i < BN_NUM - 1; i++)
    {
        for (int j = i + 1; j < BN_NUM; j++)
        {
            // 球と球の衝突判定
            SimpleMath::Vector3 v = m_ballInfo[i].position - m_ballInfo[j].position;
            if (v.LengthSquared() < (BALL_RADIUS * 2.0f) * (BALL_RADIUS * 2.0f))
            {
                // 衝突したのでボールの速度を設定する
                SetReflectionBallVelocity(&m_ballInfo[i], &m_ballInfo[j]);
            }
        }
    }

    // ゲームクリアチェック
    if (IsGameClear())
    {
        MessageBox(nullptr, L"GameClear", L"おめでとう!", MB_OK);
        InitializeGame();
    }

    // ゲームオーバーチェック
    if (IsGameOver())
    {
        MessageBox(nullptr, L"GameOver", L"残念", MB_OK);
        InitializeGame();
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

    // 床の描画
    DrawFloor(context, m_states.get(), world, m_view, m_proj);

    // 球の影の描画
    DrawShadow(context, m_states.get());

    // 球の描画
    for (int i = 0; i < BN_NUM; i++)
    {
        world = SimpleMath::Matrix::CreateTranslation(m_ballInfo[i].position);
        m_ball->Draw(world, m_view, m_proj, m_ballInfo[i].color);
    }

    // パワーメーターの描画
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

    // 共通ステートの作成
    m_states = std::make_unique<CommonStates>(device);

    auto context = m_deviceResources->GetD3DDeviceContext();

    // エフェクトの作成
    m_effect = std::make_unique<BasicEffect>(device);
    m_effect->SetLightingEnabled(false);
    m_effect->SetTextureEnabled(true);
    m_effect->SetVertexColorEnabled(false);
 
    // 入力レイアウトの作成
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionTexture>(device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );
 
    // プリミティブバッチの作成
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);
 
    // テクスチャの読み込み
    CreateDDSTextureFromFile(device, L"Resources/floor.dds", nullptr, m_floorTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"Resources/shadow.dds", nullptr, m_shadowTexture.ReleaseAndGetAddressOf());

    // ボールの作成
    m_ball = DirectX::GeometricPrimitive::CreateSphere(context, BALL_RADIUS * 2.0f);

    // パワーメーターの作成
    int width, height;
    GetDefaultSize(width, height);
    m_meter = std::make_unique<Meter>(device, context, width, height);
 
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // 射影行列の設定
    int width, height;
    GetDefaultSize(width, height);
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(45.0f),
        static_cast<float>(width) / static_cast<float>(height), 1.0f, 1000.0f);
}

void Game::InitializeGame()
{
    // プレイヤーのボール情報初期化
    m_ballInfo[BN_PLAYER].position.x = 0.0f;
    m_ballInfo[BN_PLAYER].position.y = 0.0f;
    m_ballInfo[BN_PLAYER].position.z = 0.0f;
    m_ballInfo[BN_PLAYER].direction = 0.0f;
    m_ballInfo[BN_PLAYER].speed = 0.0f;
    m_ballInfo[BN_PLAYER].color = Colors::White;

    // プレイヤーのボール以外のボール情報の初期化
    for (size_t i = 1; i < BN_NUM; i++)
    {
        // ランダムでボールの位置を設定
        m_ballInfo[i].position.x = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].position.y = 0.0f;
        m_ballInfo[i].position.z = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].direction = 0.0f;
        m_ballInfo[i].speed = 0.0f;

        // ランダムでボールの色を設定
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

void Game::SetReflectionBallVelocity(BallInfo* ball_a, BallInfo* ball_b)
{
    // 衝突したボールを反射させる（正しい計算はしていません）
    SimpleMath::Vector3 v = ball_b->position - ball_a->position;

    ball_a->direction = atan2f(v.x, v.z) + XM_PI;
    ball_b->direction = ball_a->direction + XM_PI;

    v.Normalize();
    ball_b->position = ball_a->position + (v * (BALL_RADIUS * 2.0f + 0.01f));
    ball_b->speed = ball_a->speed;
}

bool Game::IsGameClear()
{
    // 全てのボールが床の外側へ出たか？
    for (int i = 0; i < BN_NUM; i++)
    {
        if (i == BN_PLAYER) continue;
        float x = m_ballInfo[i].position.x + (FLOOR_SIZE / 2.0f);
        float z = m_ballInfo[i].position.z + (FLOOR_SIZE / 2.0f);
        if (x >= 0.0f && x <= FLOOR_SIZE && z >= 0.0f && z <= FLOOR_SIZE)
        {
            return false;
        }
    }
    return true;
}

bool Game::IsGameOver()
{
    // プレイヤーのボールが床の外側へ出たか？
    float x = m_ballInfo[BN_PLAYER].position.x + (FLOOR_SIZE / 2.0f);
    float z = m_ballInfo[BN_PLAYER].position.z + (FLOOR_SIZE / 2.0f);
    if (x < 0.0f || x > FLOOR_SIZE || z < 0.0f || z > FLOOR_SIZE)
    {
        return true;
    }
    return false;
}

void Game::DrawFloor(ID3D11DeviceContext* context,
    DirectX::CommonStates* states,
    DirectX::SimpleMath::Matrix world,
    DirectX::SimpleMath::Matrix view,
    DirectX::SimpleMath::Matrix proj)
{
    // エフェクトを適応する
    m_effect->SetWorld(world);
    m_effect->SetView(view);
    m_effect->SetProjection(proj);
    m_effect->SetTexture(m_floorTexture.Get());
    m_effect->Apply(context);

    // 入力レイアウトの設定
    context->IASetInputLayout(m_inputLayout.Get());

    // テクスチャサンプラの設定
    ID3D11SamplerState* sampler[] = { states->PointWrap() };
    context->PSSetSamplers(0, 1, sampler);

    // ブレンドステートの設定
    context->OMSetBlendState(states->Opaque(), nullptr, 0xffffffff);

    // 深度バッファの設定
    context->OMSetDepthStencilState(states->DepthDefault(), 0);

    // 床の頂点情報
    static const float halfFloorSize = FLOOR_SIZE / 2.0f;
    static VertexPositionTexture vertexes[] =
    {
        VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(0.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3( halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(halfFloorSize, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(0.0f, halfFloorSize)),
        VertexPositionTexture(SimpleMath::Vector3( halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(halfFloorSize, halfFloorSize)),
    };
    // 床のインデックス情報
    static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

    // 床を描画
    m_primitiveBatch->Begin();
    m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
    m_primitiveBatch->End();
}

void Game::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states)
{
    // エフェクトを適応する
    m_effect->SetWorld(SimpleMath::Matrix::Identity);
    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);
    m_effect->SetTexture(m_shadowTexture.Get());
    m_effect->Apply(context);

    // 入力レイアウトの設定
    context->IASetInputLayout(m_inputLayout.Get());

    // テクスチャサンプラの設定
    ID3D11SamplerState* sampler[] = { states->LinearClamp() };
    context->PSSetSamplers(0, 1, sampler);

    // アルファブレンドの設定
    context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);

    // 深度バッファの設定
    context->OMSetDepthStencilState(states->DepthNone(), 0);

    // 影の頂点情報
    static VertexPositionTexture vertexes[] =
    {
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 1.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 1.0f)),
    };
    // 影のインデックス情報
    static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

    m_primitiveBatch->Begin();

    // 影を描画
    for (int i = 0; i < BN_NUM; i++)
    {
        // 影の表示位置の設定
        float x = m_ballInfo[i].position.x;
        float z = m_ballInfo[i].position.z;
        vertexes[0].position = SimpleMath::Vector3(-BALL_RADIUS + x, -1.0f, -BALL_RADIUS + z);
        vertexes[1].position = SimpleMath::Vector3( BALL_RADIUS + x, -1.0f, -BALL_RADIUS + z);
        vertexes[2].position = SimpleMath::Vector3(-BALL_RADIUS + x, -1.0f,  BALL_RADIUS + z);
        vertexes[3].position = SimpleMath::Vector3( BALL_RADIUS + x, -1.0f,  BALL_RADIUS + z);
        m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
    }

    m_primitiveBatch->End();
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_meter.reset();

    m_ball.reset();

    m_shadowTexture.Reset();
    m_floorTexture.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
