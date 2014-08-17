///////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

///////////////////////////////////////////////////////////////////////////////////

#include "macro.h"

///////////////////////////////////////////////////////////////////////////////////

#include "CMemory.h"

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	/*
	void *
	PASCAL
	operator new
	(
		size_t size
	)
	throw()
	{
		void *p = NULL;

		__try
		{
			DEBUG_BREAK();
			p = ::CoTaskMemAlloc( size );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			DEBUG_BREAK();
		}

		return p;
	}

	///////////////////////////////////////////////////////////////////////////////

	void
	PASCAL
	operator delete
	(
		void *p
	)
	throw()
	{
		__try
		{
			DEBUG_BREAK();
			::CoTaskMemFree( p );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			DEBUG_BREAK();
		}
	}
	*/
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeAllocateMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes
	)
	throw()
	{
		HRESULT hr = S_OK;

		if( IS_NOT_NULL( lpv ) )
		{
			hr = E_POINTER;
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			__try
			{
				lpv = ::CoTaskMemAlloc( NumberofBytes );

				if( IS_NULL( lpv ) )
				{
					hr = E_OUTOFMEMORY;
					ASSERT_SUCCEEDED( hr );
				}
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				hr = E_ABORT;
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeAllocateAlignedMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes,
		size_t Alignment
	)
	throw()
	{
		HRESULT hr = S_OK;

		if( IS_NOT_NULL( lpv ) )
		{
			hr = E_POINTER;
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			__try
			{
				lpv = _aligned_malloc( NumberofBytes, Alignment );

				if( IS_NULL( lpv ) )
				{
					hr = E_OUTOFMEMORY;
					ASSERT_SUCCEEDED( hr );
				}
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				hr = E_ABORT;
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeReallocateMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes
	)
	throw()
	{
		HRESULT hr = S_OK;

		__try
		{
			lpv = ::CoTaskMemRealloc( lpv, NumberofBytes );

			if( IS_NULL( lpv ) && ( 0 != NumberofBytes ) )
			{
				hr = E_OUTOFMEMORY;
				ASSERT_SUCCEEDED( hr );
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			hr = E_ABORT;
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeReallocateAlignedMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes,
		size_t Alignment
	)
	throw()
	{
		HRESULT hr = S_OK;

		__try
		{
			lpv = _aligned_realloc( lpv, NumberofBytes, Alignment );

			if( IS_NULL( lpv ) && ( 0 != NumberofBytes ) )
			{
				hr = E_OUTOFMEMORY;
				ASSERT_SUCCEEDED( hr );
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			hr = E_ABORT;
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeZeroMemory
	(
		LPVOID lpv,
		size_t NumberofBytes
	)
	throw()
	{
		HRESULT hr = S_OK;

		if( 0 == NumberofBytes )
		{
			hr = S_FALSE;
		}
		else
		{
			__try
			{
				RtlZeroMemory( lpv, NumberofBytes );
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				hr = E_ABORT;
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeCopyMemory
	(
		LPVOID lpvDst,
		LPCVOID lpcvSrc,
		size_t NumberofBytes
	)
	throw()
	{
		HRESULT hr = S_OK;

		if( 0 == NumberofBytes )
		{
			hr = S_FALSE;
		}
		else
		{
			__try
			{
				RtlCopyMemory( lpvDst, lpcvSrc, NumberofBytes );
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				hr = E_ABORT;
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeFreeMemory
	(
		LPVOID &lpv
	)
	throw()
	{
		HRESULT hr = S_OK;

		if( IS_NULL( lpv ) )
		{
			hr = S_FALSE;
		}
		else
		{
			__try
			{
				::CoTaskMemFree( lpv );

				lpv = NULL;
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				hr = E_ABORT;
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeFreeAlignedMemory
	(
		LPVOID &lpv
	)
	throw()
	{
		HRESULT hr = S_OK;

		if( IS_NULL( lpv ) )
		{
			hr = S_FALSE;
		}
		else
		{
			__try
			{
				_aligned_free( lpv );

				lpv = NULL;
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				hr = E_ABORT;
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//virtual
	STDMETHODIMP
	CMemoryAllocator::AllocateMemory
	(
		LPVOID &reflpv,
		size_t NumberofBytes
	)
	{
		return safeAllocateMemory( reflpv, NumberofBytes );
	}

	///////////////////////////////////////////////////////////////////////////////

	//virtual
	STDMETHODIMP
	CMemoryAllocator::ReallocateMemory
	(
		LPVOID &reflpv,
		size_t NumberofBytes
	)
	{
		return safeReallocateMemory( reflpv, NumberofBytes );
	}

	///////////////////////////////////////////////////////////////////////////////

	//virtual
	STDMETHODIMP
	CMemoryAllocator::FreeMemory
	(
		LPVOID &reflpv
	)
	{
		return safeFreeMemory( reflpv );
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	CMemoryHandler::CMemoryHandler()
	:
		m_lpv( NULL ),
		m_NumberofBytes( 0 )
	{}

	///////////////////////////////////////////////////////////////////////////////

	CMemoryHandler::CMemoryHandler
	(
		const
		CMemoryHandler &cref
	)
	:
		m_lpv( NULL ),
		m_NumberofBytes( 0 )
	{
		if( cref.IsValid() )
		{
			HRESULT hr = Allocate( cref.m_NumberofBytes );
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				hr = CopyFrom( cref.m_lpv, cref.m_NumberofBytes );
				ASSERT_SUCCEEDED( hr );

				if( FAILED( hr ) )
				{
					HRESULT hr2 = Free();
					ASSERT_SUCCEEDED( hr2 );
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	const
	CMemoryHandler &
	CMemoryHandler::operator =
	(
		const
		CMemoryHandler &cref
	)
	{
		if( this != &cref )
		{
			HRESULT hr = Reallocate( cref.m_NumberofBytes );
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				hr = CopyFrom( cref.m_lpv, cref.m_NumberofBytes );
				ASSERT_SUCCEEDED( hr );

				if( FAILED( hr ) )
				{
					HRESULT hr2 = Free();
					ASSERT_SUCCEEDED( hr2 );
				}
			}
		}

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CMemoryHandler::Allocate
	(
		SIZE_T NumberofBytes,
		bool bZero
	)
	{
		HRESULT hr = S_OK;

		hr = ActualAllocateMemory( m_lpv, NumberofBytes );
		ASSERT_SUCCEEDED( hr );

		if( SUCCEEDED( hr ) )
		{
			m_NumberofBytes = NumberofBytes;

			if( bZero )
			{
				hr = Zero();
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CMemoryHandler::Reallocate
	(
		SIZE_T NumberofBytes,
		bool bZero
	)
	{
		HRESULT hr = S_OK;

		hr = ActualReallocateMemory( m_lpv, NumberofBytes );
		ASSERT_SUCCEEDED( hr );

		if( SUCCEEDED( hr ) )
		{
			m_NumberofBytes = NumberofBytes;

			if( bZero )
			{
				hr = Zero();
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	bool
	CMemoryHandler::IsValid()
	const
	{
		return IS_NOT_NULL( m_lpv );
	}

	///////////////////////////////////////////////////////////////////////////////

	bool
	CMemoryHandler::IsInvalid()
	const
	{
		return IS_NULL( m_lpv );
	}

	///////////////////////////////////////////////////////////////////////////////

	const
	LPVOID
	CMemoryHandler::cpv()
	const
	{
		return m_lpv;
	}

	///////////////////////////////////////////////////////////////////////////////

	LPVOID
	CMemoryHandler::pv()
	{
		return m_lpv;
	}

	///////////////////////////////////////////////////////////////////////////////

	const
	BYTE *
	CMemoryHandler::cpb()
	const
	{
		return reinterpret_cast< LPBYTE >( m_lpv );
	}

	///////////////////////////////////////////////////////////////////////////////

	BYTE *
	CMemoryHandler::pb()
	{
		return reinterpret_cast< LPBYTE >( m_lpv );
	}

	///////////////////////////////////////////////////////////////////////////////

	SIZE_T
	CMemoryHandler::NumberofBytes()
	const
	{
		return m_NumberofBytes;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CMemoryHandler::Zero()
	{
		HRESULT hr = S_OK;

		hr = safeZeroMemory( m_lpv, m_NumberofBytes );
		ASSERT_SUCCEEDED( hr );

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CMemoryHandler::CopyFrom
	(
		LPCVOID lpcv,
		SIZE_T NumberofBytes
	)
	{
		HRESULT hr = S_OK;

		SIZE_T NumberofBytesToCopy = min( m_NumberofBytes, NumberofBytes );

		hr = safeCopyMemory( m_lpv, lpcv, NumberofBytesToCopy );
		ASSERT_SUCCEEDED( hr );

		if( SUCCEEDED( hr ) )
		{
			if( NumberofBytes > NumberofBytesToCopy )
			{
				hr = S_FALSE;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CMemoryHandler::CopyTo
	(
		LPVOID lpv,
		SIZE_T NumberofBytes
	)
	const
	{
		HRESULT hr = S_OK;

		SIZE_T NumberofBytesToCopy = min( m_NumberofBytes, NumberofBytes );

		hr = safeCopyMemory( lpv, m_lpv, NumberofBytesToCopy );
		ASSERT_SUCCEEDED( hr );

		if( SUCCEEDED( hr ) )
		{
			if( NumberofBytes > NumberofBytesToCopy )
			{
				hr = S_FALSE;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CMemoryHandler::Free()
	{
		HRESULT hr = S_OK;

		if( IS_NULL( m_lpv ) )
		{
			hr = S_FALSE;
		}
		else
		{
			hr = ActualFreeMemory( m_lpv );
			ASSERT_SUCCEEDED( hr );

			m_NumberofBytes = 0;
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//virtual
	CMemoryHandler::~CMemoryHandler()
	{
		HRESULT hr = Free();
		ASSERT_SUCCEEDED( hr );
	}
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////