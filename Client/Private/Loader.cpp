#include "framework.h"
#include "Loader.h"

#include "GameInstance.h"

// Logo Level
#include "BackGround.h"

// GamePlay Level
#include "Free_Camera.h"

#include "Terrain.h"

#include "Player.h"

// Tool Level
#include "Tool_Camera.h"

#include "Terrain_Grid.h"

#include "SkyrimTerrain.h"
#include "Building.h"

CLoader::CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint __stdcall ThreadEntry(void* pArg)
{
	/* DX�� ����ϴ� ��� �� ��ü�� �ʱ�ȭ�Ѵ�.  */
	CoInitializeEx(nullptr, 0);

	/* ���Ӱ� ������ �����尡 ���Ѵ�. */
	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID _eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = _eNextLevel;

	// ���ο� ������ ����
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);

	if (m_hThread == 0)
		return E_FAIL;

	return S_OK;
}

_int CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT hr = 0;

	switch (m_eNextLevel)
	{
	case LEVEL_TOOL:
		hr = Loading_For_Level_Tool();
		break;
	case LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_Level_GamePlay();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&m_Critical_Section);

	return 0;
}

HRESULT CLoader::Loading_For_Level_Tool()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Texture */
	m_strLoadingText = TEXT("Loading Texture.");

	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_TOOL, TEXT("ProtoType_Component_Shader_VtxPosCol"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxPosCol.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNonAnimMesh.hlsl"), VTX_NONANIMMESH::Elements, VTX_NONANIMMESH::iNumElements))))
		return E_FAIL;


	/* Mesh*/
	m_strLoadingText = TEXT("Loading Mesh.");
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_TOOL, TEXT("ProtoType_Component_VIBuffer_Terrain_Grid"),
		CVIBuffer_Grid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Set_ProtoType_Mesh(LEVEL_TOOL);

	/* GameObject */

	// Camera
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ToolCamera"),
		CTool_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	// Grid Terrain
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GridTerrain"),
		CTerrain_Grid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Set_ProtoType_Object();

	/* �ε� �� */
	Safe_Release(pGameInstance);

	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	return S_OK;

}

HRESULT CLoader::Loading_For_Level_Logo()
{
	CGameInstance* pGameInstanc = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstanc);

	/* Texture */
	m_strLoadingText = TEXT("Loading Texture.");
	if(FAILED(pGameInstanc->Add_ProtoType_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/LogoBackGround.dds"), 1))))
		return E_FAIL;

	/* Mesh */
	m_strLoadingText = TEXT("Loading Mesh.");

	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");

	/* GameObject(����) */
	m_strLoadingText = TEXT("Loading ProtoType_GameObject.");
	// BackGround_ProtoType
	if (FAILED(pGameInstanc->Add_ProtoObject(TEXT("ProtoType_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* �ε� �� */
	Safe_Release(pGameInstanc);
	
	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region Texture
	m_strLoadingText = TEXT("Loading Texture.");

	// Terrain 
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Terrain_Mask */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* Brush */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region Mesh
	/* Mesh */
	m_strLoadingText = TEXT("Loading Mesh.");
	_matrix matInitialize = XMMatrixIdentity();

	/* Terrain */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* Player */
	matInitialize = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// matInitialize = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_Player/Idle.fbx", matInitialize, CModel::TYPE_ANIM))))
		return E_FAIL;
	//if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Fiona/Fiona.fbx", matInitialize))))
	//	return E_FAIL;
	// if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player"),
	// 	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/ArcadiaBase01.FBX", matInitialize))))
	// 	return E_FAIL;

#pragma endregion

#pragma region Shader
	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");
	
	/* Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	/* Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNonAnimMesh.hlsl"), VTX_NONANIMMESH::Elements, VTX_NONANIMMESH::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region GameObject
	/* GameObject */
	m_strLoadingText = TEXT("Loading ProtoType_GameObject.");

	// Camera
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_FreeCamera"),
		CFree_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Terrain
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Player
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Set_ProtoType_Object();

#pragma endregion

	Safe_Release(pGameInstance);

	/* �ε� �� */
	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	return S_OK;
}

HRESULT CLoader::Set_ProtoType_Mesh(LEVELID _eLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_matrix matInitialize = XMMatrixIdentity();
	matInitialize = XMMatrixScaling(0.01f, 0.01f, 0.01f);

#pragma region Terrain
	/* Terrain */
	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ArcadiaBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/ArcadiaBase01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BanneredMareBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/BanneredMareBase01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BlackSmithBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/BlackSmithBase.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BreezeBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/BreezeBase.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BridgeStone01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/BridgeStone01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CarlottaPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/CarlottaPlatform01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/CastleStairs01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CloudDistrictTerrainlod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/CloudDistrictTerrainlod.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CommonHousePlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/CommonHousePlatform01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CompanionBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/CompanionBase01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DrunkenHuntsmanBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/DrunkenHuntsmanBase.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreatHousePlatform02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/GreatHousePlatform02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreymaneBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/GreymaneBase.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GuardHouse01Arch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/GuardHouse01Arch01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HalloFdeadPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/HalloFdeadPlatform01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadMarket"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/MainRoadMarket.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadPlains01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/MainRoadPlains01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadPlains02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/MainRoadPlains02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PlainsDistrictTerrainlod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/PlainsDistrictTerrainlod.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/PondStairs01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondStairs02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/PondStairs02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallCor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/PondWallCor01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/PondWallStr01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/PondWallStr02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/PondWallStr03.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ShacksBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/ShacksBase01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ShacksBase02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/ShacksBase02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/SkyForge01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01Door01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/SkyForge01Door01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01lod"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/SkyForge01lod.FBX", matInitialize))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/StairsPlatform01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsStreamExit01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/StairsStreamExit01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/TerCloud01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/TerCloud02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/TerCloud03.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerWind01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/TerWind01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TreeCircle01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/TreeCircle01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TreeCircleWater01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/TreeCircleWater01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_UlfberhPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/UlfberhPlatform01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WaterPool01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/WaterPool01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WindDistrictTerrainLod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Terrain/WindDistrictTerrainLod.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region Building
	/* Building */
	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BreezeHome"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/BreezeHome.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Bridge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/Bridge01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Bridge02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/Bridge02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Castle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/Castle.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleBridge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/CastleBridge.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_FarmHouseWindmill"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/FarmHouseWindmill.FBX", matInitialize, CModel::TYPE_NONANIM))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GuardHouse"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/GuardHouse.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HalloftheDead"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HalloftheDead.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_House02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/House02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseBanneredmare"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseBanneredmare.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseBlackSmith"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseBlackSmith.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseGreymane"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseGreymane.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseMeadery01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseMeadery01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseMeadery01WareHouse"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseMeadery01WareHouse.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseShack01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseShack01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseShack02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseShack02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStables01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseStables01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStores01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseStores01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStores02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseStores02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseWind01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseWind02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseWind03.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/HouseWind04.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Jorvaskr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/Jorvaskr01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/MarketStand01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/MarketStand02.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/MarketStand03.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StablePlatend01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/StablePlatend01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StablePlatstr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/StablePlatstr01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableStr01end"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/StableStr01end.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableWallbackStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/StableWallbackStr01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableWallShortStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/StableWallShortStr01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Templeofk01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/Templeofk01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Trellisarch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/Trellisarch01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrellisColumn01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/TrellisColumn01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrellisComplete01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/TrellisComplete01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrelliSlattice01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/Models/Skyrim/Skyrim_WhiteRun_Building/TrelliSlattice01.FBX", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion


	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CLoader::Set_ProtoType_Object()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region SkyrimTerrain
	/* Terrain */ 
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ArcadiaBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BanneredMareBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BlackSmithBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BreezeBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BridgeStone01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CarlottaPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleStairs01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CloudDistrictTerrainlod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CommonHousePlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CompanionBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DrunkenHuntsmanBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GreatHousePlatform02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GreymaneBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GuardHouse01Arch01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HalloFdeadPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MainRoadMarket"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MainRoadPlains01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MainRoadPlains02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PlainsDistrictTerrainlod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondStairs01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondStairs02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallCor01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallStr01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallStr02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallStr03"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ShacksBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ShacksBase02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_SkyForge01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_SkyForge01Door01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_SkyForge01lod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StairsPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StairsStreamExit01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerCloud01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerCloud02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerCloud03"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerWind01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TreeCircle01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TreeCircleWater01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_UlfberhPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WaterPool01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WindDistrictTerrainLod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region Building

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BreezeHome"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Bridge01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Bridge02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Castle"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleBridge"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_FarmHouseWindmill"),
	//	CSkyrimTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GuardHouse"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HalloftheDead"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_House02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseBanneredmare"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseBlackSmith"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseGreymane"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseMeadery01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseMeadery01WareHouse"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseShack01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseShack02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStables01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStores01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStores02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind03"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind04"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Jorvaskr01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand03"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StablePlatend01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StablePlatstr01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableStr01end"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableWallbackStr01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableWallShortStr01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Templeofk01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Trellisarch01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrellisColumn01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrellisComplete01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrelliSlattice01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID eNextLevel)
{
	CLoader* pInstance = new CLoader(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Fail Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
