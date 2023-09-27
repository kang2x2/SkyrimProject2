#include "Object_Manager.h"

#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint _iLevelCount)
{
	if (m_mapLayer != nullptr)
		return E_FAIL;

	m_mapLayer = new map<const wstring, class CLayer*>[_iLevelCount];

	m_iLevelNum = _iLevelCount;

	return S_OK;
}

HRESULT CObject_Manager::Add_ProtoObject(const wstring& _strProtoTypeTag, CGameObject* _pGameObject)
{
	// ���� �߰��ϴ� ���̱� ������ �̹� �����Ͽ� ��ȯ���� nullptr�� �ƴ϶�� ����.
	if (Find_ProtoObject(_strProtoTypeTag) != nullptr)
		return E_FAIL;

	m_mapProtoTypeObj.emplace(_strProtoTypeTag, _pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strProtoTypeTag, void* pArg)
{
	// �����Ϸ��� ������ ã�´�.
	CGameObject* pProtoTypeObject = Find_ProtoObject(_strProtoTypeTag);
	if (pProtoTypeObject == nullptr) // �����Ϸ��� ������ ������ ����.
		return E_FAIL;

	// ã�� ������ �����Ͽ� �纻 ����.
	CGameObject* pCloneObject = pProtoTypeObject->Clone(pArg);
	if (pCloneObject == nullptr) // ���翡 ���� ���� ��
		return E_FAIL;

	CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);

	// �纻�� �߰��Ϸ��� ���̾ �������� ����.(���� �߰�)
	if (pLayer == nullptr)
	{
		// ���̾� ���� ����
		pLayer = CLayer::Create();

		// ���̾ ������Ʈ �߰�
		pLayer->Add_CloneObject(pCloneObject);

		// ���̾ ���ο� ���̾�� �ش� ������ map�� �߰�.
		m_mapLayer[_iLevelIndex].emplace(_strLayerTag, pLayer);
	}
	else
	{
		pLayer->Add_CloneObject(pCloneObject);
	}

	return S_OK;
}

void CObject_Manager::Tick(_float _fTimeDelta)
{
	for (size_t i = 0; i < m_iLevelNum; ++i)
	{
		for (auto& iter : m_mapLayer[i])
			iter.second->Tick(_fTimeDelta);
	}
}

void CObject_Manager::LateTick(_float _fTimeDelta)
{
	for (size_t i = 0; i < m_iLevelNum; ++i)
	{
		for (auto& iter : m_mapLayer[i])
			iter.second->LateTick(_fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint _iLevelIndex)
{
	for (auto& iter : m_mapLayer[_iLevelIndex])
	{
		Safe_Release(iter.second);
	}
	m_mapLayer[_iLevelIndex].clear();
}

CGameObject* CObject_Manager::Find_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strName)
{
	// �Ű��� ���� ������ ���̾ �˻�
	for (auto& iter : m_mapLayer[_iLevelIndex])
	{
		if (iter.first == _strLayerTag)
		{
			// �Ű��� ���� ���̾��� �̸��� ã�Ҵٸ�
			// �Ű��� ���� ������Ʈ�� �̸��� ���̾��� Find_Object�� �ѱ��.
			return iter.second->Find_CloneObject(_strName);
		}
	}

	return nullptr;
}

CGameObject* CObject_Manager::Find_ProtoObject(const wstring& _strPrototypeTag)
{
	auto iter = m_mapProtoTypeObj.find(_strPrototypeTag);

	if (iter == m_mapProtoTypeObj.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint _iLevelIndex, const wstring& _strLayerTag)
{
	if (_iLevelIndex >= m_iLevelNum)
		return nullptr;

	auto iter = m_mapLayer[_iLevelIndex].find(_strLayerTag);

	if (iter == m_mapLayer[_iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	__super::Free();

	// ���̾� �� ���� (�纻 ����)
	for (size_t i = 0; i < m_iLevelNum; ++i)
	{
		for (auto& iter : m_mapLayer[i])
		{
			Safe_Release(iter.second);
		}
		m_mapLayer[i].clear();
	}
	Safe_Delete_Array(m_mapLayer);

	// ���� ��ü �� ���� (���� ����)
	for (auto& iter : m_mapProtoTypeObj)
	{
		Safe_Release(iter.second);
	}
	m_mapProtoTypeObj.clear();

}
