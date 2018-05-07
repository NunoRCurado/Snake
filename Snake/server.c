#include "..\DLL\dll.h"


//Funcoes do registo
void updateRegistry(TCHAR playerId[], int newScore);
HKEY OpenRegistryKey(wchar_t* strKey);
DWORD GetValueFromRegistry(HKEY hKey, LPCTSTR lpValue);
void SetRegistryValue(HKEY hKey, LPCTSTR lpValue, DWORD data);
int GetMyScoreFromRegistry(TCHAR playerId[]);

void criaMapa();
void imprimeMapa();
void escreveMemoria(Game *destinoMsg, Game * origemMsg);
void iniciaJogoLocal();
void geraSnakes();
void inicializaItems();
void geraItems();
void geraCobrasAuto();
void movimentoCobrasAuto();
void movimentoCobrasPlayers();

void apagaCobra(int posicao, DWORD idCobra);

BOOL verificaPosicaoSeguinte(int nextDirection, int x, int y, int speed);
void abreMapView();


void comeItems(int item, DWORD idCobra);

void comeAlimento(DWORD idCobra);
void comeGelo(DWORD idCobra);
void comeGranada(DWORD idCobra);
void comeVodka(DWORD idCobra);

void comeOleo(DWORD idCobra);
void comeCola(DWORD idCobra);

void comeOOleo(DWORD idCobra);
void comeOCola(DWORD idCobra);
void comeOVodka(DWORD idCobra);

void comeItemEspecial(DWORD idCobra);

BOOL tempoMaxAtingido = FALSE;

DWORD WINAPI threadDuracaoItemsOleo(LPVOID idCobra);
DWORD WINAPI threadDuracaoItemsoOleo(LPVOID idCobra);
DWORD WINAPI threadDuracaoItemsoCola(LPVOID idCobra);
DWORD WINAPI threadDuracaoItemsoVodka(LPVOID idCobra);
DWORD WINAPI threadDuracaoItemsCola(LPVOID idCobra);
DWORD WINAPI threadDuracaoItemsVodka(LPVOID idCobra);


DWORD WINAPI threadTimer();
DWORD WINAPI threadStart();
DWORD WINAPI threadItems();
DWORD WINAPI threadRecebeCliente();
DWORD WINAPI threadTrataKeys();

HANDLE hMapFile, hMapFileOpen, hInit, eventoCliServer, eventoKeysPlayer;
HANDLE hThreadTimer = 0, hThreadItem = 0, hThreadStart =0;
DWORD idThreadTimer, idThreadItem, idThreadStart = 0, idThreadDuracaoItemsOleo, idThreadDuracaoItemsVodka,
idThreadDuracaoItemsCola, idThreadDuracaoItemsoVodka, idThreadDuracaoItemsoCola, idThreadDuracaoItemsoOleo, idthreadTrataKeys;

Game * clientGame;
Game game;
Game * serverGame;



//Registry
TCHAR playerId[] = TEXT("5969");
int myScore;
int score = 100;

HANDLE eventoFromServerPerdeu; 

int _tmain(int argc, TCHAR *argv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif // UNICODE







	serverGame = (Game*)malloc(sizeof(Game));

	// Abre no server para o client fazer set
	eventoSer = CreateEvent(NULL, TRUE, FALSE, _T("eventoSingle")); 
	eventoKeysPlayer = CreateEvent(NULL, TRUE, FALSE, _T("eventoKeysFromServer"));
	
	
	//Cria a mem partilhada
	hInit = createSharedMemory();



	//Espera evento do client
	WaitForSingleObject(eventoSer, INFINITE);

	//Cli criou para Server fazer SET
	eventoCliServer = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("eventoSingleServerCli")); 

	eventoFromServerPerdeu = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("PerdeuJogo"));

	//Abre mem partilhada
	abreMapView();

	//actualiza o estado do jogo com info recebida do client
	escreveMemoria(serverGame, clientGame);
	
	
	if (!serverGame->gameDefinition.gameType) {
		//LOCAL
		
		_tprintf(TEXT("Entrei"));
		hThreadStart = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadStart, NULL, 0, &idThreadStart);
	
		while (1) {

		}
		
	
	}
	else {
		closeSharedMemory();
		//MULTIPLAYER
	}


}


void escreveMemoria(Game *destinoMsg, Game * origemMsg) {
	CopyMemory(destinoMsg, origemMsg, sizeof(Game));
}

void iniciaJogoLocal()
{
	criaMapa();
	geraSnakes();
	geraCobrasAuto();
	inicializaItems();
	geraItems();
	escreveMemoria(clientGame, serverGame);
	SetEvent(eventoCliServer);
	ResetEvent(eventoCliServer);

	//imprimeMapa();
	
}

//ver as coordenadas fixas, arranjar de acordo com o tamanho do mapa? fazer random?

void geraSnakes()
{
	int numJogadores = serverGame->currentPlayers;
	int numTilesX = serverGame->gameDefinition.gameSizeX;
	int numTilesY = serverGame->gameDefinition.gameSizeY;
	int snakeStartSize = serverGame->gameDefinition.snakeSize;
	

	for (int i = 0; i < numJogadores; i++) {

		Snake *snake = (Snake*)malloc(sizeof(Snake));

		snake->snakeActualSize = snakeStartSize;
		snake->idPlayer = 1;
		snake->idSnake = 1;
		snake->size = serverGame->gameDefinition.snakeSize;
		snake->speed = SnakeSpeedDefault;
		snake->head.direction = kLEFT;

		snake->head.x = numTilesX - (numTilesX - (i + 1) * 7);
		snake->head.y = numTilesY - (numTilesY - (i + 1) * 7);
		serverGame->map[snake->head.x][snake->head.y].idSnake = snake->idSnake;
		serverGame->map[snake->head.x][snake->head.y].idTile = kSnakeHeadTile;
		


		for (int j = 0; j < snakeStartSize; j++) {
			
				snake->body[j].y = snake->head.y + 1 + j;
				snake->body[j].x = snake->head.x;

				serverGame->map[snake->body[j].x][snake->body[j].y].idSnake = snake->idSnake;
				serverGame->map[snake->body[j].x][snake->body[j].y].idTile = kSnakeTile;
				serverGame->map[snake->body[j].x][snake->body[j].y].direction = kLEFT;
			

		}
		serverGame->snakes[i] = *snake;
	
	}
}

void inicializaItems()
{
	int maxItems = serverGame->gameDefinition.nObjects;
	int k = 0;
	for (k = 0; k < maxItems; k++) {
		serverGame->objects[k].idFood = NULL;
		serverGame->objects[k].x = NULL;
		serverGame->objects[k].y = NULL;
	}
}

//falta adicionar item ao array etc
void geraItems()
{
	int maxItems = serverGame->gameDefinition.nObjects;
	int x, y;
	int xItemAntigo, yItemAntigo;
	int sizeX = serverGame->gameDefinition.gameSizeX;
	int sizeY = serverGame->gameDefinition.gameSizeY;
	int seed = time(NULL);
	int itemEscolhido;
	srand(seed);
	
	for (int i = 0; i < maxItems; i++) {
		
			itemEscolhido = 1 + (rand() % 12); //random de 1 a 11 inclusive
	
			do {
				x = 1 + (rand() % (sizeX - 2));
				y = 1 + (rand() % (sizeY - 2));
			} while (serverGame->map[x][y].idTile != kFloor);

			if (serverGame->objects[i].x != NULL) {
				xItemAntigo = serverGame->objects[i].x;
				yItemAntigo = serverGame->objects[i].y;
				serverGame->map[xItemAntigo][yItemAntigo].idTile = kFloor;

				serverGame->objects[i].idFood = itemEscolhido;
				serverGame->objects[i].x = x;
				serverGame->objects[i].y = y;
				serverGame->map[x][y].idTile = itemEscolhido;
			}
			else {
				serverGame->objects[i].idFood = itemEscolhido;
				serverGame->objects[i].x = x;
				serverGame->objects[i].y = y;
				serverGame->map[x][y].idTile = itemEscolhido;
			}

			}		
}

//ver as coordenadas fixas, arranjar de acordo com o tamanho do mapa? fazer random?
void geraCobrasAuto()
{
	int numCobrasAuto = serverGame->gameDefinition.nSnakes;
	int numTilesX = serverGame->gameDefinition.gameSizeX;
	int numTilesY = serverGame->gameDefinition.gameSizeY;
	int snakeStartSize = serverGame->gameDefinition.snakeSize;
	

	for (int i = 0; i < numCobrasAuto; i++) {

		Snake *snake = (Snake*)malloc(sizeof(Snake));

		snake->snakeActualSize = snakeStartSize;
		snake->idPlayer = 0;
		snake->idSnake = 100+i;
		snake->size = serverGame->gameDefinition.snakeSize;
		snake->speed = SnakeSpeedDefault;

		snake->head.x = 15+i;
		snake->head.y = (i+1)*4;
		serverGame->map[snake->head.x][snake->head.y].idSnake = snake->idSnake;
		serverGame->map[snake->head.x][snake->head.y].idTile = kSnakeHeadAuto;
		snake->head.direction = kLEFT;
		serverGame->map[snake->head.x][snake->head.y].direction = kLEFT;

		for (int j = 0; j < snakeStartSize; j++) {


			snake->body[j].y = snake->head.y + 1 + j;
			snake->body[j].x = snake->head.x;
			snake->body[j].direction = kLEFT;

			serverGame->map[snake->body[j].x][snake->body[j].y].idSnake = snake->idSnake;
			serverGame->map[snake->body[j].x][snake->body[j].y].idTile = kSnakeAuto;
			serverGame->map[snake->body[j].x][snake->body[j].y].direction = kLEFT;


		}
		serverGame->autoSnakes[i] = *snake;

	}
	serverGame->numAutoSnakes = numCobrasAuto;

}

void comeItems(int item, DWORD idCobra)
{
	
	switch (item) {
	case kAlimento:
		comeAlimento(idCobra);
		if (idCobra < 100) {
			for (int i = 0; i < serverGame->currentPlayers; i++) {
				if (serverGame->players[i].idSnake == idCobra) {
					serverGame->players[i].score = serverGame->players[i].score + 2;
					_tprintf(TEXT("Score actual = "), serverGame->players[i].score);
				}
			}
		}
		
		break;
	case kGelo:
		comeGelo(idCobra);
		if (idCobra < 100) {
			for (int i = 0; i < serverGame->currentPlayers; i++) {
				if (serverGame->players[i].idSnake == idCobra) {
					serverGame->players[i].score = serverGame->players[i].score -2;
				}
			}
		}
		break;
	case kOleo:
		comeOleo(idCobra);
		break;
	case kCola:
		comeCola(idCobra);
		break;		
	case kGranada:
		comeGranada(idCobra);
		if (idCobra > 100) {
		
		}
		break;
	case kVodka:
		comeVodka(idCobra);
		break;
	case kOVodka: 
		comeOVodka(idCobra);
		break;
	case kOOleo:
		comeOOleo(idCobra);
		break;
	case kOCola :
		comeOCola(idCobra);
		break;
	case kItemEspecial:
		comeItemEspecial(idCobra);
		break;
		
	case kCafe :
		break;
	}
}

void criaMapa() {
	Tile tile;
	int tamX = serverGame->gameDefinition.gameSizeX;
	int tamY = serverGame->gameDefinition.gameSizeY;

	for (int i = 0; i < tamX; i++) {
		for (int j = 0; j < tamY; j++) {
			tile.x = i;
			tile.y = j;
			if (i == 0 || i == tamX - 1 || j == 0 || j == tamY - 1) {
				tile.idTile = kWalls;
			}
			else {
				tile.idTile = kFloor;
			}
			tile.idSnake = 0; //nao tem snakes
			tile.direction = kUNDEFINED; //nao tem direcao?
			serverGame->map[i][j] = tile;
		}
	}

}

void imprimeMapa(){
	_tprintf(TEXT("\n"));
	int tamX = serverGame->gameDefinition.gameSizeX;
	int tamY = serverGame->gameDefinition.gameSizeY;

	for (int i = 0; i < tamX; i++) {
		for (int j = 0; j < tamY; j++) {
			_tprintf(TEXT("%d  "), serverGame->map[i][j].idTile);
		}
		_tprintf(TEXT("\n"));

	}

}

void movimentoCobrasAuto() {

	int seed = time(NULL);
	srand(seed);
	int direccao, direccaoProibida;
	int numCobrasAuto = serverGame->numAutoSnakes;
	int xHead, yHead, xBody, yBody;
	int snakeSize;
	int direccaoBody;
	int direccaoNova;
	int speed = 0;

	for (int i = 0; i < numCobrasAuto; i++) {

		BOOL flagColisao = TRUE;
		speed = serverGame->autoSnakes[i].speed;
		snakeSize = serverGame->autoSnakes[i].snakeActualSize;
		direccaoProibida = 0;
		//comecou
		xHead = serverGame->autoSnakes[i].head.x;
		yHead = serverGame->autoSnakes[i].head.y;
		xBody = serverGame->autoSnakes[i].body[0].x; //1 parte do corpo
		yBody = serverGame->autoSnakes[i].body[0].y; //1 parte do corpo
		direccaoProibida = verificaPosicaoCabecaTronco(xHead, yHead, xBody, yBody);

		do {
			direccao = (rand() % 4);
			flagColisao = verificaPosicaoSeguinte(direccao, xHead, yHead, speed);
		}
		while (direccaoProibida == direccao || flagColisao == false);
	


		for (int k = 0; k < speed; k++) {

			xHead = serverGame->autoSnakes[i].head.x;
			yHead = serverGame->autoSnakes[i].head.y;
			serverGame->map[xHead][yHead].direction = direccao;
			serverGame->map[xHead][yHead].idTile = kFloor;
			serverGame->map[xHead][yHead].idSnake = 0;

			switch (direccao) {
			case kUP:
				if (serverGame->map[xHead - 1 ][yHead].idTile != kFloor) {
					if (serverGame->autoSnakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead -1][yHead].idTile, serverGame->autoSnakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead  -1][yHead].idTile == kGranada) {
					serverGame->map[xHead  -1][yHead].idTile = kFloor;
				}
				else {
					serverGame->map[xHead - 1][yHead].idTile = kSnakeHeadAuto;
					serverGame->map[xHead - 1][yHead].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].head.x = serverGame->autoSnakes[i].head.x - 1;
					serverGame->autoSnakes[i].head.y = yHead;
				}

				break;
			case kDOWN:
				if (serverGame->map[xHead + 1][yHead].idTile != kFloor) {
					if (serverGame->autoSnakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead + 1][yHead].idTile, serverGame->autoSnakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead + 1][yHead].idTile == kGranada) {
					serverGame->map[xHead + 1][yHead].idTile = kFloor;
				}
				else {
					serverGame->map[xHead + 1][yHead].idTile = kSnakeHeadAuto;
					serverGame->map[xHead + 1][yHead].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].head.x = serverGame->autoSnakes[i].head.x + 1;
					serverGame->autoSnakes[i].head.y = yHead;
				}

				break;
			case kLEFT:
				if (serverGame->map[xHead][yHead-1].idTile != kFloor) {
					if (serverGame->autoSnakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead][yHead-1].idTile, serverGame->autoSnakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead ][yHead-1].idTile == kGranada) {
					serverGame->map[xHead ][yHead-1].idTile = kFloor;
				}
				else {
					serverGame->map[xHead][yHead - 1].idTile = kSnakeHeadAuto;
					serverGame->map[xHead][yHead - 1].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].head.y = serverGame->autoSnakes[i].head.y - 1;
					serverGame->autoSnakes[i].head.x = xHead;
				}
				break;
			case kRIGHT:
				if (serverGame->map[xHead][yHead+1].idTile != kFloor) {
					if (serverGame->autoSnakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead][yHead+1].idTile, serverGame->autoSnakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead][yHead+1].idTile == kGranada) {
					serverGame->map[xHead][yHead+1].idTile = kFloor;
				}
				else {
					serverGame->map[xHead][yHead + 1].idTile = kSnakeHeadAuto;
					serverGame->map[xHead][yHead + 1].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].head.y = serverGame->autoSnakes[i].head.y + 1;
					serverGame->autoSnakes[i].head.x = xHead;
				}
				break;
			}
		}
		snakeSize = serverGame->autoSnakes[i].snakeActualSize;
		for (int j = 0; j < snakeSize; j++) {
			int x = 0, y = 0;

			
			for (int p = 0; p < speed; p++) {

				x = serverGame->autoSnakes[i].body[j].x;
				y = serverGame->autoSnakes[i].body[j].y;

				direccaoBody = serverGame->map[x][y].direction;
				serverGame->map[x][y].idTile = kFloor;
				serverGame->map[x][y].idSnake = 0;

				switch (direccaoBody) {
				case kUP:

					serverGame->map[x - 1][y].idTile = kSnakeAuto;
					serverGame->map[x - 1][y].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].body[j].x = serverGame->autoSnakes[i].body[j].x - 1;
					serverGame->autoSnakes[i].body[j].y = y;
					serverGame->autoSnakes[i].body[j].direction = serverGame->map[x - 1][y].direction;

					break;
				case kDOWN:

					serverGame->map[x + 1][y].idTile = kSnakeAuto;
					serverGame->map[x + 1][y].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].body[j].x = serverGame->autoSnakes[i].body[j].x + 1;
					serverGame->autoSnakes[i].body[j].y = y;
					serverGame->autoSnakes[i].body[j].direction = serverGame->map[x + 1][y].direction;
					break;
				case kLEFT:

					serverGame->map[x][y - 1].idTile = kSnakeAuto;
					serverGame->map[x][y - 1].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].body[j].y = serverGame->autoSnakes[i].body[j].y - 1;
					serverGame->autoSnakes[i].body[j].x = x;
					serverGame->autoSnakes[i].body[j].direction = serverGame->map[x][y - 1].direction;
					break;
				case kRIGHT:

					serverGame->map[x][y + 1].idTile = kSnakeAuto;
					serverGame->map[x][y + 1].idSnake = serverGame->autoSnakes[i].idSnake;
					serverGame->autoSnakes[i].body[j].y = serverGame->autoSnakes[i].body[j].y + 1;
					serverGame->autoSnakes[i].body[j].x = x;
					serverGame->autoSnakes[i].body[j].direction = serverGame->map[x][y + 1].direction;

					break;
				}
			}
	
		}

	}


}

void movimentoCobrasPlayers()
{

	int direccao, direccaoProibida;
	int numCobrasAuto = serverGame->currentPlayers;
	int xHead, yHead, xBody, yBody;
	int snakeSize;
	int direccaoBody;
	int direccaoNova;
	int speed = 0;

	for (int i = 0; i < numCobrasAuto; i++) {

		speed = serverGame->snakes[i].speed;
		snakeSize = serverGame->snakes[i].snakeActualSize;
		direccao = serverGame->snakes[i].head.direction;
		//comecou
		

		for (int k = 0; k < speed; k++) {

			xHead = serverGame->snakes[i].head.x;
			yHead = serverGame->snakes[i].head.y;
			serverGame->map[xHead][yHead].direction = direccao;
			serverGame->map[xHead][yHead].idTile = kFloor;
			serverGame->map[xHead][yHead].idSnake = 0;

			switch (direccao) {
			case kUP:

				//Obstaculos
				if(serverGame->map[xHead - 1][yHead].idTile == kWalls ||
					serverGame->map[xHead - 1][yHead].idTile == kSnakeTile ||
					serverGame->map[xHead - 1][yHead].idTile == kSnakeAuto ||
					serverGame->map[xHead - 1][yHead].idTile == kSnakeHeadAuto){
					comeGranada(serverGame->snakes[i].idSnake);
				}
				if (serverGame->map[xHead - 1][yHead].idTile != kFloor) {
					if (serverGame->snakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead - 1][yHead].idTile, serverGame->snakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead - 1][yHead].idTile == kGranada) {
					serverGame->map[xHead - 1][yHead].idTile = kFloor;
				}
				else {
					serverGame->map[xHead - 1][yHead].idTile = kSnakeHeadTile;
					serverGame->map[xHead - 1][yHead].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].head.x = serverGame->snakes[i].head.x - 1;
					serverGame->snakes[i].head.y = yHead;
				}

				break;
			case kDOWN:

				//Obstaculos
				if (serverGame->map[xHead + 1][yHead].idTile == kWalls ||
					serverGame->map[xHead + 1][yHead].idTile == kSnakeTile ||
					serverGame->map[xHead + 1][yHead].idTile == kSnakeAuto ||
					serverGame->map[xHead + 1][yHead].idTile == kSnakeHeadAuto) {
					comeGranada(serverGame->snakes[i].idSnake);
				}

				if (serverGame->map[xHead + 1][yHead].idTile != kFloor) {
					if (serverGame->snakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead + 1][yHead].idTile, serverGame->snakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead + 1][yHead].idTile == kGranada) {
					serverGame->map[xHead + 1][yHead].idTile = kFloor;
				}
				else {
					serverGame->map[xHead + 1][yHead].idTile = kSnakeHeadTile;
					serverGame->map[xHead + 1][yHead].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].head.x = serverGame->snakes[i].head.x + 1;
					serverGame->snakes[i].head.y = yHead;
				}

				break;
			case kLEFT:


				//Obstaculos
				if (serverGame->map[xHead][yHead-1].idTile == kWalls ||
					serverGame->map[xHead][yHead-1].idTile == kSnakeTile ||
					serverGame->map[xHead][yHead-1].idTile == kSnakeAuto ||
					serverGame->map[xHead][yHead-1].idTile == kSnakeHeadAuto) {
					comeGranada(serverGame->snakes[i].idSnake);
				}

				if (serverGame->map[xHead][yHead - 1].idTile != kFloor) {
					if (serverGame->snakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead][yHead - 1].idTile, serverGame->snakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead][yHead - 1].idTile == kGranada) {
					serverGame->map[xHead][yHead - 1].idTile = kFloor;
				}
				else {
					serverGame->map[xHead][yHead - 1].idTile = kSnakeHeadTile;
					serverGame->map[xHead][yHead - 1].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].head.y = serverGame->snakes[i].head.y - 1;
					serverGame->snakes[i].head.x = xHead;
				}
				break;
			case kRIGHT:

				//Obstaculos
				if (serverGame->map[xHead][yHead + 1].idTile == kWalls ||
					serverGame->map[xHead][yHead + 1].idTile == kSnakeTile ||
					serverGame->map[xHead][yHead + 1].idTile == kSnakeAuto ||
					serverGame->map[xHead][yHead + 1].idTile == kSnakeHeadAuto) {
					comeGranada(serverGame->snakes[i].idSnake);
				}

				if (serverGame->map[xHead][yHead + 1].idTile != kFloor) {
					if (serverGame->snakes[i].snakeActualSize < MaxSnakeBodySize) {
						comeItems(serverGame->map[xHead][yHead + 1].idTile, serverGame->snakes[i].idSnake);
					}
				}
				//para nao pintar a cabeça da cobra depois de morta
				if (serverGame->map[xHead][yHead + 1].idTile == kGranada) {
					serverGame->map[xHead][yHead + 1].idTile = kFloor;
				}
				else {
					serverGame->map[xHead][yHead + 1].idTile = kSnakeHeadTile;
					serverGame->map[xHead][yHead + 1].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].head.y = serverGame->snakes[i].head.y + 1;
					serverGame->snakes[i].head.x = xHead;
				}
				break;
			}
		}
		snakeSize = serverGame->snakes[i].snakeActualSize;

		for (int j = 0; j < snakeSize; j++) {
			int x = 0, y = 0;


			for (int p = 0; p < speed; p++) {

				x = serverGame->snakes[i].body[j].x;
				y = serverGame->snakes[i].body[j].y;

				direccaoBody = serverGame->map[x][y].direction;
				serverGame->map[x][y].idTile = kFloor;
				serverGame->map[x][y].idSnake = 0;

				switch (direccaoBody) {
				case kUP:

					serverGame->map[x - 1][y].idTile = kSnakeTile;
					serverGame->map[x - 1][y].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].body[j].x = serverGame->snakes[i].body[j].x - 1;
					serverGame->snakes[i].body[j].y = y;
					serverGame->snakes[i].body[j].direction = serverGame->map[x - 1][y].direction;

					break;
				case kDOWN:

					serverGame->map[x + 1][y].idTile = kSnakeTile;
					serverGame->map[x + 1][y].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].body[j].x = serverGame->snakes[i].body[j].x + 1;
					serverGame->snakes[i].body[j].y = y;
					serverGame->snakes[i].body[j].direction = serverGame->map[x + 1][y].direction;
					break;
				case kLEFT:

					serverGame->map[x][y - 1].idTile = kSnakeTile;
					serverGame->map[x][y - 1].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].body[j].y = serverGame->snakes[i].body[j].y - 1;
					serverGame->snakes[i].body[j].x = x;
					serverGame->snakes[i].body[j].direction = serverGame->map[x][y - 1].direction;
					break;
				case kRIGHT:

					serverGame->map[x][y + 1].idTile = kSnakeTile;
					serverGame->map[x][y + 1].idSnake = serverGame->snakes[i].idSnake;
					serverGame->snakes[i].body[j].y = serverGame->snakes[i].body[j].y + 1;
					serverGame->snakes[i].body[j].x = x;
					serverGame->snakes[i].body[j].direction = serverGame->map[x][y + 1].direction;

					break;
				}
			}

		}

	}
}

BOOL verificaPosicaoSeguinte(int nextDirection, int x, int y, int speed)
{
	for (int j = 0; j < speed; j++) {
		switch (nextDirection) {
		case kUP:
			if (serverGame->map[x - (1 + j)][y].idTile == kSnakeHeadAuto ||
				serverGame->map[x - (1 + j)][y].idTile == kSnakeTile||
				serverGame->map[x - (1+j)][y].idTile == kWalls || 
				serverGame->map[x - (1+j)][y].idTile == kSnakeAuto || 
				serverGame->map[x - (1 + j)][y].idTile == kSnakeHeadTile  ) {
				return FALSE;
			}
			break;
		case kDOWN:
			if (serverGame->map[x + (j + 1)][y].idTile == kSnakeHeadAuto ||
				serverGame->map[x + (j + 1)][y].idTile == kSnakeTile ||
				serverGame->map[x + (j+1)][y].idTile == kWalls || 
				serverGame->map[x + (j+1)][y].idTile == kSnakeAuto ||
				serverGame->map[x + (j + 1)][y].idTile == kSnakeHeadTile ) {
				return FALSE;
			}
			break;
		case kLEFT:
			if (serverGame->map[x][y - (j + 1)].idTile == kSnakeHeadTile ||
				serverGame->map[x][y - (j + 1)].idTile == kSnakeTile || 
				serverGame->map[x][y - (j+1)].idTile == kWalls || 
				serverGame->map[x][y - (j+1)].idTile == kSnakeAuto ||
				serverGame->map[x][y - (j + 1)].idTile == kSnakeHeadAuto ) {
				return FALSE;
			}
			break;
		case kRIGHT:
			if (serverGame->map[x][y + (j + 1)].idTile == kSnakeHeadAuto ||
				serverGame->map[x][y + (j + 1)].idTile == kSnakeTile ||
				serverGame->map[x][y + (j+1)].idTile == kWalls ||
				serverGame->map[x][y + (j+1)].idTile == kSnakeAuto ||
				serverGame->map[x][y + (j + 1)].idTile == kSnakeHeadTile ) {
				return FALSE;
			}
			break;
		}
	}
	return TRUE;

	
}

void abreMapView()
{
	clientGame = (Game*)MapViewOfFile(
		hInit,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(Game));

	if (clientGame == NULL) {
		_tprintf(TEXT("A view da memoria partilhada nao foi criada ( erro: %d). \n"), GetLastError());
		CloseHandle(hInit);
		return NULL;
	}
}

void comeAlimento(DWORD idCobra)
{
	if (idCobra < 100) {
		for (int i = 0; i < serverGame->currentPlayers; i++) {
			if (idCobra == serverGame->snakes[i].idSnake) {

				int tam = serverGame->snakes[i].snakeActualSize;
				int direccaoBodyAnterior = serverGame->snakes[i].body[tam - 1].direction;
				serverGame->snakes[i].body[tam].direction = serverGame->snakes[i].body[tam - 1].direction;

				if (direccaoBodyAnterior == kUP) {
					serverGame->snakes[i].body[tam].x = serverGame->snakes[i].body[tam - 1].x + 1;
					serverGame->snakes[i].body[tam].y = serverGame->snakes[i].body[tam - 1].y;
				}
				else if (direccaoBodyAnterior == kDOWN) {
					serverGame->snakes[i].body[tam].x = serverGame->snakes[i].body[tam - 1].x - 1;
					serverGame->snakes[i].body[tam].y = serverGame->snakes[i].body[tam - 1].y;
				}
				else if (direccaoBodyAnterior == kLEFT) {
					serverGame->snakes[i].body[tam].x = serverGame->snakes[i].body[tam - 1].x;
					serverGame->snakes[i].body[tam].y = serverGame->snakes[i].body[tam - 1].y + 1;
				}
				else if (direccaoBodyAnterior == kRIGHT) {
					serverGame->snakes[i].body[tam].x = serverGame->snakes[i].body[tam - 1].x;
					serverGame->snakes[i].body[tam].y = serverGame->snakes[i].body[tam - 1].y - 1;
				}


				serverGame->snakes[i].snakeActualSize++;


				int direccao = serverGame->snakes[i].body[tam].direction;
				int y1 = serverGame->snakes[i].body[tam].y;
				int x1 = serverGame->snakes[i].body[tam].x;

				serverGame->map[x1][y1].direction = direccao;
				serverGame->map[x1][y1].idSnake = idCobra;
				serverGame->map[x1][y1].idTile = kSnakeTile;
			}
		}
	}
	else {
		for (int i = 0; i < serverGame->numAutoSnakes; i++) {
			if (idCobra == serverGame->autoSnakes[i].idSnake) {

				int tam = serverGame->autoSnakes[i].snakeActualSize;
				int direccaoBodyAnterior = serverGame->autoSnakes[i].body[tam - 1].direction;
				serverGame->autoSnakes[i].body[tam].direction = serverGame->autoSnakes[i].body[tam - 1].direction;

				if (direccaoBodyAnterior == kUP) {
					serverGame->autoSnakes[i].body[tam].x = serverGame->autoSnakes[i].body[tam - 1].x+1;
					serverGame->autoSnakes[i].body[tam].y = serverGame->autoSnakes[i].body[tam - 1].y;
				}
				else if (direccaoBodyAnterior == kDOWN) {
					serverGame->autoSnakes[i].body[tam].x = serverGame->autoSnakes[i].body[tam - 1].x-1;
					serverGame->autoSnakes[i].body[tam].y = serverGame->autoSnakes[i].body[tam - 1].y;
				}
				else if (direccaoBodyAnterior == kLEFT) {
					serverGame->autoSnakes[i].body[tam].x = serverGame->autoSnakes[i].body[tam - 1].x;
					serverGame->autoSnakes[i].body[tam].y = serverGame->autoSnakes[i].body[tam - 1].y+1;
				}
				else if (direccaoBodyAnterior == kRIGHT) {
					serverGame->autoSnakes[i].body[tam].x = serverGame->autoSnakes[i].body[tam - 1].x;
					serverGame->autoSnakes[i].body[tam].y = serverGame->autoSnakes[i].body[tam - 1].y-1;
				}
				
				serverGame->autoSnakes[i].snakeActualSize++;

				int direccao = serverGame->autoSnakes[i].body[tam].direction;
				int y1 = serverGame->autoSnakes[i].body[tam].y;
				int x1 = serverGame->autoSnakes[i].body[tam].x;

				serverGame->map[x1][y1].direction = direccao;
				serverGame->map[x1][y1].idSnake = idCobra;
				serverGame->map[x1][y1].idTile = kSnakeAuto;
			}
		}
	}

}

void comeGelo(DWORD idCobra)

//Nao remove a cabeça da cobra!
{
	if (idCobra < 100) {
		for (int i = 0; i < serverGame->currentPlayers; i++) {
			if (idCobra == serverGame->snakes[i].idSnake) {

				if (idCobra == serverGame->snakes[i].idSnake) {
					int tam = serverGame->snakes[i].snakeActualSize;
					int x, y;
					x = serverGame->snakes[i].body[tam - 1].x;
					y = serverGame->snakes[i].body[tam - 1].y;
					serverGame->snakes[i].snakeActualSize--;
					serverGame->map[x][y].idSnake = 0;
					serverGame->map[x][y].idTile = kFloor;
				}
			}
		}
	}
	else {
		for (int i = 0; i < serverGame->numAutoSnakes; i++) {
			if (idCobra == serverGame->autoSnakes[i].idSnake) {
				int tam = serverGame->autoSnakes[i].snakeActualSize;
				int x, y;
				x = serverGame->autoSnakes[i].body[tam - 1].x;
				y = serverGame->autoSnakes[i].body[tam - 1].y;
				serverGame->autoSnakes[i].snakeActualSize--;
				serverGame->map[x][y].idSnake = 0;
				serverGame->map[x][y].idTile = kFloor;
				
			}
		}
	}
}

void comeGranada(DWORD idCobra)
{
	if (idCobra < 100) {
		for (int i = 0; i < serverGame->currentPlayers; i++) {
			if (idCobra == serverGame->snakes[i].idSnake) {

				int x, y;

				x = serverGame->snakes[i].head.x;
				y = serverGame->snakes[i].head.y;



				for (int j = 0; j < serverGame->snakes[i].snakeActualSize; j++) {

					x = serverGame->snakes[i].body[j].x;
					y = serverGame->snakes[i].body[j].y;

					serverGame->map[x][y].idSnake = 0;
					serverGame->map[x][y].idTile = kFloor;
				}

				apagaCobra(i, idCobra);
				SetEvent(eventoFromServerPerdeu);
				ResetEvent(eventoFromServerPerdeu);
				//alterar numero de jogadores
			}
		}
	}
	else {
		for (int i = 0; i < serverGame->numAutoSnakes; i++) {
			if (idCobra == serverGame->autoSnakes[i].idSnake) {
				
				int x, y;

				x = serverGame->autoSnakes[i].head.x;
				y = serverGame->autoSnakes[i].head.y;

				

				for (int j = 0; j < serverGame->autoSnakes[i].snakeActualSize; j++) {

					x = serverGame->autoSnakes[i].body[j].x;
					y = serverGame->autoSnakes[i].body[j].y;

					serverGame->map[x][y].idSnake = 0;
					serverGame->map[x][y].idTile = kFloor;
				}

				apagaCobra(i, idCobra);
				

			}
		}
	}
}
//Fazer mudar a cor e thread de tempo
void comeVodka(DWORD idCobra)
{
	if (idCobra < 100) {
		int tam = serverGame->currentPlayers;

		for (int i = 0; i < tam; i++) {
			if (serverGame->players[i].idSnake == idCobra) {
				
				serverGame->players[i].isPlaying = TRUE;

				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDuracaoItemsVodka, idCobra, 0, &idThreadDuracaoItemsVodka);

			}
		}
	}
}

void comeOVodka(DWORD idCobra)
{
	if (idCobra < 100) {
		int tam = serverGame->currentPlayers;

		for (int i = 0; i < tam; i++) {
			if (serverGame->players[i].idSnake != idCobra) {
				
				serverGame->players[i].isPlaying = TRUE;
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDuracaoItemsoVodka, idCobra, 0, &idThreadDuracaoItemsoVodka);
			}
		}
	}
}

void comeItemEspecial(DWORD idCobra)
{

	int seed = time(NULL);
	srand(seed);
	DWORD numCobraEscolhido;

	//Jogador e que comeu item
	if (idCobra < 100) {
		int numPlayers = serverGame->currentPlayers;

		//ver se o rand reconhece dword
		do {
			numCobraEscolhido = 1 + (rand() % numPlayers);
		} while (numCobraEscolhido == idCobra);
		comeGranada(numCobraEscolhido);
	}
	else {
		int numPlayers = serverGame->numAutoSnakes;

		do {
			numCobraEscolhido = 100 + (rand() % numPlayers);
		} while (numCobraEscolhido == idCobra);
		comeGranada(numCobraEscolhido);
	}
}

void comeOleo(DWORD idCobra)
{
	if (idCobra < 100) {
		int tam = serverGame->currentPlayers;
		for (int i = 0; i < tam; i++) {
			if (serverGame->snakes[i].idSnake == idCobra) {
				serverGame->snakes[i].speed = serverGame->snakes[i].speed * 2;
				//iniciar thread?!
			}
		}
	}
	else {
		int tam = serverGame->numAutoSnakes;
		for (int i = 0; i < tam; i++) {
			if (serverGame->autoSnakes[i].idSnake == idCobra) {
				serverGame->autoSnakes[i].speed = serverGame->autoSnakes[i].speed * 2;
				
			
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDuracaoItemsOleo, idCobra, 0, &idThreadDuracaoItemsOleo);
					
				
			}
		}
	}
}

//Esta a fazer o mesmo efeito que o Oleo
void comeCola(DWORD idCobra) {

	if (idCobra < 100) {
		int tam = serverGame->currentPlayers;
		for (int i = 0; i < tam; i++) {
			if (serverGame->snakes[i].idSnake == idCobra) {
				if (serverGame->snakes[i].speed != 1) {
					serverGame->snakes[i].speed = serverGame->snakes[i].speed / 2;
					CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDuracaoItemsCola, idCobra, 0, &idThreadDuracaoItemsCola);
				}
			}
		}
	}
	else {
		int tam = serverGame->numAutoSnakes;
		for (int i = 0; i < tam; i++) {
			if (serverGame->autoSnakes[i].idSnake == idCobra) {
				if (serverGame->autoSnakes[i].speed != 1) {
					serverGame->autoSnakes[i].speed = serverGame->autoSnakes[i].speed / 2;
					CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDuracaoItemsCola, idCobra, 0, &idThreadDuracaoItemsCola);
					//iniciar thread?!
				}
			}
		}
	}
}

void comeOOleo(DWORD idCobra) {
	
	if(idCobra < 100){
		int tam = serverGame->currentPlayers;
		for (int i = 0; i < tam; i++) {
			if (serverGame->snakes[i].idSnake != idCobra) {
				serverGame->snakes[i].speed = serverGame->snakes[i].speed * 2;
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDuracaoItemsoOleo, idCobra, 0, &idThreadDuracaoItemsoOleo);
			}
		}
		}
}

//igual ao comeOOLEO
void comeOCola(DWORD idCobra) {
	if(idCobra < 100){
	int tam = serverGame->currentPlayers;
	for (int i = 0; i < tam; i++) {
		if (serverGame->snakes[i].idSnake != idCobra) {
			serverGame->snakes[i].speed = serverGame->snakes[i].speed * 2;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDuracaoItemsoCola, idCobra, 0, &idThreadDuracaoItemsoCola);
		}
	}
	}
}



DWORD WINAPI threadTimer()
{
	_tprintf(TEXT("\nThread tempo iniciada......\n"));
	int tempoMaxJogo = serverGame->gameDefinition.duration;
	Sleep(tempoMaxJogo);
	tempoMaxAtingido = TRUE;
}

DWORD WINAPI threadStart()
{
	_tprintf(TEXT("\n Thread principal iniciada...\n"));
	iniciaJogoLocal();
	hThreadTimer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadTimer, NULL, 0, &idThreadTimer);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadItems, NULL, 0, &idThreadItem);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadTrataKeys, NULL, 0, &idthreadTrataKeys);




	while (tempoMaxAtingido == FALSE) {

		movimentoCobrasPlayers();
		movimentoCobrasAuto();

		updateRegistry(playerId, score);

		score = GetMyScoreFromRegistry(playerId);

		_tprintf_s(TEXT("\nmy score is: %d"), score);

		escreveMemoria(clientGame, serverGame);

		SetEvent(eventoCliServer);
		ResetEvent(eventoCliServer);

		Sleep(1000);
		
	}

	

}

DWORD WINAPI threadItems()
{
	while (1) {
		geraItems();
		Sleep(15000);
	}
}

DWORD WINAPI threadRecebeCliente()
{
	
}

DWORD WINAPI threadTrataKeys()
{
	while (true) {
		WaitForSingleObject(eventoKeysPlayer, INFINITE);
		escreveMemoria(serverGame, clientGame);
	}
}

DWORD WINAPI threadDuracaoItemsOleo(LPVOID idCobra) {



	DWORD idCobra2 = (DWORD)idCobra;

	
	//pode dar erro se a cobra comer 2 vezes

	//ver duracao
	Sleep(10000);
	
	if (idCobra2 < 100) {
		for (int i = 0; i < serverGame->currentPlayers; i++) {
			if (serverGame->snakes[i].idSnake == idCobra2) {
				if (serverGame->snakes[i].speed > 1)
					serverGame->snakes[i].speed = serverGame->snakes[i].speed / 2;
			}
		}
	}
	else {
		for (int i = 0; i < serverGame->numAutoSnakes; i++) {
			if (serverGame->autoSnakes[i].idSnake == idCobra2) {
				if (serverGame->autoSnakes[i].speed > 1) {
					serverGame->autoSnakes[i].speed = serverGame->autoSnakes[i].speed / 2;
				}
			}
		}
	}
	
}

DWORD WINAPI threadDuracaoItemsoOleo(LPVOID idCobra)
{
	DWORD idCobra2 = (DWORD)idCobra;


	//pode dar erro se a cobra comer 2 vezes

	//ver duracao
	Sleep(10000);

	if (idCobra2 < 100) {
		for (int i = 0; i < serverGame->currentPlayers; i++) {
			if (serverGame->snakes[i].idSnake != idCobra2) {
				if (serverGame->snakes[i].speed > 1)
					serverGame->snakes[i].speed = serverGame->snakes[i].speed / 2;
			}
		}
	}
	else {
		for (int i = 0; i < serverGame->numAutoSnakes; i++) {
			if (serverGame->autoSnakes[i].idSnake != idCobra2) {
				if (serverGame->autoSnakes[i].speed > 1) {
					serverGame->autoSnakes[i].speed = serverGame->autoSnakes[i].speed / 2;
				}
			}
		}
	}
}

DWORD WINAPI threadDuracaoItemsoCola(LPVOID idCobra)
{
	DWORD idCobra2 = (DWORD)idCobra;


	//pode dar erro se a cobra comer 2 vezes

	//ver duracao
	Sleep(10000);

	if (idCobra2 < 100) {
		for (int i = 0; i < serverGame->currentPlayers; i++) {
			if (serverGame->snakes[i].idSnake != idCobra2) {
				if (serverGame->snakes[i].speed >= 1)
					serverGame->snakes[i].speed = serverGame->snakes[i].speed * 2;
			}
		}
	}
	else {
		for (int i = 0; i < serverGame->numAutoSnakes; i++) {
			if (serverGame->autoSnakes[i].idSnake != idCobra2) {
				if (serverGame->autoSnakes[i].speed >= 1) {
					serverGame->autoSnakes[i].speed = serverGame->autoSnakes[i].speed * 2;
				}
			}
		}
	}
}

DWORD WINAPI threadDuracaoItemsoVodka(LPVOID idCobra)
{
	DWORD idCobra2 = (DWORD)idCobra;

	Sleep(10000);
	if (idCobra2 < 100) {
		int tam = serverGame->currentPlayers;

		for (int i = 0; i < tam; i++) {
			if (serverGame->players[i].idSnake != idCobra2) {
				serverGame->players[i].isPlaying = FALSE;
			}
		}
	}
}

DWORD WINAPI threadDuracaoItemsCola(LPVOID idCobra)
{

	DWORD idCobra2 = (DWORD)idCobra;


	//pode dar erro se a cobra comer 2 vezes

	//ver duracao
	Sleep(10000);

	if (idCobra2 < 100) {
		for (int i = 0; i < serverGame->currentPlayers; i++) {
			if (serverGame->snakes[i].idSnake == idCobra2) {
				if (serverGame->snakes[i].speed >= 1)
					serverGame->snakes[i].speed = serverGame->snakes[i].speed * 2;
			}
		}
	}
	else {
		for (int i = 0; i < serverGame->numAutoSnakes; i++) {
			if (serverGame->autoSnakes[i].idSnake == idCobra2) {
				if (serverGame->autoSnakes[i].speed >= 1) {
					serverGame->autoSnakes[i].speed = serverGame->autoSnakes[i].speed * 2;
				}
			}
		}
	}
}

DWORD WINAPI threadDuracaoItemsVodka(LPVOID idCobra)
{
	DWORD idCobra2 = (DWORD)idCobra;

	Sleep(10000);
	if (idCobra2 < 100) {
		int tam = serverGame->currentPlayers;

		for (int i = 0; i < tam; i++) {
			if (serverGame->players[i].idSnake == idCobra2) {
				serverGame->players[i].isPlaying = FALSE;
			}
		}
	}
}

int verificaPosicaoCabecaTronco(int xHead, int yHead, int xBody, int yBody) {

	//verifica posicao da cabeça para o primeiro segmento do corpo, retorna posicao para onde a cobra nao pode virar
	if (xHead > xBody) {
		return kUP;
	}else if (xHead < xBody) {
		return kDOWN;
	}
	else if (yHead > yBody) {
		return kLEFT;
	}
	else if (yHead < yBody) {
		return kRIGHT;
	}
	
	return 0;
}



void apagaCobra(int posicao, DWORD idCobra) {

	Snake snakePlayer; //player
	Snake snakeEliminada; //auto

	if (idCobra < 100) {

		int i = 0;
			for (int j = 0; j < serverGame->currentPlayers; j++) {
				if (j != posicao) {

					Snake snake;

					snake.idPlayer = serverGame->snakes[j].idPlayer;
					snake.idSnake = serverGame->snakes[j].idSnake;
					snake.size = serverGame->snakes[j].size;
					snake.snakeActualSize = serverGame->snakes[j].snakeActualSize;
					snake.speed = serverGame->snakes[j].speed;
					snake.head.x = serverGame->snakes[j].head.x;
					snake.head.y = serverGame->snakes[j].head.y;
					snake.head.direction = serverGame->snakes[j].head.direction;

					int tam = serverGame->snakes[j].snakeActualSize;

					for (int i = 0; i < tam; i++) {
						snake.body[i].direction = serverGame->snakes[j].body[i].direction;
						snake.body[i].x = serverGame->snakes[j].body[i].x;
						snake.body[i].y = serverGame->snakes[j].body[i].y;
					}

					serverGame->snakes[i] = snake;
					i++;

				}
				else {
					snakePlayer.idSnake = NULL;
					snakePlayer.size = NULL;
					snakePlayer.snakeActualSize = NULL;
					snakePlayer.speed = NULL;
					snakePlayer.head.x = NULL;
					snakePlayer.head.y = NULL;
					snakePlayer.head.direction = NULL;

					int tam = serverGame->snakes[j].snakeActualSize;

					for (int i = 0; i < tam; i++) {
						snakePlayer.body[i].direction = NULL;
						snakePlayer.body[i].x = NULL;
						snakePlayer.body[i].y = NULL;
					}

				}

			}
			serverGame->snakes[serverGame->currentPlayers - 1] = snakePlayer;
			serverGame->currentPlayers = serverGame->currentPlayers - 1;
			//nao altero no array de players!!!
		
	}
	else {
		int i = 0;
		for (int j = 0; j < serverGame->numAutoSnakes; j++) {
			if (j != posicao) {

				Snake snake;

				snake.idSnake = serverGame->autoSnakes[j].idSnake;
				snake.size = serverGame->autoSnakes[j].size;
				snake.snakeActualSize = serverGame->autoSnakes[j].snakeActualSize;
				snake.speed = serverGame->autoSnakes[j].speed;
				snake.head.x = serverGame->autoSnakes[j].head.x;
				snake.head.y = serverGame->autoSnakes[j].head.y;
				snake.head.direction = serverGame->autoSnakes[j].head.direction;
				
				int tam = serverGame->autoSnakes[j].snakeActualSize;

				for (int i = 0; i < tam; i++) {
					snake.body[i].direction = serverGame->autoSnakes[j].body[i].direction;
					snake.body[i].x = serverGame->autoSnakes[j].body[i].x;
					snake.body[i].y = serverGame->autoSnakes[j].body[i].y;
				}

				serverGame->autoSnakes[i] = snake;
				i++;
				
			}
			else {
				snakeEliminada.idSnake = NULL;
				snakeEliminada.size = NULL;
				snakeEliminada.snakeActualSize = NULL;
				snakeEliminada.speed = NULL;
				snakeEliminada.head.x = NULL;
				snakeEliminada.head.y = NULL;
				snakeEliminada.head.direction = NULL;

				int tam = serverGame->autoSnakes[j].snakeActualSize;

				for (int i = 0; i < tam; i++) {
					snakeEliminada.body[i].direction = NULL;
					snakeEliminada.body[i].x = NULL;
					snakeEliminada.body[i].y = NULL;
				}

			}
			
		}
		serverGame->autoSnakes[serverGame->numAutoSnakes-1] = snakeEliminada;
		serverGame->numAutoSnakes = serverGame->numAutoSnakes - 1;
	
	}
}

HKEY OpenRegistryKey(wchar_t* strKey)
{
	HKEY hKey;
	LONG nError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKey, NULL, KEY_ALL_ACCESS, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND)
	{
		_tprintf_s(TEXT("\nChave nao existente. A criar..."));
		nError = RegCreateKeyEx(HKEY_LOCAL_MACHINE, strKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}
	else {
		_tprintf_s(TEXT("\nChave encontrada."));
	}

	if (nError)
		_tprintf_s(TEXT("\nChave nao encontrada ou criada."));

	return hKey;
}

void updateRegistry(TCHAR playerId[], int newScore)
{
	TCHAR sk[] = TEXT("SOFTWARE\\SNAKE");
	static DWORD regScore;

	HKEY hKey = OpenRegistryKey(sk);

	regScore = GetValueFromRegistry(hKey, playerId);

	regScore = newScore;

	SetRegistryValue(hKey, playerId, regScore);

	RegCloseKey(hKey);
}

DWORD GetValueFromRegistry(HKEY hKey, LPCTSTR lpValue)
{
	DWORD data;
	DWORD size = sizeof(data);
	DWORD type = REG_DWORD;
	LONG nError;

	nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
		data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		_tprintf_s(TEXT("\nErro a aceder a valor."));

	return data;
}

void SetRegistryValue(HKEY hKey, LPCTSTR lpValue, DWORD data)
{
	LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_DWORD, (LPBYTE)&data, sizeof(DWORD));

	if (nError)
		_tprintf_s(TEXT("\nErro a introduzir valor."));
}

int GetMyScoreFromRegistry(TCHAR playerId[])
{
	TCHAR sk[] = TEXT("SOFTWARE\\SNAAKE");

	int currentScore;

	HKEY hKey = OpenRegistryKey(sk);

	currentScore = GetValueFromRegistry(hKey, playerId);

	RegCloseKey(hKey);

	return currentScore;
}




