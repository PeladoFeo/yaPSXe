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

#include "Common.h"
#include "Console.h"

Console::Console() {
	int hConHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&coninfo);
	coninfo.dwSize.Y = 500;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();

	hConsole = GetConsoleWindow();
	SetWindowText(hConsole, "Console");

	mStdCol = CWHITE;

	/*
#if defined(_DEBUG) 
	out(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, "Debug build\n");
#elif define(_RELEASE)
	out(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, " Release build\n");
#endif
	*/
}

void Console::out(int col, const char *fmt, ...) {
#if !defined(GLOBAL_DISABLE_CSL_OUT)
	if (!fmt) return;	

	static char text[256];							
	static char *ap;										
	
	if (strlen(fmt) > 256) {
		std::printf("ERROR: Format string > 256\n");	
		return;
	}

	va_start(ap, fmt);								
	    vsprintf_s(text, fmt, ap);					
	va_end(ap);										

	SetConsoleTextAttribute(lStdHandle, col); 
	std::printf(text);	
#endif
}

void Console::out(const char *fmt, ...){
#if !defined(GLOBAL_DISABLE_CSL_OUT)
	if (!fmt) return;	

	static char text[256];							
	static char *ap;										
	
	if (strlen(fmt) > 256) {
		std::printf("ERROR: Format string > 256\n");	
		return;
	}

	va_start(ap, fmt);								
	    vsprintf_s(text, fmt, ap);					
	va_end(ap);										

	SetConsoleTextAttribute(lStdHandle, mStdCol); 
	std::printf(text);	
#endif
}