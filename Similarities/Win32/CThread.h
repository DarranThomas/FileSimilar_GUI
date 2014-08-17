///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_THREAD__
#define __WIN32__C_THREAD__

///////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_SYNCHRONIZATION_OBJECT__
	#error #include <Win32/CSynchronizationObject.h>
#endif

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////

	class
	CThread
	:
		public CSynchronizationObject
	{
	public :

		CThread();

		STDMETHODIMP
		Create
		(
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			SIZE_T dwStackSize,
			LPTHREAD_START_ROUTINE lpfThreadRoutine,
			LPVOID lpvParameter,
			int Priority = THREAD_PRIORITY_NORMAL,
			BOOL bCreateSuspended = FALSE,
			DWORD *pdwThreadId = NULL
		);

		DWORD
		STDMETHODCALLTYPE
		GetId()
		const;

		STDMETHODIMP
		GetPriority
		(
			int &refiPriority
		)
		const;

		STDMETHODIMP
		SetPriority
		(
			int iPriority
		);

		STDMETHODIMP
		GetPriorityBoost
		(
			BOOL &refbPriorityBoostDisabled
		)
		const;

		STDMETHODIMP
		SetPriorityBoost
		(
			BOOL bDisablePriorityBoost
		);

		STDMETHODIMP
		SetAffinityMask
		(
			DWORD_PTR dwptrAffinityMask,
			DWORD_PTR *pdwptrAffinityMask_Previous = NULL
		);

		STDMETHODIMP
		GetIdealProcessor
		(
			DWORD &refdwIndexofIdealProcessor
		);

		STDMETHODIMP
		SetIdealProcessor
		(
			DWORD dwIndexofIdealProcessor
		);

		STDMETHODIMP
		Resume
		(
			DWORD *pdwSuspendCount = NULL
		);

		STDMETHODIMP
		GetExitCode
		(
			DWORD &refdwExitCode
		)
		const;

		STDMETHODIMP
		Terminate
		(
			DWORD dwExitCode = HRESULT_CODE( E_ABORT )
		);

		//virtual
		~CThread();

	private :

		bool m_bIsCreated;

		DWORD m_dwThreadId;

	protected :

		//virtual
		HRESULT
		CloseHandle();
	};

	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////