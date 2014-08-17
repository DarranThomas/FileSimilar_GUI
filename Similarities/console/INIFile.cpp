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

#include "StdAfx.h"

#include "INIFile.h"

CINIFile::CINIFile()	// Default constructor
{
}

CINIFile::~CINIFile()
{
}

long CINIFile::FindReplace(string& strSource, const string& strFind, const string& strReplace)
{
	long lCount = 0;

	string::size_type nPos = 0;

	while(string::npos != (nPos = strSource.find(strFind, nPos)))
	{
		strSource.replace(nPos, strFind.length(), strReplace);
		nPos += strReplace.length();

		lCount++;
	}

	return lCount;
}

void CINIFile::RemoveWhiteSpaces(string& strSource, const string& strReplace /* = ""*/)
{
	FindReplace(strSource, " ", strReplace);
	FindReplace(strSource, "\t", strReplace);
	FindReplace(strSource, "\r", strReplace);
	FindReplace(strSource, "\n", strReplace);
}

void CINIFile::Trim(string& strSource, const string& strChars2Trim /* = " \t\r\n"*/, bool bTrimLeft /* = true*/, bool bTrimRight /* = true*/)
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

string CINIFile::ToUpper(const string& str)
{
	string strUpper = str;
	std::transform(strUpper.begin(), strUpper.end(), strUpper.begin(), toupper);
	return strUpper;
}

string CINIFile::ToLower(const string& str)
{
	string strLower = str;
	std::transform(strLower.begin(), strLower.end(), strLower.begin(), tolower);
	return strLower;
}

int CINIFile::CompareNoCase(const string& str1, const string& str2)
{
	return ToUpper(str1).compare(ToUpper(str2));
}

bool CINIFile::Load(const string& strFilePath, vector<Record>& vecrecContent)
{
	string strCurrentLine = "";													// Holds the current line from the INI file

	string strCurrentSection = "";												// Holds the current section name

	ifstream FileInput(strFilePath.c_str());									// Create an input file stream
	
	if(!FileInput.is_open())
	{
		return false;															// If the input file doesn't open, then return
	}

	vecrecContent.clear();														// Clear the vecrecContent vector

	string strComments = "";													// A string to store strComments in

	while(std::getline(FileInput, strCurrentLine))								// Read until the end of the file
	{
		Trim(strCurrentLine);													// Trim whitespace from the ends
		FindReplace(strCurrentLine, " =", "=");									// Remove white spaces from around '='
		FindReplace(strCurrentLine, "= ", "=");

		if(strCurrentLine.empty())												// Make sure its not a blank line
		{
			continue;
		}

		Record CurrentRecord = {"", '\0', "", "", ""};							// Define a new record

		if((strCurrentLine[0] == '#') || (strCurrentLine[0] == ';'))			// Is this a commented line?
		{
			if(	(strCurrentLine.find('[') == string::npos) &&					// If there is no [ or =
				(strCurrentLine.find('=') == string::npos))						// Then it'strCurrentLine a comment
			{
				strComments += strCurrentLine + '\n';							// Add the comment to the current strComments string
			}
			else
			{
				CurrentRecord.Commented = strCurrentLine[0];					// Save the comment character
				strCurrentLine.erase(strCurrentLine.begin());					// Remove the comment for further processing
				Trim(strCurrentLine);											// Remove any more whitespace
			}
		}
		else
		{
			CurrentRecord.Commented = '\0';										// else mark it as not being a comment
		}

		if(strCurrentLine.find('[') != string::npos)							// Is this line a section?
		{		
			strCurrentLine.erase(strCurrentLine.begin());						// Erase the leading bracket
			strCurrentLine.erase(strCurrentLine.find(']'));						// Erase the trailing bracket
			CurrentRecord.Comments = strComments;								// Add the strComments string (if any)
			strComments = "";													// Clear the strComments for re-use
			CurrentRecord.Section = strCurrentLine;								// Set the Section value
			CurrentRecord.Key = "";												// Set the Key value
			CurrentRecord.Value = "";											// Set the Value value
			strCurrentSection = strCurrentLine;
		}

		if(strCurrentLine.find('=') != string::npos)									// Is this line a Key/Value?
		{
			CurrentRecord.Comments = strComments;										// Add the strComments string (if any)
			strComments = "";															// Clear the strComments for re-use
			CurrentRecord.Section = strCurrentSection;									// Set the section to the current Section
			CurrentRecord.Key = strCurrentLine.substr(0, strCurrentLine.find('='));		// Set the Key value to everything before the = sign
			CurrentRecord.Value = strCurrentLine.substr(strCurrentLine.find('=') + 1);	// Set the Value to everything after the = sign
		}

		if(strComments == "")													// Don't add a record yet if its a comment line
		{
			vecrecContent.push_back(CurrentRecord);								// Add the record to vecrecContent
		}
	}

	FileInput.close();															// Close the file

	return true;
}

bool CINIFile::Save(const string& strFilePath, vector<Record>& vecrecContent)
{
	ofstream FileOutput(strFilePath.c_str());									// Create an output file stream

	if(!FileOutput.is_open())
	{
		return false;															// If the output file doesn't open, then return
	}

	long lRecordIndex = 0;
	long lRecordCount = (long) vecrecContent.size();

	for(lRecordIndex = 0; lRecordIndex < lRecordCount; lRecordIndex++)			// Loop through each vector element
	{
		FileOutput << vecrecContent[lRecordIndex].Comments;						// Write out the comments

		if(CompareNoCase(vecrecContent[lRecordIndex].Key, "") == 0)				// Is this a section?
		{
			// Then format the section
			if(lRecordIndex == 0)
			{
				if(vecrecContent[lRecordIndex].Commented == '\0')
				{
					FileOutput << "[" << vecrecContent[lRecordIndex].Section << "]" << endl;
				}
				else
				{
					FileOutput << vecrecContent[lRecordIndex].Commented << "[" << vecrecContent[lRecordIndex].Section << "]" << endl;
				}
			}
			else
			{
				if(vecrecContent[lRecordIndex].Commented == '\0')
				{
					FileOutput << endl << "[" << vecrecContent[lRecordIndex].Section << "]" << endl;
				}
				else
				{
					FileOutput << vecrecContent[lRecordIndex].Commented << endl << "[" << vecrecContent[lRecordIndex].Section << "]" << endl;
				}
			}
		}
		else
		{
			if(vecrecContent[lRecordIndex].Commented == '\0')
			{
				FileOutput << vecrecContent[lRecordIndex].Key << " = " << vecrecContent[lRecordIndex].Value << endl;
			}
			else
			{
				FileOutput << vecrecContent[lRecordIndex].Commented << vecrecContent[lRecordIndex].Key << " = " << vecrecContent[lRecordIndex].Value << endl;
			}
		}
	}

	FileOutput.close();															// Close the file

	return true;
}

string CINIFile::Content(const string& strFilePath)
{
	string strContent = "";														// Hold our return string

	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file loads
	{
		return "";
	}

	long lRecordIndex = 0;
	long lRecordCount = (long) vecrecContent.size();

	for(lRecordIndex = 0; lRecordIndex < lRecordCount; lRecordIndex++)			// Loop through the vecrecContent
	{
		if(CompareNoCase(vecrecContent[lRecordIndex].Comments, "") != 0)
		{
			strContent += vecrecContent[lRecordIndex].Comments;					// Add the comments
		}

		if(vecrecContent[lRecordIndex].Commented != '\0')
		{
			strContent += vecrecContent[lRecordIndex].Commented;				// If this is commented, then add it
		}

		if(CompareNoCase(vecrecContent[lRecordIndex].Key, "") == 0)				// Is this a section?
		{
			strContent += '[' + vecrecContent[lRecordIndex].Section + ']';		// Add the section
		}
		else
		{
			strContent += vecrecContent[lRecordIndex].Key + '=' + vecrecContent[lRecordIndex].Value;	// Or the Key value to the return string
		}

		if(lRecordIndex != (long) vecrecContent.size())
		{
			strContent += '\n';													// If this is not the last line, add a CrLf
		}
	}

	return strContent;															// Return the contents
}

vector<string> CINIFile::GetSectionNames(const string& strFilePath)
{
	vector<string> vecstrSectionNames;											// Holds the return section names

	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return vecstrSectionNames;
	}

	long lRecordIndex = 0;
	long lRecordCount = (long) vecrecContent.size();

	for(lRecordIndex = 0; lRecordIndex < lRecordCount; lRecordIndex++)			// Loop through the vecrecContent
	{
		if(CompareNoCase(vecrecContent[lRecordIndex].Key, "") == 0)				// If there is no key value, then its a section
		{
			vecstrSectionNames.push_back(vecrecContent[lRecordIndex].Section);	// Add the section to the return data
		}
	}

	return vecstrSectionNames;													// Return the section names
}

vector<CINIFile::Record> CINIFile::GetSection(const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecSection;												// Holds the return records

	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return vecrecSection;
	}

	long lRecordIndex = 0;
	long lRecordCount = (long) vecrecContent.size();

	for(lRecordIndex = 0; lRecordIndex < lRecordCount; lRecordIndex++)			// Loop through the vecrecContent
	{
		if(	(CompareNoCase(vecrecContent[lRecordIndex].Section, strSectionName) == 0) &&	// If this is the section name we want
			(CompareNoCase(vecrecContent[lRecordIndex].Key, "") != 0))						// but not the section name itself
		{
			vecrecSection.push_back(vecrecContent[lRecordIndex]);				// Add the record to the return vecRecordsSection
		}
	}

	return vecrecSection;														// Return the vecRecordsSection
}

bool CINIFile::RecordExists(const string& strKeyName, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(),
													vecrecContent.end(),
													CINIFile::RecordSectionKeyIs(strSectionName, strKeyName));	// Locate the Section/Key

	if(iter == vecrecContent.end())
	{
		return false;															// The Section/Key was not found
	}

	return true;																// The Section/Key was found
}

bool CINIFile::SectionExists(const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(),
													vecrecContent.end(),
													CINIFile::RecordSectionIs(strSectionName));		// Locate the Section

	if(iter == vecrecContent.end())
	{
		return false;															// The Section was not found
	}

	return true;																// The Section was found
}

CINIFile::Record CINIFile::GetRecord(const string& strKeyName, const string& strSectionName, const string& strFilePath)
{
	Record CurrentRecord = {"", '\0', "", "", ""};								// Holds the return record

	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return CurrentRecord;
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(), 
													vecrecContent.end(), 
													CINIFile::RecordSectionKeyIs(strSectionName, strKeyName));	// Locate the Record

	if(iter == vecrecContent.end())
	{
		return CurrentRecord;													// The Record was not found
	}

	CurrentRecord = *iter;														// The Record was found

	return CurrentRecord;														// Return the Record
}

string CINIFile::GetValue(const string& strKeyName, const string& strSectionName, const string& strFilePath)
{
	return GetRecord(strKeyName, strSectionName, strFilePath).Value;			// Get the Record
}

bool CINIFile::SetValue(const string& strKeyName, const string& strValue, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	if(!SectionExists(strSectionName, strFilePath))								// If the Section doesn't exist
	{
		Record s = {"", '\0', strSectionName, "", ""};							// Define a new section
		vecrecContent.push_back(s);												// Add the section

		Record r = {"", '\0', strSectionName, strKeyName, strValue};			// Define a new record
		vecrecContent.push_back(r);												// Add the record

		return Save(strFilePath, vecrecContent);								// Save
	}

	if(!RecordExists(strKeyName, strSectionName, strFilePath))					// If the Key doesn't exist
	{
		vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(), 
														vecrecContent.end(), 
														CINIFile::RecordSectionIs(strSectionName));	// Locate the Section
		
		iter++;																	// Advance just past the section
		
		Record r = {"", '\0', strSectionName, strKeyName, strValue};			// Define a new record
		vecrecContent.insert(iter, r);											// Add the record

		return Save(strFilePath,vecrecContent);									// Save
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(), 
													vecrecContent.end(), 
													CINIFile::RecordSectionKeyIs(strSectionName, strKeyName));	// Locate the Record

	iter->Value = strValue;														// Insert the correct value

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::RenameSection(const string& strOldSectionName, const string& strNewSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	for(vector<Record>::iterator iter = vecrecContent.begin(); iter < vecrecContent.end(); iter++)	// Loop through the records
	{
		if(CompareNoCase(iter->Section, strOldSectionName) == 0)				// Is this the OldSectionName?
		{
			iter->Section = strNewSectionName;									// Now its the NewSectionName
		}
	}

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::CommentRecord(CommentChar cc, const string& strKeyName, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(), 
													vecrecContent.end(), 
													CINIFile::RecordSectionKeyIs(strSectionName, strKeyName));	// Locate the Section/Key

	if(iter == vecrecContent.end())
	{
		return false;															// The Section/Key was not found
	}

	iter->Commented = (char) cc;												// Change the Comment value

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::UnCommentRecord(const string& strKeyName, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(), 
													vecrecContent.end(), 
													CINIFile::RecordSectionKeyIs(strSectionName, strKeyName));	// Locate the Section/Key

	if(iter == vecrecContent.end())
	{
		return false;															// The Section/Key was not found
	}

	iter->Commented = '\0';														// Remove the Comment value

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::CommentSection(CommentChar cc, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	for(vector<Record>::iterator iter = vecrecContent.begin(); iter < vecrecContent.end(); iter++)
	{
		if(CompareNoCase(iter->Section, strSectionName) == 0)					// Is this the right section?
		{
			iter->Commented = (char) cc;										// Change the comment value
		}
	}

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::UnCommentSection(const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	for(vector<Record>::iterator iter = vecrecContent.begin(); iter < vecrecContent.end(); iter++)
	{
		if(CompareNoCase(iter->Section, strSectionName) == 0)					// Is this the right section?
		{
			iter->Commented = '\0';												// Remove the comment value
		}
	}																	

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::DeleteRecord(const string& strKeyName, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(),
													vecrecContent.end(), 
													CINIFile::RecordSectionKeyIs(strSectionName, strKeyName));	// Locate the Section/Key

	if(iter == vecrecContent.end())
	{
		return false;															// The Section/Key was not found
	}

	vecrecContent.erase(iter);													// Remove the Record

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::DeleteSection(const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	for(long lRecordIndex = (long) vecrecContent.size() - 1; lRecordIndex >= 0; lRecordIndex--)	// Iterate backwards through the content
	{							
		if(CompareNoCase(vecrecContent[lRecordIndex].Section, strSectionName) == 0)		// Is this related to the Section?
		{
			vecrecContent.erase(vecrecContent.begin() + lRecordIndex);			// Then erase it
		}
	}

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::SetSectionComments(const string& strComments, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	for(vector<Record>::iterator iter = vecrecContent.begin(); iter < vecrecContent.end(); iter++)	// Loop through the records
	{
		if(	(CompareNoCase(iter->Section, strSectionName) == 0) &&				// Is this the Section?
			(CompareNoCase(iter->Key, "") == 0))								// And not a record
		{
			string strSectionComment = strComments;

			if(strSectionComment.size() >= 2)									// Is there a comment?
			{
				if(strSectionComment.substr(strSectionComment.size() - 2) != "\n")	// Does the string end in a newline?
				{
					strSectionComment += "\n";									// If not, add one
				}
			}

			iter->Comments = strSectionComment;									// Set the comments
				
			return Save(strFilePath, vecrecContent);							// Save
		}
	}

	return false;
}

bool CINIFile::SetRecordComments(const string& strComments, const string& strKeyName, const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	vector<Record>::iterator iter = std::find_if(	vecrecContent.begin(), 
													vecrecContent.end(), 
													CINIFile::RecordSectionKeyIs(strSectionName, strKeyName));	// Locate the Section/Key

	if(iter == vecrecContent.end())
	{
		return false;															// The Section/Key was not found
	}

	string strRecordComments = strComments;

	if(strRecordComments.size() >= 2)											// Is there a comment?
	{
		if(strRecordComments.substr(strRecordComments.size() - 2) != "\n")		// Does the string end in a newline?
		{
			strRecordComments += "\n";											// If not, add one
		}
	}

	iter->Comments = strRecordComments;											// Set the comments

	return Save(strFilePath, vecrecContent);									// Save
}

vector<CINIFile::Record> CINIFile::GetSections(const string& strFilePath)
{
	vector<Record> vecrecSections;												// Holds the return records
	
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return vecrecSections;
	}

	long lRecordIndex = 0;
	long lRecordCount = (long) vecrecContent.size();

	for(lRecordIndex = 0; lRecordIndex < lRecordCount; lRecordIndex++)			// Loop through the vecrecContent
	{
		if(CompareNoCase(vecrecContent[lRecordIndex].Key, "") == 0)				// If this is a section 
		{
			vecrecSections.push_back(vecrecContent[lRecordIndex]);				// Add the record to the return data
		}
	}

	return vecrecSections;														// Return the records
}

bool CINIFile::Sort(const string& strFilePath, bool bDescending)
{
	vector<CINIFile::Record> vecrecSortedContent;								// Used to hold the sorted content

	vector<CINIFile::Record> vecrecSections = GetSections(strFilePath);			// Get a list of Sections

	if(vecrecSections.empty())													// Is there anything to process?
	{
		return false;
	}

	if(bDescending)																// Descending or Ascending?
	{
		std::sort(vecrecSections.begin(), vecrecSections.end(), DescendingSectionSort());
	}
	else																		// Sort the Sections
	{
		std::sort(vecrecSections.begin(), vecrecSections.end(), AscendingSectionSort());
	}

	for(vector<Record>::iterator iter = vecrecSections.begin(); iter < vecrecSections.end(); iter++) // For each Section
	{																		
		vecrecSortedContent.push_back(*iter);									// Add the sorted Section to the content

		vector<CINIFile::Record> vecrecSection = GetSection(iter->Section, strFilePath);	// Get a list of Records for this section

		if(bDescending)															// Descending or Ascending?
		{
			std::sort(vecrecSection.begin(), vecrecSection.end(), DescendingRecordSort());
		}
		else																	// Sort the Records
		{
			std::sort(vecrecSection.begin(), vecrecSection.end(), AscendingRecordSort());
		}

		for(vector<Record>::iterator it = vecrecSection.begin(); it < vecrecSection.end(); it++)	// For each Record
		{
			vecrecSortedContent.push_back(*it);									// Add the sorted Record to the content
		}
	}
	
	return Save(strFilePath, vecrecSortedContent);								// Save
}

bool CINIFile::AddSection(const string& strSectionName, const string& strFilePath)
{
	vector<Record> vecrecContent;												// Holds the records

	if(!Load(strFilePath, vecrecContent))										// Make sure the file is loaded
	{
		return false;
	}

	Record s = {"", '\0', strSectionName, "", ""};								// Define a new section
	vecrecContent.push_back(s);													// Add the section

	return Save(strFilePath, vecrecContent);									// Save
}

bool CINIFile::Create(const string& strFilePath)
{
	vector<Record> vecrecContent;												// Create empty content

	return Save(strFilePath, vecrecContent);									// Save
}
