#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <string>
#include <vector>
#include <random>
#include "guilib.h"
#include "tictactoe.h"

COLORREF rgb = RGB(0, 0, 255);;
int width = 800;
int height = 600;
int posX = 0;
int posY = 0;
std::vector<std::string> imageNames(0);

unsigned char* crossImage;
int* crossImageWidth;
int* crossImageHeight;

unsigned char* circleImage;
int* circleImageWidth;
int* circleImageHeight;

void (*DrawCirclePtr)();
void (*DrawCrossPtr)();

void ChangeBackgroundColor(HBRUSH hBrush, HWND hWindow) {
	std::random_device rd;
	std::mt19937 mersenne(rd());

	int RED = mersenne() % 256;
	int GREEN = mersenne() % 256;
	int BLUE = mersenne() % 256;
	
	if(hBrush != NULL)
		DeleteObject(hBrush);
	rgb = RGB(RED, GREEN, BLUE);
	hBrush = CreateSolidBrush(rgb);
	SetClassLongPtr(hWindow, GCLP_HBRBACKGROUND, LONG(hBrush));
	InvalidateRect(hWindow, NULL, TRUE);
}

GridOptions* GetGridOptions(HWND hWindow, RECT* dcRect) {
	RECT wRect = {};
	bool tempRect = false;
	if (dcRect == nullptr) {
		dcRect = new RECT;
		tempRect = true;
	}

	GetClientRect(hWindow, dcRect);
	GetWindowRect(hWindow, &wRect);

	int temp;
	int cellsSizeOnX = ((temp = ((wRect.right - wRect.left) - 2 * MIN_MARGIN) / N) < 0) ? 0 : temp;
	int cellsSizeOnY = ((temp = (dcRect->bottom - 2 * MIN_MARGIN) / N) < 0) ? 0 : temp;
	int CELLSIZE = (cellsSizeOnX < cellsSizeOnY) ? cellsSizeOnX : cellsSizeOnY;

	int marginX = (((wRect.right - wRect.left) - (CELLSIZE * N)) / 2) - ((wRect.right - wRect.left) - dcRect->right) / 2;
	int marginY = (dcRect->bottom - (CELLSIZE * N)) / 2;

	GridOptions* gridOpt = new GridOptions;
	gridOpt->CELLSIZE = CELLSIZE;
	gridOpt->marginX = marginX;
	gridOpt->marginY = marginY;
	if (tempRect)
		delete dcRect;
	return gridOpt;
}

void DrawCircle(int xIndex, int yIndex, HDC &hdc, GridOptions* gridOpt) {
	HPEN hPen;
	hPen = (HPEN)SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
	int outerLeft = gridOpt->marginX + gridOpt->CELLSIZE * xIndex + 5;
	int outerTop = gridOpt->marginY + gridOpt->CELLSIZE * yIndex + 5;
	int outerRight = gridOpt->marginX + gridOpt->CELLSIZE * (xIndex + 1) - 5;
	int outerBottom = gridOpt->marginY + gridOpt->CELLSIZE * (yIndex + 1) - 5;
	if (outerRight - outerLeft < 1) {
		DeleteObject(hPen);
		return;
	}
	Ellipse(hdc, outerLeft, outerTop, outerRight, outerBottom);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 3, RGB(255, 150, 0));
	SelectPen(hdc, hPen);
	Ellipse(hdc, outerLeft, outerTop, outerRight, outerBottom);
	DeleteObject(hPen);
}

void DrawCross(int xIndex, int yIndex, HDC &hdc, GridOptions* gridOpt) {
	HPEN pen;
	pen = CreatePen(PS_SOLID, 3, RGB(255, 150, 0));
	SelectPen(hdc, pen);
	int leftUpX = gridOpt->marginX + gridOpt->CELLSIZE * xIndex + 2;
	int leftUpY = gridOpt->marginY + gridOpt->CELLSIZE * yIndex + 2;
	int rightBottomX = gridOpt->marginX + gridOpt->CELLSIZE * (xIndex + 1) - 2;
	int rightBottomY = gridOpt->marginY + gridOpt->CELLSIZE * (yIndex + 1) - 2;

	int rightUpX = gridOpt->marginX + gridOpt->CELLSIZE * (xIndex + 1) - 2;
	int rightUpY = gridOpt->marginY + gridOpt->CELLSIZE * (yIndex) + 2;
	int leftBottomX = gridOpt->marginX + gridOpt->CELLSIZE * (xIndex) + 2;
	int leftBottomY = gridOpt->marginY + gridOpt->CELLSIZE * (yIndex + 1) - 2;
	MoveToEx(hdc, leftUpX, leftUpY, NULL);
	LineTo(hdc, rightBottomX, rightBottomY);
	MoveToEx(hdc, rightUpX, rightUpY, NULL);
	LineTo(hdc, leftBottomX, leftBottomY);
	DeleteObject(pen);
}

void DrawField(HWND hWindow) {
	HDC hdc;
	RECT* dcRect = new RECT;
	PAINTSTRUCT ps;
	HPEN pen;

	GridOptions* gridOpt = GetGridOptions(hWindow, dcRect);

	InvalidateRect(hWindow, dcRect, TRUE);

	hdc = BeginPaint(hWindow, &ps);
	pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	SelectPen(hdc, pen);

	for (int i = 0; i <= N; i++) {
		MoveToEx(hdc, gridOpt->marginX, gridOpt->marginY + gridOpt->CELLSIZE * i, NULL);
		LineTo(hdc, gridOpt->marginX + gridOpt->CELLSIZE * N, gridOpt->marginY + gridOpt->CELLSIZE * i);
		MoveToEx(hdc, gridOpt->marginX + gridOpt->CELLSIZE * i, gridOpt->marginY, NULL);
		LineTo(hdc, gridOpt->marginX + gridOpt->CELLSIZE * i, gridOpt->marginY + gridOpt->CELLSIZE * N);
	}
	DeleteObject(pen);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			switch (field[i][j]) {
				case PlayerOne: {
					DrawCross(j, i, hdc, gridOpt);
				} break;

				case PlayerTwo: {
					DrawCircle(j, i, hdc, gridOpt);
				} break;
			}
		}
	}

	RECT* wRect = new RECT;
	GetWindowRect(hWindow, wRect);
	posX = wRect->left;
	posY = wRect->top;
	width = wRect->right - wRect->left;
	height = wRect->bottom - wRect->top;

	DeleteObject(hdc);
	delete dcRect;
	delete wRect;
	delete gridOpt;
	EndPaint(hWindow, &ps);
	UpdateWindow(hWindow);
}