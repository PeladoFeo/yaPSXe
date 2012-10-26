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