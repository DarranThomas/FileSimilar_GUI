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

#include "stdafx.h"

#include "Util.h"

#include <stdio.h>
#include <malloc.h>
#include <tchar.h>
#include <wchar.h>
#include <ctime>
#include <direct.h>
#include <math.h>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <iomanip>

CUtil::CUtil(void)
{
}

CUtil::~CUtil(void)
{
}

//////////////////////////////////////////////////////////////////////////
// File IO functions
//////////////////////////////////////////////////////////////////////////
char* CUtil::GetFileContent(const wstring& strFilePath, long& lFileSize, long lStart /* = 0*/, long lEnd /* = 0*/)
{
	return GetFileContent(GetStringA(strFilePath), lFileSize, lStart, lEnd);
}

char* CUtil::GetFileContent(const string& strFilePath, long& lFileSize, long lStart /* = 0*/, long lEnd /* = 0*/)
{
	// Initialize file size
	lFileSize = 0;

	//////////////////////////////////////////////////////////////////////////
	// Check valid input parameters
	//////////////////////////////////////////////////////////////////////////
	if(strFilePath.compare("") == 0)
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Open file
	//////////////////////////////////////////////////////////////////////////
	FILE* pFileInput = _fsopen(strFilePath.c_str(), "rb", _SH_DENYNO);

	if(pFileInput == NULL)
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Allocate required buffers and check validity
	//////////////////////////////////////////////////////////////////////////
	// Get the file size
	fseek(pFileInput, 0, SEEK_END);
	lFileSize = ftell(pFileInput);
	rewind(pFileInput);

	if(lFileSize <= 0)
	{
		// Close the file handle
		fclose(pFileInput);

		return NULL;
	}

	if(lEnd >= (lFileSize - 1))
	{
		lEnd = lFileSize - 1;
	}

	long lNumberOfBytes2Read = 0;

	if(lStart == 0 && lEnd == 0)
	{
		lNumberOfBytes2Read = lFileSize;
	}
	else
	if(lEnd < lFileSize && lStart <= lEnd)
	{
		lNumberOfBytes2Read = lEnd - lStart + 1;

		fseek(pFileInput, lStart, SEEK_SET);
	}
	else
	{
		// Close the file handle
		fclose(pFileInput);

		return NULL;
	}

	// Allocate memory to contain the file data
	char* pstrFileContent = new char[(long) lNumberOfBytes2Read];

	if(pstrFileContent == NULL)
	{
		// Close the file handle
		fclose(pFileInput);

		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	// Copy file content
	fread(pstrFileContent, sizeof(char), (long) lNumberOfBytes2Read, pFileInput);

	// Close the file handle
	fclose(pFileInput);

	// Return file content - must be deallocated by caller
	return pstrFileContent;
}

unsigned char* CUtil::GetFileContentUC(const wstring& strFilePath, long& lFileSize, long lStart /* = 0*/, long lEnd /* = 0*/)
{
	return GetFileContentUC(GetStringA(strFilePath), lFileSize, lStart, lEnd);
}

unsigned char* CUtil::GetFileContentUC(const string& strFilePath, long& lFileSize, long lStart /* = 0*/, long lEnd /* = 0*/)
{
	// Initialize file size
	lFileSize = 0;

	//////////////////////////////////////////////////////////////////////////
	// Check valid input parameters
	//////////////////////////////////////////////////////////////////////////
	if(strFilePath.compare("") == 0)
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Open file
	//////////////////////////////////////////////////////////////////////////
	FILE* pFileInput = _fsopen(strFilePath.c_str(), "rb", _SH_DENYNO);

	if(pFileInput == NULL)
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Allocate required buffers and check validity
	//////////////////////////////////////////////////////////////////////////
	// Get the file size
	fseek(pFileInput, 0, SEEK_END);
	lFileSize = ftell(pFileInput);
	rewind(pFileInput);

	if(lFileSize <= 0)
	{
		// Close the file handle
		fclose(pFileInput);

		return NULL;
	}

	if(lEnd >= (lFileSize - 1))
	{
		lEnd = lFileSize - 1;
	}

	long lNumberOfBytes2Read = 0;

	if(lStart == 0 && lEnd == 0)
	{
		lNumberOfBytes2Read = lFileSize;
	}
	else
	if(lEnd < lFileSize && lStart <= lEnd)
	{
		lNumberOfBytes2Read = lEnd - lStart + 1;

		fseek(pFileInput, lStart, SEEK_SET);
	}
	else
	{
		// Close the file handle
		fclose(pFileInput);

		return NULL;
	}

	// Allocate memory to contain the file data
	unsigned char* pstrFileContent = new unsigned char[(long) lNumberOfBytes2Read];

	if(pstrFileContent == NULL)
	{
		// Close the file handle
		fclose(pFileInput);

		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	// Copy file content
	fread(pstrFileContent, sizeof(unsigned char), (long) lNumberOfBytes2Read, pFileInput);

	// Close the file handle
	fclose(pFileInput);

	// Return file content - must be deallocated by caller
	return pstrFileContent;
}

bool CUtil::SetFileContent(const wstring& strFilePath, const char* pstrFileContent, long& lFileSize)
{
	return SetFileContent(GetStringA(strFilePath), pstrFileContent, lFileSize);
}

bool CUtil::SetFileContent(const string& strFilePath, const char* pstrFileContent, long& lFileSize)
{
	//////////////////////////////////////////////////////////////////////////
	// Check valid input parameters
	//////////////////////////////////////////////////////////////////////////
	if(strFilePath.compare("") == 0)
	{
		return false;
	}

	if(pstrFileContent == NULL)
	{
		return false;
	}

	if(lFileSize <= 0)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Open file
	//////////////////////////////////////////////////////////////////////////
	FILE* pFileOutput = _fsopen(strFilePath.c_str(), "wb", _SH_DENYWR);

	if(pFileOutput == NULL)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	// Write the file
	lFileSize = (long) fwrite(pstrFileContent, sizeof(char), (long) lFileSize, pFileOutput);

	// Close the file handle
	fclose(pFileOutput);

	return true;
}

bool CUtil::SetFileContent(const wstring& strFilePath, const unsigned char* pstrFileContent, long& lFileSize)
{
	return SetFileContent(GetStringA(strFilePath), pstrFileContent, lFileSize);
}

bool CUtil::SetFileContent(const string& strFilePath, const unsigned char* pstrFileContent, long& lFileSize)
{
	//////////////////////////////////////////////////////////////////////////
	// Check valid input parameters
	//////////////////////////////////////////////////////////////////////////
	if(strFilePath.compare("") == 0)
	{
		return false;
	}

	if(pstrFileContent == NULL)
	{
		return false;
	}

	if(lFileSize <= 0)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Open file
	//////////////////////////////////////////////////////////////////////////
	FILE* pFileOutput = _fsopen(strFilePath.c_str(), "wb", _SH_DENYWR);

	if(pFileOutput == NULL)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	// Write the file
	lFileSize = (long) fwrite(pstrFileContent, sizeof(unsigned char), (long) lFileSize, pFileOutput);

	// Close the file handle
	fclose(pFileOutput);

	return true;
}

bool CUtil::AppendFileContent(const wstring& strFilePath, const char* pstrFileContent, long& lFileContentSize)
{
	return AppendFileContent(GetStringA(strFilePath), pstrFileContent, lFileContentSize);
}

bool CUtil::AppendFileContent(const string& strFilePath, const char* pstrFileContent, long& lFileContentSize)
{
	//////////////////////////////////////////////////////////////////////////
	// Check valid input parameters
	//////////////////////////////////////////////////////////////////////////
	if(strFilePath.compare("") == 0)
	{
		return false;
	}

	if(pstrFileContent == NULL)
	{
		return false;
	}

	if(lFileContentSize <= 0)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Open file
	//////////////////////////////////////////////////////////////////////////
	FILE* pFileOutput = _fsopen(strFilePath.c_str(), "ab", _SH_DENYWR);

	if(pFileOutput == NULL)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	// Write the file
	lFileContentSize = (long) fwrite(pstrFileContent, sizeof(char), (long) lFileContentSize, pFileOutput);

	// Close the file handle
	fclose(pFileOutput);

	return true;
}

bool CUtil::AppendFileContent(const wstring& strFilePath, const unsigned char* pstrFileContent, long& lFileContentSize)
{
	return AppendFileContent(GetStringA(strFilePath), pstrFileContent, lFileContentSize);
}

bool CUtil::AppendFileContent(const string& strFilePath, const unsigned char* pstrFileContent, long& lFileContentSize)
{
	//////////////////////////////////////////////////////////////////////////
	// Check valid input parameters
	//////////////////////////////////////////////////////////////////////////
	if(strFilePath.compare("") == 0)
	{
		return false;
	}

	if(pstrFileContent == NULL)
	{
		return false;
	}

	if(lFileContentSize <= 0)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Open file
	//////////////////////////////////////////////////////////////////////////
	FILE* pFileOutput = _fsopen(strFilePath.c_str(), "ab", _SH_DENYWR);

	if(pFileOutput == NULL)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	// Write the file
	lFileContentSize = (long) fwrite(pstrFileContent, sizeof(unsigned char), (long) lFileContentSize, pFileOutput);

	// Close the file handle
	fclose(pFileOutput);

	return true;
}

bool CUtil::ReadFile(const wstring& strFilePath, wstring& strFileData, long lStart /* = 0*/, long lEnd /* = 0*/)
{
	string strFileDataA = "";

	bool bSuccess = ReadFile(GetStringA(strFilePath), strFileDataA, lStart, lEnd);

	strFileData = GetStringW(strFileDataA);

	return bSuccess;
}

bool CUtil::ReadFile(const wstring& strFilePath, string& strFileData, long lStart /* = 0*/, long lEnd /* = 0*/)
{
	bool bSuccess = ReadFile(GetStringA(strFilePath), strFileData, lStart, lEnd);

	return bSuccess;
}

bool CUtil::ReadFile(const string& strFilePath, string& strFileData, long lStart /* = 0*/, long lEnd /* = 0*/)
{
	strFileData = "";

	long lFileSize = 0;
	char* pstrFileContent = NULL;

	// Get the file content
	pstrFileContent = GetFileContent(strFilePath, lFileSize, lStart, lEnd);

	//////////////////////////////////////////////////////////////////////////
	// Check validity
	//////////////////////////////////////////////////////////////////////////
	if(pstrFileContent == NULL)
	{
		return false;
	}

	if(lFileSize <= 0)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	strFileData = pstrFileContent;

	if(lEnd >= (lFileSize - 1))
	{
		lEnd = lFileSize - 1;
	}

	long lNumberOfBytes2Read = 0;

	if(lStart == 0 && lEnd == 0)
	{
		lNumberOfBytes2Read = lFileSize;
	}
	else
	if(lEnd < lFileSize && lStart <= lEnd)
	{
		lNumberOfBytes2Read = lEnd - lStart + 1;
	}
	else
	{
		return false;
	}

 	strFileData[lNumberOfBytes2Read] = '\0';

	// Free the buffer
	delete[] pstrFileContent;

	return true;
}

bool CUtil::WriteFile(const wstring& strFilePath, const wstring& strFileData)
{
	return WriteFile(GetStringA(strFilePath), GetStringA(strFileData));
}

bool CUtil::WriteFile(const wstring& strFilePath, const string& strFileData)
{
	return WriteFile(GetStringA(strFilePath), strFileData);
}

bool CUtil::WriteFile(const string& strFilePath, const string& strFileData)
{
	long lFileSize = (long) strFileData.size();

	return SetFileContent(strFilePath, strFileData.c_str(), lFileSize);
}

bool CUtil::AppendFile(const wstring& strFilePath, const wstring& strFileData)
{
	return AppendFile(GetStringA(strFilePath), GetStringA(strFileData));
}

bool CUtil::AppendFile(const wstring& strFilePath, const string& strFileData)
{
	return AppendFile(GetStringA(strFilePath), strFileData);
}

bool CUtil::AppendFile(const string& strFilePath, const string& strFileData)
{
	long lFileContentSize = (long) strFileData.size();

	return AppendFileContent(strFilePath, strFileData.c_str(), lFileContentSize);
}

long CUtil::GetFileSize(const wstring& strFilePath)
{
	return GetFileSize(GetStringA(strFilePath));
}

long CUtil::GetFileSize(const string& strFilePath)
{
	// Get the the file size using better/faster method
	long lFileSize = (long) GetFileSize64(strFilePath);

	if(lFileSize > 0)
	{
		return lFileSize;
	}

	// If the previous method fails, try again the old fashioned way
	FILE* pFile = _fsopen(strFilePath.c_str(), "rb", _SH_DENYNO);
		
	if(pFile == NULL)
	{
		// File does not exist
		return -1;
	}
	else
	{
		// Get the file size
		fseek(pFile, 0, SEEK_END);
		lFileSize = ftell(pFile);
		rewind(pFile);

		// Close the file handle and free the buffer
		fclose(pFile);

		return lFileSize;
	}
}

__int64 CUtil::GetFileSize64(const wstring& strFilePath)
{
	return GetFileSize64(GetStringA(strFilePath));
}

__int64 CUtil::GetFileSize64(const string& strFilePath)
{ 
	struct __stat64 fileStat;
	
	if(_stat64(strFilePath.c_str(), &fileStat) != 0)
	{
		return -1;
	}

	return fileStat.st_size;
}

bool CUtil::IsFile(const wstring& strFilePath)
{
	return IsFile(GetStringA(strFilePath));
}

bool CUtil::IsFile(const string& strFilePath)
{
	std::ifstream ifsFile(strFilePath.c_str());

	return (ifsFile != NULL);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// String manipulation functions
//////////////////////////////////////////////////////////////////////////
long CUtil::FindReplace(wstring& strSource, const wstring& strFind, const wstring& strReplace)
{
	long lCount = 0;

	wstring::size_type nPos = 0;

	while(wstring::npos != (nPos = strSource.find(strFind, nPos)))
	{
		strSource.replace(nPos, strFind.size(), strReplace);
		nPos += strReplace.size();

		lCount++;
	}

	return lCount;
}

long CUtil::FindReplace(string& strSource, const string& strFind, const string& strReplace)
{
	long lCount = 0;

	string::size_type nPos = 0;

	while(string::npos != (nPos = strSource.find(strFind, nPos)))
	{
		strSource.replace(nPos, strFind.size(), strReplace);
		nPos += strReplace.size();

		lCount++;
	}

	return lCount;
}

void CUtil::RemoveWhiteSpaces(wstring& strSource, const wstring& strReplace /* = L""*/)
{
	FindReplace(strSource, L" ", strReplace);
	FindReplace(strSource, L"\t", strReplace);
	FindReplace(strSource, L"\r", strReplace);
	FindReplace(strSource, L"\n", strReplace);
}

void CUtil::RemoveWhiteSpaces(string& strSource, const string& strReplace /* = ""*/)
{
	FindReplace(strSource, " ", strReplace);
	FindReplace(strSource, "\t", strReplace);
	FindReplace(strSource, "\r", strReplace);
	FindReplace(strSource, "\n", strReplace);
}

wstring CUtil::CreatePadding(unsigned long ulLength, const wstring& strChars2Pad /* = L" "*/)
{
	wstring strPadding = L"";

	long lIndex = 0;
	long lCount = (long) ulLength;

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		strPadding += strChars2Pad;
	}

	return strPadding;
}

string CUtil::CreatePadding(unsigned long ulLength, const string& strChars2Pad /* = " "*/)
{
	string strPadding = "";

	long lIndex = 0;
	long lCount = (long) ulLength;

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		strPadding += strChars2Pad;
	}

	return strPadding;
}

void CUtil::RemoveGarbage(wstring& strSource, wchar_t wch /* = L' '*/)
{
	long lIndex = 0;
	long lCount = (long) strSource.size();

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		if(!isalnum(strSource[lIndex]))
		{
			strSource[lIndex] = wch;
		}
	}
}

void CUtil::RemoveGarbage(string& strSource, char ch /* = ' '*/)
{
	long lIndex = 0;
	long lCount = (long) strSource.size();

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		if(!isalnum(strSource[lIndex]))
		{
			strSource[lIndex] = ch;
		}
	}
}

void CUtil::Trim(wstring& strSource, const wstring& strChars2Trim /* = L" \t\r\n"*/, bool bTrimLeft /* = true*/, bool bTrimRight /* = true*/)
{
	size_t lStartIndex = strSource.find_first_not_of(strChars2Trim);

	if(lStartIndex == wstring::npos)
	{
		strSource.erase();
		return;
	}

	if(bTrimLeft)
	{
		strSource = strSource.substr(lStartIndex, strSource.size() - lStartIndex);
	}

	if(bTrimRight)
	{
		strSource = strSource.substr(0, strSource.find_last_not_of(strChars2Trim) + 1);
	}
}

void CUtil::Trim(string& strSource, const string& strChars2Trim /* = " \t\r\n"*/, bool bTrimLeft /* = true*/, bool bTrimRight /* = true*/)
{
	size_t lStartIndex = strSource.find_first_not_of(strChars2Trim);

	if(lStartIndex == string::npos)
	{
		strSource.erase();
		return;
	}

	if(bTrimLeft)
	{
		strSource = strSource.substr(lStartIndex, strSource.size() - lStartIndex);
	}

	if(bTrimRight)
	{
		strSource = strSource.substr(0, strSource.find_last_not_of(strChars2Trim) + 1);
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// String tokenizing functions
//////////////////////////////////////////////////////////////////////////
long CUtil::GetTokenCount(const wstring& strSource, const wstring& strDelimiter)
{
	if(strSource.compare(L"") == 0)
	{
		return 0;
	}

	long lIndex = 0;
	long lTokenCount = 0;

	while(true)
	{
		lIndex = (long) strSource.find(strDelimiter, lIndex);
		lTokenCount++;

		if(lIndex >= 0)
		{
			lIndex++;
		}
		else
		{
			break;
		}
	}

	return lTokenCount;
}

long CUtil::GetTokenCount(const string& strSource, const string& strDelimiter)
{
	if(strSource.compare("") == 0)
	{
		return 0;
	}

	long lIndex = 0;
	long lTokenCount = 0;

	while(true)
	{
		lIndex = (long) strSource.find(strDelimiter, lIndex);
		lTokenCount++;

		if(lIndex >= 0)
		{
			lIndex++;
		}
		else
		{
			break;
		}
	}

	return lTokenCount;
}

wstring CUtil::GetToken(const wstring& strSource, const wstring& strDelimiter, long lTokenIndex)
{
	if(strSource.compare(L"") == 0 || lTokenIndex < 0)
	{
		return wstring(L"");
	}

	wstring strText = strSource;
	long lDelimiterLength = (long) strDelimiter.size();

	while(true)
	{
		long lIndex = 0;
		wstring strTemp = L"";

		lIndex = (long) strText.find(strDelimiter, lIndex);

		if(lIndex >= 0)
		{
			strTemp = strText.substr(0, lIndex);
		}
		else
		{
			strTemp = strText;
		}

		strText.erase(0, lIndex + lDelimiterLength);

		lTokenIndex--;

		if(lTokenIndex < 0)
		{
			return strTemp;
		}
	}
}

string CUtil::GetToken(const string& strSource, const string& strDelimiter, long lTokenIndex)
{
	if(strSource.compare("") == 0 || lTokenIndex < 0)
	{
		return string("");
	}

	string strText = strSource;
	long lDelimiterLength = (long) strDelimiter.size();

	while(true)
	{
		long lIndex = 0;
		string strTemp = "";

		lIndex = (long) strText.find(strDelimiter, lIndex);

		if(lIndex >= 0)
		{
			strTemp = strText.substr(0, lIndex);
		}
		else
		{
			strTemp = strText;
		}

		strText.erase(0, lIndex + lDelimiterLength);

		lTokenIndex--;

		if(lTokenIndex < 0)
		{
			return strTemp;
		}
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Clipboard
//////////////////////////////////////////////////////////////////////////
void CUtil::Copy2Clipboard(const wstring& strData)
{
	Copy2Clipboard(GetStringA(strData));
}

void CUtil::Copy2Clipboard(const string& strData)
{
	if(OpenClipboard(NULL) != 0)
	{
		// Empty & lock the Clipboard & free memory associated with any previous data
		EmptyClipboard();

		// Allocate global memory for the data
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, strData.size() + 1);

		// Get a pointer to the data associated with the handle
		char* pstrData = (char*) GlobalLock(hClipboardData);

		// Copy the local data into the global memory
		strcpy_s(pstrData, strData.size() + 1, strData.c_str());

		// Unlock the global memory
		GlobalUnlock(hClipboardData);

		// Specify that the data is ANSI text
		SetClipboardData(CF_TEXT, hClipboardData);

		// Close the clipboard, unlocking it for access to other applications
		CloseClipboard();
	}
}

_tstring CUtil::PasteFromClipboard()
{
	_tstring strClipboardData = _T("");

	if(OpenClipboard(NULL) != 0) 
	{
		// Retrieve the Clipboard data, specifying ANSI text
		HANDLE hClipboardData = GetClipboardData(CF_TEXT);

		// Get a pointer to the data associated with the handle
		TCHAR* pstrData = (TCHAR*) GlobalLock(hClipboardData);

		strClipboardData = pstrData;

		// Unlock the global memory
		GlobalUnlock(hClipboardData);

		// Close the clipboard, unlocking it for access to other applications
		CloseClipboard();
	}

	return strClipboardData;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Numeric functions
//////////////////////////////////////////////////////////////////////////
unsigned long CUtil::NumberOfDigits(long lNumber)
{
	return ((unsigned long) log10((double) abs(lNumber)) + 1);
}

double CUtil::Round(double dNumber2Round, unsigned long lPlaces)
{
	if(lPlaces >= 14)
	{
		return dNumber2Round;
	}

	double dMultiple = 1.0;
	double dResult = dNumber2Round;

	dMultiple = pow(10.0, (double) lPlaces);

	// multiply the original number to 'move' the decimal point to the right
	dResult *= dMultiple;

	if(dResult < 0)
	{
		dResult -= 0.5;
	}
	else
	{
		dResult += 0.5;
	}

	// drop off everything past the decimal point
	dResult = floor(dResult);

	// 'move' the decimal point back to where it started
	dResult /= dMultiple;

	return dResult;
}

long CUtil::Round(double dNumber2Round)
{
	double dResult = dNumber2Round;

	if(dResult < 0)
	{
		dResult -= 0.5;
	}
	else
	{
		dResult += 0.5;
	}

	return (long) dResult;
}

bool CUtil::IsPowerOf2(unsigned long iNumber)
{
	return ((iNumber & (iNumber - 1)) == 0);
}

wstring CUtil::Long2StringW(unsigned long lNumber, unsigned long lPrecision)
{
	return GetStringW(Long2StringA(lNumber, lPrecision));
}

string CUtil::Long2StringA(unsigned long lNumber, unsigned long lPrecision)
{
	string strNumber = "";

	unsigned long lNumberOfDigits = (unsigned long) NumberOfDigits(lNumber);

	for(unsigned long lIndex = 0; lIndex < (lPrecision - lNumberOfDigits); lIndex++)
	{
		strNumber += "0";
	}

	strNumber += Long2StringA(lNumber);

	return strNumber;
}

wstring CUtil::Long2StringW(long lNumber)
{
	return GetStringW(Long2StringA(lNumber));
}

string CUtil::Long2StringA(long lNumber)
{
	std::ostringstream strStream;
	strStream << lNumber;

	return strStream.str();
}

wstring CUtil::Double2StringW(double dNumber, unsigned long lPrecision)
{
	return GetStringW(Double2StringA(dNumber, lPrecision));
}

string CUtil::Double2StringA(double dNumber, unsigned long lPrecision)
{
	std::ostringstream strStream;
	
	if(lPrecision <= 0)
	{
		strStream << dNumber;
	}
	else
	{
		strStream << std::fixed;
		strStream << std::setprecision(lPrecision) << dNumber;
	}

	return strStream.str();
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// String case converters, upper <-> lower
//////////////////////////////////////////////////////////////////////////
wstring CUtil::ToUpper(const wstring& str)
{
	wstring strUpper = str;
	std::transform(strUpper.begin(), strUpper.end(), strUpper.begin(), toupper);
	return strUpper;
}

string CUtil::ToUpper(const string& str)
{
	string strUpper = str;
	std::transform(strUpper.begin(), strUpper.end(), strUpper.begin(), toupper);
	return strUpper;
}

wstring CUtil::ToLower(const wstring& str)
{
	wstring strLower = str;
	std::transform(strLower.begin(), strLower.end(), strLower.begin(), tolower);
	return strLower;
}

string CUtil::ToLower(const string& str)
{
	string strLower = str;
	std::transform(strLower.begin(), strLower.end(), strLower.begin(), tolower);
	return strLower;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Compare without case sensitivity
//////////////////////////////////////////////////////////////////////////
int CUtil::CompareNoCase(const wstring& str1, const wstring& str2)
{
	return ToUpper(str1).compare(ToUpper(str2));
}

int CUtil::CompareNoCase(const string& str1, const string& str2)
{
	return ToUpper(str1).compare(ToUpper(str2));
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// System time retrieval
//////////////////////////////////////////////////////////////////////////
wstring CUtil::GetSystemTimeW(const wstring& strDateFormat /* = L"%A, %d/%m/%Y, %H:%M:%S - %Z"*/)
{
	time_t ttCurrentTime = 0;
	time(&ttCurrentTime);

	return GetSystemTimeW(ttCurrentTime, strDateFormat);
}

string CUtil::GetSystemTimeA(const string& strDateFormat /* = "%A, %d/%m/%Y, %H:%M:%S - %Z"*/)
{
	time_t ttCurrentTime = 0;
	time(&ttCurrentTime);

	return GetSystemTimeA(ttCurrentTime, strDateFormat);
}

wstring CUtil::GetSystemTimeW(const time_t& ttTime, const wstring& strDateFormat /* = L"%A, %d/%m/%Y, %H:%M:%S - %Z"*/)
{
	return GetStringW(GetSystemTimeA(ttTime, GetStringA(strDateFormat)));
}

string CUtil::GetSystemTimeA(const time_t& ttTime, const string& strDateFormat /* = "%A, %d/%m/%Y, %H:%M:%S - %Z"*/)
{
	string strTime = "";
	tm tmTime;

	if(localtime_s(&tmTime, &ttTime) == 0)
	{
		char strBuffer[MAX_PATH];

		strftime(strBuffer, sizeof(strBuffer), strDateFormat.c_str(), &tmTime);

		strTime = strBuffer;
	}

	return strTime;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Convert given number of seconds to hh:mm:ss and vice versa
//////////////////////////////////////////////////////////////////////////
wstring CUtil::GetTimeW(long lSeconds)
{
	return GetStringW(GetTimeA(lSeconds));
}

string CUtil::GetTimeA(long lSeconds)
{
	char strBuffer[16];
	sprintf_s(strBuffer, 16, "%02d:%02d:%02d\0", lSeconds / 3600, (lSeconds % 3600) / 60 , lSeconds % 60);

	return string(strBuffer);
}

long CUtil::GetTimeW(const wstring& strTime /* = L"00:00:00"*/)
{
	return GetTimeA(GetStringA(strTime));
}

long CUtil::GetTimeA(const string& strTime /* = "00:00:00"*/)
{
	if(GetTokenCount(strTime, ":") != 3)
	{
		return 0;
	}

	long lHour = atol(GetToken(strTime, ":", 0).c_str());
	long lMinute = atol(GetToken(strTime, ":", 1).c_str());
	long lSecond = atol(GetToken(strTime, ":", 2).c_str());

	return ((lHour * 60 * 60) + (lMinute * 60) + lSecond);
}

time_t CUtil::MakeTimeW(const wstring& strDateTime /* = L"00/00/0000, 00:00:00"*/)
{
	return MakeTimeA(GetStringA(strDateTime));
}

time_t CUtil::MakeTimeA(const string& strDateTime /* = "00/00/0000, 00:00:00"*/)
{
	string strDate = GetToken(strDateTime, ",", 0);
	RemoveWhiteSpaces(strDate);

	string strTime = GetToken(strDateTime, ",", 1);
	RemoveWhiteSpaces(strTime);

	tm tmTime;
	tmTime.tm_isdst = 0;
	tmTime.tm_wday = 0;
	tmTime.tm_yday = 0;

	tmTime.tm_hour = atol(GetToken(strTime, ":", 0).c_str());
	tmTime.tm_min = atol(GetToken(strTime, ":", 1).c_str());
	tmTime.tm_sec = atol(GetToken(strTime, ":", 2).c_str());

	tmTime.tm_mday = atol(GetToken(strDate, "/", 0).c_str());
	tmTime.tm_mon = atol(GetToken(strDate, "/", 1).c_str()) - 1;
	tmTime.tm_year = atol(GetToken(strDate, "/", 2).c_str()) - 1900;

	return mktime(&tmTime);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Get commonly used directory paths
//////////////////////////////////////////////////////////////////////////
wstring CUtil::GetWorkingDirectoryW()
{
	wchar_t strBuffer[MAX_PATH];

	// Get the current working directory
	_wgetcwd(strBuffer, MAX_PATH);

	return wstring(strBuffer);
}

string CUtil::GetWorkingDirectoryA()
{
	char strBuffer[MAX_PATH];

	// Get the current working directory
	_getcwd(strBuffer, MAX_PATH);

	return string(strBuffer);
}

#ifdef WIN32
wstring CUtil::GetProgramDirectoryW()
{
	wchar_t strFilePath[MAX_PATH];

	wstring strProgramDirectory = L"";

	// Get the file path of the current executable
	if(::GetModuleFileNameW(NULL, strFilePath, MAX_PATH) != 0)
	{
#ifdef UNICODE
		strProgramDirectory = GetFileDirectory(wstring(strFilePath));
#else
		strProgramDirectory = GetStringW(GetFileDirectory(GetStringA(wstring(strFilePath))));
#endif
	}

	return strProgramDirectory;
}

string CUtil::GetProgramDirectoryA()
{
	char strFilePath[MAX_PATH];

	string strProgramDirectory = "";

	// Get the file path of the current executable
	if(::GetModuleFileNameA(NULL, strFilePath, MAX_PATH) != 0)
	{
#ifdef UNICODE
	strProgramDirectory = GetStringA(GetFileDirectory(GetStringW(string(strFilePath))));
#else
	strProgramDirectory = GetFileDirectory(string(strFilePath));
#endif
	}

	return strProgramDirectory;
}
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetProgramFilesDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), _T("ProgramFilesDir"));

	// Check if a path was found in the registry
	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetWindowsDirectory()
{
	_tstring strDirectory = _T("");

	TCHAR strPath[MAX_PATH];

	// Use the API to get windows installation directory path
	HRESULT hr = SHGetFolderPath(NULL, CSIDL_WINDOWS | CSIDL_FLAG_CREATE, NULL, 0, strPath);

	if(SUCCEEDED(hr)) 
	{
		strDirectory = strPath;
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetSystemDirectory()
{
	_tstring strDirectory = _T("");

	TCHAR strPath[MAX_PATH];

	// Use the API to get windows system directory path
	HRESULT hr = SHGetFolderPath(NULL, CSIDL_SYSTEM | CSIDL_FLAG_CREATE, NULL, 0, strPath);

	if(SUCCEEDED(hr)) 
	{
		strDirectory = strPath;
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetMyDocumentsDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("Personal"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetMyMusicDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("My Music"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_MYMUSIC | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetMyPicturesDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("My Pictures"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_MYPICTURES | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetMyVideosDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("My Video"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_MYVIDEO | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetAppDataDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("AppData"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetLocalAppDataDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("Local AppData"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetDesktopDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("Desktop"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif

#ifdef WIN32
#ifdef _MFC_VER
_tstring CUtil::GetStartupDirectory()
{
	// For backward compatibility, check specific registry location for the path
	_tstring strDirectory = GetRegistryInfo(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("Startup"));

	if(strDirectory.compare(_T("")) == 0)
	{
		TCHAR strPath[MAX_PATH];

		// Since there was no path found in the registry, use the API
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_STARTUP | CSIDL_FLAG_CREATE, NULL, 0, strPath);

		if(SUCCEEDED(hr)) 
		{
			strDirectory = strPath;
		}
	}

	return strDirectory;
}
#endif
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Copy or cut a file into another directory, creating the directory path if necessary
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
bool CUtil::CopyFile2Directory(const _tstring& strSourceFilePath, const _tstring& strDestinationDirectory, bool bDeleteSource /* = false*/)
{
	if(strSourceFilePath.compare(_T("")) == 0)
	{
		return false;
	}

	if(strDestinationDirectory.compare(_T("")) == 0)
	{
		return false;
	}

	// Create the entire destination directory path, if not present
	MakeDirectory(strDestinationDirectory);

	_tstring strSourceFileName = GetFileName(strSourceFilePath, true);

	// Build the destination file path
	_tstring strDestinationFilePath = strDestinationDirectory;
	strDestinationFilePath = CUtil::AddDirectoryEnding(strDestinationFilePath);
	strDestinationFilePath += strSourceFileName;

	// Copy from source to destination
	if(!CopyFile(strSourceFilePath.c_str(), strDestinationFilePath.c_str(), FALSE))
	{
		return false;
	}
	else
	{
		if(bDeleteSource)
		{
			_tunlink(strSourceFilePath.c_str());
		}

		return true;
	}
}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Directory/File manipulation functions
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
void CUtil::GetFileList(const _tstring& strTargetDirectoryPath, const _tstring& strWildcard, bool bLookInSubdirectories, vector<_tstring>& vecstrFileList)
{
	// Check whether target directory string is empty
	if(strTargetDirectoryPath.compare(_T("")) == 0)
	{
		return;
	}

	// Remove "\\" if present at the end of the target directory
	// Then make a copy of it and use as the current search directory
	_tstring strCurrentDirectory = RemoveDirectoryEnding(strTargetDirectoryPath);

	// This data structure stores information about the file/folder that is found by any of these Win32 API functions:
	// FindFirstFile, FindFirstFileEx, or FindNextFile function
	WIN32_FIND_DATA fdDesktop = {0};

	// Format and copy the current directory
	// Note the addition of the wildcard *.*, which represents all files
	// 
	// Below is a list of wildcards that you can use
	// * (asterisk)			- represents zero or more characters at the current character position
	// ? (question mark)	- represents a single character
	//
	// Modify this function so that the function can take in a search pattern with wildcards and use it in the line below to find for e.g. only *.mpg files
	//
	// "\\?\" prefix to the file path means that the file system supports large paths/filenames
	_tstring strDesktopPath = _T("");
	strDesktopPath += _T("\\\\?\\");
	strDesktopPath += strCurrentDirectory;
	strDesktopPath = AddDirectoryEnding(strDesktopPath);

	if(strWildcard.compare(_T("")) == 0)
	{
		strDesktopPath += _T("*.*");
	}
	else
	{
		strDesktopPath += strWildcard;
	}

	// Finds the first file and populates the WIN32_FIND_DATA data structure with its information
	// The return value is a search handle used in subsequent calls to FindNextFile or FindClose functions
	HANDLE hDesktop = ::FindFirstFile(strDesktopPath.c_str(), &fdDesktop);	

	// If an invalid handle is returned by FindFirstFile function, then the directory is empty, so nothing to do but quit
	if(hDesktop == INVALID_HANDLE_VALUE)
	{
		return;
	}

	// Do this on the first file found and repeat for every next file found until all the required files that match the search pattern are found
	do 
	{
		// Reconstruct the path
		_tstring strPath = _T("");
		strPath += strCurrentDirectory;
		strPath = AddDirectoryEnding(strPath);
		strPath += fdDesktop.cFileName;

		// Check if a directory was found
		if(fdDesktop.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Get the name of the directory
			_tstring strCurrentDirectoryName = GetDirectoryName(strPath);

			// If its a current (.) or previous (..) directory indicator, just skip it
			if((strCurrentDirectoryName.compare(_T(".")) == 0) || (strCurrentDirectoryName.compare(_T("..")) == 0))
			{
				continue;
			}
			// Other wise this is a sub-directory
			else
			{
				// Check whether function was called to include sub-directories in the search
				if(bLookInSubdirectories)
				{
					// If sub-directories are to be searched as well, recursively call the function again, with the target directory as the sub-directory
					GetFileList(strPath, strWildcard, bLookInSubdirectories, vecstrFileList);
				}
			}
		}
		// A file was found
		else
		// if(fdDesktop.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
		{
			// Add the string to the vector
			vecstrFileList.push_back(_tstring(strPath));
		}
	}
	// Search for the next file that matches the search pattern
	while(::FindNextFile(hDesktop, &fdDesktop) == TRUE);

	// Close the search handle
	::FindClose(hDesktop);
}
#endif

#if WIN32
void CUtil::RenameFiles(const _tstring& strTargetDirectoryPath, const _tstring& strWildcard, bool bLookInSubdirectories, const _tstring& strFind, const _tstring& strReplace)
{
	// Check whether target directory string is empty
	if(strTargetDirectoryPath.compare(_T("")) == 0)
	{
		return;
	}

	// Check whether substring to find is empty
	if(strFind.compare(_T("")) == 0)
	{
		return;
	}

	// Vector of file paths
	vector<_tstring> vecstrFileList;
	vecstrFileList.clear();

	// Get the list of files
	GetFileList(strTargetDirectoryPath, strWildcard, bLookInSubdirectories, vecstrFileList);

	long lIndex = 0;
	long lCount = (long) vecstrFileList.size();

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		_tstring strOldFilePath = vecstrFileList[lIndex];

		_tstring strFileName = GetFileName(strOldFilePath, true);

		FindReplace(strFileName, strFind, strReplace);

		_tstring strNewFilePath = GetFileDirectory(strOldFilePath);
		strNewFilePath = AddDirectoryEnding(strNewFilePath);
		strNewFilePath += strFileName;

		_trename(strOldFilePath.c_str(), strNewFilePath.c_str());
	}
}
#endif

#ifdef WIN32
void CUtil::MakeDirectory(const _tstring& strDirectoryPath)
{
	if(strDirectoryPath.compare(_T("")) == 0)
	{
		return;
	}

	_tstring strDirectoryPathCopy = RemoveDirectoryEnding(strDirectoryPath);

	long lIndex = 0;
	long lCount = GetTokenCount(strDirectoryPathCopy, DIRECTORY_SEPARATOR);

	_tstring strDirectory2Make = GetToken(strDirectoryPathCopy, DIRECTORY_SEPARATOR, 0);

	for(lIndex = 1; lIndex < lCount; lIndex++)
	{
		strDirectory2Make += DIRECTORY_SEPARATOR;
		strDirectory2Make += GetToken(strDirectoryPathCopy, DIRECTORY_SEPARATOR, lIndex);

		CreateDirectory(strDirectory2Make.c_str(), NULL);
		SetFileAttributes(strDirectory2Make.c_str(), FILE_ATTRIBUTE_NORMAL);
	}
}
#endif

#ifdef WIN32
void CUtil::DeleteDirectory(const _tstring& strTargetDirectoryPath)
{
	// Check whether target directory string is empty
	if(strTargetDirectoryPath.compare(_T("")) == 0)
	{
		return;
	}

	// Remove "\\" if present at the end of the target directory
	// Then make a copy of it and use as the current search directory
	_tstring strCurrentDirectory = RemoveDirectoryEnding(strTargetDirectoryPath);

	// This data structure stores information about the file/folder that is found by any of these Win32 API functions:
	// FindFirstFile, FindFirstFileEx, or FindNextFile function
	WIN32_FIND_DATA fdDesktop = {0};

	// Format and copy the current directory
	// Note the addition of the wildcard *.*, which represents all files
	// 
	// Below is a list of wildcards that you can use
	// * (asterisk)			- represents zero or more characters at the current character position
	// ? (question mark)	- represents a single character
	//
	// Modify this function so that the function can take in a search pattern with wildcards and use it in the line below to find for e.g. only *.mpg files
	//
	// "\\?\" prefix to the file path means that the file system supports large paths/filenames
	_tstring strDesktopPath = _T("");
	strDesktopPath += _T("\\\\?\\");
	strDesktopPath += strCurrentDirectory;
	strDesktopPath = AddDirectoryEnding(strDesktopPath);
	strDesktopPath += _T("*.*");

	_tstring strDirectory2Delete = _T("");
	strDirectory2Delete += _T("\\\\?\\");
	strDirectory2Delete += strCurrentDirectory;

	// Finds the first file and populates the WIN32_FIND_DATA data structure with its information
	// The return value is a search handle used in subsequent calls to FindNextFile or FindClose functions
	HANDLE hDesktop = ::FindFirstFile(strDesktopPath.c_str(), &fdDesktop);	

	// If an invalid handle is returned by FindFirstFile function, then the directory is empty, so delete directory and quit
	if(hDesktop == INVALID_HANDLE_VALUE)
	{
		// Remove the directory
		RemoveDirectory(strDirectory2Delete.c_str());

		return;
	}

	// Do this on the first file found and repeat for every next file found until all the required files that match the search pattern are found
	do 
	{
		// Reconstruct the path
		_tstring strPath = _T("");
		strPath += strCurrentDirectory;
		strPath = AddDirectoryEnding(strPath);
		strPath += fdDesktop.cFileName;

		// Check if a directory was found
		if(fdDesktop.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Get the name of the directory
			_tstring strCurrentDirectoryName = GetDirectoryName(strPath);

			// If its a current (.) or previous (..) directory indicator, just skip it
			if((strCurrentDirectoryName.compare(_T(".")) == 0) || (strCurrentDirectoryName.compare(_T("..")) == 0))
			{
				continue;
			}
			// Other wise this is a sub-directory
			else
			{
				// Recursively call the function again, with the target directory as the sub-directory
				DeleteDirectory(strPath);
			}
		}
		// A file was found
		else
		// if(fdDesktop.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
		{
			// Delete the file
			_tunlink(strPath.c_str());
		}
	}
	// Search for the next file that matches the search pattern
	while(::FindNextFile(hDesktop, &fdDesktop) == TRUE);

	// Close the search handle
	::FindClose(hDesktop);

	// Remove the directory
	RemoveDirectory(strDirectory2Delete.c_str());
}
#endif

#ifdef WIN32
void CUtil::DeleteFiles(const _tstring& strTargetDirectoryPath, const _tstring& strWildcard, bool bLookInSubdirectories)
{
	vector<_tstring> vecstrFileList;

	GetFileList(strTargetDirectoryPath, strWildcard, bLookInSubdirectories, vecstrFileList);

	long lFileIndex = 0;
	long lFileCount = (long) vecstrFileList.size();

	for(lFileIndex = 0; lFileIndex < lFileCount; lFileIndex++)
	{
		_tunlink(vecstrFileList.at(lFileIndex).c_str());
	}
}
#endif

#ifdef WIN32
bool CUtil::IsDirectory(const _tstring& strDirectoryPath)
{
	if(strDirectoryPath.compare(_T("")) == 0)
	{
		return false;
	}

	CreateDirectory(strDirectoryPath.c_str(), NULL);

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return true;
	}

	return false;
}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Directory/File path string manipulation functions
//////////////////////////////////////////////////////////////////////////
wstring CUtil::AddDirectoryEnding(const wstring& strDirectoryPath)
{
	if(strDirectoryPath.compare(L"") == 0)
	{
		return wstring(L"");
	}

	wstring strDirectory = strDirectoryPath;

	if(strDirectory[strDirectory.size() - 1] != DIRECTORY_SEPARATOR_W_C)
	{
		strDirectory += DIRECTORY_SEPARATOR_W;
	}

	return strDirectory;
}

string CUtil::AddDirectoryEnding(const string& strDirectoryPath)
{
	if(strDirectoryPath.compare("") == 0)
	{
		return string("");
	}

	string strDirectory = strDirectoryPath;

	if(strDirectory[strDirectory.size() - 1] != DIRECTORY_SEPARATOR_A_C)
	{
		strDirectory += DIRECTORY_SEPARATOR_A;
	}

	return strDirectory;
}

wstring CUtil::RemoveDirectoryEnding(const wstring& strDirectoryPath)
{
	if(strDirectoryPath.compare(L"") == 0)
	{
		return wstring(L"");
	}

	wstring strDirectory = strDirectoryPath;

	if(strDirectory[strDirectory.size() - 1] == DIRECTORY_SEPARATOR_W_C)
	{
		strDirectory.erase(strDirectory.size() - 1);
	}

	return strDirectory;
}

string CUtil::RemoveDirectoryEnding(const string& strDirectoryPath)
{
	if(strDirectoryPath.compare("") == 0)
	{
		return string("");
	}

	string strDirectory = strDirectoryPath;

	if(strDirectory[strDirectory.size() - 1] == DIRECTORY_SEPARATOR_A_C)
	{
		strDirectory.erase(strDirectory.size() - 1);
	}

	return strDirectory;
}

wstring CUtil::GetRootDirectory(const wstring& strDirectoryPath)
{
	if(strDirectoryPath.compare(L"") == 0)
	{
		return wstring(L"");
	}

	wstring strRootDirectory = RemoveDirectoryEnding(strDirectoryPath);

	size_t i64Index = strRootDirectory.find_last_of(DIRECTORY_SEPARATOR_W);

	if(i64Index == wstring::npos)
	{
		return wstring(L"");
	}

	strRootDirectory.erase(i64Index);

	return strRootDirectory;
}

string CUtil::GetRootDirectory(const string& strDirectoryPath)
{
	if(strDirectoryPath.compare("") == 0)
	{
		return string("");
	}

	string strRootDirectory = RemoveDirectoryEnding(strDirectoryPath);

	size_t i64Index = strRootDirectory.find_last_of(DIRECTORY_SEPARATOR_A);

	if(i64Index == string::npos)
	{
		return string("");
	}

	strRootDirectory.erase(i64Index);

	return strRootDirectory;
}

wstring CUtil::GetDirectoryName(const wstring& strDirectoryPath)
{
	if(strDirectoryPath.compare(L"") == 0)
	{
		return wstring(L"");
	}

	wstring strDirectoryName = RemoveDirectoryEnding(strDirectoryPath);

	size_t i64Index = strDirectoryName.find_last_of(DIRECTORY_SEPARATOR_W);

	if(i64Index == wstring::npos)
	{
		return wstring(L"");
	}

	strDirectoryName.erase(0, i64Index + 1);

	return strDirectoryName;
}

string CUtil::GetDirectoryName(const string& strDirectoryPath)
{
	if(strDirectoryPath.compare("") == 0)
	{
		return string("");
	}

	string strDirectoryName = RemoveDirectoryEnding(strDirectoryPath);

	size_t i64Index = strDirectoryName.find_last_of(DIRECTORY_SEPARATOR_A);

	if(i64Index == string::npos)
	{
		return string("");
	}

	strDirectoryName.erase(0, i64Index + 1);

	return strDirectoryName;
}

wstring CUtil::GetFileDirectory(const wstring& strFilePath)
{
	if(strFilePath.compare(L"") == 0)
	{
		return wstring(L"");
	}

	wstring strDirectory = RemoveDirectoryEnding(strFilePath);

	size_t i64Index = strDirectory.find_last_of(DIRECTORY_SEPARATOR_W);

	if(i64Index == wstring::npos)
	{
		return wstring(L"");
	}

	strDirectory.erase(i64Index);

	return strDirectory;
}

string CUtil::GetFileDirectory(const string& strFilePath)
{
	if(strFilePath.compare("") == 0)
	{
		return string("");
	}

	string strDirectory = RemoveDirectoryEnding(strFilePath);

	size_t i64Index = strDirectory.find_last_of(DIRECTORY_SEPARATOR_A);

	if(i64Index == string::npos)
	{
		return string("");
	}

	strDirectory.erase(i64Index);

	return strDirectory;
}

wstring CUtil::GetFileName(const wstring& strFilePath, bool bIncludeExtension /* = false*/)
{
	if(strFilePath.compare(L"") == 0)
	{
		return wstring(L"");
	}

	wstring strFileName = strFilePath;

	size_t i64Index = strFileName.find_last_of(DIRECTORY_SEPARATOR_W);

	if(i64Index != wstring::npos)
	{
		strFileName.erase(0, i64Index + 1);
	}

	if(!bIncludeExtension)
	{
		i64Index = strFileName.find_last_of(L".");

		if(i64Index == wstring::npos)
		{
			return strFileName;
		}

		strFileName.erase(i64Index);
	}

	return strFileName;
}

string CUtil::GetFileName(const string& strFilePath, bool bIncludeExtension /* = false*/)
{
	if(strFilePath.compare("") == 0)
	{
		return string("");
	}

	string strFileName = strFilePath;

	size_t i64Index = strFileName.find_last_of(DIRECTORY_SEPARATOR_A);

	if(i64Index != string::npos)
	{
		strFileName.erase(0, i64Index + 1);
	}

	if(!bIncludeExtension)
	{
		i64Index = strFileName.find_last_of(".");

		if(i64Index == string::npos)
		{
			return strFileName;
		}

		strFileName.erase(i64Index);
	}

	return strFileName;
}

wstring CUtil::GetFileExtension(const wstring& strFilePath, bool bIncludeDot /* = false*/)
{
	if(strFilePath.compare(L"") == 0)
	{
		return wstring(L"");
	}

	wstring strFileExtension = strFilePath;

	size_t i64Index = strFileExtension.find_last_of(L".");

	if(i64Index == wstring::npos)
	{
		return wstring(L"");
	}

	if(!bIncludeDot)
	{
		strFileExtension.erase(0, i64Index + 1);
	}
	else
	{
		strFileExtension.erase(0, i64Index);
	}

	if(strFileExtension.compare(strFilePath) == 0)
	{
		strFileExtension = L"";
	}

	return strFileExtension;
}

string CUtil::GetFileExtension(const string& strFilePath, bool bIncludeDot /* = false*/)
{
	if(strFilePath.compare("") == 0)
	{
		return string("");
	}

	string strFileExtension = strFilePath;

	size_t i64Index = strFileExtension.find_last_of(".");

	if(i64Index == string::npos)
	{
		return string("");
	}

	if(!bIncludeDot)
	{
		strFileExtension.erase(0, i64Index + 1);
	}
	else
	{
		strFileExtension.erase(0, i64Index);
	}

	if(strFileExtension.compare(strFilePath) == 0)
	{
		strFileExtension = "";
	}

	return strFileExtension;
}

wstring CUtil::GetFileNamePrefix(const wstring& strFilePath, const wstring& strDelimiter)
{
	wstring strFileName = GetFileName(strFilePath);

	wstring strFileNamePrefix = strFileName;

	size_t i64Index = strFileNamePrefix.find_last_of(strDelimiter);

	if(i64Index != wstring::npos)
	{
		strFileNamePrefix.erase(i64Index);
	}

	if(i64Index <= 0 || i64Index >= strFileName.size() - 1)
	{
		strFileNamePrefix = strFileName;
	}

	return strFileNamePrefix;
}

string CUtil::GetFileNamePrefix(const string& strFilePath, const string& strDelimiter)
{
	string strFileName = GetFileName(strFilePath);

	string strFileNamePrefix = strFileName;

	size_t i64Index = strFileNamePrefix.find_last_of(strDelimiter);

	if(i64Index != string::npos)
	{
		strFileNamePrefix.erase(i64Index);
	}

	if(i64Index <= 0 || i64Index >= strFileName.size() - 1)
	{
		strFileNamePrefix = strFileName;
	}

	return strFileNamePrefix;
}

wstring CUtil::GetFileNameSuffix(const wstring& strFilePath, const wstring& strDelimiter)
{
	wstring strFileName = GetFileName(strFilePath);

	wstring strFileNameSuffix = strFileName;

	size_t i64Index = strFileNameSuffix.find_last_of(strDelimiter);

	if(i64Index != wstring::npos)
	{
		strFileNameSuffix.erase(0, i64Index + 1);
	}

	if(i64Index <= 0 || i64Index >= strFileName.size() - 1)
	{
		strFileNameSuffix = strFileName;
	}

	return strFileNameSuffix;
}

string CUtil::GetFileNameSuffix(const string& strFilePath, const string& strDelimiter)
{
	string strFileName = GetFileName(strFilePath);

	string strFileNameSuffix = strFileName;

	size_t i64Index = strFileNameSuffix.find_last_of(strDelimiter);

	if(i64Index != string::npos)
	{
		strFileNameSuffix.erase(0, i64Index + 1);
	}

	if(i64Index <= 0 || i64Index >= strFileName.size() - 1)
	{
		strFileNameSuffix = strFileName;
	}

	return strFileNameSuffix;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// String type converters, ANSI <-> UNICODE
//////////////////////////////////////////////////////////////////////////
char CUtil::Wide2Narrow(wchar_t wch)
{
	// Simple type cast works because UNICODE incorporates ASCII into itself
	char ch = (char) wch;
	return ch;
}

wchar_t CUtil::Narrow2Wide(char ch)
{
	// Simple type cast works because UNICODE incorporates ASCII into itself
	wchar_t wch = (wchar_t) ch;
	return wch;
}

wstring CUtil::GetStringW(const string& strA)
{
	wstring strW = L"";
	strW.resize(strA.size(), L'\0');

	std::transform(strA.begin(), strA.end(), strW.begin(), Narrow2Wide);

	return strW;

	/*
	BSTR bstrW = NULL;
	wstring strW = L"";

	long lStringLengthA = (long) strA.size();
	long lStringLengthW = ::MultiByteToWideChar(CP_ACP, 0, strA.c_str(), lStringLengthA, 0, 0);
	
	// Check whether conversion is successful
	if(lStringLengthW > 0)
	{
		bstrW = ::SysAllocStringLen(0, lStringLengthW);
		::MultiByteToWideChar(CP_ACP, 0, strA.c_str(), lStringLengthA, bstrW, lStringLengthW);

		strW = bstrW;
	}

	// free the BSTR
	::SysFreeString(bstrW);

	return strW;
	*/
}

wstring CUtil::GetString(const string& strA)
{
	return GetStringW(strA);
}

string CUtil::GetStringA(const wstring& strW)
{
	string strA = "";
	strA.resize(strW.size(), '\0');

	std::transform(strW.begin(), strW.end(), strA.begin(), Wide2Narrow);

	return strA;

	/*
	char* pstrA = NULL;
	string strA = "";

	long lStringLengthW = (long) strW.size();
	long lStringLengthA = ::WideCharToMultiByte(CP_ACP, 0, strW.c_str(), lStringLengthW, 0, 0, NULL, NULL);

	// Check whether conversion is successful
	if(lStringLengthA > 0)
	{
		pstrA = new char[lStringLengthA + 1];	// allocate a final null terminator as well
		::WideCharToMultiByte(CP_ACP, 0, strW.c_str(), lStringLengthW, pstrA, lStringLengthA, NULL, NULL);
		pstrA[lStringLengthA] = 0;	// Set the null terminator

		strA = pstrA;
	}

	// free the memory
	delete[] pstrA;

	return strA;
	*/
}

string CUtil::GetString(const wstring& strW)
{
	return GetStringA(strW);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Registry
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
_tstring CUtil::GetRegistryInfo(HKEY hkey, const _tstring& strRegistryPath, const _tstring& strValueName)
{
	CRegKey reg;
	_tstring strKey = _T("");

	if(reg.Open(hkey, strRegistryPath.c_str(), KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR strKeyBuffer[MAX_PATH * 4];
		unsigned long ulKeyBufferSize = MAX_PATH * 4;

		if(reg.QueryStringValue(strValueName.c_str(), strKeyBuffer, &ulKeyBufferSize) == ERROR_SUCCESS)
		{
			strKey = strKeyBuffer;
		}

		reg.Close();
	}

	return strKey;
}

DWORD CUtil::GetRegistryValue(HKEY hkey, const _tstring& strRegistryPath, const _tstring& strValueName)
{
	CRegKey reg;
	DWORD dwKey = 0;

	if(reg.Open(hkey, strRegistryPath.c_str(), KEY_READ) == ERROR_SUCCESS)
	{
		if(reg.QueryDWORDValue(strValueName.c_str(), dwKey) == ERROR_SUCCESS)
		{
			// do nothing!
		}

		reg.Close();
	}

	return dwKey;
}

bool CUtil::SetRegistryInfo(HKEY hkey, const _tstring& strRegistryPath, const _tstring& strValueName, const _tstring& strValue)
{
	CRegKey reg;
	bool bSuccess = false;

	if(reg.Open(hkey, strRegistryPath.c_str(), KEY_WRITE) == ERROR_SUCCESS)
	{
		if(reg.SetStringValue(strValueName.c_str(), strValue.c_str(), REG_SZ) == ERROR_SUCCESS)
		{
			bSuccess = true;

			reg.Flush();
		}

		reg.Close();
	}

	return bSuccess;
}

bool CUtil::SetRegistryValue(HKEY hkey, const _tstring& strRegistryPath, const _tstring& strValueName, DWORD dwValue)
{
	CRegKey reg;
	bool bSuccess = false;

	if(reg.Open(hkey, strRegistryPath.c_str(), KEY_WRITE) == ERROR_SUCCESS)
	{
		if(reg.SetDWORDValue(strValueName.c_str(), dwValue) == ERROR_SUCCESS)
		{
			bSuccess = true;

			reg.Flush();
		}

		reg.Close();
	}

	return bSuccess;
}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Internet
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
LONG CUtil::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR regdata)
{
	HKEY hkey;
	long lResult = 0;

	lResult = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

	if(lResult == ERROR_SUCCESS)
	{
		TCHAR strDataBuffer[MAX_PATH * 4];
		long lDataSize = MAX_PATH * 4;

		RegQueryValue(hkey, NULL, strDataBuffer, &lDataSize);
		
		lstrcpy(regdata, strDataBuffer);
		
		RegCloseKey(hkey);
	}

	return lResult;
}
#endif

#ifdef WIN32
HINSTANCE CUtil::OpenURL(LPCTSTR strURL)
{
	TCHAR strkey[MAX_PATH * 2];

	// First try ShellExecute()
	HINSTANCE hinst = ShellExecute(NULL, _T("open"), strURL, NULL, NULL, SW_SHOW);

	// If it failed, get the .htm registry key and lookup the program
	if((INT_PTR) hinst <= HINSTANCE_ERROR)
	{
		if(GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), strkey) == ERROR_SUCCESS)
		{
			lstrcat(strkey, _T("\\shell\\open\\command"));

			if(GetRegKey(HKEY_CLASSES_ROOT, strkey, strkey) == ERROR_SUCCESS)
			{
				TCHAR *pos;
				
				pos = _tcsstr(strkey, _T("\"%1\""));
				
				if(pos == NULL)		// No quotes found
				{
					pos = _tcsstr(strkey, _T("%1"));		// Check for %1, without quotes 
					
					if(pos == NULL)						// No parameter at all...
					{
						pos = strkey + lstrlen(strkey) - 1;
					}
					else
					{
						*pos = _T('\0');				// Remove the parameter
					}
				}
				else
				{
					*pos = _T('\0');					// Remove the parameter
				}

				lstrcat(pos, _T(" "));
				lstrcat(pos, strURL);

				USES_CONVERSION;
				hinst = (HINSTANCE) (INT_PTR) WinExec(T2A(strkey), SW_SHOW);
			}
		}
	}

	return hinst;
}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Execution
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
bool CUtil::Execute(const _tstring& strFilePath, const _tstring& strParameters /* = _T("")*/, bool bShow /* = true*/, bool bWait /* = false*/)
{
	SHELLEXECUTEINFO SEI;

	SEI.cbSize = sizeof(SHELLEXECUTEINFO);
	SEI.fMask = SEE_MASK_NOCLOSEPROCESS;
	SEI.hwnd = NULL;
	SEI.lpVerb = _T("open");

	SEI.lpFile = strFilePath.c_str();
	SEI.lpParameters = strParameters.c_str();
	SEI.lpDirectory = NULL;

	if(bShow)
	{
		SEI.nShow = SW_SHOWNORMAL;
	}
	else
	{
		SEI.nShow = SW_HIDE;
	}

	SEI.hInstApp = NULL;

	if(!::ShellExecuteEx(&SEI))
	{
		return false;
	}
	else
	{
		if(bWait)
		{
			::WaitForSingleObject(SEI.hProcess, INFINITE);
		}
	}

	// Release handles
	CloseHandle(SEI.hProcess);

	return true;
}
#endif

#ifdef WIN32
bool CUtil::Execute(const _tstring& strCommandLine, bool bShow /* = true*/, bool bWait /* = false*/)
{
	STARTUPINFO SI;
	memset(&SI, 0, sizeof(SI));
	SI.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION PI;
	memset(&PI, 0, sizeof(PI));

	DWORD dwCreationFlags = 0;

	if(bShow)
	{
		dwCreationFlags = CREATE_NEW_CONSOLE;
	}
	else
	{
		dwCreationFlags = CREATE_NO_WINDOW;
	}

	if(!::CreateProcess(NULL, (LPTSTR) strCommandLine.c_str(), NULL, NULL, TRUE, dwCreationFlags, NULL, NULL, &SI, &PI))
	{
		return false;
	}
	else
	{
		if(bWait)
		{
			::WaitForSingleObject(PI.hProcess, INFINITE);
		}
	}

	// Release handles
	CloseHandle(PI.hProcess);
	CloseHandle(PI.hThread); 

	return true;
}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// System
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
long CUtil::GetProcessorCount()
{
	SYSTEM_INFO SystemInfo;

	::GetSystemInfo(&SystemInfo);

	long lProcessorCount = SystemInfo.dwNumberOfProcessors;

	return lProcessorCount;
}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Random generators
//////////////////////////////////////////////////////////////////////////
wstring CUtil::GenerateRandomStringW(long lMaxLength, bool bIncludeAlpha /* = true*/, bool bIncludeNumbers /* = true*/)
{
	return GetStringW(GenerateRandomStringA(lMaxLength, bIncludeAlpha, bIncludeNumbers));
}

string CUtil::GenerateRandomStringA(long lMaxLength, bool bIncludeAlpha /* = true*/, bool bIncludeNumbers /* = true*/)
{
	string strRamdom = "";

	long lLength = lMaxLength + 1;

	while(lLength > lMaxLength)
	{
		lLength = rand() + 1;
	}

	long lIndex = 0;
	long lCount = lLength;

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		while(true)
		{
			char cRandom = char (rand() / 128);

			if(bIncludeAlpha && bIncludeNumbers)
			{
				if(isalnum(cRandom))
				{
					strRamdom += cRandom;

					break;
				}
			}
			else
			if(bIncludeAlpha)
			{
				if(isalpha(cRandom))
				{
					strRamdom += cRandom;

					break;
				}
			}
			else
			if(bIncludeNumbers)
			{
				if(isdigit(cRandom))
				{
					strRamdom += cRandom;

					break;
				}
			}
			else
			{
				strRamdom += cRandom;

				break;
			}
		}
	}

	return strRamdom;
}

// Generate random number between A & B
float CUtil::GetRandomNumber(float fA, float fB, float fGrain /* = 10.0f*/)
{
	if(fA == fB)
	{
		return 0.0;
	}

	long lRandomNumber = rand() * rand();

	float fRange = abs(fA - fB);
	int iDivisor = (int) ((fRange * fGrain) + 1.0f);

	if(fA < fB)
	{
		return (((float) (lRandomNumber % iDivisor) / fGrain) + (abs(fA)));
	}
	else
	{
		return (((float) (lRandomNumber % iDivisor) / fGrain) + (abs(fB)));
	}
}

double CUtil::GetRandomNumber(double dA, double dB, double dGrain /* = 10.0*/)
{
	if(dA == dB)
	{
		return 0.0;
	}

	long lRandomNumber = rand() * rand();

	double dRange = abs(dA - dB);
	int iDivisor = (int) ((dRange * dGrain) + 1.0);

	if(dA < dB)
	{
		return (((double) (lRandomNumber % iDivisor) / dGrain) + (abs(dA)));
	}
	else
	{
		return (((double) (lRandomNumber % iDivisor) / dGrain) + (abs(dB)));
	}
}

bool CUtil::RollDice(double dProbability)
{
	long lDivisor = 1000000;

	if(dProbability <= (1.0 / lDivisor))
	{
		return false;
	}

	if(dProbability >= (1.0 - (1.0 / lDivisor)))
	{
		return true;
	}

	long lRandomNumber = rand() * rand();

	long lRequiredRange = (long) (dProbability * lDivisor);

	return ((lRandomNumber % lDivisor) <= lRequiredRange);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Self destruct
//////////////////////////////////////////////////////////////////////////
void CUtil::SelfDestruct()
{
	// This is the name of the temporary BAT file
	const char strTemporaryBATFileName[] = "_SelfDestruct.bat";

	// Temporary .bat file data
	static char strTemporaryBATFileData[] = 
											":Repeat\r\n"
											"del \"%s\"\r\n"
											"if exist \"%s\" goto Repeat\r\n"
											"rmdir \"%s\"\r\n"
											"del \"%s\"";


	char strModuleName[_MAX_PATH];		// absolute path of calling EXE file

	char strTemporaryBATFilePath[_MAX_PATH];	// absolute path of temporary BAT file

	char strFolder[_MAX_PATH];

	GetTempPathA(_MAX_PATH, strTemporaryBATFilePath) ;
	strcat_s(strTemporaryBATFilePath, _MAX_PATH, strTemporaryBATFileName);

	GetModuleFileNameA(NULL, strModuleName, MAX_PATH);
	strcpy_s(strFolder, _MAX_PATH, strModuleName);

	char* pb = strrchr(strFolder, '\\');

	if(pb != NULL)
	{
		*pb = NULL;
	}

	long lBATFileDataSize = (long) (strlen(strTemporaryBATFileData) + strlen(strModuleName) * 2 + strlen(strTemporaryBATFilePath) + _MAX_PATH);
	char* pstrBATFileData = (char*) alloca(lBATFileDataSize);
	sprintf_s(pstrBATFileData, lBATFileDataSize, strTemporaryBATFileData, strModuleName, strModuleName, strFolder, strTemporaryBATFilePath);

	WriteFile(string(strTemporaryBATFilePath), string(pstrBATFileData));

	ShellExecuteA(NULL, "open", strTemporaryBATFilePath, NULL, NULL, SW_HIDE);

	free(pstrBATFileData);

	// Delete the parent folder
	DeleteDirectory(GetProgramDirectory());
}
//////////////////////////////////////////////////////////////////////////
