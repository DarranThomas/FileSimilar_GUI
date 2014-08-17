///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_SYNCHRONIZATION_OBJECT__
#define __WIN32__C_SYNCHRONIZATION_OBJECT__

///////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__T_HANDLE__
	#error #include <Win32/THandle.h>
#endif

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////

	class
	CSynchronizationObject
	:
		public CHandle
	{
	public :

		static
		HRESULT
		STDAPICALLTYPE
		WaitForSingleObject
		(
			HANDLE handle,
			DWORD dwTimeout = INFINITE
		);

		static
		HRESULT
		STDAPICALLTYPE
		WaitForMultipleObjects
		(
			DWORD dwNumberofHandles,
			const
			HANDLE *cpHandles,
			DWORD *pdwIndexofSignaled = NULL,
			BOOL bWaitAll = FALSE,
			DWORD dwTimeout = INFINITE
		);

		static
		HRESULT
		STDAPICALLTYPE
		MsgLoop();

		static
		HRESULT
		STDAPICALLTYPE
		MsgWaitForMultipleObjects
		(
			DWORD dwNumberofHandles,
			const
			HANDLE *cpHandles,
			DWORD *pdwIndexofSignaled = NULL,
			BOOL bWaitAll = FALSE,
			DWORD dwTimeout = INFINITE,
			DWORD dwWakeMask = QS_ALLINPUT
		);

	public :

		STDMETHODIMP
		Wait
		(
			DWORD dwTimeout = INFINITE
		)
		const;

		STDMETHODIMP
		IsSignaled()
		const;
	};

	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////