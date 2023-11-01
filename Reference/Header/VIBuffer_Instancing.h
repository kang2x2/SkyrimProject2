#pragma once

/* �ټ��� Instance�� �׷����� Ŭ�������� �θ� */
/* Instance : �� �ϳ��� ��������. */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		_float3 vCenter; // ��� ��������
		_float3 vRange; // ��ŭ ������ �����Ұ���
		_float  fSpeedMin, fSpeedMax = 0.f; // ���ǵ� ����
		_float  fScaleMin, fScaleMax = 0.f; // ������ ����
		_float  fLifeTimeMin, fLifeTimeMax = 0.f; // �����ֱ� ����
		_uint   iNumInstance = 0; // ���� �� �ν��Ͻ� ��.
	}INSTANCE_DESC;
protected:
	CVIBuffer_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_ProtoType(const INSTANCE_DESC& _InstanceDesc);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Update(_float _fTimeDelta);
	virtual HRESULT Render() override;

protected:
	_uint			m_iStrideInstance = 0; // ũ��
	_uint			m_iNumInstance = 0; // �׷��� ����
	_uint			m_iNumIndicesPerInstance = 0; 
	ID3D11Buffer*	m_pVBInstance = nullptr;
	VTXINSTANCE*	m_pVertices = nullptr;

	_float*			m_pSpeedAry = nullptr;
	_float*			m_pLifeTimeAry = nullptr;
	_float*			m_pTimeAccAry = nullptr;

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END

