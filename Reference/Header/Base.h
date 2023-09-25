#pragma once

#include "Engine_Defines.h"

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* RefCnt ����. ���� ���� �� Cnt ��ȯ. */
	unsigned int AddRef();
	/* RefCnt ����. ���� ���� �� Cnt ��ȯ. */
	unsigned int Release();

private:
	unsigned int m_iRefCnt = 0;

public:
	virtual void Free();
};

