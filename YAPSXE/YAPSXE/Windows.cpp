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
#include "Windows.h"
#include "Psx.h"
#include "Gpu.h"
#include "Cpu.h"
#include "Interpreter.h"
#include "OpenGL.h"
#include "Console.h"
#include "Memory.h"
#include "Spu.h"
#include "Config.h"
#include "resource.h"

#if defined (_DEBUG)
#include "CpuDebugger.h"
#endif

BOOL Window::OpenFileDlg(HWND hwnd, char *out, char *filter) {
	if (!out) return FALSE;
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = (LPSTR)out;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn)) {
		return TRUE;
	} else {
		return FALSE;
	}
}


/*
 * Displays a directory selection dialog. CoInitialize must be called before calling this function.
 * szBuf must be MAX_PATH characters in length. hWnd may be 0.
 */
BOOL Window::GetFolderSelection(HWND hWnd, LPTSTR szBuf, LPCTSTR szTitle) {
	LPITEMIDLIST pidl     = 0;
	BROWSEINFO   bi       = { 0 };
	BOOL         bResult  = FALSE;

	bi.hwndOwner      = hWnd;
	bi.pszDisplayName = szBuf;
	bi.pidlRoot       = 0;
	bi.lpszTitle      = szTitle;
	bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

	if ((pidl = SHBrowseForFolder(&bi)) != 0) {
		bResult = SHGetPathFromIDList(pidl, szBuf);
		CoTaskMemFree(pidl);
	}

	return bResult;
}

BOOL Window::SaveFileDlg(HWND hWnd, char *out, char *filter) {
	if (!out) return FALSE;
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = (LPSTR)out;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileName(&ofn)) {
	    return TRUE;
	} else {
		return FALSE;
	}
} 

LRESULT DisplayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Psx *psx = Psx::GetInstance();
	static  BOOL cursorhidden = FALSE;

	if (Psx::GetInstance()->mDispWnd->mFullscreen) {
		if (!cursorhidden) {
			ShowCursor(FALSE);
			cursorhidden = TRUE;
		}
	} else {
		if (cursorhidden) {
			ShowCursor(TRUE);
			cursorhidden = FALSE;
		}
	}

    switch(msg) {
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
			};
		} break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
			break;

        case WM_DESTROY:
            PostQuitMessage(0);
			Psx::GetInstance()->SignalQuit();
			break;

		case WM_KEYDOWN: {
			if (wParam == VK_ESCAPE) {
				psx->SignalQuit();
			} else if (wParam == VK_F1) {
				psx->gl->ToggleFullscreenMode();
			}
		} break;

		case WM_SIZE:
			Psx::GetInstance()->mDispWnd->width = lParam & 0xffff;
			Psx::GetInstance()->mDispWnd->height = lParam >> 16;
			if (Psx::GetInstance()->gl->mInitialised) {
				//glViewport(0, 0, Psx::GetInstance()->mMainWnd->width, Psx::GetInstance()->mMainWnd->height);
			}
			break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT mMainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Psx *psx = Psx::GetInstance();
    switch(msg) {
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDM_EMULATION_EXIT:
					DestroyWindow(hwnd);
					break;

				case IDM_EMULATION_EXECUTE:
					psx->Execute();
					break;

				case IDM_EMULATION_LOAD_SAVESTATE: {
					char path[MAX_PATH];
					psx->cpu->SetPsxCpu(PSX_CPU_HALTED);
					if (Window::OpenFileDlg(hwnd, path, 
						"Savestates (*.svt)\0*.svt\0All files (*.*)\0*.*\0\0")
						) {
						psx->StartSaveStateEmulation(path);
					} 
				} break;

				case IDM_EMULATION_CREATE_SAVESTATE: {
					char path[MAX_PATH];
					psx->cpu->SetPsxCpu(PSX_CPU_HALTED);
					if (Window::SaveFileDlg(hwnd, path,
						"All files (*.*)\0*.*\0\0")
						) {
						psx->CreateSaveStateFile(path);
					}
					psx->cpu->SetPsxCpu(PSX_CPU_RUNNING);
				} break;

				case IDM_EMULATION_RESET:
					psx->ResetPsx();
					psx->ClearVram();
					psx->Execute();
					break;

				case IDM_EMULATION_PAUSE: {
					u32 state = GetMenuState(psx->mMainWnd->GetHMenu(), 
						IDM_EMULATION_PAUSE, MF_BYCOMMAND);
					if (state & MF_GRAYED)
						break;
					if (state & MF_CHECKED) {
						psx->cpu->SetPsxCpu(PSX_CPU_RUNNING);
						CheckMenuItem(psx->mMainWnd->GetHMenu(), 
							IDM_EMULATION_PAUSE, MF_UNCHECKED);
					} else {
						psx->cpu->SetPsxCpu(PSX_CPU_HALTED);
						CheckMenuItem(psx->mMainWnd->GetHMenu(), 
							IDM_EMULATION_PAUSE, MF_CHECKED);
					}
				} break;

				case IDM_DEBUG_CPU:
#if defined (_DEBUG)
					psx->mCpuDbg->OpenDebugger();
#endif
					break;

				case IDM_CONFIG_CONFIGURE:
					DialogBoxParam(psx->hInst, MAKEINTRESOURCE(IDD_CONFIG_DLG),
									hwnd, (DLGPROC)ConfigDlgProc, 0);
					break;
			};
		} break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
			break;

        case WM_DESTROY:
            PostQuitMessage(0);
			psx->quit = TRUE;
			break;

		case WM_LBUTTONDOWN: {
		} break;

		case WM_SIZE:
			psx->mMainWnd->width = lParam & 0xffff;
			psx->mMainWnd->height = lParam >> 16;
			//if (Psx::GetInstance()->gl->mInitialised) {
				//glViewport(0, 0, Psx::GetInstance()->mMainWnd->width, Psx::GetInstance()->mMainWnd->height);
			//}
			break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void Window::MenuCreate() {
	hMenu = CreateMenu();
}

void Window::MenuSet() {
	SetMenu(hWnd, hMenu);
}

HMENU Window::PopupMenuCreate(char *name) {
	HMENU hSubMenu = CreatePopupMenu(); 
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, name);
	return hSubMenu;
}

void Window::AddMenuItem(HMENU hSubMenu, int menuItemID, char *name) {
	AppendMenu(hSubMenu, MF_STRING, menuItemID, name);
}

void Window::AddMenuSeperator(HMENU hSubMenu) {
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, 0);
}

BOOL Window::IsResolutionSupported(int width, int height) {
	DEVMODE mode;
	for (int i = 0; EnumDisplaySettings(0, i, &mode); i++) {
		if (ChangeDisplaySettings(&mode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
			if (width == mode.dmPelsWidth && height == mode.dmPelsHeight) {
				return TRUE;
			}
	}
	return FALSE;
}

BOOL Window::WindowCreate(const char *title, 
								const char *className, 
								int width, int height,
								BOOL b_vScrollBar,
								BOOL bCreateShow, 
								WinProc proc,
								COLORREF bgCol,
								BOOL child, 
								HWND hParent,
								int addedStyles) 
{
    WNDCLASSEX wc;

	mFullscreen = FALSE;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = (WNDPROC)proc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = Psx::GetInstance()->hInst;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(bgCol);	
    wc.lpszMenuName  = 0;
    wc.lpszClassName = className;
    wc.hIconSm       = LoadIcon(0, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)) {
        MessageBox(0, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

	if (!(hWnd=CreateWindowEx(	WS_EX_APPWINDOW,						
								className,						
								title,
								WS_OVERLAPPEDWINDOW | 
								WS_CLIPSIBLINGS | WS_CLIPCHILDREN  | 
								(child ? WS_CHILD : 0) 
								| (b_vScrollBar ? WS_VSCROLL : 0) | addedStyles,					
								CW_USEDEFAULT, CW_USEDEFAULT,								
								width,height,	
								hParent,							
								0,								
								Psx::GetInstance()->hInst,							
								0)))	{
		MessageBox(0,"Window Creation Error", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;							
	}

	if (bCreateShow) {
		ShowWindow(hWnd, TRUE);
		mIsOpen = TRUE;
	} else {
		ShowWindow(hWnd, FALSE);
		mIsOpen = FALSE;
	}

    UpdateWindow(hWnd);

	return TRUE;
}

/* processes messages from all windows created on the current thread */
void Window::ProcessMessages() {
	static MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);			
		DispatchMessage(&msg);			
	}
}

static BOOL CreateMainWindows(Psx *psx) {
#if defined (_DEBUG) 
	char *title = "yaPSXe (debug build)";
#elif defined (NDEBUG)
	char *title = "yaPSXe (release build)";
#endif
	HMENU hSubMenu;

	/* create the menu window */
	if (!psx->mMainWnd->WindowCreate(title, "ClassNamePSX", 300, 200, 
			FALSE, TRUE, mMainWndProc, RGB(200,200,200))) {
		return FALSE;
	}

	hSubMenu = psx->mMainWnd->PopupMenuCreate("&Emulation");
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_EMULATION_EXECUTE, "&Execute");
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_EMULATION_LOAD_SAVESTATE, "&Load Savestate");
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_EMULATION_CREATE_SAVESTATE, "&Create Savestate");
	psx->mMainWnd->AddMenuSeperator(hSubMenu);
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_EMULATION_RESET, "&Reset");
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_EMULATION_PAUSE, "&Pause");
	EnableMenuItem(psx->mMainWnd->GetHMenu(), IDM_EMULATION_PAUSE, MF_BYCOMMAND | MF_GRAYED);
	psx->mMainWnd->AddMenuSeperator(hSubMenu);
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_EMULATION_EXIT, "&Exit");
	hSubMenu = psx->mMainWnd->PopupMenuCreate("&Config");
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_CONFIG_CONFIGURE, "&Configure");
#if defined (_DEBUG)
	hSubMenu = psx->mMainWnd->PopupMenuCreate("&Debug");	
	psx->mMainWnd->AddMenuItem(hSubMenu, IDM_DEBUG_CPU, "&R3000A");
#endif
	psx->mMainWnd->MenuSet();

	/* create the video output window */
	if (!psx->mDispWnd->WindowCreate("", "ClassNameDisplayPSX", 1024, 512, 
			FALSE, FALSE, DisplayWndProc, RGB(0,0,0), 0, 0, 0)) {
		return FALSE;
	}

	return TRUE;
}

static void CreateThreads(Psx *psx) {
#if defined (_DEBUG)
	psx->mCpuDbg->mCpuDbgThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)
		DebuggerThreadEntryFunc,0,0, &psx->mCpuDbg->mCpuDbgThreadId);
#endif 

	/* everything happens in this thread, apart from window management and debugging */
	psx->mEmuThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)EmulationThreadEntryFunc,
		0, 0, &psx->mEmuThreadId);
}

int WINAPI WinMain( HINSTANCE hInstance, 
					HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine, 
					int nCmdShow)
{
	Psx *psx = Psx::GetInstance();

	CoInitialize(0);

	if (!CreateMainWindows(psx)) {
		return 1;
	}

	CreateThreads(psx);

	DWORD start = timeGetTime();
	static char title[256];

	while (!psx->quit) {
		if (psx->cpu->state == PSX_CPU_RUNNING) {
			if (timeGetTime()-start > 1000) {
				float speed = ((float)psx->cpu->mTotalCycles / Psx::CPU_CLOCK) * 100.0;
				sprintf_s(title, 256, "%.2f fps (%.2f%%)", (speed / 100.0) * 60.0, speed);
				SetWindowText(psx->mDispWnd->GetHwnd(), title);

				start = timeGetTime();
				psx->cpu->mTotalCycles = 0;
			}
		} else {
			SetWindowText(psx->mDispWnd->GetHwnd(), "Paused");
		}

		Window::ProcessMessages();
		Sleep(10);
	}

	return psx->mMainWnd->GetMsg().wParam;
}