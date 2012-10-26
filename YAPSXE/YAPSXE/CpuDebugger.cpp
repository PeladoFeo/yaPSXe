#include "Windows.h"
#include "Psx.h"
#include "Common.h"
#include "Breakpoints.h"
#include "Cpu.h"
#include "Interpreter.h"
#include "Memory.h"
#include "Console.h"

#if defined (_DEBUG)

#include "CpuDebugger.h"


CpuDebugger::CpuDebugger() {
	psx = CPsx::GetInstance();

	mmMainWnd = new CWindow;
	mChildDasm = new CWindow;
	mChildRegs = new CWindow;
	mChildMem = new CWindow;

	mDasmStartAddr = 0;
	mMemoryViewAddr = /*0xbfc00000*/0x1000;
	mRegTabCurSel = 0;

	mDasmFont = CreateFont( 16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
							ANSI_CHARSET,OUT_DEVICE_PRECIS,CLIP_MASK,
							ANTIALIASED_QUALITY, DEFAULT_PITCH,
							"Courier New");
}

CpuDebugger::~CpuDebugger() {
	delete mmMainWnd;
	delete mChildDasm;
	delete mChildRegs;
	delete mChildMem;
}

void CpuDebugger::HandleDasmRightClick(int x, int y) {
	POINT ptCursorPos;
	GetCursorPos(&ptCursorPos);

	mDasmRclickXpos = x;
	mDasmRclickYpos = y; 

	HMENU hPopupMenu = CreatePopupMenu();
	AppendMenu(hPopupMenu, MF_STRING, IDM_DBG_CTX_SETPC, "Set PC");
	AppendMenu(hPopupMenu, MF_STRING, IDM_DBG_CTX_CHECK_BP, 
		(!psx->mPCBreakpoints->CheckBreakpoint(mDasmStartAddr + ((y/14)*4)) ? 
		"Add breakpoint" : "Remove breakpoint"));
	if (psx->mPCBreakpoints->vBreakpoints.size() > 0)
		AppendMenu(hPopupMenu, MF_STRING, IDM_DBG_CTX_REM_ALL_BP, "Remove all breakpoints");

	TrackPopupMenu(hPopupMenu, 0, ptCursorPos.x, ptCursorPos.y, 0, mChildDasm->GetHwnd(), NULL);
	DestroyMenu(hPopupMenu);

	UpdateDebugger();
}

void CpuDebugger::OpenDebugger() {
	mmMainWnd->ShowWnd(TRUE);
	mDasmStartAddr = psx->cpu->pc;
	psx->cpu->SetCpuState(PSX_CPU_STEPPING);
	UpdateDebugger();
}

void CpuDebugger::UpdateDebugger() {
	UpdateWindow(mmMainWnd->GetHwnd());
	InvalidateRect(mmMainWnd->GetHwnd(), NULL, TRUE);

	UpdateWindow(mChildDasm->GetHwnd());
	InvalidateRect(mChildDasm->GetHwnd(), NULL, TRUE);

	UpdateWindow(mChildRegs->GetHwnd());
	InvalidateRect(mChildRegs->GetHwnd(), NULL, TRUE);

	UpdateWindow(mChildMem->GetHwnd());
	InvalidateRect(mChildMem->GetHwnd(), NULL, TRUE);
}

void CpuDebugger::StepDebugger() {
	psx->interpreter->bDoStep = TRUE;
	while (!psx->interpreter->bDoStep) Sleep(10);
}

void SizeWindow(HWND hwnd, int width, int height) {
	SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT CpuWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static CPsx *psx = CPsx::GetInstance();

    switch(msg) {
		case WM_CREATE: {
		} break;

		case WM_KEYDOWN: {
			if (wParam == VK_F5) {
				psx->cpu->SetCpuState(PSX_CPU_RUNNING);
			} else if (wParam == VK_F6) {
				psx->cpu->SetCpuState(PSX_CPU_STEPPING);
				psx->mCpuDbg->UpdateDebugger();
			} else if (wParam == VK_F7) {
				psx->mCpuDbg->StepDebugger();
				psx->mCpuDbg->mLastDasmChange = DASM_CHANGE_STEP;
			} else if (wParam == VK_DOWN) {
				psx->mCpuDbg->mLastDasmChange = DASM_CHANGE_SCROLL;
				psx->mCpuDbg->mDasmStartAddr += 4;
				psx->mCpuDbg->UpdateDebugger();
			} else if (wParam == VK_UP) {
				psx->mCpuDbg->mLastDasmChange = DASM_CHANGE_SCROLL;
				psx->mCpuDbg->mDasmStartAddr -= 4;
				psx->mCpuDbg->UpdateDebugger();
			}
			break;
		} break;

		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDM_ACTION_RUN:
					psx->cpu->SetCpuState(PSX_CPU_RUNNING);
					break;

				case IDM_ACTION_BREAK:
					psx->cpu->SetCpuState(PSX_CPU_STEPPING);
					psx->mCpuDbg->UpdateDebugger();
					break;

				case IDM_ACTION_STEP:
					psx->mCpuDbg->StepDebugger();
					psx->mCpuDbg->mLastDasmChange = DASM_CHANGE_STEP;
					break;
			}
		} break;

		case WM_SIZE:
			psx->mCpuDbg->mmMainWnd->width = LOWORD(lParam);
			psx->mCpuDbg->mmMainWnd->height = HIWORD(lParam);
			break;

        case WM_CLOSE:
			psx->mCpuDbg->mmMainWnd->ShowWnd(FALSE);

			if (psx->mDispWnd->IsOpen()) {
				psx->cpu->SetCpuState(PSX_CPU_RUNNING);
			}
			break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void CpuDebugger::DrawDasmWindow() {
	static CPsx *psx = CPsx::GetInstance();

	mChildDasm->hDC = BeginPaint(mChildDasm->GetHwnd(), &mChildDasm->ps);

	static char str[256];
	static PsxOpcode op;
	static int charHeight = 14;
	HDC hDC = mChildDasm->hDC;
	int width = mChildDasm->width;
	int height = mChildDasm->height;
	u32 pc;

	if (mLastDasmChange == DASM_CHANGE_STEP) {
		/* change the starting address to disassemble from if necessary */
		if (psx->cpu->pc < mDasmStartAddr || psx->cpu->pc > ( mDasmStartAddr + ((height/charHeight-1)*4) )) {
			mDasmStartAddr = psx->cpu->pc;
		}
	}

	pc = mDasmStartAddr;

	SelectObject(hDC, mDasmFont);

	for (int i = 0; i < height/charHeight+1; i++, pc += 4) {
		op.full = psx->mem->Read32(pc);
		sprintf_s(str, 256, "%08x %s", pc, CpuDebugger::DasmOne(op,pc));

		if (pc == psx->cpu->pc) {
			RECT TextRect = { 0, (i*charHeight), width, (i*charHeight)+charHeight+2 };
			SetTextColor(hDC, RGB(0,0,0));
			SetBkColor(hDC, RGB(200,200,250));
			ExtTextOut(hDC, 4, i*charHeight, ETO_OPAQUE, &TextRect, str, strlen(str), NULL);
		} else {
			if (psx->mPCBreakpoints->CheckBreakpoint(mDasmStartAddr+(i*4))) {
				RECT TextRect = { 0, (i*charHeight), 71, (i*charHeight)+charHeight+2 };
				SetBkColor(hDC, RGB(255,0,0));
				ExtTextOut(hDC, 4, i*charHeight, ETO_OPAQUE, &TextRect, "", 0, NULL);
			}
			SetTextColor(hDC, RGB(0,0,0));
			SetBkMode(hDC, TRANSPARENT);
			TextOut(mChildDasm->hDC, 4, i*charHeight, str, strlen(str));
		}
	}

	HPEN grey=CreatePen(PS_SOLID, 1, RGB(150,150,150));
	POINT pntArray[2];
	pntArray[0].x=71; pntArray[0].y=0; pntArray[1].x=71; pntArray[1].y=height;
	HGDIOBJ oldPen = SelectObject(hDC, grey);
	Polyline(hDC, pntArray, 2);

	EndPaint(mChildDasm->GetHwnd(), &mChildDasm->ps);
}

LRESULT CpuDasmWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static CPsx *psx = CPsx::GetInstance();

    switch(msg) {
		case WM_CREATE:
			break;

		case WM_KEYDOWN:
			if (wParam == VK_DOWN) {
				psx->mCpuDbg->mLastDasmChange = DASM_CHANGE_SCROLL;
				psx->mCpuDbg->mDasmStartAddr += 4;
			} else if (wParam == VK_UP) {
				psx->mCpuDbg->mLastDasmChange = DASM_CHANGE_SCROLL;
				psx->mCpuDbg->mDasmStartAddr -= 4;
			}
			psx->mCpuDbg->UpdateDebugger();
			break;

		case WM_RBUTTONUP: 
			psx->mCpuDbg->HandleDasmRightClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		case WM_PAINT: {
			if (CPsx::GetInstance()->cpu->state != PSX_CPU_RUNNING) {
				psx->mCpuDbg->DrawDasmWindow();
			}
		} break;

        case WM_CLOSE:
			psx->mCpuDbg->mChildDasm->ShowWnd(FALSE);
			break;

		case WM_SIZE:
			psx->mCpuDbg->mChildDasm->width = LOWORD(lParam);
			psx->mCpuDbg->mChildDasm->height = HIWORD(lParam);

	//		RECT rect;
	//		GetWindowRect(hWnd, &rect);
	//		CPsx::GetInstance()->csl->out("size: %d %d\n", rect.right-rect.left, rect.bottom-rect.top);
			break;

		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDM_DBG_CTX_SETPC:
					psx->cpu->pc = (psx->mCpuDbg->mDasmStartAddr + ((psx->mCpuDbg->mDasmRclickYpos/14)*4));
					psx->cpu->delay_type = PSX_NO_DELAY;
					psx->mCpuDbg->UpdateDebugger();
					break;

				case IDM_DBG_CTX_CHECK_BP: {
					u32 addr = (psx->mCpuDbg->mDasmStartAddr + ((psx->mCpuDbg->mDasmRclickYpos/14)*4));

					if (!psx->mPCBreakpoints->CheckBreakpoint(addr)) {
						psx->mPCBreakpoints->AddBreakpoint(addr);
					} else {
						psx->mPCBreakpoints->RemoveBreakpoint(addr);
					}

					psx->mCpuDbg->UpdateDebugger();
				} break;

				case IDM_DBG_CTX_REM_ALL_BP: {
					psx->mPCBreakpoints->vBreakpoints.clear();
					psx->mCpuDbg->UpdateDebugger();
				} break;
			}
		} break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

static void AppendTabItem(HWND hTab, char *text) {
	static TCITEM tie; 
	int count;
	if (lstrlen(text) !=0 ) {
		tie.mask = TCIF_TEXT;
		tie.pszText = text;
		count = SendMessage(hTab, TCM_GETITEMCOUNT, 0, 0);
		SendMessage(hTab, TCM_INSERTITEM, count, (LPARAM) (LPTCITEM) &tie);
	}
}

LRESULT CpuRegWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static CpuDebugger *dbg = CPsx::GetInstance()->mCpuDbg;

    switch(msg) {
		case WM_CREATE:
			SetWindowPos(hWnd, NULL, 309, 430, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			dbg->hRegTabCtrl = CreateWindow( WC_TABCONTROL, "", 
										WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
										0, 0, 
										dbg->mChildRegs->width, 
										dbg->mChildRegs->height, 
										hWnd, NULL, CPsx::GetInstance()->hInst, NULL);
			AppendTabItem(dbg->hRegTabCtrl, "CPU");
			AppendTabItem(dbg->hRegTabCtrl, "COP0");
			SendMessage(dbg->hRegTabCtrl, WM_SETFONT, WPARAM (dbg->mDasmFont), TRUE);
			break;

		case WM_SIZE:
			dbg->mChildRegs->width = LOWORD(lParam);
			dbg->mChildRegs->height = HIWORD(lParam);
			SizeWindow(dbg->hRegTabCtrl, 
				dbg->mChildRegs->width+2, 
				dbg->mChildRegs->height+1);
			break;

		case WM_PAINT: {
			if (CPsx::GetInstance()->cpu->state != PSX_CPU_RUNNING) {
				HDC hTabDC = GetDC(dbg->hRegTabCtrl);

				static int charHeight = 14;
				static char str[256];

				SelectObject(hTabDC, dbg->mDasmFont);

				switch (dbg->mRegTabCurSel) {
					// CPU tab
					case 0: {
						int x = 10;
						for (int i = 0, j = 0; i < 32; i++, j++) {
							if (i == 16) {
								j = 0;
								x = 120;
							}
							sprintf_s(str, 256, "%s: %08x", CpuDebugger::GetGprRegName(i), CPsx::GetInstance()->cpu->GPR[i]);
							TextOut(hTabDC, x, j*charHeight+30, str, strlen(str));
						}
					} break;
				}

				// always release the dc or the windows end up unresponsive
				ReleaseDC(hWnd,hTabDC);
			}
		} break;

		case WM_NOTIFY: {
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;

			if (lpnmhdr->code == TCN_SELCHANGE) {
				dbg->mRegTabCurSel = TabCtrl_GetCurSel(dbg->hRegTabCtrl);
				InvalidateRect(dbg->hRegTabCtrl, NULL, FALSE);
				UpdateWindow(dbg->hRegTabCtrl);
			} return 0;
		} break;

        case WM_CLOSE:
			dbg->mChildRegs->ShowWnd(FALSE);
			break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

u8 CpuDebugger::DebugCpuReadByte(u32 addr) {
	if (addr < 0x00200000) {
		return psx->mem->RAM[addr];
	} else if (addr >= 0x1f800000 && addr <= 0x1f8003ff) {
		return 0;	/* ***** TODO ***** */
	} else if (addr >= 0x1f801000 && addr <= 0x1f802fff) {
		return psx->mem->HwRead8(addr);
	} else if (addr >= 0x80000000 && addr <= 0x801fffff) {
		return psx->mem->RAM[addr & 0x1fffffff];
	} else if (addr >= 0xa0000000 && addr <= 0xa01fffff) {
		return psx->mem->RAM[addr & 0x1fffffff];
	} else if (addr >= 0xbfc00000 && addr <= 0xbfc7ffff) {
		return psx->mem->BIOS[addr & 0x7ffff];
	} else {
		return 0;
	}
}

LRESULT CpuMemoryWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static CpuDebugger *dbg = CPsx::GetInstance()->mCpuDbg;

    switch(msg) {
		case WM_CREATE:
			SetWindowPos(hWnd, NULL, 309, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			break;

		case WM_SIZE:
			dbg->mChildMem->width = LOWORD(lParam);
			dbg->mChildMem->height = HIWORD(lParam);
			break;

		case WM_PAINT: {
			if (CPsx::GetInstance()->cpu->state != PSX_CPU_RUNNING) {
				dbg->mChildMem->hDC = BeginPaint(hWnd, &dbg->mChildMem->ps);

				static char str[256];
				static char subStr[256];
				static int charHeight = 14;
				int height = dbg->mChildMem->height;
				int width = dbg->mChildMem->width;
				HPEN grey = CreatePen(PS_SOLID, 1, RGB(150,150,150));
				POINT verticalLine[] = { {71, 0}, {71, height} };

				SelectObject(dbg->mChildMem->hDC, dbg->mDasmFont);

				u32 pc = dbg->mMemoryViewAddr;

				memset(str, 0, 256);
				memset(subStr, 0, 256);

				for (int i = 0; i < height/charHeight+1; i++) {
					sprintf_s(str, 256, "%08x ", pc);

					int lineAddr = pc;

					/* hex part */
					for (int j = 0; j < 4; j++, pc += 4) {
						sprintf_s(subStr, 256, "%02x%02x%02x%02x ", 
							dbg->DebugCpuReadByte(pc),
							dbg->DebugCpuReadByte(pc+1),
							dbg->DebugCpuReadByte(pc+2),
							dbg->DebugCpuReadByte(pc+3));
						strcat_s(str, 256, subStr);
					}

					/* ascii part */
					for (int j = 0; j < 16; j++, lineAddr++) {
						u8 byte = dbg->DebugCpuReadByte(lineAddr);
						if (byte == 0) byte = '.';
						sprintf_s(subStr, 256, "%c", byte);
						strcat_s(str, 256, subStr);
					}

					TextOut(dbg->mChildMem->hDC, 4, i*charHeight, str, strlen(str));
				}

				SelectObject(dbg->mChildMem->hDC, grey);
				Polyline(dbg->mChildMem->hDC, verticalLine, 2);

				EndPaint(hWnd, &dbg->mChildMem->ps);
			}
		} break;

        case WM_CLOSE:
			dbg->mChildMem->ShowWnd(FALSE);
			break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void DebuggerThreadEntryFunc() {
	CpuDebugger *dbg = CPsx::GetInstance()->mCpuDbg;

	if (!dbg->mmMainWnd->WindowCreate("Debugger", "ClassNamemCpuDbg", 860, 818, 
			FALSE,FALSE, CpuWndProc, RGB(171,171,171))) {
		CPsx::GetInstance()->SignalQuit();
		return;
	}
	HMENU hSubMenu = dbg->mmMainWnd->PopupMenuCreate("&Action");
	dbg->mmMainWnd->AddMenuItem(hSubMenu, IDM_ACTION_RUN, "&Run\tF5");
	dbg->mmMainWnd->AddMenuItem(hSubMenu, IDM_ACTION_BREAK, "&Break\tF6");
	dbg->mmMainWnd->AddMenuItem(hSubMenu, IDM_ACTION_STEP, "&Step\tF7");
	dbg->mmMainWnd->MenuSet();

	if (!dbg->mChildDasm->WindowCreate("Dasm", "ClassNamemCpuDbgDasm", 
			310, 760, FALSE, TRUE, CpuDasmWndProc, RGB(255,255,255), TRUE, 
			dbg->mmMainWnd->GetHwnd())) {
		CPsx::GetInstance()->SignalQuit();
		return;
	}

	if (!dbg->mChildRegs->WindowCreate("Registers", "ClassNamemCpuDbgRegs", 
			242, 330, FALSE, TRUE, CpuRegWndProc, RGB(255,255,255), TRUE, 
			dbg->mmMainWnd->GetHwnd(), WS_CLIPCHILDREN)) {
		CPsx::GetInstance()->SignalQuit();
		return;
	}

	if (!dbg->mChildRegs->WindowCreate("Memory", "ClassNamemCpuDbgMemory", 
			537, 431, FALSE, TRUE, CpuMemoryWndProc, RGB(255,255,255), TRUE, 
			dbg->mmMainWnd->GetHwnd())) {
		CPsx::GetInstance()->SignalQuit();
		return;
	}

	for (;;) {
		CWindow::ProcessMessages();
		Sleep(10);
	}
}

#endif