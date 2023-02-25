//
// Meter.h
//
// Author Imase Hideyasu
#pragma once

class Meter
{
private:

	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitive;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// ウインドウのサイズ
	int m_windowWidth;
	int m_windowHeight;

	// パワー
	float m_power;

public:

	// コンストラクタ
	Meter(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);

	// パワーを設定する関数
	void SetPower(float power);

	// 描画関数
	void Draw(ID3D11DeviceContext* context, DirectX::CommonStates* states);

};

