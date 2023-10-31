#pragma once

/* 다수의 Instance를 그려내는 클래스들의 부모 */
/* Instance : 모델 하나라 생각하자. */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
protected:
	CVIBuffer_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_ProtoType()		 override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Update(_float _fTimeDelta);
	virtual HRESULT Render() override;

protected:
	_uint			m_iStrideInstance = 0; // 크기
	_uint			m_iNumInstance = 0; // 그려낼 개수
	_uint			m_iNumIndicesPerInstance = 0; 
	ID3D11Buffer*	m_pVBInstance = nullptr;

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END

