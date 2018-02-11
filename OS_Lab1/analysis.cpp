#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <iostream>
#include <string> 
#include <time.h>
#pragma comment(lib, "User32.lib")

using namespace std;

int search(string catalogName)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	// If the directory is not specified as a command-line argument,
	// print usage.

	/*if (argc != 2)
	{
	_tprintf(TEXT("\nUsage: %s <directory name>\n"), argv[0]);
	return (-1);
	}

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	StringCchLength(argv[1], MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
	_tprintf(TEXT("\nDirectory path is too long.\n"));
	return (-1);
	}*/

	/*TCHAR * writable = new TCHAR[nameCatalog.size() + 1];
	copy(nameCatalog.begin(), nameCatalog.end(), writable);
	writable[nameCatalog.size()] = '\0';*/

	const char * c = catalogName.c_str();
	_tprintf(TEXT("\nTarget directory is %s\n\n"), c);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(szDir, MAX_PATH, c);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		//DisplayErrorBox(TEXT("FindFirstFile"));
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			_tprintf(TEXT("%s   <DIR>\n"), ffd.cFileName);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;

			TCHAR tchDate[80];
			SYSTEMTIME st;
			FileTimeToSystemTime(&ffd.ftLastWriteTime, &st);

			GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE,
				&st, nullptr, tchDate, _countof(tchDate));

			//_tprintf(TEXT("  %s   %ld bytes \n"), ffd.cFileName, filesize.QuadPart);
			cout << ffd.cFileName << "--" << filesize.QuadPart << "--" << tchDate << endl;
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		//DisplayErrorBox(TEXT("FindFirstFile"));
	}
	system("pause");

	FindClose(hFind);
	return dwError;
}

