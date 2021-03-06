///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////////

#include "FolderCompare.h"

#include "Util.h"
#include "INIFile.h"

//////////////////////////////////////////////////////////////////////////

void helpstring();

///////////////////////////////////////////////////////////////////////////////////

int
__cdecl
_tmain
(
	int argc,
	_TCHAR *argv[]
)
{
	if( 2 != argc )
	{
		return 0;
	}
	else
	{
		string strInputINIFilePath = "";

#ifdef UNICODE
		strInputINIFilePath = CUtil::GetStringA(argv[1]);
#else
		strInputINIFilePath = argv[1];
#endif

		string strInputFolder1 = CINIFile::GetValue("InputFolder1", "MAIN", strInputINIFilePath);
		string strInputFolder2 = CINIFile::GetValue("InputFolder2", "MAIN", strInputINIFilePath);

		long lSearchLength = atol(CINIFile::GetValue("SearchLength", "MAIN", strInputINIFilePath).c_str());

		string strOutputFolder = CINIFile::GetValue("OutputFolder", "MAIN", strInputINIFilePath);

		long lThreadCount = atol(CINIFile::GetValue("Threads", "MAIN", strInputINIFilePath).c_str());

		string strWildcardFolder1 = CINIFile::GetValue("WildcardFolder1", "MAIN", strInputINIFilePath);
		string strWildcardFolder2 = CINIFile::GetValue("WildcardFolder2", "MAIN", strInputINIFilePath);

		bool bRemoveDelimiters = (CUtil::CompareNoCase(CINIFile::GetValue("RemoveDelimiters", "MAIN", strInputINIFilePath), "YES") == 0);
		bool bRemoveNewLines = (CUtil::CompareNoCase(CINIFile::GetValue("RemoveNewLines", "MAIN", strInputINIFilePath), "YES") == 0);
		bool bShowProgress = (CUtil::CompareNoCase(CINIFile::GetValue("ShowProgress", "MAIN", strInputINIFilePath), "YES") == 0);

		bool bIgnore0Similarities = (CUtil::CompareNoCase(CINIFile::GetValue("Ignore0Similarities", "MAIN", strInputINIFilePath), "YES") == 0);

		string strExcludeListFilePath = CINIFile::GetValue("ExcludeListFilePath", "MAIN", strInputINIFilePath);
		string strKeywordListFilePath = CINIFile::GetValue("KeywordListFilePath", "MAIN", strInputINIFilePath);
		
		long lThreshold = atol(CINIFile::GetValue("Threshold", "MAIN", strInputINIFilePath).c_str());

		CFolderCompare FC;

#ifdef UNICODE
		FC.SetData(	CUtil::GetStringW(strInputFolder1),
					CUtil::GetStringW(strInputFolder2),
					lSearchLength,
					CUtil::GetStringW(strOutputFolder),
					lThreadCount,
					CUtil::GetStringW(strWildcardFolder1),
					CUtil::GetStringW(strWildcardFolder2),
					bRemoveDelimiters,
					bRemoveNewLines,
					bShowProgress,
					CUtil::GetStringW(strExcludeListFilePath),
					CUtil::GetStringW(strKeywordListFilePath),
					lThreshold,
					bIgnore0Similarities);
#else
		FC.SetData(	strInputFolder1,
					strInputFolder2,
					lSearchLength,
					strOutputFolder,
					lThreadCount,
					strWildcardFolder1,
					strWildcardFolder2
					bRemoveDelimiters,
					bRemoveNewLines,
					bShowProgress,
					strExcludeListFilePath,
					strKeywordListFilePath,
					lThreshold,
					bIgnore0Similarities);
#endif

		FC.Start();

		while(FC.IsRunning())
		{
			Sleep(125);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////