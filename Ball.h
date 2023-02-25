//
// Ball.h
//
// Author Imase Hideyasu

#pragma once

class Ball
{
private:

	// ���f���f�[�^�ւ̃|�C���^
	const DirectX::Model& m_model;

public:
	
	// �R���X�g���N�^
	Ball(ID3D11DeviceContext* context, float radius, const DirectX::Model& model);

	// �`��֐�
	void Draw(ID3D11DeviceContext* context,
		const DirectX::CommonStates& states,
		DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj);

};

