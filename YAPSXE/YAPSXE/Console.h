/*  yaPSXe - PSX emulator
 *  Copyright (C) 2011-2012 Ryan Hackett
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H

#include "Common.h"

/* Console colours */
#define CYELLOW		(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY)
#define CRED		(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define CGREEN		(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define CWHITE		(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY)
#define CCYAN		(FOREGROUND_BLUE | FOREGROUND_GREEN)

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