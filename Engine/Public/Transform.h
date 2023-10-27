#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float fSpeedPerSec = 0.f;
		_float fRotationRadianPerSec = 0.f;
		_float fZoomPerSec = 0.f;
	}TRANSFORM_DESC;
	
private:
	CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	/* ���� �����󿡼��� ����� �� �� ����(��, ��, ��, ��)�� �����Ѵ�. */
	_vector Get_State(STATE _eState)
	{
		//  XMLoadFloat4x4 : float4x4�� XMMATRIX�� ġȯ.
		return XMLoadFloat4x4(&m_WorldMatrix).r[_eState];
	}
	_float3 Get_Scaled();
	// _vector�� �ƴ� _fvector�� ���� ���ĺ��� ���� vector const ���̴�.
	/* ������Ŀ� �� �࿡ �ʿ��� ������ �־��ش�. */
	_float3 Get_Rotated();

	void Set_State(STATE _eState, _fvector _vState);
	/* ����� ������ ������ vScale�� ����ִ� ���� ������ ���Ѵ�. */
	void Set_Scaling(const _float3& _vScale);

	void Set_WorldMatrix(const _matrix& _matrix);

	/* �ӵ� ���� */
	void Set_Speed(_float _fSpeed) { m_fSpeedPerSec = _fSpeed; }

	// ���� ��� ��ȯ
	_float4x4 Get_WorldMatrix_Float4x4() const
	{
		return m_WorldMatrix;
	}
	_matrix Get_WorldMatrix() const
	{
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	// ���� ����� ��ȯ
	_float4x4 Get_WorldMatrix_Float4x4_Inverse() const
	{
		_float4x4 tempMatrix;
		XMStoreFloat4x4(&tempMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
		return tempMatrix;
	}
	_matrix Get_WorldMatrix_Inverse() const
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

public:
	virtual HRESULT Initialize_ProtoType() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	HRESULT Bind_ShaderResources(class CShader* _pShader, const char* _pConstantName);

public:
	void Go_Foward(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Backward(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Left(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Right(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Up(_float _fTimeDelta);
	void Go_Down(_float _fTimeDelta);

	void Zoom_In(_float _fTimeDelta);
	void Zoom_Out(_float _fTimeDelta);

	void Fix_Rotation(FXMVECTOR _vAxis, _float _fRadian);
	void Turn(FXMVECTOR _vAxis, _float _fTimeDelta, _float _fRotationRadianPerSec = 0.f);
	void LookAt(_fvector _vPoint);
	void SetLook(_fvector _vPoint);
	void Chase(_fvector _vPoint, _float _fTimeDelta, _float _fMargin = 0.1f);

private:
	/* XMFLOAT2, XMFLOAT3, XMFLOAT4 : ����뺤��. */
	/* XMFLOAT4X4 : �ܼ� float 16���� �����ϰ� �ִ� ����ü. */
	/* ���� ������ ����� ������ ���� �ʴ�. */
	/* ����� ������ �����ϴ� ����� ���� �����ϴ� Ÿ��. */

	/* XMVECTOR : ����뺤��. */
	/* XMMATRIX : ����� ������ ���� �����Ǵ� Ÿ��. */

	_float4x4			m_WorldMatrix; // �����

	_float				m_fSpeedPerSec = { 10.0f }; // �ǽð� �̵� �ӵ�.
	_float				m_fRotationRadianPerSec = { 0.0f }; // �ǽð� ȸ�� �ӵ�.
	_float				m_fZoomPerSec = { 0.0f }; // �ǽð� �� �ӵ�.

public:
	static CTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

