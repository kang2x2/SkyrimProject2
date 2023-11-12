#include "framework.h"
#include "Camera_Player.h"

#include "GameInstance.h"

#include "Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera(_pDevice, _pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs)
    : CCamera(rhs)
{
}

HRESULT CCamera_Player::Initialize_ProtoType()
{
    return S_OK;
}

HRESULT CCamera_Player::Initialize_Clone(void* pArg)
{
    FREE_PLAYERCAMERA_DESC* PlayerCameraDesc = (FREE_PLAYERCAMERA_DESC*)pArg;

    m_pPlayer = PlayerCameraDesc->m_pPlayer;
    m_fMouseSensitive = PlayerCameraDesc->fMouseSensitive;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_strName = TEXT("GamePlay_PlayerCamera");

    m_vRelativeCamPos = { 0.f, 4.f, -4.f };

    return S_OK;
}

void CCamera_Player::Tick_3st(_float4x4 _matWorld, _float4 _vTargetPos, _float _fTimeDelta)
{
    fNear = 0.2f;

    Mouse_Fix();
    Zoom(_fTimeDelta);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    /* ���콺 ������ ������ ��´�.*/
    _long mouseMoveY = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X);
    _long mouseMoveX = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y);

    /* ȸ�� ��� ����. */
    _matrix matRot = XMMatrixRotationY(rotationSpeed * mouseMoveY * _fTimeDelta);
    m_matAccumulateRotY = XMMatrixMultiply(m_matAccumulateRotY, matRot);
    matRot = XMMatrixRotationX(rotationSpeed * mouseMoveX * _fTimeDelta);
    m_matAccumulateRotX = XMMatrixMultiply(m_matAccumulateRotX, matRot);

    /* ������ ����� ȸ�� ��ĵ��� ī�޶��� world ��İ� ��� �����Ѵ�.*/
    /*  1. ī�޶� �ٶ� ������ ���� ��� ��ġ ����� ����.
        2. ������ ���� ���콺�� ���� ȸ�� ��ĵ��� ���Ѵ�.(������ x���� ����)
        3. ī�޶��� ��ġ ����� �����Ͽ� ���Ѵ�.
        ���. ī�޶��� ��ġ ��Ŀ��� ó���� ���� ��� ��ġ ��ŭ �������� ������ ����. */
    matWorld = XMMatrixTranslation(m_vRelativeCamPos.x, m_vRelativeCamPos.y, m_vRelativeCamPos.z);
    matWorld = XMMatrixMultiply(matWorld, m_matAccumulateRotX);
    matWorld = XMMatrixMultiply(matWorld, m_matAccumulateRotY);
    matWorld = XMMatrixMultiply(matWorld, XMMatrixTranslation(_vTargetPos.x, _vTargetPos.y, _vTargetPos.z));
    // XMStoreFloat4x4(&_matWorld, matWorld * XMLoadFloat4x4(&_matWorld));

    m_pTransformCom->Set_WorldMatrix(matWorld);

    _vTargetPos.y += 1.5f;

    m_pTransformCom->LookAt(XMLoadFloat4(&_vTargetPos));

    Safe_Release(pGameInstance);

    __super::Tick(_fTimeDelta);
}

void CCamera_Player::Tick_1st(_float4x4 _matWorld, _vector& _vPlayerLook, _float _fTimeDelta)
{
    fNear = 0.005f;

    Mouse_Fix();

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);
    
    /* ���콺 ������ ������ ��´�.*/
    _long mouseMoveY = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X);
    _long mouseMoveX = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y);

    _matrix matRot = XMMatrixRotationY(rotationSpeed * mouseMoveY * _fTimeDelta);
    m_matAccumulateRotY = XMMatrixMultiply(m_matAccumulateRotY, matRot);
    matRot = XMMatrixRotationX(rotationSpeed * mouseMoveX * _fTimeDelta);
    m_matAccumulateRotX = XMMatrixMultiply(m_matAccumulateRotX, matRot);

    _long	MouseMove = 0l;

    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * m_fMouseSensitive * _fTimeDelta);

    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y))
        m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * m_fMouseSensitive * _fTimeDelta);

    vEye = {_matWorld._41, _matWorld._42, _matWorld._43, 1.f};

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vEye)); // ��ġ


    Safe_Release(pGameInstance);

    __super::Tick(_fTimeDelta);
}

void CCamera_Player::LateTick(_float _fTimeDelta)
{
}

void CCamera_Player::Mouse_Fix()
{
    ShowCursor(false);

    POINT		pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    ClientToScreen(g_hWnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

void CCamera_Player::Zoom(_float _fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    _long mouseMove = 0l;
    if (mouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_WHEEL))
    {
        if (mouseMove > 0)
        {
            if (m_vRelativeCamPos.y > 1.5f)
                m_vRelativeCamPos.y -= 0.5f;
            if (m_vRelativeCamPos.z > 1.5f)
                m_vRelativeCamPos.z -= 0.5f;
        }
        else if (mouseMove < 0)
        {
            if (m_vRelativeCamPos.y < 4.f)
                m_vRelativeCamPos.y += 0.5f;
            if (m_vRelativeCamPos.z < 4.f)
                m_vRelativeCamPos.z += 0.5f;
        }
    }

    Safe_Release(pGameInstance);
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_Player* pInstance = new CCamera_Player(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_ProtoType()))
    {
        MSG_BOX("Fail Create : CCamera_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Player::Clone(void* _pArg)
{
    CCamera_Player* pInstance = new CCamera_Player(*this);

    if (FAILED(pInstance->Initialize_Clone(_pArg)))
    {
        MSG_BOX("Fail Clone : CCamera_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Player::Free()
{
    __super::Free();


}
