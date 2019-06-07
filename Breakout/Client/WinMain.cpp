//Base.c
#include <windows.h>
#include <tchar.h>
#include "../ClientDLL/Communication.h"
#include "resource.h"

#define WIND_WIDTH 1500
#define WIND_HEIGHT 850
#define Game_WIDTH 1200
#define Info_WIDTH 300

LRESULT CALLBACK MainProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK LoginProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szProgName[] = TEXT("Breakout");
TCHAR staticTxtForMainWindow[4][50] = {
			TEXT("Jogadores ativos: "),
			TEXT("Vida: "),
			TEXT("Pontos: ")
};
int staticTxtMainWindPos[4][2] = { {80,100},{80,500},{80,550} };
int maxX = GetSystemMetrics(SM_CXSCREEN);
int maxY = GetSystemMetrics(SM_CYSCREEN);

Client * client;

HBITMAP hBackground;
BITMAP background;
HDC memdcGame;
HDC memdcData;
UINT leftKey = TEXT('A');
UINT rightKey = TEXT('D');

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

BOOL CALLBACK LoginProc(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) {
	TCHAR username[100];
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
		SendDlgItemMessage(hWnd, IDC_OPTION_REMOTE, BM_SETCHECK, 1, 0);
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
					}
				}
			}

			SelectObject(memdcGame, GetStockObject(DEFAULT_GUI_FONT));
			SetBkMode(memdcGame, TRANSPARENT);
			SetTextColor(memdcGame, RGB(0, 0, 0));

			TextOut(memdcGame, Game_WIDTH / 2, WIND_HEIGHT / 3, TEXT("Waiting for Server to start the game!"), _tcslen(TEXT("Waiting for Server to start the game!")));

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
					message.id = local->getClientID();
					message.type = LEAVE;

					if (local != NULL) {
						local->sendMessage(message);
						delete local;
					}
					if (remote != NULL) {
						remote->sendMessage(message);
						delete remote;
					}
					PostQuitMessage(0);
					return TRUE;
			}
			break;

		case WM_CLOSE:
			message.id = local->getClientID();
			message.type = LEAVE;

			if (local != NULL) {
				local->sendMessage(message);
				delete local;
			}
			if (remote != NULL) {
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
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING,
				0, (LPARAM)TEXT("João 100pts"));
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING,
				0, (LPARAM)TEXT("Ana 50pts"));

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

	static HBITMAP hbitmapGame;
	static HBITMAP hbitmapData;

	ClientMsg message = { 0 };

	switch (messg) {
		case WM_CREATE:
			hBackground = (HBITMAP)LoadImage(NULL, TEXT("Images/background.bmp"), IMAGE_BITMAP, Game_WIDTH, WIND_HEIGHT, LR_LOADFROMFILE);
			GetObject(hBackground, sizeof(background), &background);

			//Create double buffer
			hdc = GetDC(hWnd);

			memdcGame = CreateCompatibleDC(hdc);
			memdcData = CreateCompatibleDC(hdc);

			hbitmapGame = CreateCompatibleBitmap(hdc, Game_WIDTH, WIND_HEIGHT);
			hbitmapData = CreateCompatibleBitmap(hdc, Info_WIDTH, WIND_HEIGHT);

			SelectObject(memdcGame, hbitmapGame);
			SelectObject(memdcData, hbitmapData);
			SelectObject(memdcGame, GetStockObject(WHITE_BRUSH));
			SelectObject(memdcData, GetStockObject(LTGRAY_BRUSH));

			PatBlt(memdcGame, 0, 0, Game_WIDTH, WIND_HEIGHT, PATCOPY);
			PatBlt(memdcData, 0, 0, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT, PATCOPY);

			SelectObject(memdcGame, hBackground);

			//clear window background 
			//SelectObject(hdc, GetStockObject(WHITE_BRUSH));
			//PatBlt(hdc, 0, 0, maxX, maxY, PATCOPY);

			InvalidateRect(hWnd, NULL, FALSE);

			ReleaseDC(hWnd, hdc);

			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOGIN), NULL, (DLGPROC)LoginProc);

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
			
			hdc = BeginPaint(hWnd, &ps);

			//Paint Game
			BitBlt(hdc, 0, 0, Game_WIDTH, WIND_HEIGHT, memdcGame, 0, 0, SRCCOPY);
			
			//Paint side info
			BitBlt(hdc, Game_WIDTH + 15, 0, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT -50, memdcData, 0, 0, SRCCOPY);

			EndPaint(hWnd, &ps);
			break;

		case WM_LBUTTONDOWN:
			if (LOWORD(lParam) > Game_WIDTH)
				break;

			message.id = client->getClientID();
			message.type = PRECISE_MOVE;
			message.message.preciseMove = LOWORD(lParam);
			client->sendMessage(message);
			break;

		case WM_KEYDOWN:
			if (wParam == rightKey) {
				message.id = client->getClientID();
				message.type = MOVE;
				message.message.basicMove = LEFT;
				client->sendMessage(message);
			}
			else if (wParam == leftKey) {
				message.id = client->getClientID();
				message.type = MOVE;
				message.message.basicMove = RIGHT;
				client->sendMessage(message);
			}
			break;

		case WM_ERASEBKGND:
			return(1); // Prevent erasing the background to reduce flickering
			break;
		
		case WM_GETMINMAXINFO:
			lp = (LPMINMAXINFO)lParam;
			lp->ptMinTrackSize.x = WIND_WIDTH;
			lp->ptMinTrackSize.y = WIND_HEIGHT;
			lp->ptMaxTrackSize.x = WIND_WIDTH;
			lp->ptMaxTrackSize.y = WIND_HEIGHT;
			break;

		case WM_DESTROY:
			//TODO: Free bitmap memory 

			message.id = client->getClientID();
			message.type = LEAVE;
			
			client->sendMessage(message);
			free(client);
			DeleteDC(memdcData);
			DeleteDC(memdcGame);

			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, messg, wParam, lParam);
			break;
	}
	return(0);
}