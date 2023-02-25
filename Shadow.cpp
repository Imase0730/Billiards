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
	// �G�t�F�N�g�̍쐬
	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetLightingEnabled(false);
	m_effect->SetTextureEnabled(true);
	m_effect->SetVertexColorEnabled(false);

	// ���̓��C�A�E�g�̍쐬
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionTexture>(device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf())
	);

	// �v���~�e�B�u�o�b�`�̍쐬
	m_primitive = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// �e�N�X�`���̓ǂݍ���
	CreateDDSTextureFromFile(device, L"Resources/shadow.dds", nullptr, m_texture.ReleaseAndGetAddressOf());
}

void Shadow::Draw(ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	DirectX::SimpleMath::Matrix world,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix proj)
{
	// �G�t�F�N�g��K������
	m_effect->SetWorld(world);
	m_effect->SetView(view);
	m_effect->SetProjection(proj);
	m_effect->SetTexture(m_texture.Get());
	m_effect->Apply(context);

	// ���̓��C�A�E�g�̐ݒ�
	context->IASetInputLayout(m_inputLayout.Get());

	// �e�N�X�`���T���v���̐ݒ�
	ID3D11SamplerState* sampler[] = { states->LinearClamp() };
	context->PSSetSamplers(0, 1, sampler);

	// �A���t�@�u�����h�̐ݒ�
	context->OMSetBlendState(states->AlphaBlend(),nullptr, 0xffffffff);

	// �[�x�o�b�t�@�̐ݒ�
	context->OMSetDepthStencilState(states->DepthNone(), 0);

	// �e�̒��_���
	static VertexPositionTexture vertexes[] =
	{
		VertexPositionTexture(SimpleMath::Vector3(-m_radius, -1.0f, -m_radius), SimpleMath::Vector2(0.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3( m_radius, -1.0f, -m_radius), SimpleMath::Vector2(1.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3(-m_radius, -1.0f,  m_radius), SimpleMath::Vector2(0.0f, 1.0f)),
		VertexPositionTexture(SimpleMath::Vector3( m_radius, -1.0f,  m_radius), SimpleMath::Vector2(1.0f, 1.0f)),
	};
	// �e�̃C���f�b�N�X���
	static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

	// �e��`��
	m_primitive->Begin();
	m_primitive->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitive->End();

}
