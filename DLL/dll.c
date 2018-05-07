// DLL.cpp : Defines the exported functions for the DLL application.  
// Compile by using: cl /EHsc /DMATHLIBRARY_EXPORTS /LD MathLibrary.cpp  


#include "DLL.h"  

HANDLE hMapFile, hCreateMutex;

__declspec(dllexport) BOOL isGame;
__declspec(dllexport) HANDLE eventoSer;
__declspec(dllexport) HANDLE eventoCli;

HANDLE createSharedMemory() {

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(Game),
		szNAME);

	if (hMapFile == NULL) {
		_tprintf(TEXT(
			"Houve azar na memoria partilhada ( erro %d).\n"), GetLastError());
		return NULL;
	}

	_tprintf(TEXT(
		"Inicializei a memoria"));

	return hMapFile;

}

//ver importante
HANDLE openSharedMemory()
{
	return hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szNAME);
}

BOOL closeSharedMemory() {
	//Tira o handle;
	CloseHandle(hMapFile);

	return true;
}

HANDLE createMutex() {
	hCreateMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (hCreateMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	return hCreateMutex;
}


