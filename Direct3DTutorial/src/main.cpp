#include "windowApplication.h"
#include "exceptions.h"
#include "dxApplication.h"

using namespace std;
using namespace mini;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);
	auto exitCode = EXIT_FAILURE;
	try
	{
		DxApplication app(hInstance, 1280, 720, L"Pierce the Heavens");
		exitCode = app.Run();
	}
	catch (Exception& e)
	{
		MessageBoxW(nullptr, e.getMessage().c_str(), L"Error", MB_OK);
		exitCode = e.getExitCode();
	}
	catch (exception& e)
	{
		string s(e.what());
		MessageBoxW(nullptr, wstring(s.begin(), s.end()).c_str(), L"Error", MB_OK);
	}
	catch (const char* str)
	{
		string s(str);
		MessageBoxW(nullptr, wstring(s.begin(), s.end()).c_str(), L"Error", MB_OK);
	}
	catch (const wchar_t* str)
	{
		MessageBoxW(nullptr, str, L"Error", MB_OK);
	}
	catch (...)
	{
		MessageBoxW(nullptr, L"Unknown error", L"Error", MB_OK);
	}
	return exitCode;
}