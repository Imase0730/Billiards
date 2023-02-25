//
// Ball.h
//
// Author Imase Hideyasu

#pragma once

class Ball
{
private:

	// ��
	std::unique_ptr<DirectX::GeometricPrimitive> m_ball;

public:
	
	// �R���X�g���N�^
	Ball(ID3D11DeviceContext* context, float radius);

	// �`��֐�
	void Draw(DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj,
		DirectX::SimpleMath::Color color);

};

