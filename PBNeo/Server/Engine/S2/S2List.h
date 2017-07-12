#ifndef __S2_LIST_H__
#define __S2_LIST_H__

typedef INT32 (*COMPAREPROC)( void * p1, void *p2);

#define			DEFAULT_STATIC_BUFFER_SIZE		8

class S2_EXPORT_BASE S2List 
{
	struct QSTACK 
	{
		INT32 left;
		INT32 right;
	};

private:
	QSTACK *				m_pStackBuffer;
	INT32					m_nStackDepth;
	INT32					m_nStackTop;

protected :

	INT32					m_i32MaxCount;
	INT32					m_i32Count;
	INT32					m_i32AllocUnit;
	BOOL					m_bOptimizeWhenClear;

public:
	void**					Items;

private:
	
	void*					_defaultBuf[ DEFAULT_STATIC_BUFFER_SIZE ];
	BOOL					m_bIsUseMalloc; 

	inline	void			__Free();

protected:

	BOOL ResizeBuffer( INT32 Count);

	// sort by komet
	INT32					_Partition( INT32 si, INT32 ei, COMPAREPROC pProc);
	void					_RecursiveQuickSort( INT32 si, INT32 ei, COMPAREPROC pProc);
	void					_InsertionSort( INT32 si, INT32 ei, COMPAREPROC pProc);

	void					_Qpush( INT32 left, INT32 right);
	void					_Qpop( INT32* left, INT32* right);
	void					_NonRecursiveQuickSort( INT32 si, INT32 ei, COMPAREPROC pProc);

	virtual INT32			_IndexOf( const void * p) const;

public:

	S2List( INT32 i32AllocUnit = 8);
	virtual ~S2List(void);

	virtual INT32			Add( void * p);															// 일반적인 Add
	virtual INT32			AddOnce( void * p);														// 하나의 값은 한번만 Add 할때 사용
	virtual INT32			Insert( INT32 Idx, void * p);
	virtual INT32			Delete( INT32 Idx);
	virtual INT32			Remove( void * p);

	virtual void			Clear( BOOL FreeItem = FALSE);

	void					SetOptmizeEnableWhenClear( BOOL bFlag)				{	m_bOptimizeWhenClear = bFlag;	}
	void					SetAllocationUnit( INT32 AllocUnit);
	
	INT32					GetCount(void) const			{ return m_i32Count; }
	void					SetCount( INT32 Count);
	void*					GetItem( INT32 idx)				{ return Items[ idx]; }
	void					SetItem( INT32 idx, void * p);

	const void *	GetItem( INT32 idx) const				{ return Items[ idx]; }

	inline	const void *	GetSafeItem( INT32 idx) const
	{
		if ( idx >= 0 && idx < m_i32Count )
			return Items[ idx];
		return NULL;
	}
	inline	void *	GetSafeItem( INT32 idx)
	{
		return const_cast<void *>(static_cast<const S2List &>(*this).GetSafeItem(idx));
	}

	INT32					FindItem( void * p) const		{ return _IndexOf( p); }

	void *					GetFirst(void)					{ return Items[ 0]; }
	void *					GetLast(void)					{ return Items[ m_i32Count-1]; }

	void					Sort( COMPAREPROC pProc);


	void					Copy( const S2List *List);
	void					AddFromList( S2List * pSrcList)
	{
		INT32 i;
		for( i = 0; i  < pSrcList->GetCount(); i++)
		{
			Add( pSrcList->Items[i]);
		}
	}

	void					OptimizeMemory(void);

};

#endif
