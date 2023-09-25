#include "Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager);

CTimer_Manager::CTimer_Manager()
{
}

_float CTimer_Manager::Compute_TimeDelta(const wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	/* Ÿ�ӵ�Ÿ�� ����ϰ� �����Ѵ�. */
	return pTimer->Compute_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const wstring& strTimerTag)
{
	/* map�� �ߺ��� Ű�� ������� �ʴ���.*/
	CTimer* pTimer = Find_Timer(strTimerTag);

	/* �߰��ϰ����ϴ� strTimerTag�� �ش��ϴ� Pair�����Ͱ� �̹� �߰��Ǿ��־���. */
	if (nullptr != pTimer)
		return E_FAIL;

	m_mapTimer.insert({ strTimerTag, CTimer::Create() });

	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const wstring& strTimerTag) const
{
	/* �ʿ��� �������ִ� Find�Լ��� ����Ž���� �����Ѵ�. */
	auto	iter = m_mapTimer.find(strTimerTag);

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free(void)
{
	__super::Free();

	for (auto& Pair : m_mapTimer)
	{
		Safe_Release(Pair.second);
	}
	m_mapTimer.clear();
}