//
// Ball.cpp
//
// Author Imase Hideyasu

#include "pch.h"
#include "Ball.h"

using namespace DirectX;

// コンストラクタ
Ball::Ball(ID3D11DeviceContext* context, float radius, const Model& model)
	: m_model(model)
{
}

// 描画関数
void Ball::Draw(ID3D11DeviceContext* context,
	const DirectX::CommonStates& states,
	DirectX::SimpleMath::Matrix world,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix proj)
{
	m_model.Draw(context, states, world, view, proj);
}

