#include "GameObject.h"
#include "GameInstance.h"

#include <fstream>

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

HRESULT CGameObject::Initialize_Clone(const wstring& _strModelComTag, void* pArg)
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

HRESULT CGameObject::Object_FileSave(wofstream& outFile) const
{
	outFile << m_strName << '\n';
	outFile << m_strHasLayerTag << '\n';
	outFile << m_strModelComTag << '\n';
	outFile << m_bHasMesh << '\n';
	outFile << m_bDead << '\n';
	outFile << m_pDevice << '\n';
	outFile << m_pContext << '\n';
	outFile << m_fX << '\n';
	outFile << m_fY << '\n';
	outFile << m_fSizeX << '\n';
	outFile << m_fSizeY << '\n';

	wstring matrixStr;
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			matrixStr += std::to_wstring(m_ViewMatrix.m[row][col]) + L" ";
		}
		matrixStr += L"\n";
	}

	outFile << matrixStr << '\n';

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			matrixStr += std::to_wstring(m_ProjMatrix.m[row][col]) + L" ";
		}
		matrixStr += L"\n";
	}

	outFile << matrixStr << '\n';

	for (const auto& pair : m_mapComponent) {
		outFile << pair.first << L'\n';  // 키 저장
		outFile << pair.second << L'\n';  // 값 저장
	}

	return S_OK;
}

HRESULT CGameObject::Object_FileLoad(std::ifstream& inFile)
{
	return S_OK;
}

HRESULT CGameObject::Add_CloneComponent(_uint _iLevelIndex, const wstring& _strProtoTypeTag, const wstring& _strComponentTag, CComponent** _ppOut, void* pArg)
{
	// 이미 등록 되어있는 컴포넌트 그룹이라면 실패. 
	if (Find_ProtoTypeComponent(_strComponentTag) != nullptr)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 사본 생성.
	CComponent* pComponent = pGameInstance->Clone_Component(_iLevelIndex, _strProtoTypeTag, pArg);
	if (pComponent == nullptr)
		return E_FAIL;

	// 검색이 가능하게 map에 삽입
	m_mapComponent.emplace(_strComponentTag, pComponent);

	// 자식의 멤버 변수에 저장(현재 컴포넌트들을 자식이 가지고 있게 관리하고 있다.)
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

CGameObject* CGameObject::Clone(const wstring& _strModelComTag, void* _pArg)
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
