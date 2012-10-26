#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H

#include "Common.h"

/* Console colours */
#define CYELLOW		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define CRED		FOREGROUND_RED | FOREGROUND_INTENSITY
#define CGREEN		FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define CWHITE		FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define CCYAN		FOREGROUND_BLUE | FOREGROUND_GREEN

class Console {
public:
	Console();

	void out(int col, const char *fmt, ...);
	void out(const char *fmt, ...);

	void SetStdColour(int col) { mStdCol = col; }

private:
	HANDLE lStdHandle;
	HWND hConsole;
	int mStdCol;
};

#endif /* CONSOLE_H */