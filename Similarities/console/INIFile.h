/*
//////////////////////////////////////////////////////////////////////////
COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:
//////////////////////////////////////////////////////////////////////////

CINIFile : Copyright (C) 2008, Shupantha Kazi Imam (shupantha@yahoo.com)

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

V 1.0
*/

#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <tchar.h>

#include <vector>
#include <string>

using std::vector;
using std::string;

using namespace std;

class CINIFile
{
public:
	CINIFile();
	virtual ~CINIFile();

public:
	struct Record
	{
		string	Comments;
		char	Commented;
		string	Section;
		string	Key;
		string	Value;
	};

	enum CommentChar
	{
		Pound = '#',
		SemiColon = ';'
	};

public:
	static bool AddSection(const string& strSectionName, const string& strFilePath);
	
	static bool CommentRecord(CommentChar cc, const string& strKeyName, const string& strSectionName, const string& strFilePath);
	static bool CommentSection(CommentChar cc, const string& strSectionName, const string& strFilePath);
	
	static string Content(const string& strFilePath);

	static bool Create(const string& strFilePath);

	static bool DeleteRecord(const string& strKeyName, const string& strSectionName, const string& strFilePath);
	static bool DeleteSection(const string& strSectionName, const string& strFilePath);

	static Record GetRecord(const string& strKeyName, const string& strSectionName, const string& strFilePath);
	static vector<Record> GetSection(const string& strSectionName, const string& strFilePath);

	static vector<string> GetSectionNames(const string& strFilePath);
	
	static string GetValue(const string& strKeyName, const string& strSectionName, const string& strFilePath);

	static bool RecordExists(const string& strKeyName, const string& strSectionName, const string& strFilePath);

	static bool RenameSection(const string& strOldSectionName, const string& strNewSectionName, const string& strFilePath);

	static bool SectionExists(const string& strSectionName, const string& strFilePath);
	
	static bool SetRecordComments(const string& strComments, const string& strKeyName, const string& strSectionName, const string& strFilePath);
	static bool SetSectionComments(const string& strComments, const string& strSectionName, const string& strFilePath);
	
	static bool SetValue(const string& strKeyName, const string& strValue, const string& strSectionName, const string& strFilePath);
	
	static bool Sort(const string& strFilePath, bool bDescending = false);
	
	static bool UnCommentRecord(const string& strKeyName, const string& strSectionName, const string& strFilePath);
	static bool UnCommentSection(const string& strSectionName, const string& strFileName);

private:
	//////////////////////////////////////////////////////////////////////////
	// String manipulation functions
	//////////////////////////////////////////////////////////////////////////
	// Returns the number of successful substrings replaced
	static long FindReplace(string& strSource, const string& strFind, const string& strReplace);

	// Removes/Replaces all white spaces
	static void RemoveWhiteSpaces(string& strSource, const string& strReplace = "");

	// A function to trim strings from both ends
	static void Trim(string& strSource, const string& strChars2Trim = " \t\r\n", bool bTrimLeft = true, bool bTrimRight = true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// String case converters, upper <-> lower
	//////////////////////////////////////////////////////////////////////////
	static string ToUpper(const string& str);
	static string ToLower(const string& str);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Compare without case sensitivity
	//////////////////////////////////////////////////////////////////////////
	static int CompareNoCase(const string& str1, const string& str2);
	//////////////////////////////////////////////////////////////////////////

	static vector<Record> GetSections(const string& strFilePath);

	static bool Load(const string& strFilePath, vector<Record>& vecRecords);	
	static bool Save(const string& strFilePath, vector<Record>& vecRecords);

	struct RecordSectionIs : std::unary_function<Record, bool>
	{
		string section_;

		RecordSectionIs(const string& section): section_(section){}

		bool operator() (const Record& rec) const
		{
			return (CompareNoCase(rec.Section, section_) == 0);
		}
	};

	struct RecordSectionKeyIs : std::unary_function<Record, bool>
	{
		string section_;
		string key_;

		RecordSectionKeyIs(const string& section, const string& key): section_(section), key_(key){}

		bool operator() (const Record& rec) const
		{
			return ((CompareNoCase(rec.Section, section_) == 0) && (CompareNoCase(rec.Key, key_) == 0));
		}
	};

	struct AscendingSectionSort
	{
		bool operator() (Record& Start, Record& End)
		{
			return Start.Section < End.Section;
		}
	};

	struct DescendingSectionSort
	{
		bool operator() (Record& Start, Record& End)
		{
			return Start.Section > End.Section;
		}
	};

	struct AscendingRecordSort
	{
		bool operator() (Record& Start, Record& End)
		{
			return Start.Key < End.Key;
		}
	};

	struct DescendingRecordSort
	{
		bool operator() (Record& Start, Record& End)
		{
			return Start.Key > End.Key;
		}
	};
};
