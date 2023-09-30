#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCalculator : public CBase
{
	DECLARE_SINGLETON(CCalculator)
private:
	CCalculator();
	virtual ~CCalculator() = default;

public:
	// 오버 라이딩
	_float3 Picking_Grid(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const POINT& _WinMousePos, class CGameObject* _pTerrain, const _float3* _vec) const;
	_float3 Picking_Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const POINT& _WinMousePos) const;

private:

public:
	virtual void Free() override;
};

END

