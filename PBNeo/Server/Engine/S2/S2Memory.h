#ifndef __S2_MEMORY_H__
#define __S2_MEMORY_H__

#ifdef UNICODE

#define WIDEN2(x)		L ## x
#define WIDEN(x)		WIDEN2(x)
#define __TFILE__		WIDEN(__FILE__)

#else

#define __TFILE__		__FILE__

#endif

namespace S2Memory
{
	S2_EXPORT_BASE void		Create(void);
	S2_EXPORT_BASE void		Destroy(void);

	S2_EXPORT_BASE void*	Alloc(INT32 nBytes, const TCHAR * strFileName = NULL, INT32 i32Line = 0);
	S2_EXPORT_BASE void*	Realloc(void * pPointer, INT32 nBytes, const TCHAR * strFileName = NULL, INT32 i32Line = 0);
	S2_EXPORT_BASE void		Free(void * pPointer, const TCHAR * strFileName = NULL, INT32 i32Line = 0);
	//EXPORT_BASE void*	New(void * pPointer, const TCHAR * strFileName = NULL, INT32 i32Line = 0);
		
	inline void				Copy( void * pDest, void * pSrc, UINT32 sz)		{ memcpy( pDest, pSrc, sz);	}
	inline void				FillZero( void * pDest, UINT32 sz)				{ memset( pDest, 0,	sz);	}
	inline void				Fill(void * pDest, INT32 Val, UINT32 sz)		{ memset( pDest, Val, sz);	}
	void					Dump(void);
}

#define S2_ALLOC(size)					S2Memory::Alloc(size, __TFILE__, __LINE__); 
//#define S2_ALLOC_ZERO(size)				{ void* pPoint = S2Memory::Alloc(size, __FILE__, __LINE__);	S2Memory::FillZero(pPoint,size); }
#define S2_REALLOC(P, S)				S2Memory::Realloc(P, S, __TFILE__, __LINE__); 
#define S2_FILLZERO(P, S)				S2Memory::FillZero(P,S); 
#define	S2_SAFE_FREE(P)					if(P!=NULL){S2Memory::Free(P); P=NULL; }
//#define S2_NEW(P)						S2Memory::New(size, __FILE__, __LINE__); 

#define SAFE_DELETE(p)			{ if(p)	{	delete p;	p=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p)	{	delete[] p; p=NULL; } }

#endif
