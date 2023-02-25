//
// Ball.h
//
// Author Imase Hideyasu

#pragma once

class Ball
{
private:

	// 球
	std::unique_ptr<DirectX::GeometricPrimitive> m_ball;

public:
	
	// コンストラクタ
	Ball(ID3D11DeviceContext* context, float radius);

	// 描画関数
	void Draw(DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj,
		DirectX::SimpleMath::Color color);

};

