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
#ifndef WINDOWS_H
#define WINDOWS_H

#include "Common.h"

#pragma comment(linker,"/manifestdependency:\"type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

enum {
	IDM_EMULATION_EXECUTE,
	IDM_EMULATION_LOAD_SAVESTATE,
	IDM_EMULATION_CREATE_SAVESTATE,
	IDM_EMULATION_EXIT,
	IDM_EMULATION_RESET,
	IDM_EMULATION_PAUSE,
	IDM_EMULATION_RUN,
	IDM_CONFIG_CONFIGURE,
	IDM_DEBUG_CPU,
};

LRESULT DisplayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef LRESULT (*WinProc)(HWND,UINT,WPARAM,LPARAM);

class CWindow {
public:
	CWindow() {
		MenuCreate();
	}

	~CWindow() {
		//UnregisterClass(lpcstrClassName, hInstance);
	}

	BOOL WindowCreate(const char *title, 
						 const char *className, 
						 int width, int height,
						 BOOL b_vScrollBar,
						 BOOL bCreateShow,
						 WinProc proc,
						 COLORREF bgCol, 
						 BOOL child = FALSE, 
						 HWND parent = 0,
						 int addedStyles = 0);

	void KillWindow();

	static void ProcessMessages();

	static BOOL IsResolutionSupported(int width, int height);
	static BOOL OpenFileDlg(HWND hwnd, char *out, char *filter);
	static BOOL SaveFileDlg(HWND hWnd, char *out, char *filter);
	static BOOL GetFolderSelection(HWND hWnd, LPTSTR szBuf, LPCTSTR szTitle);

	void MenuCreate();
	void MenuSet();
	HMENU PopupMenuCreate(char *name);
	void AddMenuItem(HMENU hSubMenu, int MenuItemID, char *name);
	void AddMenuSeperator(HMENU hSubMenu);

	MSG GetMsg() { return Msg; }
	HWND GetHwnd() { return hWnd; }
	HMENU GetHMenu() { return hMenu; }
	BOOL IsOpen() { return mIsOpen; }

	void ShowWnd(BOOL show) {
		mIsOpen = show;
		ShowWindow(hWnd, show);
	}

	HDC hDC;
	PAINTSTRUCT ps;
	BOOL mFullscreen;
	int width,height;

private:
	HWND hWnd;
	MSG Msg;
	HMENU hMenu;
	BOOL mIsOpen;
};

#endif /* WINDOWS_H */