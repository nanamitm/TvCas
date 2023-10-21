#include "IniConfig.h"

namespace IniConfig
{
#define INI_FILE_EXT	L"ini"
#define LOG_FILE_EXT	L"log"
#define EMMLOG_FILE_EXT L"emm.log"

// Section
#define SECTION_NAME_SETTING	L"Setting"
#define SECTION_NAME_OPTION		L"Option"
#define SECTION_NAME_SCHUB		L"SCHub"

// Key
#define INIKEY_NAME_EMMLOG		L"EMMLOG"
#define INIKEY_NAME_LOG			L"LOG"
#define INIKEY_NAME_USE_SYS_DLL	L"USE_SYSTEM_DLL"
#ifdef TVCAS_B1_EXPORTS
#define INIKEY_NAME_B1EMM		L"B1EMM"
#endif

//default
#define INIKEY_DEF_EMMLOG		false
#define INIKEY_DEF_LOG			false
#define INIKEY_DEF_USESYSDLL	false
#ifdef TVCAS_B1_EXPORTS
#define INIKEY_DEF_B1EMM		false
#endif

// global
Config g_Config;

	using namespace std;

	Config::Config(void)
		: m_EMMLoging(INIKEY_DEF_EMMLOG)
		, m_LogEnable(INIKEY_DEF_LOG)
		, m_UseSysDLL(INIKEY_DEF_USESYSDLL)
#ifdef TVCAS_B1_EXPORTS
		, m_EnableB1EMM(INIKEY_DEF_B1EMM)
#endif
	{

	};

	void Config::Load(void)
	{
		std::filesystem::path fullpath, sys_path;
		wstring temp;
		bool def_b;
	
		UtilFileIO::get_module_fullpath(fullpath);
		m_IniFile.open(fullpath, INI_FILE_EXT);

		def_b = INIKEY_DEF_LOG;
		m_IniFile.load_bool(SECTION_NAME_SETTING, INIKEY_NAME_LOG, m_LogEnable, &def_b);
		if (m_LogEnable)
		{
			m_LogFile.open(fullpath, LOG_FILE_EXT);
			m_LogFile.printf("%s : Log Open\n", UtilFileIO::get_timestamp_string());
			m_LogFile.flush();
		}

		def_b = INIKEY_DEF_EMMLOG;
		m_IniFile.load_bool(SECTION_NAME_SETTING, INIKEY_NAME_EMMLOG, m_EMMLoging, &def_b);
		if (m_EMMLoging)
		{
			m_EMMLogFile.open(fullpath, EMMLOG_FILE_EXT);
			m_EMMLogFile.printf(" // Log open()  %s\n", UtilFileIO::get_timestamp_string());
		}

		def_b = INIKEY_DEF_USESYSDLL;
		m_IniFile.load_bool(SECTION_NAME_SCHUB, INIKEY_NAME_USE_SYS_DLL, m_UseSysDLL, &def_b);

		m_Paths.clear();
		for (int i = 0; i < 8; i++)
		{
			wchar_t key[8];
			swprintf_s(key, 8, L"dll%02d", i);

			m_IniFile.load_string(SECTION_NAME_SCHUB, key, temp, L"");
			if (!temp.empty())
			{
				filesystem::path path = temp;

				UtilFileIO::get_absolute_path(path);

				m_Paths.push_back(path);
			}
		}
		
		if (m_UseSysDLL && UtilFileIO::get_system_dll_path(sys_path))// システムデフォルトを追加
		{
			sys_path.append("WinSCard.dll");

			m_Paths.push_back(sys_path);
		}


#ifdef TVCAS_B1_EXPORTS
		def_b = INIKEY_DEF_B1EMM;
		m_IniFile.load_bool(SECTION_NAME_OPTION, INIKEY_NAME_B1EMM, m_EnableB1EMM, &def_b);
		if (m_LogEnable && m_EnableB1EMM)
		{
			m_LogFile.printf("%s : Enable B1 EMM\n", UtilFileIO::get_timestamp_string());
			m_LogFile.flush();
		}
#endif
	};

	void Config::Save(void)
	{
		if (m_LogEnable)
		{
			m_LogFile.printf("%s : Log Close\n", UtilFileIO::get_timestamp_string());
			m_LogFile.close();
		}

		if (m_EMMLoging)
		{
			m_EMMLogFile.printf(" // Log close()  %s\n", UtilFileIO::get_timestamp_string());
			m_EMMLogFile.close();
		}

		m_IniFile.save_bool(SECTION_NAME_SETTING, INIKEY_NAME_LOG, m_LogEnable);
		
		m_IniFile.save_bool(SECTION_NAME_SETTING, INIKEY_NAME_EMMLOG, m_EMMLoging);

		m_IniFile.save_bool(SECTION_NAME_SCHUB, INIKEY_NAME_USE_SYS_DLL, m_UseSysDLL);

#ifdef TVCAS_B1_EXPORTS
		//m_IniFile.save_bool(SECTION_NAME_OPTION, INIKEY_NAME_B1EMM, m_EnableB1EMM);
#endif
	};

	void Config::WritePrintf(const char* fmt ...)
	{
		if (m_LogEnable)
		{
			m_LogFile.printf(fmt);
			m_LogFile.flush();
		}
	}
	void Config::WriteWPrintf(const wchar_t* fmt ...)
	{
		if (m_LogEnable)
		{
			m_LogFile.wprintf(fmt);
			m_LogFile.flush();
		}
	}

	
	void  Config::WriteErroMsg(const char* err_msg)
	{
		if (!err_msg || !m_LogEnable) return;

		m_LogFile.printf("%s: %s\n", UtilFileIO::get_timestamp_string(),err_msg);
		m_LogFile.flush();
	};
	void Config::WriteErroMsg(const wchar_t* err_msg)
	{
		if (!err_msg || !m_LogEnable) return;

		m_LogFile.printf("WriteErroMsg\n");
		m_LogFile.wprintf(L"%s: %s\n", UtilFileIO::get_timestamp_wstring(), err_msg);
		m_LogFile.flush();
	};
	
	void Config::WriteEMMLog(const BYTE* EmmData, DWORD EmmSize)
	{
		if (!m_LogEnable) return;
		
		m_LogFile.print_timestamp_line();
		m_LogFile.printf("Receive: EMM Command (%d Byte)\n", EmmSize);
		m_LogFile.dump_hex(EmmData, EmmSize, 1);
		m_LogFile.flush();
	};

	void Config::WriteEMMLogingData(const BYTE* EmmData, DWORD EmmSize)
	{
		if (!m_EMMLoging) return;

		m_EMMLogFile.dump_hex(EmmData, EmmSize,1);
	};
};