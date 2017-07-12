#ifndef  __S2_LIST_BIN_H__
#define __S2_LIST_BIN_H__

#include "S2List.h"

class S2_EXPORT_BASE S2ListBin : public S2List 
{

protected:
	COMPAREPROC				m_pCompProc;

public: //protected:
	INT32 GetAddPosition( void * p) const;		// ���� �ܺο����� ������ġ�� �˼��ֵ��� �����մϴ�...

public:
	S2ListBin( INT32 AllocUnit = 16) ;
	virtual ~S2ListBin(void);

	void SetCompareProc( COMPAREPROC pUserProc)						{ m_pCompProc = pUserProc; }
	COMPAREPROC GetCompareProc(void)								{ return m_pCompProc; }

	virtual INT32 Add( void * p);
	virtual INT32 FindItem( void * p);
};

#endif
