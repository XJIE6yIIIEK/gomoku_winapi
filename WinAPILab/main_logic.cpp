#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <tchar.h> 
#include "guilib.h"
#include "tictactoe.h"
#include "srlzsys.h"
#include "main_logic.h"

const char* crossName;
const char* circleName;

bool startProg = true;
const TCHAR szWinClass[] = _T("tictactoe");
const TCHAR szWinName[] = _T("TicTacToe");
WNDCLASS wincl = { 0 };
HBRUSH hBrush;

void RunNotepad() {
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	puts("Starting Notepad...");
	CreateProcess(_T("C:\\Windows\\Notepad.exe"),
				  NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

LRESULT CALLBACK WindowProcedure(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam) {
	bool placed = false;
	switch (message) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		case WM_LBUTTONUP:
			if (currPlayer == Player2)
				PlaceCircle(lParam, hWindow, &placed);
			else
				PlaceCross(lParam, hWindow, &placed);
		case WM_PAINT:
		case WM_SIZE: {
			if(placed || startProg)
				DrawField(hWindow);
			return 0;
		}

		case WM_MOVE: {
			RECT* wRect = new RECT;
			GetWindowRect(hWindow, wRect);
			posX = wRect->left;
			posY = wRect->top;
			delete wRect;
			return 0;
		}

		case WM_KEYUP: {
			switch (wParam) {
				case VK_RETURN: {
					if (startProg) {
						startProg = false;
						return 0;
					}
					ChangeBackgroundColor(hBrush, hWindow);
					return 0;
				}

				case 0x51: {
					char highBit = ((unsigned short)GetKeyState(VK_CONTROL)) >> 15;
					if ((bool)highBit) {
						PostQuitMessage(0);
						return 0;
					}
				} break;

				case 0x43: {
					char highBit = ((unsigned short)GetKeyState(VK_SHIFT)) >> 15;
					if ((bool)highBit) {
						RunNotepad();
						return 0;
					}
				} break;

				case VK_ESCAPE: {
					PostQuitMessage(0);
					return 0;
				}
			}
		}
	}

	return DefWindowProc(hWindow, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	wchar_t gr[128] = L"TEST";
	SetConsoleTitle((LPCSTR)gr);
	BOOL bMessageOk;
	MSG message;
	HWND hWindow;
	SetIOMode();
	LoadSettings();
	CreateField();

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szWinClass;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);

	hBrush = CreateSolidBrush(rgb);
	wincl.hbrBackground = hBrush;

	if (!RegisterClass(&wincl))
		return 0;

	hWindow = CreateWindow(
		szWinClass,
		szWinName,
		WS_OVERLAPPEDWINDOW,
		posX,
		posY,
		width,
		height,
		HWND_DESKTOP,
		NULL,
		hThisInstance,
		NULL
	);

	HMODULE hModule = LoadLibrary(_T("imagelib.dll"));
	DWORD er = GetLastError();
	if (hModule != NULL) {
		typedef unsigned char* (_cdecl* DLLFUNC)(const char*, int*, int*);
		DLLFUNC dllFunc = (DLLFUNC)GetProcAddress(hModule, _T("LoadImageS"));
		crossImageWidth = new int(0);
		crossImageHeight = new int(0);
		circleImageWidth = new int(0);
		circleImageHeight = new int(0);
		crossImage = dllFunc(crossName, crossImageWidth, crossImageHeight);
		circleImage = dllFunc(circleName, circleImageWidth, circleImageHeight);
		FreeLibrary(hModule);
	}

	ShowWindow(hWindow, nCmdShow);

	while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0) {
		if (bMessageOk == -1) {
			puts("error");
			break;
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	SaveSettings();
	DestroyWindow(hWindow);
	UnregisterClass(szWinClass, hThisInstance);
	DeleteObject(hBrush);

	ClearField();

	return 0;
}