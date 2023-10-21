
#include <stddef.h>
#include <time.h>
#include <stdarg.h>

#include <iostream>
#include <filesystem>

#include "UtilFileIO.h"

using namespace std;

// ref: https://stackoverflow.com/a/6924293/17124142
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace UtilFileIO
{
	const char* get_timestamp_string(void)
	{
		static char txt[32];
		time_t now = time(NULL);
		struct tm tm_now;

		if (localtime_s(&tm_now, &now) == 0) strftime(txt, 32, "%Y-%m-%d %H:%M:%S", &tm_now);
		else txt[0] = '\0';

		return txt;
	};

	const wchar_t* get_timestamp_wstring(void)
	{
		static wchar_t txt[32];
		time_t now = time(NULL);
		struct tm tm_now;

		if (localtime_s(&tm_now, &now) == 0) wcsftime(txt,32, L"%Y-%m-%d %H:%M:%S", &tm_now);
		else txt[0] = '\0';

		return txt;
	};

	void get_module_fullpath(filesystem::path& fullpath)
	{
		TCHAR buf[MAX_PATH];

		::GetModuleFileName((HINSTANCE)&__ImageBase, buf, MAX_PATH);
		fullpath = buf;
	};

	bool get_absolute_path(filesystem::path& path)
	{
		if (path.is_absolute()) return true;

		try
		{
			filesystem::path temp;

			temp = filesystem::absolute(path);

			if (temp.empty()) return false;

			path = temp;

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	
	bool get_relative_path(filesystem::path& path)
	{
		if (path.is_relative()) return true;
		
		try
		{
			filesystem::path temp;
			
			temp = filesystem::relative(path);

			if( temp.empty()) return false;

			path = temp;

			return true;
		}
		catch (...)
		{
			return false;
		}
	};

	bool get_system_dll_path(filesystem::path& path)
	{
		TCHAR sys_path[MAX_PATH+1];
		int i = 0;

		if (::GetSystemDirectory(sys_path, MAX_PATH+1) == 0) return false;

		path = sys_path;

		return true;
	};

	bool wide_to_multi_capi(wstring const& src, string& m_str)// wstring Å® string
	{
		std::size_t converted{};
		std::vector<char> dest(src.size() * sizeof(wchar_t) + 1, '\0');
		if (::_wcstombs_s_l(&converted, dest.data(), dest.size(), src.data(), _TRUNCATE, ::_create_locale(LC_ALL, "jpn")) != 0) {
			return false;
		}
		dest.resize(std::char_traits<char>::length(dest.data()));
		dest.shrink_to_fit();
		m_str = std::string(dest.begin(), dest.end());

		return true;
	};

	bool multi_to_wide_capi(std::string const& src, wstring &w_str)// string Å® wstring
	{
		std::size_t converted{};
		std::vector<wchar_t> dest(src.size(), L'\0');
		if (::_mbstowcs_s_l(&converted, dest.data(), dest.size(), src.data(), _TRUNCATE, ::_create_locale(LC_ALL, "jpn")) != 0) {
			return false;
		}
		dest.resize(std::char_traits<wchar_t>::length(dest.data()));
		dest.shrink_to_fit();
		
		w_str = std::wstring(dest.begin(), dest.end());
		
		return true;	
	};

	string wide_to_Utf8(const wstring& w_string) // wstring Å® UTF8
	{
		int buf_size = ::WideCharToMultiByte(CP_UTF8, 0, w_string.c_str(), -1, (char*)NULL, 0, NULL, NULL);
		CHAR* buf_UTF8 = new CHAR[buf_size];

		::WideCharToMultiByte(CP_UTF8, 0, w_string.c_str(), -1, buf_UTF8, buf_size, NULL, NULL);

		std::string ret(buf_UTF8, buf_UTF8 + buf_size - 1);

		delete[] buf_UTF8;

		return ret;
	};

	wstring Utf8_to_wide(const string &UTF8_str) // UTF8 Å® wstring
	{
		int buf_size = ::MultiByteToWideChar(CP_UTF8, 0, UTF8_str.c_str(), -1, (wchar_t*)NULL, 0);
		wchar_t* buf_w = (wchar_t*)new wchar_t[buf_size];

		::MultiByteToWideChar(CP_UTF8, 0, UTF8_str.c_str(), -1, buf_w, buf_size);

		std::wstring ret(buf_w, buf_w + buf_size - 1);

		delete[] buf_w;

		return ret;
	}
}; // namespace UtilFileIO

namespace UtilIniIO
{
	bool CIniFile::open(const filesystem::path& file_name, const filesystem::path* ext)
	{
		m_filepath = file_name;

		if (ext != NULL) m_filepath.replace_extension(*ext);
		
		return true;
	};
	bool CIniFile::open(const filesystem::path& file_name, const TCHAR* ext)
	{
		if (ext != NULL) return open(file_name, &filesystem::path(ext));
		else return open(file_name, (const filesystem::path*)NULL);
	};


	bool CIniFile::load_string(const wchar_t* section, const wchar_t* key, wstring& value, wchar_t* def_value)
	{
		wchar_t temp[1024];

		::GetPrivateProfileString(section, key, NULL, temp, 1024, m_filepath.c_str());
		if (::GetLastError() == ERROR_SUCCESS)
		{
			value = temp;
		}
		else
		{
			if (def_value == NULL) return false;

			value = def_value;
		}
		return true;
	};
	bool CIniFile::load_int(const wchar_t* section, const wchar_t* key, int& value, int* def_value)
	{
		wstring temp;

		if (load_string(section, key, temp))
		{
			try
			{
				value = stoi(temp);

				return true;
			}
			catch (...){}
		}
		if (def_value == NULL) return false;
		
		value = *def_value;

		return true;
	};
	bool CIniFile::load_hex(const wchar_t* section, const wchar_t* key, int& value, int* def_value)
	{
		wstring temp;

		if (load_string(section, key, temp))
		{
			try
			{
				value = stoi(temp, nullptr, 16);

				return true;
			}
			catch (...) {}
		}
		if (def_value == NULL) return false;

		value = *def_value;

		return true;
	};
	bool CIniFile::load_bool(const wchar_t* section, const wchar_t* key, bool& value, bool* def_value)
	{
		int temp;
		
		if (load_int(section, key, temp))
		{
			value = (temp != 0);

			return true;
		}

		if (def_value == NULL) return false;

		value = *def_value;

		return true;
	}

	bool CIniFile::save_string(const wchar_t* section, const wchar_t* key, const wstring& value)
	{
		if (::WritePrivateProfileString(section, key, value.c_str(), m_filepath.c_str()) != 0)
		{
			return true;
		}
		return false;
	};
	bool CIniFile::save_int(const wchar_t* section, const wchar_t* key, const int& value)
	{
		if (::WritePrivateProfileString(section, key, std::to_wstring(value).c_str(), m_filepath.c_str()) != 0)
		{
			return true;
		}
		return false;
	};
	bool CIniFile::save_hex(const wchar_t* section, const wchar_t* key, const int& value)
	{
		wchar_t temp[32];

		if (_snwprintf_s(temp, _countof(temp), _TRUNCATE, L"0x%x", value) <= 0) return false;
		
		if (::WritePrivateProfileString(section, key, temp, m_filepath.c_str()) != 0)
		{
			return true;
		}
		return false;
	};
	bool CIniFile::save_bool(const wchar_t* section, const wchar_t* key, const bool&value)
	{
		int temp = value ? 1 : 0;
		
		if (::WritePrivateProfileString(section, key, std::to_wstring(temp).c_str(), m_filepath.c_str()) != 0)
		{
			return true;
		}
		return false;
	};
}; // namespace UtilIniIO

namespace UtilLogIO
{
	bool CLogFile::open(const filesystem::path& file_name, const filesystem::path *ext)
	{
		close();

		m_filepath = file_name;

		if (ext != NULL) m_filepath.replace_extension(*ext);

		log_s.open(m_filepath, ios::app);
		if (log_s.is_open()) return true;

		m_filepath.clear();

		return false;
	};

	bool  CLogFile::open(const filesystem::path& file_name, const TCHAR* ext)
	{	
		if (ext != NULL) return open(file_name, &filesystem::path(ext));
		else return open(file_name, (const filesystem::path*)NULL);
	};

	void  CLogFile::close()
	{
		m_filepath.clear();
		
		_close();
	};

	void  CLogFile::flush(void) 
	{
		if (log_s.is_open())
		{
			log_s << std::flush;

			_close();
		}
	};
	
	void  CLogFile::printf(const char *fmt ...)
	{
		if (fmt && _open())
		{
			char temp[1024];
			va_list ap;

			va_start(ap, fmt);
			vsnprintf(temp, 1024, fmt, ap);
			va_end(ap);

			log_s << temp;
		}
	};

	void  CLogFile::wprintf(const wchar_t* fmt ...)
	{
		if (fmt && _open())
		{
			wchar_t temp[1024];
			va_list ap;
			string str_temp;

			va_start(ap, fmt);
			_vsnwprintf_s(temp, 1024, fmt, ap);
			va_end(ap);

			
			UtilFileIO::wide_to_multi_capi(wstring(temp), str_temp);

			log_s << str_temp;
		}
	};

	void CLogFile::dump_hex(const void* p, size_t size)
	{
		int i, n;
		char buf[1024]{};
		static const char tbl[] = "0123456789abcdef";

		if (p == NULL || size == 0) return;

		for (i = n = 0; (size_t)i < size; i++)
		{
			const BYTE b = *((const BYTE*)p+i);

			buf[n++] = tbl[b >> 4];
			buf[n++] = tbl[b & 0x0f];
			buf[n++] = ' ';
			
			if (n > 1024 - 4)
			{
				buf[n] = '\0';

				log_s << buf;

				n = 0;
			}
		}
		
		buf[n] = '\0';

		log_s << buf << '\n';
	}
	
	void  CLogFile::dump_hex(const void* p, size_t size, size_t tab_count)
	{
		if (size == 0) return;

		if (_open())
		{
			const BYTE *b = (const BYTE*)p;
			size_t i, n;
			char buf[256]{};
			static const char tbl[] = "0123456789abcdef";

			for (i = n = 0; i < size; i++) {

				if ( (i % 32) == 0) {
					if (i != 0)
					{
						buf[n] = '\0';

						log_s << buf << endl;

						n = 0;
					}
					
					for (size_t j = 0; j < tab_count; j++) log_s << "    "; 
				}

				buf[n++] = tbl[*b >> 4];
				buf[n++] = tbl[*b & 0x0f];
				b++;
				if( ( (i % 32) != 31) && (i != (size-1))) buf[n++] = ' ';
			}
			
			buf[n] = '\0';

			log_s << buf << endl;
		}
	};

	void  CLogFile::print_timestamp(void)
	{
		printf("%s", UtilFileIO::get_timestamp_string());
	};
	void  CLogFile::print_timestamp_line()
	{
		printf("---- %s ----------------\n", UtilFileIO::get_timestamp_string());
	};
	void  CLogFile::print_line(void)
	{
		printf("------------------------------\n", UtilFileIO::get_timestamp_string());
	};


	bool CLogFile::_open(void)
	{
		if (!m_filepath.empty())
		{
			if(!log_s.is_open()) log_s.open(m_filepath, ios::app);

			return log_s.is_open();
		}
		return false;
	};
	void CLogFile::_close(void)
	{
		log_s.close();
	};

};//amespace UtilLogIO