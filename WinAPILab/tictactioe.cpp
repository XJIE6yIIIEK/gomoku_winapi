#include "guilib.h"
#include "tictactoe.h"

CellsAfflication** field;
PlayerTurn currPlayer;

void CreateField() {
	field = new CellsAfflication*[N];
	for (int i = 0; i < N; i++) {
		field[i] = new CellsAfflication[N];
		for (int j = 0; j < N; j++)
			field[i][j] = Free;
	}
}

void ClearField() {
	for (int i = 0; i < N; i++)
		delete[] field[i];
	delete field;
}

void PlaceCircle(LPARAM lParam, HWND hWindow, bool* placed) {
	GridOptions* gridOpt = GetGridOptions(hWindow);
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	bool found = false;
	for (int i = 0; i <= N && !found; i++) {
		for (int j = 0; j <= N && !found; j++) {
			if (yPos > gridOpt->marginY && xPos > gridOpt->marginX && xPos < gridOpt->marginX + gridOpt->CELLSIZE * j && yPos < gridOpt->marginY + gridOpt->CELLSIZE * i) {
				if (field[i - 1][j - 1] == Free) {
					field[i - 1][j - 1] = PlayerTwo;
					*placed = true;
				}
				found = true;
			}
		}
	}
	delete gridOpt;
	if(*placed)
		currPlayer = Player1;
}

void PlaceCross(LPARAM lParam, HWND hWindow, bool* placed) {
	GridOptions* gridOpt = GetGridOptions(hWindow);
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	bool found = false;
	for (int i = 0; i <= N && !found; i++) {
		for (int j = 0; j <= N && !found; j++) {
			if (yPos > gridOpt->marginY && xPos > gridOpt->marginX && xPos < gridOpt->marginX + gridOpt->CELLSIZE * j && yPos < gridOpt->marginY + gridOpt->CELLSIZE * i) {
				if (field[i - 1][j - 1] == Free) {
					field[i - 1][j - 1] = PlayerOne;
					*placed = true;
				}
				found = true;
			}
		}
	}
	delete gridOpt;
	if(*placed)
		currPlayer = Player2;
}