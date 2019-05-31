//Base.c
#include <windows.h>
#include <tchar.h>
#include "resource.h"

#define WIND_WIDTH 1200
#define WIND_HEIGHT 1200
#define Game_WIDTH 800

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

TCHAR szProgName[] = TEXT("Base");

TCHAR staticTxtForMainWindow[4][50] = {
			TEXT("Jogadores ativos: "),
			TEXT("Vida: "),
			TEXT("Pontos: ")
};

int staticTxtMainWindPos[4][2] = {{100,100},{100,700},{100,750}};


int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nCmdShow) {
	HWND hWnd; 
	MSG lpMsg; 
	WNDCLASSEX wcApp; 

	wcApp.cbSize = sizeof(WNDCLASSEX); 
	wcApp.hInstance = hInst; 

	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = TrataEventos; 
	wcApp.style = CS_HREDRAW | CS_VREDRAW;
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON4));
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON3));
	wcApp.hCursor = LoadCursor(NULL, IDC_CROSS);
	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU2); 
	wcApp.cbClsExtra = 0; 
	wcApp.cbWndExtra = 0; 
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	
	if (!RegisterClassEx(&wcApp))
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

BOOL CALLBACK FuncaoCaixa1(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) {
	TCHAR username[100];
	TCHAR IP[12];
	TCHAR frase[120];
	//Trata a caixa do login
	switch (messg) {
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_EDIT1,
			TEXT("Preencha username"));
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			//Buscar texto das caixas de texto
			GetDlgItemText(hWnd, IDC_EDIT1, username, 100);
			GetDlgItemText(hWnd, IDC_IPADDRESS1, IP, 12);
			//Concatenar e mostrar
			_stprintf_s(frase, 120, TEXT("Username:%s \t IP: %s"),
				username, IP);
			MessageBox(hWnd, frase, TEXT("Texto lido"), MB_OK);
			EndDialog(hWnd, 0);
			return TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL) {
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

BOOL CALLBACK FuncaoCaixa2(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) {
	//Trata a caixa do top 10
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

void AddWindowInfo(HWND hWnd, HDC memdcData) {
	SetBkMode(memdcData, TRANSPARENT);
	for (int i = 0; i < 4; i++) {
		TextOut(memdcData, staticTxtMainWindPos[i][0], staticTxtMainWindPos[i][1], staticTxtForMainWindow[i], _tcslen(staticTxtForMainWindow[i]));
	}

	InvalidateRect(hWnd, NULL, true);
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) {
	HDC hdc; static TCHAR c;
	static HDC memdcGame;
	static HDC memdcData;
	HBITMAP hbitmapGame;
	HBITMAP hbitmapData;
	static int x = 10, y = 10, xi = 0, yi = 0, xf = 0, yf = 0;
	PAINTSTRUCT ps;
	switch (messg) {
		
	case WM_DESTROY: 
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_ABOUT:
			MessageBox(hWnd, TEXT("Joana - 21260792\nWallace - 21230618"), TEXT("Done by:"), MB_OK);
			break;
		case ID_TOP10:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TOP10), NULL,(DLGPROC)FuncaoCaixa2);
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//BitBlt(hdc, 0, 0, Game_WIDTH,WIND_HEIGHT, memdcGame, 0, 0, SRCCOPY);
		BitBlt(hdc, Game_WIDTH, 0, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT, memdcData, 0,0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		TextOut(memdcGame, x, y, &c, 1);
		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_MBUTTONDOWN:
		xi = LOWORD(lParam);
		yi = HIWORD(lParam);
		break;
	case  WM_MBUTTONUP:
		xf = LOWORD(lParam);
		yf = HIWORD(lParam);
		hdc = GetDC(hWnd);
		Rectangle(hdc, xi, yi, xf, yf);
		Rectangle(memdcGame, xi, yi, xf, yf);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_RBUTTONDOWN:
		AddWindowInfo(hWnd, memdcData);
		InvalidateRect(hWnd, NULL, true);
		/*hdc = GetDC(hWnd);
		Ellipse(hdc, LOWORD(lParam), HIWORD(lParam),
			LOWORD(lParam) + 100, HIWORD(lParam) + 100);
		Ellipse(memdcGame, LOWORD(lParam), HIWORD(lParam),
			LOWORD(lParam) + 100, HIWORD(lParam) + 100);
		ReleaseDC(hWnd, hdc);*/
		break;
	case WM_CHAR:
		c = wParam;
		hdc = GetDC(hWnd);
		TextOut(hdc, x, y, &c, 1);
		TextOut(memdcGame, x, y, &c, 1);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_RIGHT) {
			x += 10;
			hdc = GetDC(hWnd);
			TextOut(hdc, x, y, &c, 1);
			TextOut(memdcGame, x, y, &c, 1);
			ReleaseDC(hWnd, hdc);
		}
		break;
	case WM_CREATE:
		hdc = GetDC(hWnd);
		memdcGame = CreateCompatibleDC(hdc);
		memdcData = CreateCompatibleDC(hdc);
		hbitmapGame = CreateCompatibleBitmap(hdc, Game_WIDTH, WIND_HEIGHT);
		hbitmapData = CreateCompatibleBitmap(hdc, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT);
		SelectObject(memdcGame, hbitmapGame);
		SelectObject(memdcData, hbitmapData);
		SelectObject(memdcGame, GetStockObject(WHITE_BRUSH));
		SelectObject(memdcData, GetStockObject(GRAY_BRUSH));
		PatBlt(memdcGame, 0, 0, Game_WIDTH, WIND_HEIGHT, PATCOPY);
		PatBlt(memdcData, 0, 0, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT, PATCOPY);
		

		ReleaseDC(hWnd, hdc);
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOGIN), hWnd, (DLGPROC)FuncaoCaixa1);
		
	default:
		return DefWindowProc(hWnd, messg, wParam, lParam);
		break;
	}
	return(0);
}

