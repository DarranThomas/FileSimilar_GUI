#pragma once

#include "Util.h"

class CFoundInData
{
public:
	CFoundInData(void);
	~CFoundInData(void);

public:
	_tstring m_strKey;
	_tstring m_strValue;

public:
	bool operator < (const CFoundInData& FIB)
	{
		return (m_strKey < FIB.m_strKey);
	}

	bool operator <= (const CFoundInData& FIB)
	{
		return (m_strKey <= FIB.m_strKey);
	}

	bool operator > (const CFoundInData& FIB)
	{
		return (m_strKey > FIB.m_strKey);
	}

	bool operator >= (const CFoundInData& FIB)
	{
		return (m_strKey >= FIB.m_strKey);
	}
};
