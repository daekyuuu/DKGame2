#include "stdafx.h"
#include "S2List.h"
#include "S2Memory.h"

#define		NEARESTMULTIPLE( val, unit)			(((val + unit - 1) / unit) * unit)

S2List::S2List( INT32 i32AllocUnit )
{
	S2ASSERT(i32AllocUnit > 0);

	m_i32MaxCount			= 0;
	m_i32Count				= 0;
	m_i32AllocUnit			= i32AllocUnit;
	m_bOptimizeWhenClear	= TRUE;
	Items					= NULL;

	S2Memory::FillZero(_defaultBuf, sizeof(_defaultBuf));
	m_bIsUseMalloc = FALSE; 

	m_pStackBuffer		= NULL;
	m_nStackDepth		= 0;
	m_nStackTop			= -1;
}

S2List::~S2List(void)
{
	__Free();
}

inline	void	S2List::__Free()
{
	if( m_bIsUseMalloc )S2_SAFE_FREE(Items);
	S2_SAFE_FREE( m_pStackBuffer);
	m_nStackDepth		= 0;
	m_nStackTop			= -1;
}

BOOL S2List::ResizeBuffer( INT32 nCount )
{
	INT32 NewCount = NEARESTMULTIPLE( nCount, m_i32AllocUnit );

	// 새로운 할당사이즈가 멤버 배열로 커버할 수 있다면
	// 멤버 배열로 생성한다.
	if( m_i32MaxCount != NewCount )
	{
		if (m_bIsUseMalloc)
		{
			Items = (void **) S2Memory::Realloc( Items,  NewCount * sizeof(void *) );
			if(NULL == Items)
			{	
				// I3Log 남길것				
			#ifdef UNLOCK_LOG
				I3BCLOG(LOCATION, "[ERROR]ResizeBuffer Fail(%d)[m_i32MaxCount(%d) != NewCount(%d)] ", nCount, m_i32MaxCount, NewCount);				
			#endif
				return FALSE;
			}
			m_i32MaxCount = NewCount;
		}
		else
		{
			Items = _defaultBuf;
			m_i32MaxCount = DEFAULT_STATIC_BUFFER_SIZE;

			if ( nCount >= DEFAULT_STATIC_BUFFER_SIZE)
			{
				Items = (void **) S2_ALLOC( (NewCount * sizeof(void *)));
				S2Memory::Copy(Items, _defaultBuf, sizeof(_defaultBuf));
				m_bIsUseMalloc = TRUE;
				m_i32MaxCount = NewCount;
			}
		}
	}

	return TRUE;
}

void S2List::SetAllocationUnit( INT32 AllocUnit)
{
	INT32 NewCount;

	S2ASSERT( AllocUnit > 0);

	m_i32AllocUnit = AllocUnit;

	if( m_i32MaxCount > 0)
	{
		NewCount = NEARESTMULTIPLE( m_i32MaxCount, m_i32AllocUnit);

		ResizeBuffer( NewCount);
	}
}

void S2List::OptimizeMemory(void)
{
	ResizeBuffer( m_i32Count );
}

void S2List::SetCount( INT32 NewCount )
{
	if( NewCount+1 >= m_i32MaxCount) 
	{
		ResizeBuffer( NewCount + 1);
	}

	m_i32Count = NewCount;
}

void S2List::SetItem( INT32 idx, void * p)
{
	if( idx >= m_i32MaxCount)
	{
		ResizeBuffer( idx + 1);
	}

	Items[idx] = p;
}

INT32 S2List::_IndexOf( const void * p ) const
{
	INT32 i;
	
	for( i = 0; i < m_i32Count; i++) 
	{
		if( Items[i] != p ) continue;
		
		return i;
	}

	return -1;
}

INT32 S2List::Add( void * p)
{
	if( m_i32Count+1 >= m_i32MaxCount )
	{
		if( FALSE == ResizeBuffer( m_i32Count + 1) ) 
		{
			//I3TRACE( "S2List::Add() ResizeBuffer() failed\n");
			return -1;
		}
	}

	Items[m_i32Count] = p;
	m_i32Count++;

	return m_i32Count-1;
}

INT32 S2List::AddOnce( void* p )
{
	INT32 idx;

	idx = _IndexOf( p);
	if( idx != -1)
	{
		return idx;
	}

	return Add( p );
}

INT32 S2List::Insert( INT32 idx, void * p)
{
	BOOL Rv;
	size_t MoveSize;

	if(m_i32Count+1 >= m_i32MaxCount)
	{
		Rv = ResizeBuffer( m_i32Count + 1);
		if( Rv == FALSE)
		{
			return -1;
		}
	}

	MoveSize = ( m_i32Count - idx) * sizeof(void *);
	if( MoveSize > 0)
	{
		memmove( &(Items[idx+1]), &(Items[idx]), MoveSize);
	}

	Items[idx] = p;
	m_i32Count++;

	return idx;
}

INT32 S2List::Delete( INT32 Idx)
{
	INT32 MoveLength;

	if( m_i32Count == 0)
	{
		return -1;
	}

	MoveLength = m_i32Count - Idx - 1;
	if( MoveLength > 0)
	{
		S2Memory::Copy( &Items[Idx], &Items[Idx+1], MoveLength * sizeof(void *));
	}
	m_i32Count--;

	return m_i32Count;
}

INT32 S2List::Remove( void * p)
{
	INT32 Idx;

	Idx = _IndexOf( p);
	if( Idx == -1)
	{
		return -1;
	}

	Delete( Idx);

	return Idx;
}

void S2List::Clear( BOOL bFreeItem)
{
	if( bFreeItem == FALSE)
	{
		if( m_bOptimizeWhenClear == TRUE)
		{
			ResizeBuffer( m_i32AllocUnit);
		}

		m_i32Count = 0;
	}
	else
	{
		m_i32MaxCount = m_i32Count = 0;
		__Free();
	}
}

void S2List::_InsertionSort( INT32 si, INT32 ei, COMPAREPROC pProc)
{
	INT32 i, j;
	for( i=si+1; i<=ei; i++)
	{
		const void* temp = Items[ i];
		j = i;

		while( --j >= 0 && pProc( Items[j], (void*) temp) > 0) 
		{
			Items[j+1] = Items[j];
		}

		Items[j+1] = (void*) temp;
	}
}

INT32 S2List::_Partition( INT32 si, INT32 ei, COMPAREPROC pProc)
{	//	위키피아에 있는 퀵정렬 알고리즘을 사용함.
	//	http://ko.wikipedia.org/wiki/%ED%80%B5_%EC%A0%95%EB%A0%AC#.EC.86.8C.EC.8A.A4.EC.BD.94.EB.93.9C
	INT32 pivot = si, left = si, right = ei;
	//INT32 median = si + (abs(ei - si) / 2);		//	중간 위치

	const void * pPivotItem = Items[ si];	//	주의! const로 지정해야 밑에서 얘기치 않게 포인터 주소가 바뀌는 상황을 방지함.

	while( left < right)
	{
		while( (pProc( Items[ right], (void*) pPivotItem) >= 0) && (left < right))
		{
			right--;
		}

		if( left != right)
		{
			Items[ left] = Items[ right];

			left++;
		}

		while( (pProc( Items[ left], (void*) pPivotItem) <= 0) && (left < right))
		{
			left++;
		}

		if( left != right)
		{
			Items[ right] = Items[ left];

			right--;
		}
	}

	Items[ left] = (void*) pPivotItem;

	pivot = left;

	return pivot;
}

/*--------------------------------------------------
Recursive Quick sort (by komet)

장점:
정렬할 데이터가 이미 준비되어 있고 모든 데이터를 정렬해야 할 경우 가장 빠른 수행속도를 보여준다.

단점:
과다로 재귀호출시 스택 오버플로 나므로 사용시 주의 요함.
축(Pivot)값이 같은 것끼리는 순서관계가 파괴된다. (중요한 데이터의 경우 퀵정렬을 안쓰는 것이 좋다?)
안정성이 없다.
--------------------------------------------------*/
void S2List::_RecursiveQuickSort( INT32 si, INT32 ei, COMPAREPROC pProc)
{
	INT32 pivot = _Partition( si, ei, pProc);

	if( si < pivot)
		_RecursiveQuickSort( si, pivot - 1, pProc);

	if( ei > pivot)
		_RecursiveQuickSort( pivot + 1, ei, pProc);
}

/*--------------------------------------------------
Non Recursive Quick sort (by komet)
--------------------------------------------------*/

const INT32 g_defStackDepth = 256;  // 스택 크기 필요시 더 늘리면 됩니다.

void S2List::_Qpush( INT32 left, INT32 right)
{
	m_nStackTop++;
	S2ASSERT( m_nStackTop >= 0 && m_nStackTop < m_nStackDepth);

	m_pStackBuffer[ m_nStackTop].left = left;
	m_pStackBuffer[ m_nStackTop].right = right;
}

void S2List::_Qpop( INT32* left, INT32* right)
{
	S2ASSERT( m_nStackTop >= 0 && m_nStackTop < m_nStackDepth);

	*left = m_pStackBuffer[ m_nStackTop].left;
	*right = m_pStackBuffer[ m_nStackTop].right;
	m_nStackTop--;
}

void S2List::_NonRecursiveQuickSort( INT32 si, INT32 ei, COMPAREPROC pProc)
{
	if( m_nStackDepth < m_i32Count)	S2_SAFE_FREE( m_pStackBuffer);

	//	퀵소트용 스택 버퍼 할당. 
	//	S2List에서 한번이라도 Sort를 하면 스택 버퍼는 한번만 생성되며 Sort가 끝난후에도 스택 버퍼는 남아있습니다.
	//	하지만 나중에 S2List 해제시 스택 버퍼는 해제됩니다.
	if( m_pStackBuffer == NULL)
	{	
		INT32 depth = g_defStackDepth;

		m_pStackBuffer = (QSTACK*) S2_ALLOC( sizeof( QSTACK) * depth);
		m_nStackDepth = depth;
	}

	S2Memory::FillZero( m_pStackBuffer, sizeof( QSTACK) * m_nStackDepth);

	INT32 i,j,s = si,e = ei;

	m_nStackTop = -1;

	_Qpush( s, e);

	do 
	{
		_Qpop( &s, &e);

		do 
		{
			i = s;
			j = e;
			const void * pPivotItem = Items[ (s+e)/2];

			do
			{
				while( pProc( Items[i], (void*) pPivotItem) < 0)	i++;
				while( pProc( Items[j], (void*) pPivotItem) > 0)	j--;

				if( i <= j)
				{
					const void * temp = Items[i];
					Items[i] = Items[j];
					Items[j] = (void*) temp;
					i++;
					j--;
				}

			} while( i <= j);

			if( i<e)
			{
				_Qpush( i, e);
			}

			e = j;

		} while ( s < e);

	} while ( m_nStackTop != -1);
}

/*--------------------------------------------------
아이템 정렬 퍼포먼스 테스트

조건:
Intel Core2 Quad 2.4GHz
S2List 내에 INT32 정수 30000개 정렬을 3번 체크
GetTickCount() 함수로 시간 체크함.

결과:
Linear sort : (1) 22047 ms (2) 21562 ms (3) 21563 ms
Insertion sort : (1) 9187 ms (2) 9422 ms (3) 9250 ms
Recursive Quick sort  : (1) 16 ms  (2) 16 ms  (3) 15 ms
Non Recursive Quick sort : (1) 31 ms  (2) 31 ms  (3) 16 ms
--------------------------------------------------*/

#define CUTOFF (10)		// 퀵정렬 최소 갯수

void S2List::Sort( COMPAREPROC pProc)
{
	S2ASSERT( pProc != NULL);

	if( m_i32Count <= 1)	return;

	if( CUTOFF < m_i32Count)
	{
		_NonRecursiveQuickSort( 0, m_i32Count - 1, pProc);
	}
	else	// 작은 배열에서는 삽입정렬을 사용함으로서 효율을 증대시킨다.
	{
		_InsertionSort( 0, m_i32Count - 1, pProc);
	}
}

void S2List::Copy( const S2List *List)
{
	if( List->GetCount() <= 0)
	{
		return;
	}	
	m_i32Count = List->GetCount();
	ResizeBuffer( m_i32Count);
	S2Memory::Copy( Items, List->Items, sizeof(void *) * m_i32Count);
}
