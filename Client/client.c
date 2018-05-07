
#include "..\DLL\dll.h"

void escreveMemoria(Game *destinoMsg, Game * origemMsg);
void clientSettings();
void clientSettingsLocal();
void imprimeMapa();
BOOL testeSharedMemory();
HANDLE  hMapFileOpen, eventoServerCli;
Game * serverGame;
Game * clientGame;


int _tmain(int argc, TCHAR *argv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif // UNICODE

	clientGame = (Game*)malloc(sizeof(Game));

	eventoServerCli = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("eventoSingle"));
	eventoCli = CreateEvent(NULL, TRUE, FALSE, _T("eventoSingleServerCli"));
	
	//open sharedMemory 
	testeSharedMemory();
	clientSettings(clientGame);

	if (clientGame->gameDefinition.gameType == 0) {
		clientSettingsLocal(clientGame);
	}
	
	
	escreveMemoria(serverGame, clientGame);

	SetEvent(eventoServerCli);
	ResetEvent(eventoServerCli);

	WaitForSingleObject(eventoCli, INFINITE);

	escreveMemoria(clientGame, serverGame);

	imprimeMapa();
	int x = 0;

}


void escreveMemoria(Game *destinoMsg, Game * origemMsg) {
	CopyMemory(destinoMsg, origemMsg, sizeof(Game));
}

void clientSettings()
{
	_tprintf(TEXT("Introduza tamanho maximo horizontal: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.gameSizeX);
	_tprintf(TEXT("\nIntroduza tamanho maximo vertical: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.gameSizeY);
	_tprintf(TEXT("\nIntroduza numero maximo de jogadores: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.nMaxPlayers);
	_tprintf(TEXT("\nIntroduza numero de jogadores locais: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.nLocalPlayers);
	_tprintf(TEXT("\nIntroduza a duracao do jogo: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.duration);
	_tprintf(TEXT("\nIntroduza numero maximo de objectos: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.nObjects);
	_tprintf(TEXT("\nIntroduza numero de cobras automaticas: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.nSnakes);
	_tprintf(TEXT("\nIntroduza tamanho inicial das cobras: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.snakeSize);
	_tprintf(TEXT("\nIntroduza probabilidade de spawn objecto: "));
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.objectProbability);
	_tprintf(TEXT("\nSinglePlayer ou MultiPlayer? 0/1: ")); // 0 single 1 multi?
	_tscanf(TEXT("%d"), &clientGame->gameDefinition.gameType);
}

void clientSettingsLocal()
{
	

		if (clientGame->gameDefinition.nLocalPlayers == 1) {

			Player *jogador = (Player*)malloc(sizeof(Player));

			jogador->player_id = 1;
			jogador->idSnake = 1;

			//por em funcao
			/*_tprintf(TEXT("\n Introduza nome de jogador: "));
			_tscanf(TEXT("%s"), &jogador->name);
			_tprintf(TEXT("\n Introduza tecla para cima: "));
			_tscanf(TEXT("%s"), &jogador->buttonUp);
			_tprintf(TEXT("\n Introduza tecla para direita: "));
			_tscanf(TEXT("%s"), &jogador->buttonRight);
			_tprintf(TEXT("\n Introduza tecla para esquerda: "));
			_tscanf(TEXT("%s"), &jogador->buttonLeft);
			_tprintf(TEXT("\n Introduza tecla para baixo: "));
			_tscanf(TEXT("%s"), &jogador->buttonDown);*/

			jogador->isPlaying = TRUE;
			jogador->score = 0;
			clientGame->currentPlayers = 1;
			clientGame->players[0] = *jogador;

			//destruir ponteiro?

		}
		else {

			Player *jogador = (Player*)malloc(sizeof(Player));
			Player *jogador2 = (Player*)malloc(sizeof(Player));

			jogador->player_id = 1;
			jogador->idSnake = 1;


			/*_tprintf(TEXT("\n Introduza nome de jogador 1: "));
			_tscanf(TEXT("%s"), &jogador->name);
			_tprintf(TEXT("\n Introduza tecla para cima: "));
			_tscanf(TEXT("%s"), &jogador->buttonUp);
			_tprintf(TEXT("\n Introduza tecla para direita: "));
			_tscanf(TEXT("%s"), &jogador->buttonRight);
			_tprintf(TEXT("\n Introduza tecla para esquerda: "));
			_tscanf(TEXT("%s"), &jogador->buttonLeft);
			_tprintf(TEXT("\n Introduza tecla para baixo: "));
			_tscanf(TEXT("%s"), &jogador->buttonDown);*/

			jogador->isPlaying = TRUE;
			jogador->score = 0;
			clientGame->players[0] = *jogador;

			jogador2->player_id = 2;
			jogador2->idSnake = 2;


		/*	_tprintf(TEXT("\n Introduza nome de jogador 2: "));
			_tscanf(TEXT("%s"), &jogador2->name);
			_tprintf(TEXT("\n Introduza tecla para cima: "));
			_tscanf(TEXT("%s"), &jogador2->buttonUp);
			_tprintf(TEXT("\n Introduza tecla para direita: "));
			_tscanf(TEXT("%s"), &jogador2->buttonRight);
			_tprintf(TEXT("\n Introduza tecla para esquerda: "));
			_tscanf(TEXT("%s"), &jogador2->buttonLeft);
			_tprintf(TEXT("\n Introduza tecla para baixo: "));
			_tscanf(TEXT("%s"), &jogador2->buttonDown);*/

			jogador2->isPlaying = TRUE;
			jogador2->score = 0;
			clientGame->currentPlayers = 2;
			clientGame->players[1] = *jogador2;
		}
	}


BOOL testeSharedMemory() {

	hMapFileOpen = openSharedMemory();
	if (hMapFileOpen == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d). Creating File Mapping...\n"),
			GetLastError());
		return FALSE;
	}

	serverGame = (Game *)MapViewOfFile(hMapFileOpen,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		sizeof(Game));

	if (serverGame == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFileOpen);
		return FALSE;
	}
	return TRUE;
}


void imprimeMapa() {
	_tprintf(TEXT("\n"));
	int tamX = clientGame->gameDefinition.gameSizeX;
	int tamY = clientGame->gameDefinition.gameSizeY;

	for (int i = 0; i < tamX; i++) {
		for (int j = 0; j < tamY; j++) {
			_tprintf(TEXT("%d  "), clientGame->map[i][j].idTile);
		}
		_tprintf(TEXT("\n"));

	}

}
