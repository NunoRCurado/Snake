// DLL.h - Contains declaration of Function class  
#pragma once  
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <tchar.h>
#include <memory.h> 
#include "structs.h"
#include <Shlwapi.h>
#include <stdbool.h>
#include <fcntl.h>
#include <io.h>
#include <assert.h>




#define SHMEMSIZE 4096
#define GAMEBUFSIZE sizeof(Game)


#ifdef SNAKE_API_EXPORTS
#define SNAKE_API __declspec(dllexport)
#else
#define SNAKE_API __declspec(dllimport)
#endif

	TCHAR szNAME[] = TEXT("fmMsgSpace");
	//Variável global da DLL
	extern SNAKE_API BOOL isGame;
	extern SNAKE_API HANDLE eventoSer, eventoCli;
	//Funções a serem exportadas/importadas
	SNAKE_API HANDLE createSharedMemory();
	SNAKE_API HANDLE openSharedMemory();
	SNAKE_API BOOL closeSharedMemory();
	SNAKE_API HANDLE createMutex();
	

