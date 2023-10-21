#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>

#include <Windows.h>


namespace UtilFileIO {

	using namespace std;

	const char* get_timestamp_string(void);
	const wchar_t* get_timestamp_wstring(void);
	void get_module_fullpath(filesystem::path& fullpath);

	bool get_absolute_path(filesystem::path& path);
	bool get_relative_path(filesystem::path &path);

	bool get_system_dll_path(filesystem::path& path);

	bool wide_to_multi_capi(wstring const& src, string& m_str); // wstring → string
	bool multi_to_wide_capi(std::string const& src, wstring& w_str); // string → wstring;

	string wide_to_Utf8(const wstring& w_string); // wstring → UTF8
	wstring Utf8_to_wide(const string& UTF8_str); // UTF8 → wstring

}; // namespace UtilFileIO


namespace UtilIniIO
{
	using namespace std;

	class CIniFile
	{
	public:
		bool open(const filesystem::path& file_name, const filesystem::path* ext = NULL);
		bool open(const filesystem::path& file_name, const TCHAR* ext = NULL);

		void close(void) { m_filepath.clear(); };

		bool load_string(const wchar_t* section, const wchar_t* key, wstring& value, wchar_t* def_value=NULL);
		bool load_int(const wchar_t* section, const wchar_t* key, int& value, int* def_value = NULL);
		bool load_hex(const wchar_t* section, const wchar_t* key, int& value, int* def_value = NULL);
		bool load_bool(const wchar_t* section, const wchar_t* key, bool& value, bool* def_value = NULL);

		bool save_string(const wchar_t* section, const wchar_t* key, const wstring& value);
		bool save_int(const wchar_t* section, const wchar_t* key, const int& value);
		bool save_hex(const wchar_t* section, const wchar_t* key, const int& value);
		bool save_bool(const wchar_t* section, const wchar_t* key, const bool& value);

	protected:
		filesystem::path m_filepath;
	};
}; // namespace UtilIniIO

namespace UtilLogIO
{
	using namespace std;

	class CLogFile
	{
	public:
		bool open(const filesystem::path& file_name, const filesystem::path* ext = NULL);
		bool open(const filesystem::path& file_name, const TCHAR* ext = NULL);

		void close();

		void flush(void);

		void printf(const char* fmt ...);
		void wprintf(const wchar_t* fmt ...);
		void dump_hex(const void* p, size_t size);// 16進数データダンプを追記（ベタ出力高速版）
		void dump_hex(const void* p, size_t size, size_t tab_count);// 16進数データダンプを追記（タブ計算&32byte折返し整形版）

		void print_timestamp(void);
		void print_timestamp_line(void);
		void print_line( void);

	protected:
		ofstream log_s;

		filesystem::path m_filepath;

		bool _open(void);
		void _close(void);
	};
	
};//amespace UtilLogIO