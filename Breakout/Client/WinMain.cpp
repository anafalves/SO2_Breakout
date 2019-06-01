//Base.c
#include <windows.h>
#include <tchar.h>
#include "resource.h"

#define WIND_WIDTH 1300
#define WIND_HEIGHT 850
#define Game_WIDTH 1000
#define Info_WIDTH 300

LRESULT CALLBACK EventsHandler(HWND, UINT, WPARAM, LPARAM);
TCHAR szProgName[] = TEXT("Base");
HWND global_hWnd = NULL;
TCHAR staticTxtForMainWindow[4][50] = {
			TEXT("Jogadores ativos: "),
			TEXT("Vida: "),
			TEXT("Pontos: ")
};

int staticTxtMainWindPos[4][2] = {{80,100},{80,500},{80,550}};

bool registerWIndow(HINSTANCE hInst) {
	WNDCLASSEX wcApp;

	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInst;

	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = EventsHandler;
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
	
	if (!registerWIndow(hInst))
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
HBITMAP hBitmap1  = NULL;
BITMAP bmp1;
HDC memdcGame;
HDC memdcData;
HGDIOBJ oldBitmap01;
int maxX, maxY;

LRESULT CALLBACK EventsHandler(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) {
	HDC hdc = NULL; static TCHAR c;

	HBITMAP hbitmapGame;
	HBITMAP hbitmapData;
	static int x = 10, y = 10, xi = 0, yi = 0, xf = 0, yf = 0;
	PAINTSTRUCT ps;
	HDC tempDC;

	switch (messg) {
	/*case WM_SIZE:
		break;*/
	case WM_DESTROY:
		DeleteDC(memdcData);
		DeleteDC(memdcGame);
		global_hWnd = NULL;
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
		case ID_LEAVE:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		oldBitmap01 = SelectObject(memdcGame, hBitmap1);
		//to clean deslocations of window' imgs//
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		PatBlt(hdc, 0, 0, maxX, maxY, PATCOPY);
		///////////////////////////////////////////
		BitBlt(hdc, 0, 0, Game_WIDTH, WIND_HEIGHT, memdcGame, 0, 0, SRCCOPY);
		BitBlt(hdc, Game_WIDTH + 15, 0, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT -50, memdcData, 0, 0, SRCCOPY);
		SelectObject(memdcGame, oldBitmap01);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		TextOut(memdcGame, x, y, &c, 1);

		MessageBox(hWnd, TEXT("WM_LBUTTONDOWN"), TEXT("gg"), MB_OK);
		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_RBUTTONDOWN:
		AddWindowInfo(hWnd, memdcData);
		InvalidateRect(hWnd, NULL, true);
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
	case WM_ERASEBKGND:
		return(1); // Prevent erasing the background to reduce flickering
		break;
	case WM_CREATE:
		maxX = GetSystemMetrics(SM_CXSCREEN);
		maxY = GetSystemMetrics(SM_CYSCREEN);
		hBitmap1 = (HBITMAP)LoadImage(NULL, TEXT("background.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBitmap1 == NULL)
			MessageBox(NULL, TEXT("Error Loading Image"), TEXT("ERROR"), MB_ICONWARNING | MB_DEFBUTTON2);
		GetObject(hBitmap1, sizeof(bmp1), &bmp1);

		hdc = GetDC(hWnd);
		memdcGame = CreateCompatibleDC(hdc);
		memdcData = CreateCompatibleDC(hdc);
		hbitmapGame = CreateCompatibleBitmap(hdc, Game_WIDTH, WIND_HEIGHT);
		hbitmapData = CreateCompatibleBitmap(hdc, WIND_WIDTH - Game_WIDTH, WIND_HEIGHT - 50);

		SelectObject(memdcGame, hbitmapGame);
		SelectObject(memdcData, hbitmapData);
		SelectObject(memdcGame, GetStockObject(WHITE_BRUSH));
		SelectObject(memdcData, GetStockObject(LTGRAY_BRUSH));
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



//HBITMAP ScaleBitmapInt(HBITMAP hBitmap,WORD wNewWidth,WORD wNewHeight)
//{
//
//	// Create a memory DC compatible with the display
//	HDC sourceDC, destDC;
//	sourceDC.CreateCompatibleDC(NULL);
//	destDC.CreateCompatibleDC(NULL);
//
//	// Get logical coordinates
//	BITMAP bm;
//	::GetObject(hBitmap, sizeof(bm), &bm);
//
//	// Create a bitmap to hold the result
//	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL),
//		wNewWidth, wNewHeight);
//
//	// Select bitmaps into the DCs
//	HBITMAP hbmOldSource = (HBITMAP)::SelectObject(sourceDC.m_hDC, hBitmap);
//	HBITMAP hbmOldDest = (HBITMAP)::SelectObject(destDC.m_hDC, hbmResult);
//
//
//
//	destDC.StretchBlt(0, 0, wNewWidth, wNewHeight, &sourceDC,
//		0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
//
//	// Reselect the old bitmaps
//	::SelectObject(sourceDC.m_hDC, hbmOldSource);
//	::SelectObject(destDC.m_hDC, hbmOldDest);
//
//	return hbmResult;
//
//}

