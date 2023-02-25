//
// Ball.cpp
//
// Author Imase Hideyasu

#include "pch.h"
#include "Ball.h"

using namespace DirectX;

// コンストラクタ
Ball::Ball(ID3D11DeviceContext* context, float radius)
{
	m_ball = DirectX::GeometricPrimitive::CreateSphere(context, radius * 2.0f);
}

// 描画関数
void Ball::Draw(DirectX::SimpleMath::Matrix world,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix proj,
	DirectX::SimpleMath::Color color)
{
	m_ball->Draw(world, view, proj, color);
}

