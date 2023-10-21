#pragma once

#include <Windows.h>
#include <vector>

#include "UtilFileIO.h"

namespace IniConfig
{
	using namespace std;

	typedef vector<filesystem::path> dlls;
	typedef dlls::iterator dlls_i;

	class Config
	{
	public:
		void Load(void);
		void Save(void);

		Config(void);

		bool GetLogEnable(void) { return m_LogEnable; }
		void SetLogEnable(bool value) { m_LogEnable = value; }
		
		bool GetEMMLoging() { return m_EMMLoging; }
		void SetEMMLoging(bool value) { m_EMMLoging; }

		void GetDynamicSCardReaderPath(dlls& dlls) { dlls = m_Paths; };

#ifdef TVCAS_B1_EXPORTS
		bool GetEnabeB1EMM(){ return m_EnableB1EMM; }
		void SetEnableB1EMM(bool value) { m_EnableB1EMM = value; }
#endif

		void WritePrintf(const char* fmt ...);
		void WriteWPrintf(const wchar_t* fmt ...);
		void WriteErroMsg(const char* err_msg);
		void WriteErroMsg(const wchar_t* err_msg);
		void WriteEMMLog(const BYTE *EmmData, DWORD EmmSize);
		void WriteEMMLogingData(const BYTE* EmmData, DWORD EmmSize);

	protected:
		UtilIniIO::CIniFile m_IniFile;
		UtilLogIO::CLogFile m_LogFile;
		UtilLogIO::CLogFile m_EMMLogFile;

		dlls m_Paths;

		bool m_LogEnable;
		bool m_EMMLoging;
		bool m_UseSysDLL;

#ifdef TVCAS_B1_EXPORTS
		bool m_EnableB1EMM;
#endif
	};

	extern Config g_Config;
};
