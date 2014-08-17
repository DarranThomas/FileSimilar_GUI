///////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

///////////////////////////////////////////////////////////////////////////////////

#include "macro.h"
#include "THandle.h"
#include "CSynchronizationObject.h"

///////////////////////////////////////////////////////////////////////////////////

#include "CThread.h"

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	CThread::CThread()
	:
		m_bIsCreated( false ),
		m_dwThreadId( static_cast< DWORD >( -1 ) )
	{}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::Create
	(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		SIZE_T dwStackSize,
		LPTHREAD_START_ROUTINE lpfThreadRoutine,
		LPVOID lpvParameter,
		int Priority,
		BOOL bCreateSuspended,
		DWORD *pdwThreadId
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
			BOOL bAlterPriority = ( THREAD_PRIORITY_NORMAL != Priority );

			DWORD dwCreationFlags = ( bAlterPriority || bCreateSuspended ) ? CREATE_SUSPENDED : 0;

			DWORD dwThreadId = 0;

			m_h = ::CreateThread( lpSecurityAttributes, dwStackSize, lpfThreadRoutine, lpvParameter, dwCreationFlags, &dwThreadId );

			if( IsInvalid() )
			{
				hr = GetLastError();
				ASSERT_SUCCEEDED( hr );
			}
			else
			{
				m_bIsCreated = true;

				m_dwThreadId = dwThreadId;

				if( bAlterPriority )
				{
					hr = SetPriority( Priority );
					ASSERT_SUCCEEDED( hr );

					if( SUCCEEDED( hr ) )
					{
						if( !bCreateSuspended )
						{
							hr = Resume();
							ASSERT_SUCCEEDED( hr );
						}
					}
				}
			}

			if( IS_NOT_NULL( pdwThreadId ) )
			{
				( *pdwThreadId ) = dwThreadId;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	DWORD
	STDMETHODCALLTYPE
	CThread::GetId()
	const
	{
		return m_dwThreadId;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::GetPriority
	(
		int &refiPriority
	)
	const
	{
		HRESULT hr = S_OK;

		refiPriority = ::GetThreadPriority( m_h );

		if( THREAD_PRIORITY_ERROR_RETURN == refiPriority )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::SetPriority
	(
		int iPriority
	)
	{
		HRESULT hr = S_OK;

		BOOL br = ::SetThreadPriority( m_h, iPriority );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::GetPriorityBoost
	(
		BOOL &refbPriorityBoostDisabled
	)
	const
	{
		HRESULT hr = S_OK;

		BOOL br = ::GetThreadPriorityBoost( m_h, &refbPriorityBoostDisabled );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::SetPriorityBoost
	(
		BOOL bDisablePriorityBoost
	)
	{
		HRESULT hr = S_OK;

		BOOL br = ::SetThreadPriorityBoost( m_h, bDisablePriorityBoost );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::SetAffinityMask
	(
		DWORD_PTR dwptrAffinityMask,
		DWORD_PTR *pdwptrAffinityMask_Previous
	)
	{
		HRESULT hr = S_OK;

		DWORD_PTR dwptrAffinityMask_Previous = ::SetThreadAffinityMask( m_h, dwptrAffinityMask );

		if( 0 == dwptrAffinityMask_Previous )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			if( IS_NOT_NULL( pdwptrAffinityMask_Previous ) )
			{
				( *pdwptrAffinityMask_Previous ) = dwptrAffinityMask_Previous;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::GetIdealProcessor
	(
		DWORD &refdwIndexofIdealProcessor
	)
	{
		HRESULT hr = S_OK;

		refdwIndexofIdealProcessor = ::SetThreadIdealProcessor( m_h, MAXIMUM_PROCESSORS );

		if( static_cast< DWORD >( -1 ) == refdwIndexofIdealProcessor )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::SetIdealProcessor
	(
		DWORD dwIndexofIdealProcessor
	)
	{
		HRESULT hr = S_OK;

		DWORD dwIndexofPreviousIdealProcessor = ::SetThreadIdealProcessor( m_h, dwIndexofIdealProcessor );

		if( static_cast< DWORD >( -1 ) == dwIndexofPreviousIdealProcessor )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::Resume
	(
		DWORD *pdwSuspendCount
	)
	{
		HRESULT hr = S_OK;

		DWORD dwSuspendCount = ::ResumeThread( m_h );

		if( static_cast< DWORD >( -1 ) == dwSuspendCount )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			if( IS_NOT_NULL( pdwSuspendCount ) )
			{
				( *pdwSuspendCount ) = dwSuspendCount;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CThread::GetExitCode
	(
		DWORD &refdwExitCode
	)
	const
	{
		HRESULT hr = S_OK;

		BOOL br = ::GetExitCodeThread( m_h, &refdwExitCode );

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
	CThread::Terminate
	(
		DWORD dwExitCode
	)
	{
		HRESULT hr = S_OK;

		DEBUG_BREAK();

		BOOL br = ::TerminateThread( m_h, dwExitCode );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			m_dwThreadId = static_cast< DWORD >( -1 );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//virtual
	CThread::~CThread()
	{
		HRESULT hr = Free();
		ASSERT_SUCCEEDED( hr );
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//virtual
	HRESULT
	CThread::CloseHandle()
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
			hr = CHandle::CloseHandle();
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				m_bIsCreated = false;

				m_dwThreadId = static_cast< DWORD >( -1 );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////