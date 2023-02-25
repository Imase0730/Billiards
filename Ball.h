//
// Ball.h
//
// Author Imase Hideyasu

#pragma once

class Ball
{
private:

	// モデルデータへのポインタ
	const DirectX::Model& m_model;

public:
	
	// コンストラクタ
	Ball(ID3D11DeviceContext* context, float radius, const DirectX::Model& model);

	// 描画関数
	void Draw(ID3D11DeviceContext* context,
		const DirectX::CommonStates& states,
		DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj);

};

