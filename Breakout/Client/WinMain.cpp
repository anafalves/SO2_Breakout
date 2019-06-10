//Base.c
#include <windows.h>
#include <tchar.h>
#include <string>
#include "../ClientDLL/Communication.h"
#include "resource.h"
#include "../Server/GameData.h"
#include "Mmsystem.h"


#define WIND_WIDTH 1500
#define WIND_HEIGHT 800
#define Game_WIDTH 1200
#define Info_WIDTH 300

LRESULT CALLBACK MainProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK LoginProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ReceiveGameThread(LPVOID * args);
void deleteResources();

GameData game;

TCHAR szProgName[] = TEXT("Breakout");
tstring staticTxtForMainWindow[2] = {
			TEXT("Vida: "),
			TEXT("Pontos: ")
};
tstring txtLife = TEXT("Life: ");
tstring txtPoints = TEXT("Points: ");
tstring txtAux;
int staticTxtMainWindPos[2][2] = { {80,500},{80,550} };
int maxX = GetSystemMetrics(SM_CXSCREEN);
int maxY = GetSystemMetrics(SM_CYSCREEN);

Client * client;
TCHAR username[100];
COLORREF transparent_color = RGB(1, 1, 1); //TODO:

///////// Bitmaps' Handles ////////
HBITMAP hBackground;
HBITMAP hBall;
HBITMAP hNormalGoodTile;
HBITMAP hNormalDamagedTile;
HBITMAP hBonusTile;
HBITMAP hPlatformPlayer;
HBITMAP hPlatformOthers;
HBITMAP hSpeedUpBonus;
HBITMAP hSpeedDownBonus;
HBITMAP hLifeBonus;
HBITMAP hTripleBonus;
HBITMAP hUnbrokenTile;

void LoadResources();
void printResources();
//////////////////////////////////

HDC memdcAux;
HDC paint_hdc;
HDC memdcGame;
HDC memdcData;
UINT leftKey = TEXT('A');
UINT rightKey = TEXT('D');

bool CONTINUE = false;
bool CONTINUE2 = true;
bool registerWindow(HINSTANCE hInst) {
	WNDCLASSEX wcApp;

	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInst;

	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = MainProc;
	wcApp.style = CS_HREDRAW | CS_VREDRAW;
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON4));
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON3));
	wcApp.hCursor = LoadCursor(NULL, IDC_CROSS);
	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU2);
	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if (!RegisterClassEx(&wcApp))
		return false;
	return true;
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;
	MSG lpMsg;

	if (!registerWindow(hInst))
		return(0);

	hWnd = CreateWindow(
		szProgName,
		TEXT("Breakout - SO2 2018-2019"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WIND_WIDTH,
		WIND_HEIGHT,
		(HWND)HWND_DESKTOP,
		(HMENU)NULL,
		(HINSTANCE)hInst,
		0);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}

	return((int)lpMsg.wParam);
}

//thread
DWORD WINAPI GetUpdates(LPVOID args) {
	HWND hWnd = (HWND) args;

	do {
		continue;
	} while (!CONTINUE && CONTINUE2);

	while (CONTINUE) {
		game = client->receiveBroadcast();
	
		InvalidateRect(hWnd, NULL, TRUE);	
	}
	return 0;
}

void printGameDataOnScreen(HWND hWnd) {
	HANDLE hUpdate;
	
	hUpdate = CreateThread(NULL, 0, GetUpdates, (LPVOID) hWnd,0, NULL);
}

BOOL CALLBACK LoginProc(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) {

	TCHAR key[10];
	TCHAR IP[15];
	TCHAR phrase[120];
	ClientMsg message;

	int answer;
	RemoteClient * remote = NULL;
	LocalCLient * local = NULL;

	switch (messg) {
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_EDIT1, TEXT("Preencha username"));
		SetDlgItemText(hWnd, IDC_EDIT_LEFT, TEXT("A"));
		SetDlgItemText(hWnd, IDC_EDIT_RIGHT, TEXT("D"));
		SendDlgItemMessage(hWnd, IDC_OPTION_LOCAL, BM_SETCHECK, 1, 0);
		remote = NULL;
		local = NULL;
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			//Buscar texto das caixas de texto
			GetDlgItemText(hWnd, IDC_EDIT1, username, 50);
			GetDlgItemText(hWnd, IDC_IPADDRESS1, IP, 15);

			if (IsDlgButtonChecked(hWnd, IDC_OPTION_REMOTE) == BST_CHECKED) {
				if (local != NULL) {
					delete local;
					local = NULL;
				}

				if (_tcslen(username) < 3 || IP[0] == TEXT('0')) {
					MessageBox(hWnd, TEXT("Data invalid, check Username and IP address!"), TEXT("Invalid Input"), MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if (remote == NULL) {
					remote = getRemoteClientInstance();
					if (!remote->connect(IP))
					{
						_stprintf_s(phrase, 120, TEXT("Error, Server is not connected to IP: %s!"), IP);
						MessageBox(hWnd, phrase, TEXT("Connection Error"), MB_OK | MB_ICONEXCLAMATION);
						delete remote;
						remote = NULL;
						break;
					}

					if (!remote->isConnected()) {
						_stprintf_s(phrase, 120, TEXT("Error, Server is not connected to IP: %s!"), IP);
						MessageBox(hWnd, phrase, TEXT("Connection Error"), MB_OK | MB_ICONEXCLAMATION);
						delete remote;
						remote = NULL;
						break;
					}
				}
				
				answer = remote->login(username);
				if (answer == CONNECTION_ERROR) {
					MessageBox(hWnd, TEXT("Connection Error while trying to log-in!"), TEXT("Connection Error"), MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				else if (answer == SERVER_FULL) {
					MessageBox(hWnd, TEXT("Server Full, Please try again later!"), TEXT("Server Full"), MB_OK | MB_ICONINFORMATION);
					break;
				}
				else if (answer == INVALID_USERNAME) {
					MessageBox(hWnd, TEXT("Please change your name!"), TEXT("Existing name"), MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				else {
					client = remote;
					CONTINUE = true;
					CONTINUE2 = false;
					client->CONTINUE = true;
				}
			}
			else
			{
				if (remote != NULL) {
					delete remote;
					remote = NULL;
				}

				if (local == NULL) {
					static LocalCLient * local = getLocalClientInstance();
					if (!local->isReady()) {
						MessageBox(hWnd, TEXT("Error while trying to connect to local Server!"), TEXT("Connection Error"), MB_OK | MB_ICONEXCLAMATION);
						break;
					}
					
					answer = local->login(username);
					if (answer == CONNECTION_ERROR) {
						MessageBox(hWnd, TEXT("Connection Error while trying to log-in!"), TEXT("Connection Error"), MB_OK | MB_ICONEXCLAMATION);
						break;
					}
					else if (answer == SERVER_FULL) {
						MessageBox(hWnd, TEXT("Server Full, Please try again later!"), TEXT("Server Full"), MB_OK | MB_ICONINFORMATION);
						break;
					}
					else if (answer == INVALID_USERNAME) {
						MessageBox(hWnd, TEXT("Please change your name!"), TEXT("Existing name"), MB_OK | MB_ICONEXCLAMATION);
						break;
					}
					else if (answer == CONNECTION_TIMED_OUT) {
						MessageBox(hWnd, TEXT("Server did not responde in time, Connection Timed Out"), TEXT("Timed Out"), MB_OK | MB_ICONEXCLAMATION);
						break;
					}
					else {
						client = local;
						CONTINUE = true;
						CONTINUE2 = false;
					}
				}
			}

			//SelectObject(memdcGame, GetStockObject(DEFAULT_GUI_FONT));
			//SetBkMode(memdcGame, TRANSPARENT);
			//SetTextColor(memdcGame, RGB(0, 0, 0));

			//TextOut(memdcGame, Game_WIDTH / 2, WIND_HEIGHT / 3, TEXT("Waiting for Server to start the game!"), _tcslen(TEXT("Waiting for Server to start the game!")));

			GetDlgItemText(hWnd, IDC_EDIT_LEFT, key, 10);
			leftKey = LOBYTE(key[0]);

			GetDlgItemText(hWnd, IDC_EDIT_RIGHT, key, 10);
			rightKey = LOBYTE(key[0]);

			EndDialog(hWnd, 0);
			return TRUE;

				case IDC_EDIT_LEFT:
					if (HIWORD(wParam) == EN_CHANGE)
					{
						GetDlgItemText(hWnd, IDC_EDIT_LEFT, key, 10);
						if (_tcslen(key) > 1) {
							SetDlgItemText(hWnd, IDC_EDIT_LEFT, &key[1]);
						}
					}

				case IDC_EDIT_RIGHT:
					if (HIWORD(wParam) == EN_CHANGE)
					{
						GetDlgItemText(hWnd, IDC_EDIT_RIGHT, key, 10);
						if (_tcslen(key) > 1) {
							SetDlgItemText(hWnd, IDC_EDIT_RIGHT, &key[1]);
						}
					}

					return TRUE;

				case IDCANCEL:
					if (local != NULL &&!(local->isReady())) {
						message.id = local->getClientID();
						message.type = LEAVE;
						local->sendMessage(message);
						delete local;
					}
					if (remote != NULL && !remote->isConnected()) {
						message.id = local->getClientID();
						message.type = LEAVE;
						remote->sendMessage(message);
						delete remote;
					}
					PostQuitMessage(0);
					return TRUE;
			}
			break;

		case WM_CLOSE:

			if (local != NULL && !(local->isReady())) {
				message.id = local->getClientID();
				message.type = LEAVE;
				local->sendMessage(message);
				delete local;
			}
			if (remote != NULL && !remote->isConnected()) {
				message.id = local->getClientID();
				message.type = LEAVE;
				remote->sendMessage(message);
				delete remote;
			}
			PostQuitMessage(0);
			break;
	}
	return FALSE;
}

BOOL CALLBACK Top10Proc(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) 
{
	Top10 top10;
	ClientMsg msg;
	ServerMsg resp;
	tstring place;

	switch (messg) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_ABOUT:
					EndDialog(hWnd, 0);
					return TRUE;
			}

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
		
		case WM_INITDIALOG:
			msg.id = client->getClientID();
			msg.type = TOP10;
			
			client->sendMessage(msg);
			resp = client->receiveMessage();

			top10 = resp.message.top10;

			for (int i = 0; i < 10; i++){
				place = tto_string(i + 1);
				place += (TEXT(": "));
				place += top10.position[i].username;
				place += (TEXT(" - "));
				place += tto_string(top10.position[i].points);

				SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0,
					(LPARAM)place.c_str());

				place.clear();
			}

			return TRUE;
	}

	return FALSE;
}


LRESULT CALLBACK MainProc(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) 
{
	HDC hdc;
	PAINTSTRUCT ps;
	LPMINMAXINFO lp;

	HBITMAP hbitmapData;

	ClientMsg message = { 0 };

	switch (messg) {
		case WM_CREATE:
			LoadResources();

			//Create double buffer
			hdc = GetDC(hWnd);

			memdcGame = CreateCompatibleDC(hdc);
			memdcData = CreateCompatibleDC(hdc);
			memdcAux = CreateCompatibleDC(hdc);

			hbitmapData = CreateCompatibleBitmap(hdc, Info_WIDTH, WIND_HEIGHT);

			SelectObject(memdcData, hbitmapData);
			SelectObject(memdcData, GetStockObject(LTGRAY_BRUSH));

			PatBlt(memdcData, 0, 0, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT, PATCOPY);

			SelectObject(memdcGame, hBackground);

			InvalidateRect(hWnd, NULL, FALSE);
			ReleaseDC(hWnd, hdc);

			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOGIN), NULL, (DLGPROC)LoginProc);
			printGameDataOnScreen(hWnd);

			DeleteObject(hbitmapData);
			
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_ABOUT:
					MessageBox(hWnd, TEXT("Joana - 21260792\nWallace - 21230618"), TEXT("Done by:"), MB_OK);
					break;

				case ID_TOP10:
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TOP10), NULL,(DLGPROC)Top10Proc);
					break;

				case ID_LEAVE:
					DestroyWindow(hWnd);
					break;
			}
			break;

		case WM_PAINT:
			//InvalidateRect(hWnd, NULL, TRUE);
			
			paint_hdc = BeginPaint(hWnd, &ps);

			SelectObject(memdcGame, hBackground);
			BitBlt(paint_hdc, 0, 0, Game_WIDTH, WIND_HEIGHT, memdcGame, 0, 0, SRCCOPY);
			
			//Paint Game
			printResources();

			//Paint side info
			BitBlt(paint_hdc, Game_WIDTH, 0, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT, memdcData, 0, 0, SRCCOPY);

			EndPaint(hWnd, &ps);
			break;

		case WM_LBUTTONDOWN:
			if (LOWORD(lParam) > Game_WIDTH)
				break;

			message.id = client->getClientID();
			message.type = PRECISE_MOVE;
			message.message.preciseMove = LOWORD(lParam);
			client->sendMessage(message);
			PlaySoundA((LPCSTR)"van-sliding-door.wav", NULL, SND_FILENAME | SND_ASYNC | SND_MEMORY);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_LEFT || wParam == leftKey) {
				message.id = client->getClientID();
				message.type = MOVE;
				message.message.basicMove = LEFT;
				client->sendMessage(message);
			}
			else if (wParam == VK_RIGHT || wParam == rightKey) {
				message.id = client->getClientID();
				message.type = MOVE;
				message.message.basicMove = RIGHT;
				client->sendMessage(message);
			}
			PlaySoundA((LPCSTR)"van-sliding-door.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;

		case WM_ERASEBKGND:
			return(1); // Prevent erasing the background to reduce flickering
			break;
		
		case WM_GETMINMAXINFO:
			lp = (LPMINMAXINFO)lParam;
			lp->ptMinTrackSize.x = WIND_WIDTH;
			lp->ptMinTrackSize.y = WIND_HEIGHT + 50;
			lp->ptMaxTrackSize.x = WIND_WIDTH;
			lp->ptMaxTrackSize.y = WIND_HEIGHT + 50;
			break;

		case WM_DESTROY:
			message.id = client->getClientID();
			message.type = LEAVE;
			client->sendMessage(message);

			free(client);
			DeleteDC(memdcData);
			DeleteDC(memdcGame);
			deleteResources();

			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, messg, wParam, lParam);
			break;
	}
	return(0);
}

DWORD WINAPI ReceiveGameThread(LPVOID * args) {
	return 0;
}

void LoadResources() {
	hBackground = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBall = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 35, 35, LR_DEFAULTSIZE);
	hNormalGoodTile = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP10), IMAGE_BITMAP, 50, 20, LR_DEFAULTSIZE);
	hNormalDamagedTile = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 50, 20, LR_DEFAULTSIZE);
	hBonusTile = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP12), IMAGE_BITMAP, 50, 20, LR_DEFAULTSIZE);
	hUnbrokenTile = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP9), IMAGE_BITMAP, 50, 20, LR_DEFAULTSIZE);
	hPlatformPlayer = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP13), IMAGE_BITMAP, 120, 30, LR_DEFAULTSIZE);
	hPlatformOthers = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP14), IMAGE_BITMAP, 120, 30, LR_DEFAULTSIZE);
	hSpeedUpBonus = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP6), IMAGE_BITMAP, 50, 20, LR_DEFAULTSIZE);
	hSpeedDownBonus = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 50, 20, LR_DEFAULTSIZE);
	hLifeBonus = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP7), IMAGE_BITMAP, 40, 40, LR_DEFAULTSIZE);
	hTripleBonus = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP11), IMAGE_BITMAP, 50, 20, LR_DEFAULTSIZE);
}

void deleteResources() {
	DeleteObject(hBackground);
	DeleteObject(hBall);
	DeleteObject(hNormalGoodTile);
	DeleteObject(hNormalDamagedTile);
	DeleteObject(hBonusTile);
	DeleteObject(hUnbrokenTile);
	DeleteObject(hPlatformOthers);
	DeleteObject(hPlatformPlayer);
	DeleteObject(hSpeedUpBonus);
	DeleteObject(hSpeedDownBonus);
	DeleteObject(hLifeBonus);
	DeleteObject(hTripleBonus);
}

void printResources() {
	SelectObject(memdcAux, hBall);
	for (auto & ball:game.balls) {
		if(ball.active)
			TransparentBlt(paint_hdc, ball.posX, ball.posY, 35, 35, memdcAux, 0, 0, 35, 35, RGB(255,255,255));
	}

	for (auto &tile:game.tiles) {
		if (tile.active) {
			if (tile.resistance == -1) {
				SelectObject(memdcAux, hUnbrokenTile);
			}
			else if (tile.bonus != NORMAL)
				SelectObject(memdcAux, hBonusTile);
			else {
				if(tile.resistance > 2)
					SelectObject(memdcAux, hNormalGoodTile);
				else
					SelectObject(memdcAux, hNormalDamagedTile);	
			}
		
			TransparentBlt(paint_hdc, tile.posX, tile.posY, 50, 20, memdcAux, 0, 0, 50, 20, RGB(255, 255, 255));
		}
	}
		
	for (auto & player : game.players) {
		if (!player.active)
			continue;
		if (_tcscmp(player.name,username) != 0)
			SelectObject(memdcAux, hPlatformOthers);
		else {
			SelectObject(memdcAux, hPlatformPlayer);
			txtAux = txtLife + tto_string(player.lives);
			TextOut(memdcData, staticTxtMainWindPos[0][0], staticTxtMainWindPos[0][1], txtAux.c_str(),txtAux.size());
			txtAux = txtPoints + tto_string(player.points);
			TextOut(memdcData, staticTxtMainWindPos[1][0], staticTxtMainWindPos[1][1], txtAux.c_str(), txtAux.size());
		}
			
		BitBlt(paint_hdc, player.posX, player.posY, 100, 40, memdcAux, 0, 0, SRCCOPY);
	}
	
}