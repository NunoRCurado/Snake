#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdbool.h>
#include <Shlwapi.h>

//Constants
#define kBufferSize 512
#define MaxItems 20
#define MaxPlayers 20
#define MaxSnakes 20
#define MaxAutoSnakes 5
#define MaxSnakeBodySize 10
#define ObjectProbability 50
#define sharedMemoryName TEXT("SharedMemory")
#define hMapChangedEvent TEXT("hMapChangedEvent")
#define MaxSizeX 80
#define MaxSizeY 40
#define Duration 1000
#define kUP 1
#define kDOWN 0
#define kLEFT 3
#define kRIGHT 2 
#define kUNDEFINED 10
#define SnakeSpeedDefault 2 // 2 tile a cada iteraçao?

//Para os settings verificoes
#define MIN_SIZE_SNAKE 1
#define MAX_SIZE_SNAKE 8
#define MAX_DURATION_GAME 5
#define MIN_DURATION_GAME 1
#define MAX_NUMBER_ITEM 40
#define MIN_NUMBER_ITEM 0
#define MIN_PROP_RARE_ITEMS 0
#define MAX_PROP_RARE_ITEMS 100
#define MAX_HORIZONTAL_SIZE 100
#define MIN_HORIZONTAL_SIZE 20
#define MAX_VERTICAL_SIZE 50
#define MIN_VERTICAL_SIZE 10
#define MAX_LOCAL_PLAYERS 2
#define MIN_LOCAL_PLAYERS 1

#define MAX_COBRAS_AUTO 3
#define MIN_COBRAS_AUTO 0


#define MAX_NUMBER_PLAYER 20
#define MIN_NUMBER_PLAYER 2

//Snake
#define kSnakeHeadTile 40
#define kSnakeTile 20
#define kSnakeAuto 30
#define kSnakeHeadAuto 50

//Chao mapa
#define kWalls 15
#define kFloor 16



//Objectos comuns e invulgares
#define kAlimento 1
#define kGelo 2
#define kOleo 3
#define kCola 4

//Objectos raros
#define kGranada 5
#define kVodka 6
#define kOVodka 7
#define kOOleo 8
#define kOCola 9
#define kCafe 10
#define kItemEspecial 11



//cores para as cobras

#define kBlue 1
#define kRed 2
#define kGreen 3
#define kYellow 4
#define kOrange 5
#define kBlack 6



typedef struct Snake Snake;
typedef struct Player Player;
typedef struct Object Object;
typedef struct Game Game;
typedef struct GameDefinition GameDefinition;
typedef struct Tile Tile;
typedef struct ThreadItems ThreadItems;

struct ThreadItems{
	DWORD idCobra;
	DWORD idThread;
};

struct Tile {
	DWORD idTile; 
	DWORD idSnake; // 0 = nao tem snakes
	DWORD x;
	DWORD y;
	DWORD direction;
};

struct Snake {
	DWORD idSnake; 
	DWORD idPlayer; //se nao tiver player associado = snakeAuto
	Tile head; //sitio onde está a cobra
	Tile body[MaxSnakeBodySize];
	DWORD speed;
	DWORD size;
	DWORD snakeActualSize; //tamanho da cobra 
	DWORD color;
};

struct Player {
	TCHAR name[kBufferSize];
	DWORD player_id; //this value must be the same as the position on the array of pipe handles || maior que 0
	DWORD idSnake; //id da cobra que pertence ao jogador
	TCHAR buttonUp;
	TCHAR buttonRight;
	TCHAR buttonLeft;
	TCHAR buttonDown;
	bool isPlaying;
	DWORD score;
};

struct Object {
	DWORD x;
	DWORD y;
	DWORD idFood;

};

struct GameDefinition {
	DWORD nLocalPlayers; //Numero de jogadores locais na maquina, 1 ou 2 jogadores
	DWORD nMaxPlayers; //Numero maximo de jogadores humanos a criar jogo
	DWORD nSnakes; //Numero de snakes automaticas
	DWORD nObjects; //Numero total de objetos existentes no mapa
	DWORD snakeSize; //Tamanho inicial das serpentes
	DWORD duration; //Duração que o jogo pode tomar 
	DWORD objectProbability; //Probabilidade de um objecto aparecer 0-100%
	DWORD gameSizeX; //Dimensão do tabuleiro horizontalmente
	DWORD gameSizeY; //Dimensão do tabuleiro Verticalmente
	BOOL gameType;// 0 LOCAL  1 MULTIPLAYER
};


struct Game {
	Player players[MaxPlayers]; //Array de Jogadores incritos na partida
	Snake snakes[MaxSnakes]; // Array de serpentes
	Snake autoSnakes[MaxAutoSnakes];
	Object objects[MaxItems]; //Array de Objectos e de cada tipo
	Tile map[MaxSizeY][MaxSizeX]; //Mapa da partida que vai ser partilhado entre jogadores
	DWORD currentPlayers; //Numero de jogadores ativos, de forma controlar o array de jogadores
	DWORD numAutoSnakes; //numero actual de snakes autos
	bool gameIsReady; //Booleana para saber se os jogadores já estão todos pronto para o jogo começar
	bool mapChanged;
	GameDefinition gameDefinition;

};

