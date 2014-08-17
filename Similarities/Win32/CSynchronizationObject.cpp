///////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

///////////////////////////////////////////////////////////////////////////////////

#include "macro.h"
#include "THandle.h"

///////////////////////////////////////////////////////////////////////////////////

#include "CSynchronizationObject.h"

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CSynchronizationObject::WaitForSingleObject
	(
		HANDLE handle,
		DWORD dwTimeout
	)
	{
		HRESULT hr = S_OK;

		DWORD dw = ::WaitForSingleObject( handle, dwTimeout );

		if( WAIT_OBJECT_0 == dw )
		{
		}
		else
		if( WAIT_TIMEOUT == dw )
		{
			hr = S_FALSE;
		}
		else
		if( WAIT_FAILED == dw )
		{
			hr = Win32::GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			hr = E_UNEXPECTED;
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CSynchronizationObject::WaitForMultipleObjects
	(
		DWORD dwNumberofHandles,
		const
		HANDLE *cpHandles,
		DWORD *pdwIndexofSignaled,
		BOOL bWaitAll,
		DWORD dwTimeout
	)
	{
		HRESULT hr = S_OK;

		DWORD dw = ::WaitForMultipleObjects( dwNumberofHandles, cpHandles, bWaitAll, dwTimeout );

		if( ( WAIT_OBJECT_0 <= dw ) && ( dw < WAIT_OBJECT_0 + dwNumberofHandles ) )
		{
			if( IS_NOT_NULL( pdwIndexofSignaled ) )
			{
				( *pdwIndexofSignaled ) = ( dw - WAIT_OBJECT_0 );
			}
		}
		else
		if( WAIT_TIMEOUT == dw )
		{
			hr = S_FALSE;
		}
		else
		if( WAIT_FAILED == dw )
		{
			hr = Win32::GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			hr = E_UNEXPECTED;
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CSynchronizationObject::MsgLoop()
	{
		HRESULT hr = S_OK;

		MSG msg;

		while( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( WM_QUIT == msg.message )
			{
				hr = S_FALSE;

				break;
			}

			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CSynchronizationObject::MsgWaitForMultipleObjects
	(
		DWORD dwNumberofHandles,
		const
		HANDLE *cpHandles,
		DWORD *pdwIndexofSignaled,
		BOOL bWaitAll,
		DWORD dwTimeout,
		DWORD dwWakeMask
	)
	{
		HRESULT hr = S_OK;

		DWORD dw = ::MsgWaitForMultipleObjects( dwNumberofHandles, cpHandles, bWaitAll, dwTimeout, dwWakeMask );

		if( ( WAIT_OBJECT_0 <= dw ) && ( dw <= WAIT_OBJECT_0 + dwNumberofHandles ) )
		{
			if( IS_NOT_NULL( pdwIndexofSignaled ) )
			{
				( *pdwIndexofSignaled ) = ( dw - WAIT_OBJECT_0 );
			}
		}
		else
		if( WAIT_TIMEOUT == dw )
		{
			hr = S_FALSE;
		}
		else
		if( WAIT_FAILED == dw )
		{
			hr = Win32::GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			hr = E_UNEXPECTED;
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CSynchronizationObject::Wait
	(
		DWORD dwTimeout
	)
	const
	{
		return WaitForSingleObject( m_h, dwTimeout );
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CSynchronizationObject::IsSignaled()
	const
	{
		return Wait( 0 );
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////