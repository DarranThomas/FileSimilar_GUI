///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_FILE__
#define __WIN32__C_FILE__

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
	CFile
	:
		public THandle< HANDLE, INVALID_HANDLE_VALUE >
	{
	public :

		STDMETHODIMP
		OpenA
		(
			LPCSTR pszFilePath,
			DWORD dwDesiredAccess,
			DWORD dwShareMode,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwCreationDisposition,
			DWORD dwFlags,
			HANDLE hTemplate = NULL
		);

		STDMETHODIMP
		OpenW
		(
			LPCWSTR pszFilePath,
			DWORD dwDesiredAccess,
			DWORD dwShareMode,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwCreationDisposition,
			DWORD dwFlags,
			HANDLE hTemplate = NULL
		);

		STDMETHODIMP
		Open
		(
			LPCTSTR pszFilePath,
			DWORD dwDesiredAccess,
			DWORD dwShareMode,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwCreationDisposition,
			DWORD dwFlags,
			HANDLE hTemplate = NULL
		);

		// Return codes :
		// S_OK - succeeded, all data was read;
		// HRESULT_FROM_WIN32( ERROR_PARTIAL_COPY ) - not all data was read;
		// HRESULT_FROM_WIN32( ERROR_HANDLE_EOF ) - end-of-file reached, no data was read
		// S_IO_PENDING - I/O pending;
		STDMETHODIMP
		Read
		(
			VOID *pvBuffer,
			DWORD dwNumberofBytesToRead,
			DWORD *pdwNumberofBytesRead = NULL,
			LPOVERLAPPED lpOverlapped = NULL
		);

		// Return codes :
		// S_OK - succeeded, all data was written;
		// HRESULT_FROM_WIN32( ERROR_PARTIAL_COPY ) - not all data requested was written;
		// S_IO_PENDING - I/O pending;
		// S_PIPE_BEING_CLOSED - pipe is about to be disconnected (at client side ?), so should be re-connected.
		STDMETHODIMP
		Write
		(
			const
			VOID *cpvBuffer,
			DWORD dwNumberofBytesToWrite,
			DWORD *pdwNumberofBytesWritten = NULL,
			LPOVERLAPPED lpOverlapped = NULL
		);

		// Return codes :
		// S_OK - succeeded;
		// S_PIPE_NOT_CONNECTED - pipe is disconnected (at client side ?), so should be re-connected;
		STDMETHODIMP
		GetOverlappedResult
		(
			LPOVERLAPPED lpOverlapped,
			DWORD *pdwNumberofBytesTransferred = NULL,
			BOOL bWait = FALSE
		);

		STDMETHODIMP
		CancelIO();

	private :

		HRESULT
		ActualOpen
		(
			bool bUnicode,
			LPCVOID lpcvFilePath,
			DWORD dwDesiredAccess,
			DWORD dwShareMode,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwCreationDisposition,
			DWORD dwFlags,
			HANDLE hTemplate
		);
	};

	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////