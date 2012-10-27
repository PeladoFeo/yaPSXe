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
#ifndef CPU_DEBUGGER_H
#define CPU_DEBUGGER_H

#include "Common.h"

#if defined (_DEBUG)

#define DASM_CHANGE_STEP	0
#define DASM_CHANGE_SCROLL	1

enum {
	IDM_ACTION_RUN,
	IDM_ACTION_BREAK,
	IDM_ACTION_STEP,
	IDM_DBG_CTX_SETPC,
	IDM_DBG_CTX_CHECK_BP,
	IDM_DBG_CTX_REM_ALL_BP,
};

void DebuggerThreadEntryFunc();

class CpuDebugger {
public:
	CpuDebugger();
	~CpuDebugger();

	void OpenDebugger();
	void UpdateDebugger();
	void HandleDasmRightClick(int x, int y);
	void StepDebugger();
	void DrawDasmWindow();

	u8 DebugCpuReadByte(u32 addr);

	HANDLE mCpuDbgThread;
	DWORD mCpuDbgThreadId;

	CPsx *psx;

	CWindow *mmMainWnd;
	CWindow *mChildDasm;
	CWindow *mChildRegs;
	CWindow *mChildMem;

	u32 mMemoryViewAddr;

	HFONT mDasmFont;
	HWND hRegTabCtrl;
	int mRegTabCurSel;

	u32 mDasmStartAddr;
	int mLastDasmChange;

	int mDasmRclickXpos;
	int mDasmRclickYpos;

	static char *GetGprRegName(int index) {
		static const char *sGprRegNames[] = {
			"r0", "at", "v0", "v1", "a0", "a1","a2", "a3",
			"t0", "t1", "t2", "t3", "t4", "t5","t6", "t7",
			"s0", "s1", "s2", "s3", "s4", "s5","s6", "s7",
			"t8", "t9", "k0", "k1", "gp", "sp","fp", "ra"
		};
		return (char*)sGprRegNames[index];
	}

	static char *GetCp0RegName(int index) {
		static const char *sCp0RegNames[] = {
		"Index"     , "Random"    , "EntryLo0", "EntryLo1", "Context" , "PageMask"  , "Wired"     , "*Check me*",
		"BadVAddr"  , "Count"     , "EntryHi" , "Compare" , "Status"  , "Cause"     , "ExceptPC"  , "PRevID"    ,
		"Config"    , "LLAddr"    , "WatchLo" , "WatchHi" , "XContext", "*RES*"     , "*RES*"     , "*RES*"     ,
		"*RES*"     , "*RES* "    , "PErr"    , "CacheErr", "TagLo"   , "TagHi"     , "ErrorEPC"  , "*RES*"     
		};
		return (char*)sCp0RegNames[index];
	}

	static char *DasmOne(PsxOpcode code, u32 pc);
};

#endif

#endif /* CPU_DEBUGGER_H */