//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

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

    // ボールの色
    enum class BallColor
    {
        Yellow,
        Magenta,
        Cyan,
        NumItems
    };

    // プレイヤーのボールの番号
    static const int BN_PLAYER;

    // ボールの半径
    static const float BALL_RADIUS;

    // 床の大きさ
    static const float FLOOR_SIZE;

    // ボールが初期配置されるエリアのサイズ
    static const float BALL_PUT_AERA_SIZE;

    // １フレーム辺りのカメラの回転角度
    static const float FRAME_ROTATE_ANGLE;

    // １フレーム辺りのパワーの変化量
    static const float AMOUNT_OF_CHANGE_OF_POWER;

    // 球の最大速度
    static const float MAX_BALL_SPEED;

    // ボール情報
    struct BallInfo
    {
        DirectX::SimpleMath::Vector3 position;  // 位置
        float direction;                        // 方向
        float speed;                            // 速さ
        DirectX::SimpleMath::Color color;       // 色
    };

    // ボールの数
    static const int BN_NUM = 20;

    BallInfo m_ballInfo[BN_NUM];

    // ビュー行列
    DirectX::SimpleMath::Matrix m_view;

    // 射影行列
    DirectX::SimpleMath::Matrix m_proj;

    // 共通ステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    // ボール
    std::unique_ptr<DirectX::GeometricPrimitive> m_ball;

    // パワーメーター
    std::unique_ptr<Meter> m_meter;

    // パワー
    float m_power;

    // ベーシックエフェクト
    std::unique_ptr<DirectX::BasicEffect> m_effect;

    // プリミティブバッチ
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // 床のテクスチャハンドル
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floorTexture;

    // 球の影のテクスチャハンドル
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;

private:

    // ゲームのリセット関数
    void InitializeGame();

    // 衝突したボールの速度を設定する関数
    void SetBallSpeed(BallInfo* ball_a, BallInfo* ball_b);

    // ゲームクリアチェック
    bool CheckGameClear();

    // ゲームオーバーチェック
    bool CheckGameOver();

    // 床を描画する関数
    void DrawFloor(ID3D11DeviceContext* context,
        DirectX::CommonStates* states,
        DirectX::SimpleMath::Matrix world,
        DirectX::SimpleMath::Matrix view,
        DirectX::SimpleMath::Matrix proj);

    // 球の影を描画する関数
    void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states);

};
