#pragma once
#include <map>
#include <string>

enum Mode {
	MapView,
	StandartIO,
	WinAPIFunc,
	FileVars,
	Null
};

enum Option {
	Width,
	Height,
	X,
	Y,
	Background,
	Cells,
	Circle,
	Cross
};

struct FileMap {
	HANDLE hFile;
	HANDLE hMap;
	size_t fSize;
	unsigned char* dataPtr;
};

struct RegResult {
	std::string option;
	std::string value;
};

extern std::map<std::string, Mode> modeMap;

extern std::map<std::string, Option> optionMap;

void SetIOMode();
void LoadSettings();
void SaveSettings();