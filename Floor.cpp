//
// Floor.cpp
//
// Author Imase Hideyasu

#include "pch.h"
#include "Floor.h"

using namespace DirectX;

// �R���X�g���N�^
Floor::Floor(ID3D11Device* device, ID3D11DeviceContext* context, float floorSize)
	: m_floorSize(floorSize)
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
	CreateDDSTextureFromFile(device, L"Resources/floor.dds", nullptr, m_texture.ReleaseAndGetAddressOf());
}

// �`��֐�
void Floor::Draw(ID3D11DeviceContext* context,
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
	ID3D11SamplerState* sampler[] = { states->PointWrap() };
	context->PSSetSamplers(0, 1, sampler);

	// �u�����h�X�e�[�g�̐ݒ�
	context->OMSetBlendState(states->Opaque(),nullptr, 0xffffffff);
	
	// �[�x�o�b�t�@�̐ݒ�
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// ���̒��_���
	static const float halfFloorSize = m_floorSize / 2.0f;
	static VertexPositionTexture vertexes[] =
	{
		VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(          0.0f,          0.0f)),
		VertexPositionTexture(SimpleMath::Vector3( halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2( halfFloorSize,          0.0f)),
		VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(          0.0f, halfFloorSize)),
		VertexPositionTexture(SimpleMath::Vector3( halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2( halfFloorSize, halfFloorSize)),
	};
	// ���̃C���f�b�N�X���
	static uint16_t indexes[] = {0, 1, 2, 1, 3, 2};

	// ����`��
	m_primitive->Begin();
	m_primitive->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitive->End();
}
