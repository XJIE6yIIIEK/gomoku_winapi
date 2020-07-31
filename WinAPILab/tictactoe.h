#pragma once
const int N = 25;

enum CellsAfflication {
	PlayerOne,
	PlayerTwo,
	Free
};

enum PlayerTurn {
	Player1,
	Player2
};

extern CellsAfflication** field;
extern PlayerTurn currPlayer;

void CreateField();
void ClearField();
void PlaceCircle(LPARAM, HWND, bool*);
void PlaceCross(LPARAM, HWND, bool*);