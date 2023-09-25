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
	_float3 Return_WorldMousePos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos) const;

private:

public:
	virtual void Free() override;
};

END

