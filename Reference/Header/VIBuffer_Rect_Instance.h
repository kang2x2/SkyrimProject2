#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Rect_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_ProtoType(_uint iNumInstance);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Update(_float _fTimeDelta);

public:

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

