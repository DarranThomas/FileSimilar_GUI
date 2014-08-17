/*
//////////////////////////////////////////////////////////////////////////
COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:
//////////////////////////////////////////////////////////////////////////

CUtil : Copyright (C) 2008, Shupantha Kazi Imam (shupantha@yahoo.com)

//////////////////////////////////////////////////////////////////////////
Covered code is provided under this license on an "as is" basis, without
warranty of any kind, either expressed or implied, including, without
limitation, warranties that the covered code is free of defects,
merchantable, fit for a particular purpose or non-infringing. The entire
risk as to the quality and performance of the covered code is with you.
Should any covered code prove defective in any respect, you (not the
initial developer or any other contributor) assume the cost of any
necessary servicing, repair or correction. This disclaimer of warranty
constitutes an essential part of this license. No use of any covered code
is authorized hereunder except under this disclaimer.

Permission is hereby granted to use, copy, modify, and distribute this
source code, or portions hereof, for any purpose, including commercial
applications, freely and without fee, subject to the following
restrictions: 

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
//////////////////////////////////////////////////////////////////////////

V 4.5
*/

#pragma once

#ifdef WIN32
#include <windows.h>
#include "atlbase.h"
#endif

#include <sys\types.h> 
#include <sys\stat.h> 

#include <vector>
#include <string>

using std::vector;

using std::string;
using std::wstring;

#ifdef UNICODE
#define _tstring		wstring
#else
#define _tstring		string
#endif

#ifdef WIN32
#define DIRECTORY_SEPARATOR_W		L"\\"
#define DIRECTORY_SEPARATOR_W_C		L'\\'

#define DIRECTORY_SEPARATOR_A		"\\"
#define DIRECTORY_SEPARATOR_A_C		'\\'
#else
#define DIRECTORY_SEPARATOR_W		L"/"
#define DIRECTORY_SEPARATOR_W_C		L'/'

#define DIRECTORY_SEPARATOR_A		"/"
#define DIRECTORY_SEPARATOR_A_C		'/'
#endif

#ifdef UNICODE
#define DIRECTORY_SEPARATOR		DIRECTORY_SEPARATOR_W
#define DIRECTORY_SEPARATOR_C	DIRECTORY_SEPARATOR_W_C
#else
#define DIRECTORY_SEPARATOR		DIRECTORY_SEPARATOR_A
#define DIRECTORY_SEPARATOR_C	DIRECTORY_SEPARATOR_A_C
#endif

class CUtil
{
public:
	CUtil(void);
	~CUtil(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// File IO functions
	//////////////////////////////////////////////////////////////////////////
	// Get/Set binary file content
	static char* GetFileContent(const wstring& strFilePath, long& lFileSize, long lStart = 0, long lEnd = 0);
	static char* GetFileContent(const string& strFilePath, long& lFileSize, long lStart = 0, long lEnd = 0);
	static unsigned char* GetFileContentUC(const wstring& strFilePath, long& lFileSize, long lStart = 0, long lEnd = 0);
	static unsigned char* GetFileContentUC(const string& strFilePath, long& lFileSize, long lStart = 0, long lEnd = 0);

	static bool SetFileContent(const wstring& strFilePath, const char* pstrFileContent, long& lFileSize);
	static bool SetFileContent(const string& strFilePath, const char* pstrFileContent, long& lFileSize);
	static bool SetFileContent(const wstring& strFilePath, const unsigned char* pstrFileContent, long& lFileSize);
	static bool SetFileContent(const string& strFilePath, const unsigned char* pstrFileContent, long& lFileSize);

	static bool AppendFileContent(const wstring& strFilePath, const char* pstrFileContent, long& lFileContentSize);
	static bool AppendFileContent(const string& strFilePath, const char* pstrFileContent, long& lFileContentSize);
	static bool AppendFileContent(const wstring& strFilePath, const unsigned char* pstrFileContent, long& lFileContentSize);
	static bool AppendFileContent(const string& strFilePath, const unsigned char* pstrFileContent, long& lFileContentSize);

	// Read/Write ANSI text data to/from file
	static bool ReadFile(const wstring& strFilePath, wstring& strFileData, long lStart = 0, long lEnd = 0);
	static bool ReadFile(const wstring& strFilePath, string& strFileData, long lStart = 0, long lEnd = 0);
	static bool ReadFile(const string& strFilePath, string& strFileData, long lStart = 0, long lEnd = 0);

	static bool WriteFile(const wstring& strFilePath, const wstring& strFileData);
	static bool WriteFile(const wstring& strFilePath, const string& strFileData);
	static bool WriteFile(const string& strFilePath, const string& strFileData);

	static bool AppendFile(const wstring& strFilePath, const wstring& strFileData);
	static bool AppendFile(const wstring& strFilePath, const string& strFileData);
	static bool AppendFile(const string& strFilePath, const string& strFileData);

	// Get the file size in bytes
	static long GetFileSize(const wstring& strFilePath);
	static long GetFileSize(const string& strFilePath);
	static __int64 GetFileSize64(const wstring& strFilePath);
	static __int64 GetFileSize64(const string& strFilePath);

	// Check to see if its a valid file
	static bool IsFile(const wstring& strFilePath);
	static bool IsFile(const string& strFilePath);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// String manipulation functions
	//////////////////////////////////////////////////////////////////////////
	// Returns the number of successful substrings replaced
	static long FindReplace(wstring& strSource, const wstring& strFind, const wstring& strReplace);
	static long FindReplace(string& strSource, const string& strFind, const string& strReplace);

	// Removes/Replaces all white spaces
	static void RemoveWhiteSpaces(wstring& strSource, const wstring& strReplace = L"");
	static void RemoveWhiteSpaces(string& strSource, const string& strReplace = "");

	// Create padding
	static wstring CreatePadding(unsigned long ulLength, const wstring& strChars2Pad = L" ");
	static string CreatePadding(unsigned long ulLength, const string& strChars2Pad = " ");

	// Remove/Replace all characters that are not alpha numeric with specified characters
	static void RemoveGarbage(wstring& strSource, wchar_t wch = L' ');
	static void RemoveGarbage(string& strSource, char ch = ' ');

	// A function to trim strings from both ends
	static void Trim(wstring& strSource, const wstring& strChars2Trim = L" \t\r\n", bool bTrimLeft = true, bool bTrimRight = true);
	static void Trim(string& strSource, const string& strChars2Trim = " \t\r\n", bool bTrimLeft = true, bool bTrimRight = true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// String tokenizing functions
	//////////////////////////////////////////////////////////////////////////
	static long GetTokenCount(const wstring& strSource, const wstring& strDelimiter);
	static long GetTokenCount(const string& strSource, const string& strDelimiter);

	static wstring GetToken(const wstring& strSource, const wstring& strDelimiter, long lTokenIndex);
	static string GetToken(const string& strSource, const string& strDelimiter, long lTokenIndex);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Clipboard
	//////////////////////////////////////////////////////////////////////////
	static void Copy2Clipboard(const wstring& strData);
	static void Copy2Clipboard(const string& strData);

	static _tstring PasteFromClipboard();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Numeric functions
	//////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
	#define Long2String Long2StringW
	#define Double2String Double2StringW
#else
	#define Long2String Long2StringA
	#define Double2String Double2StringA
#endif

	static unsigned long NumberOfDigits(long lNumber);

	static double Round(double dNumber2Round, unsigned long lPlaces);
	static long Round(double dNumber2Round);

	static bool IsPowerOf2(unsigned long iNumber);

	static wstring Long2StringW(unsigned long lNumber, unsigned long lPrecision);
	static string Long2StringA(unsigned long lNumber, unsigned long lPrecision);

	static wstring Long2StringW(long lNumber);
	static string Long2StringA(long lNumber);

	static wstring Double2StringW(double dNumber, unsigned long lPrecision = 0);
	static string Double2StringA(double dNumber, unsigned long lPrecision = 0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// String case converters, upper <-> lower
	//////////////////////////////////////////////////////////////////////////
	static wstring ToUpper(const wstring& str);
	static string ToUpper(const string& str);

	static wstring ToLower(const wstring& str);
	static string ToLower(const string& str);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Compare without case sensitivity
	//////////////////////////////////////////////////////////////////////////
	static int CompareNoCase(const wstring& str1, const wstring& str2);
	static int CompareNoCase(const string& str1, const string& str2);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// System time retrieval
	//////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
#define GetSystemTime GetSystemTimeW
#else
#define GetSystemTime GetSystemTimeA
#endif

	static wstring GetSystemTimeW(const wstring& strDateFormat = L"%A, %d/%m/%Y, %H:%M:%S - %Z");
	static string GetSystemTimeA(const string& strDateFormat = "%A, %d/%m/%Y, %H:%M:%S - %Z");

	static wstring GetSystemTimeW(const time_t& ttTime, const wstring& strDateFormat = L"%A, %d/%m/%Y, %H:%M:%S - %Z");
	static string GetSystemTimeA(const time_t& ttTime, const string& strDateFormat = "%A, %d/%m/%Y, %H:%M:%S - %Z");
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Convert given number of seconds to hh:mm:ss and vice versa
	//////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
#define GetTime GetTimeW
#else
#define GetTime GetTimeA
#endif

	static wstring GetTimeW(long lSeconds);
	static string GetTimeA(long lSeconds);

	static long GetTimeW(const wstring& strTime = L"00:00:00");
	static long GetTimeA(const string& strTime = "00:00:00");

	//////////////////////////////////////////////////////////////////////////
	// Retrieve time_t from string: dd/mm/yyyy, hh:mm:ss
	//////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
#define MakeTime MakeTimeW
#else
#define MakeTime MakeTimeA
#endif

	static time_t MakeTimeW(const wstring& strDateTime = L"00/00/0000, 00:00:00");
	static time_t MakeTimeA(const string& strDateTime = "00/00/0000, 00:00:00");
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Get commonly used directory paths
	//////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
#define GetWorkingDirectory GetWorkingDirectoryW
#define GetProgramDirectory GetProgramDirectoryW
#else
#define GetWorkingDirectory GetWorkingDirectoryA
#define GetProgramDirectory GetProgramDirectoryA
#endif

	static wstring GetWorkingDirectoryW();
	static string GetWorkingDirectoryA();

	static wstring GetProgramDirectoryW();
	static string GetProgramDirectoryA();

	static _tstring GetProgramFilesDirectory();
	static _tstring GetWindowsDirectory();
	static _tstring GetSystemDirectory();
	static _tstring GetMyDocumentsDirectory();
	static _tstring GetMyMusicDirectory();
	static _tstring GetMyPicturesDirectory();
	static _tstring GetMyVideosDirectory();
	static _tstring GetAppDataDirectory();
	static _tstring GetLocalAppDataDirectory();
	static _tstring GetDesktopDirectory();
	static _tstring GetStartupDirectory();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Copy or cut a file into another directory, creating the directory path if necessary
	//////////////////////////////////////////////////////////////////////////
	static bool CopyFile2Directory(const _tstring& strSourceFilePath, const _tstring& strDestinationDirectory, bool bDeleteSource = false);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Directory/File manipulation functions
	//////////////////////////////////////////////////////////////////////////
	// Get list of all files in the target directory
	static void GetFileList(const _tstring& strTargetDirectoryPath, const _tstring& strWildcard, bool bLookInSubdirectories, vector<_tstring>& vecstrFileList);

	// Rename all files in the target directory
	static void RenameFiles(const _tstring& strTargetDirectoryPath, const _tstring& strWildcard, bool bLookInSubdirectories, const _tstring& strFind, const _tstring& strReplace);

	// Create the entire directory path
	static void MakeDirectory(const _tstring& strDirectoryPath);

	// Delete the entire directory path, including all files and folders within
	static void DeleteDirectory(const _tstring& strTargetDirectoryPath);

	// Delete files from directory
	static void DeleteFiles(const _tstring& strTargetDirectoryPath, const _tstring& strWildcard, bool bLookInSubdirectories);

	// Check whether the given path is a directory
	static bool IsDirectory(const _tstring& strDirectoryPath);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Directory/File path string manipulation functions
	//////////////////////////////////////////////////////////////////////////
	// Add "\" to the end of a directory path, if not present
	static wstring AddDirectoryEnding(const wstring& strDirectoryPath);
	static string AddDirectoryEnding(const string& strDirectoryPath);

	// Remove "\" from the end of a directory path, if present
	static wstring RemoveDirectoryEnding(const wstring& strDirectoryPath);
	static string RemoveDirectoryEnding(const string& strDirectoryPath);

	// Get the previous directory from a given directory path
	static wstring GetRootDirectory(const wstring& strDirectoryPath);
	static string GetRootDirectory(const string& strDirectoryPath);

	// Get the name of the directory form a given directory path: e.g. C:\Program Files\XYZ, will return XYZ
	static wstring GetDirectoryName(const wstring& strDirectoryPath);
	static string GetDirectoryName(const string& strDirectoryPath);

	// Get the directory from a file path
	static wstring GetFileDirectory(const wstring& strFilePath);
	static string GetFileDirectory(const string& strFilePath);

	// Get the file name including/excluding the extension from a given file path
	static wstring GetFileName(const wstring& strFilePath, bool bIncludeExtension = false);
	static string GetFileName(const string& strFilePath, bool bIncludeExtension = false);

	// Get the file extension including/excluding the "." from a given file path
	static wstring GetFileExtension(const wstring& strFilePath, bool bIncludeDot = false);
	static string GetFileExtension(const string& strFilePath, bool bIncludeDot = false);

	// Get the file prefix / suffix
	static wstring GetFileNamePrefix(const wstring& strFilePath, const wstring& strDelimiter);
	static string GetFileNamePrefix(const string& strFilePath, const string& strDelimiter);

	static wstring GetFileNameSuffix(const wstring& strFilePath, const wstring& strDelimiter);
	static string GetFileNameSuffix(const string& strFilePath, const string& strDelimiter);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// String type converters, ANSI <-> UNICODE
	//////////////////////////////////////////////////////////////////////////
	static char Wide2Narrow(wchar_t wch);
	static wchar_t Narrow2Wide(char ch);

	static wstring GetStringW(const string& strA);
	static wstring GetString(const string& strA);

	static string GetStringA(const wstring& strW);
	static string GetString(const wstring& strW);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Registry
	//////////////////////////////////////////////////////////////////////////
	static _tstring GetRegistryInfo(HKEY hKey, const _tstring& strRegistryPath, const _tstring& strValueName);
	static  DWORD GetRegistryValue(HKEY hKey, const _tstring& strRegistryPath, const _tstring& strValueName);

	static bool SetRegistryInfo(HKEY hkey, const _tstring& strRegistryPath, const _tstring& strValueName, const _tstring& strValue);
	static bool SetRegistryValue(HKEY hkey, const _tstring& strRegistryPath, const _tstring& strValueName, DWORD dwValue);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Internet
	//////////////////////////////////////////////////////////////////////////
private:
	static LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR regdata);	// Only used by OpenURL()

public:
	static HINSTANCE OpenURL(LPCTSTR strURL);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Execution
	//////////////////////////////////////////////////////////////////////////
	static bool Execute(const _tstring& strFilePath, const _tstring& strParameters = _T(""), bool bShow = true, bool bWait = false);
	static bool Execute(const _tstring& strCommandLine, bool bShow = true, bool bWait = false);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// System
	//////////////////////////////////////////////////////////////////////////
	static long GetProcessorCount();
	////////////////////////////////////////////////////////////////////////// 

	//////////////////////////////////////////////////////////////////////////
	// Random generators
	//////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
#define GenerateRandomString GenerateRandomStringW
#else
#define GenerateRandomString GenerateRandomStringA
#endif

	// Generate random string
	static wstring GenerateRandomStringW(long lMaxLength, bool bIncludeAlpha = true, bool bIncludeNumbers = true);
	static string GenerateRandomStringA(long lMaxLength, bool bIncludeAlpha = true, bool bIncludeNumbers = true);

	// Generate random number between A & B
	static float GetRandomNumber(float fA, float fB, float fGrain = 10.0f);
	static double GetRandomNumber(double dA, double dB, double dGrain = 10.0);

	// Simulate die roll in a bernoulli trial
	static bool RollDice(double dProbability);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Self destruct code
	//////////////////////////////////////////////////////////////////////////
	static void SelfDestruct();
	//////////////////////////////////////////////////////////////////////////
};
