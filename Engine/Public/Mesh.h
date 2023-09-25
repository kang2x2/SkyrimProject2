#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CBase
{
private:
	CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMesh();
	virtual ~CMesh() = default;
};

END

