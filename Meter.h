//
// Meter.h
//
// Author Imase Hideyasu
#pragma once

class Meter
{
private:

	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitive;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// �E�C���h�E�̃T�C�Y
	int m_windowWidth;
	int m_windowHeight;

	// �p���[
	float m_power;

public:

	// �R���X�g���N�^
	Meter(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);

	// �p���[��ݒ肷��֐�
	void SetPower(float power);

	// �`��֐�
	void Draw(ID3D11DeviceContext* context, DirectX::CommonStates* states);

};

