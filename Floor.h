//
// Floor.h
//
// Author Imase Hideyasu

#pragma once

class Floor
{
private:

	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitive;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// 床の大きさ
	float m_floorSize;

public:

	// コンストラクタ
	Floor(ID3D11Device* device, ID3D11DeviceContext* context, float floorSize);

	// 描画関数
	void Draw(ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj);
};

