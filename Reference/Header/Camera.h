#pragma once

#include "GameObject.h"
#include "Transform.h"

/* Ŭ���̾�Ʈ���� ���� ī�޶� ���� �θ� �ȴ�. */
/* ��� ī�޶󿡰� �ʿ��� ����� ���������� �����Ͽ� �ڽĿ��� ��ӳ����ֱ����ؼ�. */

/* ��� ī�޶󿡰� �ʿ��� ��� :  */
/* ī�޶��� ���� ����� �̿��Ͽ� �佺���̽� ��ȯ���.*/
/* �������(fov, aspect, near, far)�� �����ϴ��۾�.  */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CTransform::tagTransformDesc
	{
		_float4 vEye, vAt;
		_float  fFovY, fAspect, fNear, fFar;
		_matrix matView;
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // ����
	virtual HRESULT Initialize_Clone(void* pArg) override; // �纻
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;

	const _vector& Get_CamLook() { return m_pTransformCom->Get_State(CTransform::STATE_LOOK); }
	// void Set_CamLook(const _vector& _vPlayerLook) { m_pTransformCom->Set_State(CTransform::STATE_LOOK, _vPlayerLook); }
protected:
	class CTransform* m_pTransformCom = nullptr;
	class CPipeLine*  m_pPipeLine = nullptr;

	_float4 vEye, vAt;
	_float  fFovY, fAspect, fNear, fFar;
	_matrix matView;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

