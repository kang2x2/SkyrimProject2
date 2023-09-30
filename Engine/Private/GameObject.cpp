#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CGameObject::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CGameObject::Initialize_Clone(const wstring& _strModelComTag)
{
	return S_OK;
}

void CGameObject::Tick(_float _fTimeDelta)
{
}

void CGameObject::LateTick(_float _fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent* CGameObject::Get_Component(const wstring& _strComponentName)
{
	for (auto& iter : m_mapComponent)
	{
		if (iter.first == _strComponentName)
			return iter.second;
	}

	return nullptr;
}

HRESULT CGameObject::Add_CloneComponent(_uint _iLevelIndex, const wstring& _strProtoTypeTag, const wstring& _strComponentTag, CComponent** _ppOut, void* pArg)
{
	// �̹� ��� �Ǿ��ִ� ������Ʈ �׷��̶�� ����. 
	if (Find_ProtoTypeComponent(_strComponentTag) != nullptr)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// �纻 ����.
	CComponent* pComponent = pGameInstance->Clone_Component(_iLevelIndex, _strProtoTypeTag, pArg);
	if (pComponent == nullptr)
		return E_FAIL;

	// �˻��� �����ϰ� map�� ����
	m_mapComponent.emplace(_strComponentTag, pComponent);

	// �ڽ��� ��� ������ ����(���� ������Ʈ���� �ڽ��� ������ �ְ� �����ϰ� �ִ�.)
	*_ppOut = pComponent;

	Safe_AddRef(pComponent);
	Safe_Release(pGameInstance);

	return S_OK;
}

CComponent* CGameObject::Find_ProtoTypeComponent(const wstring& _strProtoTypeTag)
{
	auto iter = m_mapComponent.find(_strProtoTypeTag);

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

CGameObject* CGameObject::Clone(const wstring& _strModelComTag)
{
	return nullptr;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& iter : m_mapComponent)
		Safe_Release(iter.second);

	m_mapComponent.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
