///////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

///////////////////////////////////////////////////////////////////////////////////

#include "macro.h"
#include "THandle.h"
#include "CSynchronizationObject.h"

///////////////////////////////////////////////////////////////////////////////////

#include "CProcess.h"

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CProcess::GetSystemAffinityMask
	(
		DWORD_PTR &refdwptrSystemAffinityMask
	)
	{
		HRESULT hr = S_OK;

		DWORD_PTR dwptrProcessAffinityMask;

		BOOL br = ::GetProcessAffinityMask( ::GetCurrentProcess(), &dwptrProcessAffinityMask, &refdwptrSystemAffinityMask );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	SIZE_T
	STDAPICALLTYPE
	CProcess::NumberofProcessorsInAffinityMask
	(
		DWORD_PTR dwptrAffinityMask
	)
	{
		SIZE_T result = 0;

		while( 0 != dwptrAffinityMask )
		{
			if( 1 & dwptrAffinityMask )
			{
				++ result;

				dwptrAffinityMask >>= 1;
			}
		}

		return result;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CProcess::GetNumberofProcessors
	(
		SIZE_T &refNumberofProcessors
	)
	{
		HRESULT hr = S_OK;

		DWORD_PTR dwptrProcessAffinityMask = 0;
		DWORD_PTR dwptrSystemAffinityMask = 0;

		BOOL br = ::GetProcessAffinityMask( ::GetCurrentProcess(), &dwptrProcessAffinityMask, &dwptrSystemAffinityMask );

		if( !br )
		{
			hr = Win32::GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			refNumberofProcessors = NumberofProcessorsInAffinityMask( dwptrSystemAffinityMask );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	CProcess::CProcess()
	:
		m_bIsCreated( false ),
		m_dwProcessId( static_cast< DWORD >( -1 ) )
	{}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::Open
	(
		DWORD dwDesiredAccess,
		BOOL bInheritHandle,
		DWORD dwProcessId
	)
	{
		HRESULT hr = S_OK;

		if( IsValid() )
		{
			hr = E_UNEXPECTED;
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			m_h = ::OpenProcess( dwDesiredAccess, bInheritHandle, dwProcessId );

			if( IsInvalid() )
			{
				hr = GetLastError();
				ASSERT_SUCCEEDED( hr );
			}
			else
			{
				m_dwProcessId = dwProcessId;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	DWORD
	STDMETHODCALLTYPE
	CProcess::GetId()
	const
	{
		return m_dwProcessId;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::GetPriorityClass
	(
		DWORD &refdwPriorityClass
	)
	const
	{
		HRESULT hr = S_OK;

		refdwPriorityClass = ::GetPriorityClass( m_h );

		if( 0 == refdwPriorityClass )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::SetPriorityClass
	(
		DWORD dwPriorityClass
	)
	{
		HRESULT hr = S_OK;

		BOOL br = ::SetPriorityClass( m_h, dwPriorityClass );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::GetAffinityMask
	(
		DWORD_PTR &refdwptrProcessAffinityMask
	)
	const
	{
		HRESULT hr = S_OK;

		DWORD_PTR dwptrSystemAffinityMask;

		BOOL br = ::GetProcessAffinityMask( m_h, &refdwptrProcessAffinityMask, &dwptrSystemAffinityMask );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::GetAffinityMask
	(
		DWORD_PTR &refdwptrProcessAffinityMask,
		DWORD_PTR &refdwptrSystemAffinityMask
	)
	const
	{
		HRESULT hr = S_OK;

		BOOL br = ::GetProcessAffinityMask( m_h, &refdwptrProcessAffinityMask, &refdwptrSystemAffinityMask );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::SetAffinityMask
	(
		DWORD_PTR dwptrAffinityMask
	)
	{
		HRESULT hr = S_OK;

		BOOL br = ::SetProcessAffinityMask( m_h, dwptrAffinityMask );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::GetExitCode
	(
		DWORD &refdwExitCode
	)
	const
	{
		HRESULT hr = S_OK;

		BOOL br = ::GetExitCodeProcess( m_h, &refdwExitCode );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			if( STILL_ACTIVE == refdwExitCode )
			{
				hr = IsSignaled();
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CProcess::Terminate
	(
		DWORD dwExitCode
	)
	{
		HRESULT hr = S_OK;

		DEBUG_BREAK();

		BOOL br = ::TerminateProcess( m_h, dwExitCode );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			m_dwProcessId = static_cast< DWORD >( -1 );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//virtual
	CProcess::~CProcess()
	{
		HRESULT hr = Free();
		ASSERT_SUCCEEDED( hr );
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//virtual
	HRESULT
	CProcess::CloseHandle()
	{
		HRESULT hr = S_OK;

		if( m_bIsCreated )
		{
			hr = IsSignaled();
			ASSERT_SUCCEEDED( hr );

			if( S_FALSE == hr )
			{
				hr = Terminate();
				ASSERT_SUCCEEDED( hr );
			}
		}

		if( SUCCEEDED( hr ) )
		{
			hr = CSynchronizationObject::CloseHandle();
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				m_bIsCreated = false;

				m_dwProcessId = static_cast< DWORD >( -1 );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////