#include "stdafx.h"
#include "S2Memory.h"

UINT32 s_ui32MemorySize;

void S2Memory::Create(void)
{
	s_ui32MemorySize = 0;
	return; 
}

void S2Memory::Destroy(void)
{
	return; 
}

void * S2Memory::Alloc(INT32 nBytes, const TCHAR * strFileName ,INT32 i32Line )
{
	//strFileName;	i32Line;
	//void * pRv = malloc( nBytes ); 
	//s_ui32MemorySize += nBytes;
	//return pRv; 

	return NULL;
}

void * S2Memory::Realloc(void * pPointer, INT32 nBytes, const TCHAR * strFileName, INT32 i32Line )
{
	//strFileName;	i32Line;
	//void * pRv = realloc( pPointer, nBytes );
	//return pRv; 
	return NULL;
}

void S2Memory::Free(void * pPointer, const TCHAR * strFileName, INT32 i32Line )
{
	//strFileName;	i32Line;
	//free( pPointer ); 	
	return; 
}
