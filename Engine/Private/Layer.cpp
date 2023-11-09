#include "Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_CloneObject(CGameObject* _pGameObject)
{
	if (_pGameObject == nullptr)
		return E_FAIL;

	m_ltCloneObj.push_back(_pGameObject);

	return S_OK;
}

HRESULT CLayer::Delete_CloneObject(const wstring& _strName)
{
	for (auto iter = m_ltCloneObj.begin(); iter != m_ltCloneObj.end(); )
	{
		if ((*iter)->Get_IsDead())
		{
			Safe_Release(*iter);
			iter = m_ltCloneObj.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	return S_OK;
}

void CLayer::PriorityTick(_float _fTimeDelta)
{
	for (auto& iter : m_ltCloneObj)
	{
		if (iter != nullptr)
			iter->PriorityTick(_fTimeDelta);
	}
}

void CLayer::Tick(_float _fTimeDelta)
{
	for (auto& iter : m_ltCloneObj)
	{
		if (iter != nullptr)
			iter->Tick(_fTimeDelta);
	}
}

void CLayer::LateTick(_float _fTimeDelta)
{
	for (auto& iter : m_ltCloneObj)
	{
		if (iter != nullptr)
		{
			iter->LateTick(_fTimeDelta);
			if (iter->Get_IsDead())
				Safe_Release(iter);
		}
	}

	//for (auto iter = m_ltCloneObj.begin(); iter != m_ltCloneObj.end(); )
	//{
	//	if ((*iter)->Get_IsDead())
	//	{
	//		Safe_Release(*iter);
	//		iter = m_ltCloneObj.erase(iter);
	//	}
	//	else
	//	{
	//		++iter;
	//	}
	//}
}

CGameObject* CLayer::Find_CloneObject(const wstring& _strName)
{
	for (auto& iter : m_ltCloneObj)
	{
		// �Ű��� ���� ������Ʈ�� �̸��� ã�Ҵٸ� �ش� �̸��� ���� ������Ʈ�� ��ȯ�Ѵ�.
		if (iter->Get_Name() == _strName)
			return iter;
	}

	return nullptr;
}

CLayer* CLayer::Create()
{
	CLayer* pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	__super::Free();

	for (auto& iter : m_ltCloneObj)
		Safe_Release(iter);

	m_ltCloneObj.clear();
}
