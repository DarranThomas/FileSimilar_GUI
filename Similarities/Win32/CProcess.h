///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_PROCESS__
#define __WIN32__C_PROCESS__

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
	CProcess
	:
		public CSynchronizationObject
	{
	public :

		static
		HRESULT
		STDAPICALLTYPE
		GetSystemAffinityMask
		(
			DWORD_PTR &refdwptrSystemAffinityMask
		);

		static
		SIZE_T
		STDAPICALLTYPE
		NumberofProcessorsInAffinityMask
		(
			DWORD_PTR dwptrAffinityMask
		);

		static
		HRESULT
		STDAPICALLTYPE
		GetNumberofProcessors
		(
			SIZE_T &refNumberofProcessors
		);

	public :

		CProcess();

		STDMETHODIMP
		Open
		(
			DWORD dwDesiredAccess,
			BOOL bInheritHandle,
			DWORD dwProcessId
		);

		DWORD
		STDMETHODCALLTYPE
		GetId()
		const;

		STDMETHODIMP
		GetPriorityClass
		(
			DWORD &refdwPriorityClass
		)
		const;

		STDMETHODIMP
		SetPriorityClass
		(
			DWORD dwPriorityClass
		);

		STDMETHODIMP
		GetAffinityMask
		(
			DWORD_PTR &refdwptrProcessAffinityMask
		)
		const;

		STDMETHODIMP
		GetAffinityMask
		(
			DWORD_PTR &refdwptrProcessAffinityMask,
			DWORD_PTR &refdwptrSystemAffinityMask
		)
		const;

		STDMETHODIMP
		SetAffinityMask
		(
			DWORD_PTR dwptrAffinityMask
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
		~CProcess();

	private :

		bool m_bIsCreated;

		DWORD m_dwProcessId;

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