//
// Floor.cpp
//
// Author Imase Hideyasu

#include "pch.h"
#include "Floor.h"

using namespace DirectX;

// コンストラクタ
Floor::Floor(ID3D11Device* device, ID3D11DeviceContext* context, float floorSize)
	: m_floorSize(floorSize)
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
	CreateDDSTextureFromFile(device, L"Resources/floor.dds", nullptr, m_texture.ReleaseAndGetAddressOf());
}

// 描画関数
void Floor::Draw(ID3D11DeviceContext* context,
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
	ID3D11SamplerState* sampler[] = { states->PointWrap() };
	context->PSSetSamplers(0, 1, sampler);

	// ブレンドステートの設定
	context->OMSetBlendState(states->Opaque(),nullptr, 0xffffffff);
	
	// 深度バッファの設定
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// 床の頂点情報
	static const float halfFloorSize = m_floorSize / 2.0f;
	static VertexPositionTexture vertexes[] =
	{
		VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(          0.0f,          0.0f)),
		VertexPositionTexture(SimpleMath::Vector3( halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2( halfFloorSize,          0.0f)),
		VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(          0.0f, halfFloorSize)),
		VertexPositionTexture(SimpleMath::Vector3( halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2( halfFloorSize, halfFloorSize)),
	};
	// 床のインデックス情報
	static uint16_t indexes[] = {0, 1, 2, 1, 3, 2};

	// 床を描画
	m_primitive->Begin();
	m_primitive->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitive->End();
}
