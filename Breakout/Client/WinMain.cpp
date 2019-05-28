//Base.c
#include <windows.h>
#include <tchar.h>
#include "resource.h"


LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

TCHAR szProgName[] = TEXT("Base");

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
	wcApp.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_ARROW));
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
		800, 
		600, 
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

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg,
	WPARAM wParam, LPARAM lParam) {
	HDC hdc; static TCHAR c;
	static HDC memdc;
	HBITMAP hbitmap1;
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
		BitBlt(hdc, 0, 0, 800, 600, memdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		hdc = GetDC(hWnd);
		TextOut(hdc, x, y, &c, 1);
		TextOut(memdc, x, y, &c, 1);
		ReleaseDC(hWnd, hdc);
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
		Rectangle(memdc, xi, yi, xf, yf);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_RBUTTONDOWN:
		hdc = GetDC(hWnd);
		Ellipse(hdc, LOWORD(lParam), HIWORD(lParam),
			LOWORD(lParam) + 100, HIWORD(lParam) + 100);
		Ellipse(memdc, LOWORD(lParam), HIWORD(lParam),
			LOWORD(lParam) + 100, HIWORD(lParam) + 100);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_CHAR:
		c = wParam;
		hdc = GetDC(hWnd);
		TextOut(hdc, x, y, &c, 1);
		TextOut(memdc, x, y, &c, 1);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_RIGHT) {
			x += 10;
			hdc = GetDC(hWnd);
			TextOut(hdc, x, y, &c, 1);
			TextOut(memdc, x, y, &c, 1);
			ReleaseDC(hWnd, hdc);
		}
		break;
	case WM_CREATE:
		hdc = GetDC(hWnd);
		memdc = CreateCompatibleDC(hdc);
		hbitmap1 = CreateCompatibleBitmap(hdc, 800, 600);
		SelectObject(memdc, hbitmap1);
		SelectObject(memdc, GetStockObject(WHITE_BRUSH));
		PatBlt(memdc, 0, 0, 800, 600, PATCOPY);
		ReleaseDC(hWnd, hdc);
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOGIN), hWnd, (DLGPROC)FuncaoCaixa1);
	default:
		return DefWindowProc(hWnd, messg, wParam, lParam);
		break;
	}
	return(0);
}