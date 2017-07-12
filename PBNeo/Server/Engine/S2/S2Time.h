#ifndef __S2_TIME_H__
#define __S2_TIME_H__

#include "S2Thread.h"

class S2ThreadTimer
{
	S2Thread				m_Thread;

	DATE32					m_dt32Local;			// 썸머타임 적용되는 로컬시(변환후)
	DATE32					m_dt32Stand;			// 썸머타임 무시하는 표준시(변환후)

public:
	S2ThreadTimer();
	~S2ThreadTimer();

	BOOL					Create();
	void					Destroy();

	void					OnUpdate();

	DATE32					GetLocalTime()						{	return m_dt32Local;			}
	DATE32					GetStandTime()						{	return m_dt32Stand;			}
};

namespace S2Time
{
	S2_EXPORT_BASE BOOL				Init(void);
	S2_EXPORT_BASE void				Destroy(void);
	S2_EXPORT_BASE inline DATE32	LocalTime(void);
	S2_EXPORT_BASE inline DATE32	StandTime(void);
};

#endif
