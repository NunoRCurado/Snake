#include "..\DLL\dll.h"
#include "resource.h"







#define TAM 500
#define BUFSIZE 4096
// Variável global hInstance usada para guardar "hInst" inicializada na função
// WinMain(). "hInstance" é necessária no bloco WinProc() para lançar a Dialog
// box em DialogBox(...) 
HINSTANCE hInstance;


// Declaração antecipada da função de processamento da janela "WndProc()"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//trata dos eventos da janela principal

// Declaração antecipada da função de processamento da DialogBox, "DialogProc()"
BOOL CALLBACK DialogPS(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogPM(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogSinglePlayer(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogMultiPlayer(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

BOOLEAN	verifySettingRequest(int request, int valueToVerify);
void pintaMapaGraphic();
void iniciaBitMaps(HINSTANCE hInst);
void destroiBitMaps();
void inicializaSincronização();
void clientSettingsLocal();


void escreveMemoria(Game *destinoMsg, Game * origemMsg);

BOOL testeMemoriaPartilhada();
HANDLE  hMapFileOpen, hThreadUpdateMap, hThreadStartGame, hThreadSendComand, hThreadPrintMap;
HANDLE eventoFromServer, eventoFromServerPerdeu, eventoKeysFromServer;
DWORD WINAPI ThreadStartGame();
DWORD WINAPI ThreadSendComand();
DWORD WINAPI ThreadPrintMap();
DWORD WINAPI ThreadMorreu();
DWORD ThreadStartGameID = 0, ThreadSendComandID = 0, ThreadPrintMapID = 0, ThreadMorreuID;
HANDLE hPipeClienteComandos, hPipeClienteActualizacao;
HWND hWnd;			// handler da janela (a gerar por CreateWindow())
HBITMAP initialImage, hbit;
HBITMAP walls, alimento, gelo, granada, vodka, oleo, cola, oVodka, oOleo, oCola, head, body, bodyAuto, special, cafe;
HDC h_walls, h_alimento, h_gelo, h_granada, h_vodka, h_oleo, h_cola, h_oVodka, h_oOleo, h_oCola, h_head, h_body, h_bodyAuto, h_special, h_cafe;
HDC memdc, hdc, auxdc;
HWND hWnd2;

Game * serverGame;
Game * clientGame;

TCHAR szProgName[] = TEXT("Snake Game");

void inicializaSincronização() {
	eventoFromServer = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("eventoSingle"));
	eventoKeysFromServer = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("eventoKeysFromServer"));
	eventoFromServerPerdeu = CreateEvent(NULL, TRUE, FALSE, _T("PerdeuJogo"));
	eventoCli =				 CreateEvent(NULL, TRUE, FALSE, _T("eventoSingleServerCli"));

}



void escreveMemoria(Game *destinoMsg, Game * origemMsg) {
	CopyMemory(destinoMsg, origemMsg, sizeof(Game));
}

//ver isto exame
BOOL testeMemoriaPartilhada() {

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

DWORD WINAPI ThreadStartGame() {

	hThreadSendComand = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadSendComand, NULL, 0, &ThreadSendComandID);
	hThreadPrintMap = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadPrintMap, NULL, 0, &ThreadPrintMapID);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadMorreu, NULL, 0, &ThreadMorreuID);

	while (1) {
		
	}
	return 0;
}

DWORD WINAPI ThreadSendComand() {

	return 0;
}

DWORD WINAPI ThreadPrintMap() {
	while (TRUE) {

		WaitForSingleObject(eventoCli, INFINITE);
		escreveMemoria(clientGame, serverGame);
		pintaMapaGraphic();
	}

	return 0;
}

DWORD WINAPI ThreadMorreu(){
	
	
	WaitForSingleObject(eventoFromServerPerdeu, INFINITE);
	
	MessageBox(hWnd, TEXT("MORREU!"), TEXT("ADEUS"), MB_OK);

	return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	MSG lpMsg;			// Estrutura das mensagens
						// Estrutura que define a classe da janela
	WNDCLASSEX wcApp;

	clientGame = (Game*)malloc(sizeof(Game));

	inicializaSincronização();

	testeMemoriaPartilhada();

	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInst;
	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = WndProc;
	wcApp.style = CS_HREDRAW | CS_VREDRAW;
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// ícon normal=Aplicação do Windows	
	wcApp.hIconSm = LoadIcon(hInst, (LPCTSTR)IDI_ICON1);													// programador		
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);			// rato = "seta"
	wcApp.lpszMenuName = (LPCTSTR)IDR_MENU1;		// Menu da janela		
	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if (!RegisterClassEx(&wcApp))
		return(0);

	iniciaBitMaps(hInst);


	hWnd = CreateWindow(
		szProgName,				// Nome da janela e/ou programa
		TEXT("Snake"),	// Título da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela 
		CW_USEDEFAULT,			// Posição x 
		CW_USEDEFAULT,			// Posição y 
		CW_USEDEFAULT,			// Largura 
		CW_USEDEFAULT,			// Altura 
		(HWND)HWND_DESKTOP,	// handle da janela pai (HWND_DESKTOP para 1ª)
		(HMENU)NULL,			// handle do menu (se tiver menu)
		(HINSTANCE)hInst,			// handle da instância actual (vem de WinMain())
		(LPSTR)NULL);			// Não há parâmetros adicionais 

								// ============================================================================
								// Carregar as definições das teclas aceleradoras (atalhos de opções dos Menus)
								// LoadAccelerators(instância_programa, ID_da_resource_atalhos)
								// ============================================================================

	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela
								// "nCmdShow"= modo, parâmetro de WinMain()
	UpdateWindow(hWnd);			// Refrescar a janela (gera WM_PAINT) 


	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MENU1)); //this is for accelerator keys

	while (GetMessage(&lpMsg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(lpMsg.hwnd, hAccelTable, &lpMsg)) {
			TranslateMessage(&lpMsg);		// Pré-processamento da mensagem
			DispatchMessage(&lpMsg);		// Enviar a mensagem traduzida de volta
		}									// ao Windows
	}
	return((int)lpMsg.wParam);	// Status = Parâmetro "wParam" da estrutura "lpMsg"

	destroiBitMaps();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	int resposta;					// Resposta a MessageBox
	PAINTSTRUCT ps;
	// Processamento das mensagens
	switch (messg)
	{

		//ver exame
	case WM_CREATE:
		hdc = GetDC(hWnd);
		memdc = CreateCompatibleDC(hdc);// Criar janela virtual
										//initialImage = CreateCompatibleBitmap(hdc, maxX, maxY);
										// Criar janela virtual
		hbit = CreateCompatibleBitmap(hdc, 20 * 32, 10 * 32);


		SelectObject(memdc, hbit);


		ReleaseDC(hWnd, hdc);

		break;

		//ver exame
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case id_local:
			hWnd2 = hWnd;

			DialogBox(hInstance, (LPCWSTR)IDD_SinglePlayerInfo, hWnd, (DLGPROC)DialogPS);
			break;
		case id_host:
			hWnd2 = hWnd;

			DialogBox(hInstance, (LPCWSTR)IDD_MultiplayerInfo, hWnd, (DLGPROC)DialogPM);
			break;
		case id_Join:
			hWnd2 = hWnd;

			DialogBox(hInstance, (LPCWSTR)IDD_MultiplayerInfo, hWnd, (DLGPROC)DialogPM);
			break;
		case ID_EXIT:
			//ver exame
			resposta = MessageBox(hWnd, TEXT("Terminar o Programa?"), TEXT("Fim"), MB_YESNO | MB_ICONQUESTION);
			if (resposta == IDYES)
				PostQuitMessage(0);				// Se YES, terminar programa
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam) {


			
		case VK_LEFT:
			if (clientGame->players[0].isPlaying == FALSE) {


				if (clientGame->snakes[0].head.direction != kRIGHT) {

					clientGame->snakes[0].head.direction = kLEFT;


					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}
			}
			else {

				if (clientGame->snakes[0].head.direction != kLEFT) {

					clientGame->snakes[0].head.direction = kRIGHT;
					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}

			}
			break;

		case VK_RIGHT:

			if (clientGame->players[0].isPlaying == FALSE) {

				if (clientGame->snakes[0].head.direction != kLEFT) {

					clientGame->snakes[0].head.direction = kRIGHT;
					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}
			}
			else {
				if (clientGame->snakes[0].head.direction != kRIGHT) {

					clientGame->snakes[0].head.direction = kLEFT;
					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}

			}
			break;

		case VK_UP:


			if (clientGame->players[0].isPlaying == FALSE) {

				if (clientGame->snakes[0].head.direction != kDOWN) {

					clientGame->snakes[0].head.direction = kUP;
					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}

			}
			else {
				if (clientGame->snakes[0].head.direction != kUP) {

					clientGame->snakes[0].head.direction = kDOWN;
					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}


			}
			break;

		case VK_DOWN:

			if (clientGame->players[0].isPlaying == FALSE) {

				if (clientGame->snakes[0].head.direction != kUP) {
					clientGame->snakes[0].head.direction = kDOWN;
					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}
			}
			else {

				if (clientGame->snakes[0].head.direction != kDOWN) {
					clientGame->snakes[0].head.direction = kUP;
					escreveMemoria(serverGame, clientGame);
					SetEvent(eventoKeysFromServer);
					ResetEvent(eventoKeysFromServer);
				}
			}
			break;
		}
		break;
		//apanha o X da janela
	case WM_CLOSE:
		resposta = MessageBox(hWnd, TEXT("Terminar o Programa?"), TEXT("Fim"), MB_YESNO | MB_ICONQUESTION);
		if (resposta == IDYES)
			PostQuitMessage(0);
		break;

	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}
	return(0);
}

BOOL CALLBACK DialogPS(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	TCHAR str[TAM];		// Buffer para strings

	switch (messg)
	{

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//ir buscar o nome do jogar
		case id_nextSinglePlayer:
			GetDlgItemText(hWnd, id_nomePlayerSingle, str, 20);
			if (_tcslen(str) == 0) {
				MessageBox(hWnd, TEXT("Nao tem nada introduzido no Nome"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			//falta meter cases das teclas
			//METER A GUARDAR NOME
			EndDialog(hWnd, IDCANCEL);
			DialogBox(hInstance, (LPCWSTR)IDD_SINGLEPLAYER, hWnd, (DLGPROC)DialogSinglePlayer);



			return 1;
			// Clicou no Close 
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return 1;
		}
	}
	return 0;
}

BOOL CALLBACK DialogPM(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	TCHAR str[TAM];		// Buffer para strings

	switch (messg)
	{

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK_nextMulti:
			GetDlgItemText(hWnd, id_nomePlayerMulti, str, 20);
			if (_tcslen(str) == 0) {
				MessageBox(hWnd, TEXT("Nao tem nada introduzido no Nome"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			//falta meter cases das teclas
			//METER A GUARDAR NOME
			EndDialog(hWnd, IDCANCEL);
			DialogBox(hInstance, (LPCWSTR)IDD_MULTIPLAYER, hWnd, (DLGPROC)DialogMultiPlayer);
			return 1;
			// Clicou no Close 
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return 1;
		}
	}

	//Faltam teclas e guardar nome
	return 0;
}

BOOL CALLBACK DialogMultiPlayer(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	int i;				// Index da List Box
	TCHAR str[TAM];		// Buffer para strings
	GameDefinition setup;

	switch (messg)
	{

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK_StartMulti:
			GetDlgItemText(hWnd, id_MaxItems, str, 20);
			if (!verifySettingRequest(1, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o numero de items"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.nObjects = _wtoi(str);

			GetDlgItemText(hWnd, id_maxMultiplayerPlayers, str, 20);
			if (!verifySettingRequest(5, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o numero de Players"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.nMaxPlayers = _wtoi(str);

			GetDlgItemText(hWnd, id_TamCobra, str, 20);
			if (!verifySettingRequest(2, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o tamanho da snake"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.snakeSize = _wtoi(str);

			GetDlgItemText(hWnd, id_tempoJogo, str, 20);
			if (!verifySettingRequest(3, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal a duracao do jogo"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.duration = _wtoi(str);
			setup.duration = setup.duration * 60000;

			GetDlgItemText(hWnd, id_itemsRaros, str, 20);
			if (!verifySettingRequest(4, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal a percentagem de items raros"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.objectProbability = _wtoi(str);

			GetDlgItemText(hWnd, id_tamX, str, 20);
			if (!verifySettingRequest(6, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o tamanho horizontal do mapa"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.gameSizeX = _wtoi(str);

			GetDlgItemText(hWnd, id_tamY, str, 20);
			if (!verifySettingRequest(7, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o tamanho vertical do mapa"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.gameSizeY = _wtoi(str);

			GetDlgItemText(hWnd, id_numCobrasAuto2, str, 20);
			if (!verifySettingRequest(9, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o numero de cobras auto"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.nSnakes = _wtoi(str);

			MessageBox(hWnd, TEXT("Jogo bem configurado"), TEXT("Game Start"), MB_OK);
			setup.gameType = 1;
			setup.nLocalPlayers = 1;
			clientGame->gameDefinition = setup;
			escreveMemoria(serverGame, clientGame);

			SetEvent(eventoFromServer);
			ResetEvent(eventoFromServer);

			WaitForSingleObject(eventoCli, INFINITE);
			escreveMemoria(clientGame, serverGame);

			hThreadStartGame = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadStartGame, NULL, 0, &ThreadStartGameID);
			EndDialog(hWnd, IDCANCEL_MultiPlayer);
			return 1;
			// Clicou no Close 
		case IDCANCEL_MultiPlayer:
			EndDialog(hWnd, 0);
			return 1;
		}
	}
	return 0;
}

BOOL CALLBACK DialogSinglePlayer(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	int i;				// Index da List Box
	TCHAR str[TAM];		// Buffer para strings
	GameDefinition setup;

	switch (messg)
	{

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK_StartSingle:
			GetDlgItemText(hWnd, id_MaxItems, str, 20);
			if (!verifySettingRequest(1, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o numero de items"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.nObjects = _wtoi(str);

			GetDlgItemText(hWnd, id_localPlayersSingle, str, 20);
			if (!verifySettingRequest(5, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o numero de Players"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.nLocalPlayers = _wtoi(str);

			GetDlgItemText(hWnd, id_TamCobra, str, 20);
			if (!verifySettingRequest(2, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o tamanho da snake"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.snakeSize = _wtoi(str);

			GetDlgItemText(hWnd, id_tempoJogo, str, 20);
			if (!verifySettingRequest(3, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal a duracao do jogo"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.duration = _wtoi(str);
			setup.duration = setup.duration * 60000;

			GetDlgItemText(hWnd, id_itemsRaros, str, 20);
			if (!verifySettingRequest(4, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal a percentagem de items raros"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.objectProbability = _wtoi(str);

			GetDlgItemText(hWnd, id_tamX, str, 20);
			if (!verifySettingRequest(6, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o tamanho horizontal do mapa"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.gameSizeX = _wtoi(str);

			GetDlgItemText(hWnd, id_tamY, str, 20);
			if (!verifySettingRequest(7, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o tamanho vertical do mapa"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.gameSizeY = _wtoi(str);

			GetDlgItemText(hWnd, id_numCobrasAuto, str, 20);
			if (!verifySettingRequest(9, _wtoi(str))) {
				MessageBox(hWnd, TEXT("Introduziu mal o numero de cobras auto"), TEXT("Dados Incorretos"), MB_OK);
				break;
			}
			setup.nSnakes = _wtoi(str);

			MessageBox(hWnd, TEXT("Jogo bem configurado"), TEXT("Game Start"), MB_OK);

			//forçar o local
			setup.gameType = 0;
			setup.nLocalPlayers = 1;
			clientSettingsLocal();

			clientGame->gameDefinition = setup;
			escreveMemoria(serverGame, clientGame);

			SetEvent(eventoFromServer);
			ResetEvent(eventoFromServer);

			WaitForSingleObject(eventoCli, INFINITE);
			escreveMemoria(clientGame, serverGame);

			hThreadStartGame = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadStartGame, NULL, 0, &ThreadStartGameID);
			EndDialog(hWnd, IDCANCEL_SinglePlayer);
			return 1;
			// Clicou no Close 
		case IDCANCEL_SinglePlayer:
			EndDialog(hWnd, 0);
			return 1;
		}
	}
	return 0;
}

void iniciaBitMaps(HINSTANCE hInst) {


	//ref dos bitmaps
	walls = LoadBitmap(hInst, MAKEINTRESOURCE(id_wall));
	alimento = LoadBitmap(hInst, MAKEINTRESOURCE(id_alimento));
	gelo = LoadBitmap(hInst, MAKEINTRESOURCE(id_gelo));
	granada = LoadBitmap(hInst, MAKEINTRESOURCE(id_granada));
	vodka = LoadBitmap(hInst, MAKEINTRESOURCE(id_vodka));
	oleo = LoadBitmap(hInst, MAKEINTRESOURCE(id_oleo));
	cola = LoadBitmap(hInst, MAKEINTRESOURCE(id_cola));
	cafe = LoadBitmap(hInst, MAKEINTRESOURCE(id_coffe));
	special = LoadBitmap(hInst, MAKEINTRESOURCE(id_special));
	oVodka = LoadBitmap(hInst, MAKEINTRESOURCE(id_kVodka));
	oOleo = LoadBitmap(hInst, MAKEINTRESOURCE(id_kOleo));
	oCola = LoadBitmap(hInst, MAKEINTRESOURCE(id_oCola));
	head = LoadBitmap(hInst, MAKEINTRESOURCE(id_head));
	body = LoadBitmap(hInst, MAKEINTRESOURCE(id_body));
	bodyAuto = LoadBitmap(hInst, MAKEINTRESOURCE(id_BodyAuto));

	//ver 
	h_walls = CreateCompatibleDC(hdc);
	h_alimento = CreateCompatibleDC(hdc);
	h_gelo = CreateCompatibleDC(hdc);
	h_granada = CreateCompatibleDC(hdc);
	h_vodka = CreateCompatibleDC(hdc);
	h_oleo = CreateCompatibleDC(hdc);
	h_cola = CreateCompatibleDC(hdc);
	h_cafe = CreateCompatibleDC(hdc);
	h_special = CreateCompatibleDC(hdc);
	h_oVodka = CreateCompatibleDC(hdc);
	h_oOleo = CreateCompatibleDC(hdc);
	h_oCola = CreateCompatibleDC(hdc);
	h_head = CreateCompatibleDC(hdc);
	h_body = CreateCompatibleDC(hdc);
	h_bodyAuto = CreateCompatibleDC(hdc);

	//associa bit map ao hdc para criar
	SelectObject(h_walls, walls);
	SelectObject(h_alimento, alimento);
	SelectObject(h_gelo, gelo);
	SelectObject(h_granada, granada);
	SelectObject(h_vodka, vodka);
	SelectObject(h_oleo, oleo);
	SelectObject(h_cola, cola);
	SelectObject(h_cafe, cafe);
	SelectObject(h_special, special);
	SelectObject(h_oVodka, oVodka);
	SelectObject(h_oOleo, oOleo);
	SelectObject(h_oCola, oCola);
	SelectObject(h_head, head);
	SelectObject(h_body, body);
	SelectObject(h_bodyAuto, bodyAuto);

}

void pintaMapaGraphic() {

	PAINTSTRUCT Ps;
	HDC hdc;
	int ROW = clientGame->gameDefinition.gameSizeY;
	int COLUMNS = clientGame->gameDefinition.gameSizeX;
	int x, y;
	int size = (COLUMNS + 1) *(ROW + 1);
	InvalidateRect(hWnd2, NULL, TRUE);
	hdc = BeginPaint(hWnd2, &Ps);

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			if (clientGame->map[i][j].idTile == kWalls) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_walls, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kAlimento) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_alimento, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kGelo) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_gelo, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kGranada) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_granada, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kVodka) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_vodka, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kOleo) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_oleo, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kCola) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_cola, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kCafe) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_cafe, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kItemEspecial) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_special, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kOOleo) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_oOleo, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kOCola) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_oCola, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kOVodka) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_oVodka, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kSnakeHeadAuto) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_head, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kSnakeAuto) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_bodyAuto, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kSnakeHeadTile) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_head, 0, 0, SRCCOPY);
			}
			else if (clientGame->map[i][j].idTile == kSnakeTile) {
				BitBlt(hdc, clientGame->map[i][j].y * 32, clientGame->map[i][j].x * 32, 32, 32, h_body, 0, 0, SRCCOPY);
			}
			DeleteDC(auxdc);
	}

		
	}


}

BOOLEAN	verifySettingRequest(int request, int valueToVerify) {
	int v = 0;
	switch (request) {
	case 1:
		if (valueToVerify >= MIN_NUMBER_ITEM && valueToVerify <= MAX_NUMBER_ITEM) {
			v = 1;
		}

		break;
	case 2:
		if (valueToVerify >= MIN_SIZE_SNAKE && valueToVerify <= MAX_SIZE_SNAKE) {
			v = 1;
		}
		break;
	case 3:
		valueToVerify * 1000;
		if (valueToVerify >= MIN_DURATION_GAME && valueToVerify <= MAX_DURATION_GAME) {
			v = 1;
		}
		break;
	case 4:
		if (valueToVerify >= MIN_PROP_RARE_ITEMS && valueToVerify <= MAX_PROP_RARE_ITEMS) {
			v = 1;
		}
		break;
	case 5:
		if (valueToVerify >= MIN_LOCAL_PLAYERS && valueToVerify <= MAX_LOCAL_PLAYERS) {
			v = 1;
		}
		break;
	case 6:
		if (valueToVerify >= MIN_HORIZONTAL_SIZE && valueToVerify <= MAX_HORIZONTAL_SIZE) {
			v = 1;
		}
		break;
	case 7:
		if (valueToVerify >= MIN_VERTICAL_SIZE && valueToVerify <= MAX_VERTICAL_SIZE) {
			v = 1;
		}
		break;
	case 8:
		if (valueToVerify >= 2 && valueToVerify <= MAX_NUMBER_PLAYER) {
			v = 1;
		}
		break;
	case 9:
		if (valueToVerify >= MIN_COBRAS_AUTO && valueToVerify <= MAX_COBRAS_AUTO) {
			v = 1;
		}
		break;

	default:
		break;
	}

	if (v == 1) {
		return TRUE;
	}
	_tprintf(TEXT("Error: value out of bound, try again.\n"));
	return FALSE;

}

void destroiBitMaps() {
	DeleteDC(h_walls);
	DeleteDC(h_alimento);
	DeleteDC(h_head);
	DeleteDC(h_cola);
	DeleteDC(h_body);
	DeleteDC(h_bodyAuto);
	DeleteDC(h_gelo);
	DeleteDC(h_granada);
	DeleteDC(h_oleo);
	DeleteDC(h_oCola);
	DeleteDC(h_oOleo);
	DeleteDC(h_oVodka);
	DeleteDC(h_vodka);
	DeleteDC(h_cafe);
	DeleteDC(h_special);;

	DeleteObject(walls);
	DeleteObject(cafe);
	DeleteObject(special);
	DeleteObject(bodyAuto);
	DeleteObject(alimento);
	DeleteObject(head);
	DeleteObject(cola);
	DeleteObject(body);
	DeleteObject(gelo);
	DeleteObject(granada);
	DeleteObject(oleo);
	DeleteObject(oCola);
	DeleteObject(oOleo);
	DeleteObject(oVodka);
	DeleteObject(vodka);
}

void clientSettingsLocal()
{

	
		Player *jogador = (Player*)malloc(sizeof(Player));

		jogador->player_id = 1;
		jogador->idSnake = 1;

		jogador->isPlaying = FALSE;
		jogador->score = 0;
		clientGame->currentPlayers = 1;
		clientGame->players[0] = *jogador;


}