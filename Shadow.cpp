//
// Shadowr.cpp
//
// Author Imase Hideyasu
#include "pch.h"
#include "Shadow.h"

using namespace DirectX;

Shadow::Shadow(ID3D11Device* device, ID3D11DeviceContext* context, float radius)
	: m_radius(radius)
{
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
	m_primitive = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// テクスチャの読み込み
	CreateDDSTextureFromFile(device, L"Resources/shadow.dds", nullptr, m_texture.ReleaseAndGetAddressOf());
}

void Shadow::Draw(ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	DirectX::SimpleMath::Matrix world,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix proj)
{
	// エフェクトを適応する
	m_effect->SetWorld(world);
	m_effect->SetView(view);
	m_effect->SetProjection(proj);
	m_effect->SetTexture(m_texture.Get());
	m_effect->Apply(context);

	// 入力レイアウトの設定
	context->IASetInputLayout(m_inputLayout.Get());

	// テクスチャサンプラの設定
	ID3D11SamplerState* sampler[] = { states->LinearClamp() };
	context->PSSetSamplers(0, 1, sampler);

	// アルファブレンドの設定
	context->OMSetBlendState(states->AlphaBlend(),nullptr, 0xffffffff);

	// 深度バッファの設定
	context->OMSetDepthStencilState(states->DepthNone(), 0);

	// 影の頂点情報
	static VertexPositionTexture vertexes[] =
	{
		VertexPositionTexture(SimpleMath::Vector3(-m_radius, -1.0f, -m_radius), SimpleMath::Vector2(0.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3( m_radius, -1.0f, -m_radius), SimpleMath::Vector2(1.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3(-m_radius, -1.0f,  m_radius), SimpleMath::Vector2(0.0f, 1.0f)),
		VertexPositionTexture(SimpleMath::Vector3( m_radius, -1.0f,  m_radius), SimpleMath::Vector2(1.0f, 1.0f)),
	};
	// 影のインデックス情報
	static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

	// 影を描画
	m_primitive->Begin();
	m_primitive->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitive->End();

}
