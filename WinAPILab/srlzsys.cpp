#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <tchar.h>
#include <shellapi.h>
#include <string>
#include <map>
#include <regex>
#include <iterator>
#include <iostream>
#include <fstream>
#include "srlzsys.h"
#include "guilib.h"
#include "main_logic.h"

std::map<std::string, Mode> modeMap = {
	{"-m", MapView},
	{"-s", StandartIO},
	{"-w", WinAPIFunc},
	{"-fv", FileVars},
};

std::map<std::string, Option> optionMap = {
	{"window.size.width", Width},
	{"window.size.height", Height},
	{"game.cells", Cells},
	{"window.position.x", X},
	{"window.position.y", Y},
	{"window.background", Background},
	{"image.circle", Circle},
	{"image.cross", Cross}
};

Mode mode = MapView;

char** argv = nullptr;
int argc;

void GetCommandLineArguments(char**& argv, int& argc) {
	LPWSTR* lpArgv = CommandLineToArgvW(GetCommandLineW(), &argc);
	argv = new char* [argc];
	for (int i = 0; i < argc; i++) {
		int size = wcslen(lpArgv[i]) + 1;
		argv[i] = new char[size];
		wcstombs(argv[i], lpArgv[i], size);
	}
	LocalFree(lpArgv);
}

void SetIOMode() {
	char** argv = nullptr;
	int argc;
	GetCommandLineArguments(argv, argc);
	if (argc > 1) {
		std::string str(argv[1]);
		mode = modeMap[str];
	}
	for (int i = 0; i < argc; i++)
		delete argv[i];
	delete argv;
}

void SetSettings(RegResult* regRes) {
	switch (optionMap[regRes->option]) {
		case Cross: {
			crossName = (regRes->value.c_str());
		} break;

		case Circle: {
			circleName = (regRes->value.c_str());
		} break;

		case X: {
			try {
				posX = std::stoi(regRes->value);
			} catch (std::invalid_argument) {
				posX = 0;
			}
			delete regRes;
		} break;

		case Y: {
			try {
				posY = std::stoi(regRes->value);
			} catch (std::invalid_argument) {
				posY = 0;
			}
			delete regRes;
		} break;

		case Background: {
			std::regex reg("([0-9]+)");
			std::smatch match;
			std::vector<std::string> rgbVec(0);
			std::sregex_iterator currentMatch(regRes->value.begin(), regRes->value.end(), reg);
			std::sregex_iterator lastMatch;
			while (currentMatch != lastMatch) {
				std::smatch match = *currentMatch;
				rgbVec.push_back(match.str());
				currentMatch++;
			}
			rgb = RGB(stoi(rgbVec[0]), stoi(rgbVec[1]), stoi(rgbVec[2]));
			delete regRes;
		} break;

		case Width: {
			try {
				width = std::stoi(regRes->value);
			} catch (std::invalid_argument) {
				width = 640;
			}
			delete regRes;
		} break;

		case Height: {
			try {
				height = std::stoi(regRes->value);
			} catch (std::invalid_argument) {
				height = 640;
			}
			delete regRes;
		} break;
	}
}

void SetSettings(std::vector<std::string> &arg) {
	std::regex reg("(.+)?=(.+)?");
	for (size_t i = 0; i < arg.size(); i++) {
		RegResult* regRes = new RegResult;
		std::smatch match;
		std::regex_match(arg[i], match, reg);

		regRes->option = match.str(1);
		regRes->value = match.str(2);

		SetSettings(regRes);
	}
}

void SetSettings(std::string str) {
	std::regex reg("(.+)?=(.+)?");
	std::vector<std::string> strVec(0);
	std::sregex_iterator currentMatch(str.begin(), str.end(), reg);
	std::sregex_iterator lastMatch;
	while (currentMatch != lastMatch) {
		std::smatch match = *currentMatch;
		strVec.push_back(match.str());
		currentMatch++;
	}
	SetSettings(strVec);
}

void LoadSettings() {
	switch (mode) {
		case MapView: {
			HANDLE hFile = CreateFile("config.ini", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile == INVALID_HANDLE_VALUE) {
				return;
			}
			
			DWORD dwFileSize = GetFileSize(hFile, nullptr);
			if (dwFileSize == INVALID_FILE_SIZE) {
				CloseHandle(hFile);
				return;
			}

			HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (hMapping == nullptr) {
				CloseHandle(hFile);
				return;
			}

			LPVOID lpvFile = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
			char* dataPtr = (char*)lpvFile;
			if (dataPtr == nullptr) {
				CloseHandle(hMapping);
				CloseHandle(hFile);
				return;
			}

			std::string str(dataPtr);

			SetSettings(str);

			UnmapViewOfFile(dataPtr);
			CloseHandle(hMapping);
			CloseHandle(hFile);
		} break;

		case StandartIO: {
			std::ifstream readFile;
			readFile.open("config.ini");
			if (!readFile.is_open())
				return;
			std::vector<std::string> strVec(0);

			while (!readFile.eof()) {
				std::string str;
				std::getline(readFile, str);
				if(str != "")
					strVec.push_back(str);
			}
			readFile.close();

			SetSettings(strVec);
		} break;

		case WinAPIFunc: {
			HANDLE hFile = CreateFile("config.ini", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile == INVALID_HANDLE_VALUE) {
				return;
			}

			DWORD dwFileSize = GetFileSize(hFile, nullptr);
			if (dwFileSize == INVALID_FILE_SIZE) {
				CloseHandle(hFile);
				return;
			}

			int dwBufSize = dwFileSize / sizeof(char) + 1;
			char* dataPtr = new char[dwBufSize];
			LPDWORD lpdwBytesReaded = nullptr;
			ReadFile(hFile, dataPtr, dwFileSize, lpdwBytesReaded, nullptr);
			dataPtr[dwBufSize - 1] = '\0';
			std::string str(dataPtr);

			SetSettings(str);

			CloseHandle(hFile);
			delete[] dataPtr;
			delete lpdwBytesReaded;
		} break;

		case FileVars: {
			FILE* file = fopen("config.ini", "r+");
			if (file == NULL)
				return;
			fseek(file, 0, SEEK_END);
			long lSize = ftell(file);
			rewind(file);

			char* buffer = new char[lSize];
			fread(buffer, sizeof(char), lSize, file);

			std::string str(buffer);
			SetSettings(str);

			fclose(file);
			delete file;
			delete[] buffer;
		} break;
	}
}

void SaveSettings() {
	switch (mode) {
		case MapView: {
			std::string str;
			str = "window.size.width=" + std::to_string(width) + '\n';
			str += "window.size.height=" + std::to_string(height) + '\n';
			str += "window.position.x=" + std::to_string(posX) + '\n';
			str += "window.position.y=" + std::to_string(posY) + '\n';
			str += "window.background=" + std::to_string(GetRValue(rgb)) + ',' + std::to_string(GetGValue(rgb)) + ',' + std::to_string(GetBValue(rgb)) + '\n';
			str += "image.circle=" + (std::string)circleName + '\n';
			str += "image.cross=" + (std::string)crossName;
			if (imageNames.size() != 0) {
				str += '\n';
				for (size_t i = 0; i < imageNames.size(); i++) {
					str += "resources.images.name=" + imageNames[i];
					if (imageNames.size() - i != 0)
						str += '\n';
				}
			}

			DWORD dwSize = (sizeof(char) * (int)str.size());
			HANDLE hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile == INVALID_HANDLE_VALUE) {
				hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
				if (hFile == INVALID_HANDLE_VALUE) {
					return;
				}
			}

			HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, dwSize, nullptr);
			if (hMapping == nullptr) {
				DWORD s = GetLastError();
				CloseHandle(hFile);
				return;
			}

			LPVOID lpvFile = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, dwSize);
			char* buff = (char*)lpvFile;
			const char* temp = str.c_str();
			strcpy(buff, str.c_str());

			UnmapViewOfFile(lpvFile);
			CloseHandle(hMapping);
			SetFilePointer(hFile, dwSize, nullptr, FILE_BEGIN);
			SetEndOfFile(hFile);
			CloseHandle(hFile);
		} break;

		case StandartIO: {
			std::ofstream writeFile;
			writeFile.open("config.ini");			

			writeFile << "window.size.width=" << std::to_string(width) << std::endl;
			writeFile << "window.size.height=" << std::to_string(height) << std::endl;
			writeFile << "window.position.x=" << std::to_string(posX) << std::endl;
			writeFile << "window.position.y=" << std::to_string(posY) << std::endl;
			writeFile << "window.background=" << std::to_string(GetRValue(rgb)) << "," << std::to_string(GetGValue(rgb)) << "," << std::to_string(GetBValue(rgb)) << std::endl;
			writeFile << "image.cross=" << crossName << std::endl;
			writeFile << "image.circle=" << circleName;
			if (imageNames.size() != 0) {
				writeFile << std::endl;
				for (size_t i = 0; i < imageNames.size(); i++) {
					writeFile << "resources.images.name=" << imageNames[i];
					if (imageNames.size() - 1 == 0)
						writeFile << std::endl;
				}
			}
			writeFile << EOF;
			writeFile.close();
		} break;

		case WinAPIFunc: {
			std::string str;
			str = "window.size.width=" + std::to_string(width) + '\n';
			str += "window.size.height=" + std::to_string(height) + '\n';
			str += "window.position.x=" + std::to_string(posX) + '\n';
			str += "window.position.y=" + std::to_string(posY) + '\n';
			str += "window.background=" + std::to_string(GetRValue(rgb)) + ',' + std::to_string(GetGValue(rgb)) + ',' + std::to_string(GetBValue(rgb)) + '\n';
			str += "image.circle=" + (std::string)circleName + '\n';
			str += "image.cross=" + (std::string)crossName;
			if (imageNames.size() != 0) {
				str += '\n';
				for (size_t i = 0; i < imageNames.size(); i++) {
					str += "resources.images.name=" + imageNames[i];
					if (imageNames.size() - i == 0)
						str += '\n';
				}
			}

			DWORD dwSize = sizeof(char) * (int)str.size();
			HANDLE hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile == INVALID_HANDLE_VALUE) {
				hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
				if (hFile == INVALID_HANDLE_VALUE) {
					return;
				}
			}
			LPDWORD lpdwBytesReaded = nullptr;
			WriteFile(hFile, str.c_str(), dwSize, lpdwBytesReaded, nullptr);
			
			SetFilePointer(hFile, dwSize, nullptr, FILE_BEGIN);
			SetEndOfFile(hFile);
			CloseHandle(hFile);
			delete lpdwBytesReaded;
		} break;

		case FileVars: {
			std::string str;
			str = "window.size.width=" + std::to_string(width) + '\n';
			str += "window.size.height=" + std::to_string(height) + '\n';
			str += "window.position.x=" + std::to_string(posX) + '\n';
			str += "window.position.y=" + std::to_string(posY) + '\n';
			str += "window.background=" + std::to_string(GetRValue(rgb)) + ',' + std::to_string(GetGValue(rgb)) + ',' + std::to_string(GetBValue(rgb)) + '\n';
			str += "image.circle=" + (std::string)circleName + '\n';
			str += "image.cross=" + (std::string)crossName;
			if (imageNames.size() != 0) {
				str += '\n';
				for (size_t i = 0; i < imageNames.size(); i++) {
					str += "resources.images.name=" + imageNames[i];
					if (imageNames.size() - i == 0)
						str += '\n';
				}
			}

			FILE* file = fopen("config.ini", "w+");
			if (file == NULL)
				return;

			fwrite(str.c_str(), sizeof(char), (int)str.size(), file);
			fclose(file);
			delete file;
		} break;
	}
}