#pragma once

#include "Model.h"
#include "Renderer.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Grid.h"
#include "VIBuffer_Cube.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

BEGIN(Engine)

/* ������Ʈ���� ������ �����ϴ� Manager�̸� 
   �纻�� �ʿ�� �ϴ� ��ü���� �����Ѵ�. */

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default; 

public:
	HRESULT Reserve_Manager(_uint _iLevelIndex);
	HRESULT Add_ProtoType_Component(_uint _iLevelIndex, const wstring& _strProtoTypeTag, class CComponent* _pProtoTypeComponent);
	class CComponent* Clone_Component(_uint _iLevelIndex, const wstring& _strProtoTypeTag, void* _pArg);
	class CComponent* Find_ProtoType(_uint _iLevelIndex, const wstring& _strProtoTypeTag);

private:
	_uint								   m_iLevelIndex = 0;
	map<const wstring, class CComponent*>* m_mapProtoTypeObj = nullptr;

public:
	virtual void Free() override;
};

END

