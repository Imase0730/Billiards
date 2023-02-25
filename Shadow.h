//
// Shadow.h
//
// Author Imase Hideyasu
#pragma once

class Shadow
{
private:

	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitive;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// 半径
	float m_radius;

public:

	// コンストラクタ
	Shadow(ID3D11Device* device, ID3D11DeviceContext* context, float radius);

	// 描画関数
	void Draw(ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj);

};

