#ifndef __S2_CRITICALSECTION_H__
#define	__S2_CRITICALSECTION_H__

class S2_EXPORT_BASE S2CriticalSection
{	
protected:
	CRITICAL_SECTION		m_CriticalSection; 

public: 
	S2CriticalSection(); 
	virtual ~S2CriticalSection();

	void					Enter(void); 	
	void					Leave(void); 
}; 

#endif