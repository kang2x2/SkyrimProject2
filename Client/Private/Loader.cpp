#include "framework.h"
#include "Loader.h"

#include "GameInstance.h"

// Logo Level
#include "BackGround.h"

// GamePlay Level
#include "Free_Camera.h"

#include "Terrain.h"

#include "Player.h"
#include "Player_Body.h"

#include "Weapon_IronSword.h"

// Tool Level
#include "Tool_Camera.h"

#include "Terrain_Grid.h"

#include "SkyrimTerrain.h"
#include "Building.h"
#include "StoneWork.h"

CLoader::CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint __stdcall ThreadEntry(void* pArg)
{
	/* DX가 사용하는 모든 컴 객체를 초기화한다.  */
	CoInitializeEx(nullptr, 0);

	/* 새롭게 생성된 스레드가 일한다. */
	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID _eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = _eNextLevel;

	// 새로운 스레드 생성
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
	case LEVEL_ZERO:
		hr = Loading_For_Level_Zero();
		break;
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

HRESULT CLoader::Loading_For_Level_Zero()
{
	//m_strLoadingText = TEXT("Loading Mesh.");
	//
	//Set_ProtoType_Mesh(LEVEL_ZERO);

	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	return S_OK;
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

	_matrix matInitialize = XMMatrixIdentity();

	//if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player_Body"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/BreezeHome", matInitialize, CModel::TYPE_ANIM))))
	//	return E_FAIL;

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

	/* 로딩 끝 */
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

	/* GameObject(원본) */
	m_strLoadingText = TEXT("Loading ProtoType_GameObject.");
	// BackGround_ProtoType
	if (FAILED(pGameInstanc->Add_ProtoObject(TEXT("ProtoType_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* 로딩 끝 */
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
	// ../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Player/Player_NonEquip_Stand
	// XMMatrixScaling(0.01f, 0.01f, 0.01f) * 
	//  * XMMatrixRotationY(XMConvertToRadians(180.0f))
	// 
	//if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player_Body"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Player/Player_1Hand_Stand", matInitialize, CModel::TYPE_ANIM))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player_Body"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Player/Player_1Hand_Stand", matInitialize, CModel::TYPE_ANIM))))
		return E_FAIL;

	/* Weapon */
	//if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Weapon_IronSword"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Weapon/Iron_LongSword/Iron_LongSword", matInitialize, CModel::TYPE_NONANIM))))
	//	return E_FAIL;
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Weapon_IronSword"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Weapon/Iron_LongSword", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;


	//matInitialize = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Player/Player", matInitialize, CModel::TYPE_ANIM))))
	//	return E_FAIL;


	Set_ProtoType_Mesh(LEVEL_GAMEPLAY);

#pragma endregion

#pragma region Shader
	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");
	
	/* Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	/* Shader_VtxNonAnimMesh */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNonAnimMesh.hlsl"), VTX_NONANIMMESH::Elements, VTX_NONANIMMESH::iNumElements))))
		return E_FAIL;
	/* Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxAnimMesh.hlsl"), VTX_ANIMMESH::Elements, VTX_ANIMMESH::iNumElements))))
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
	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	// Player 
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player_Body"),
		CPlayer_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Weapon
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Weapon_IronSword"),
		CWeapon_IronSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Set_ProtoType_Object();

#pragma endregion

	Safe_Release(pGameInstance);

	/* 로딩 끝 */
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

#pragma region SkyrimTerrain
	/* Terrain */
 	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ArcadiaBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/ArcadiaBase01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BanneredMareBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BanneredMareBase01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BlackSmithBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BlackSmithBase", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BreezeBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BreezeBase", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BridgeStone01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BridgeStone01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CarlottaPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CarlottaPlatform01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CastleStairs01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CloudDistrictTerrainlod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CloudDistrictTerrainlod", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CommonHousePlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CommonHousePlatform01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CompanionBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CompanionBase01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DrunkenHuntsmanBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/DrunkenHuntsmanBase", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DryGoodsBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/DryGoodsBase", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreatHousePlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GreatHousePlatform01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreatHousePlatform02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GreatHousePlatform02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreymaneBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GreymaneBase", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GuardHouse01Arch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GuardHouse01Arch01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HalloFdeadPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/HalloFdeadPlatform01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadMarket"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/MainRoadMarket", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadPlains01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/MainRoadPlains01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadPlains02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/MainRoadPlains02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PlainsDistrictTerrainlod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PlainsDistrictTerrainlod", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondStairs01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondStairs02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondStairs02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallCor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallCor01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallStr01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallStr02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallStr03", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ShacksBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/ShacksBase01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ShacksBase02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/ShacksBase02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/SkyForge01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01Door01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/SkyForge01Door01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01lod"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/SkyForge01lod", matInitialize))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/StairsPlatform01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsStreamExit01"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/StairsStreamExit01", matInitialize, CModel::TYPE_NONANIM))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerCloud01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerCloud02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerCloud03", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerWind01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerWind01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TreeCircle01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TreeCircle01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TreeCircleWater01"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TreeCircleWater01", matInitialize, CModel::TYPE_NONANIM))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_UlfberhPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/UlfberhPlatform01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WaterPool01"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/WaterPool01", matInitialize, CModel::TYPE_NONANIM))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WindDistrictTerrainLod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/WindDistrictTerrainLod", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region Building
	/* Building */
	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BreezeHome"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/BreezeHome", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Bridge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Bridge01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Bridge02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Bridge02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Castle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Castle", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleBridge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleBridge", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_FarmHouseWindmill"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/FarmHouseWindmill", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GuardHouse"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/GuardHouse", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HalloftheDead"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HalloftheDead", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_House02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/House02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseBanneredmare"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseBanneredmare", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseBlackSmith"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseBlackSmith", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseGreymane"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseGreymane", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseMeadery01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseMeadery01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseMeadery01WareHouse"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseMeadery01WareHouse", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseShack01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseShack01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseShack02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseShack02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStables01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseStables01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStores01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseStores01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStores02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseStores02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind03", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind04", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Jorvaskr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Jorvaskr01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/MarketStand01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/MarketStand02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/MarketStand03", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StablePlatend01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StablePlatend01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StablePlatstr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StablePlatstr01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableStr01end"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StableStr01end", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableWallbackStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StableWallbackStr01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableWallShortStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StableWallShortStr01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Templeofk01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Templeofk01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Trellisarch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Trellisarch01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrellisColumn01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/TrellisColumn01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrellisComplete01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/TrellisComplete01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrelliSlattice01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/TrelliSlattice01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region StoneWork

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsWater01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/StairsWater01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCap01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCap01Nohighcol"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCap01Nohighcol", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve30up128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve30up128", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve30up128a"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve30up128a", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve45"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve45", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve45up128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve45up128", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve60"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve60", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallGateDrawBridge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallGateDrawBridge01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallGatetarTrap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallGatetarTrap01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallMainGate01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallMainGate01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallMainGate01ext"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallMainGate01ext", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallMainGateHouse01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallMainGateHouse01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile03", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile04", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile05", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRuinsFree01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRuinsFree01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRuinsFree01Entance01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRuinsFree01Entance01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01nohighcoll"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01nohighcoll", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01stockades01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01stockades01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01stockades02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01stockades02", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01stockades03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01stockades03", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrrubbled01free"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrrubbled01free", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrrubbled01short"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrrubbled01short", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrrubbled02free"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrrubbled02free", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrup128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrup128", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrup128shift128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrup128shift128", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrup128shift128nohighcol"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrup128shift128nohighcol", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTierdivide01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTierdivide01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTotem01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTotem01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTower01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTower01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTower01collpiece"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTower01collpiece", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowercap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowercap01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowerChunk01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowerChunk01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowerFree01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowerFree01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowerRubbled01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowerRubbled01", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallWaterGrate01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallWaterGrate01", matInitialize, CModel::TYPE_NONANIM))))
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

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DryGoodsBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GreatHousePlatform01"),
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
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Bridge01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Bridge02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Castle"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleBridge"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_FarmHouseWindmill"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GuardHouse"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HalloftheDead"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_House02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseBanneredmare"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseBlackSmith"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseGreymane"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseMeadery01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseMeadery01WareHouse"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseShack01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseShack02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStables01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStores01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStores02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind03"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind04"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Jorvaskr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand03"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StablePlatend01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StablePlatstr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableStr01end"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableWallbackStr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableWallShortStr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Templeofk01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Trellisarch01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrellisColumn01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrellisComplete01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrelliSlattice01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region StoneWork

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StairsWater01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCap01Nohighcol"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve30up128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve30up128a"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve45"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve45up128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve60"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallGateDrawBridge01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallGatetarTrap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallMainGate01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallMainGate01ext"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallMainGateHouse01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile05"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRuinsFree01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRuinsFree01Entance01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01nohighcoll"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01stockades01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01stockades02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01stockades03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrrubbled01free"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrrubbled01short"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrrubbled02free"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrup128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrup128shift128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrup128shift128nohighcol"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTierdivide01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTotem01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTower01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTower01collpiece"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowercap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowerChunk01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowerFree01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowerRubbled01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallWaterGrate01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
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
