#pragma once
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <vector>

struct GridOptions {
	int CELLSIZE;
	int marginX;
	int marginY;
};

extern COLORREF rgb;
extern int width;
extern int height;
extern int posX;
extern int posY;
extern std::vector<std::string> imageNames;

extern unsigned char* crossImage;
extern int* crossImageWidth;
extern int* crossImageHeight;

extern unsigned char* circleImage;
extern int* circleImageWidth;
extern int* circleImageHeight;

const int MIN_MARGIN = 15;

GridOptions* GetGridOptions(HWND hWindow, RECT* dcRect = nullptr);
void DrawField(HWND);
void ChangeBackgroundColor(HBRUSH, HWND);