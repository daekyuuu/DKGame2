#include "stdafx.h"
#include "S2CriticalSection.h"

S2CriticalSection::S2CriticalSection()
{
	::InitializeCriticalSection( &m_CriticalSection );
	return;
}

S2CriticalSection::~S2CriticalSection()
{
	::DeleteCriticalSection( &m_CriticalSection );
	return; 
}

void S2CriticalSection::Enter(void)
{
	::EnterCriticalSection( &m_CriticalSection );
	return; 
}

void S2CriticalSection::Leave(void)
{
	::LeaveCriticalSection( &m_CriticalSection );
	return; 
}
