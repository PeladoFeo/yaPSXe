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
 
#ifndef PSX_H
#define PSX_H

#include "Common.h"

#define HSYNC   	(Psx::CPU_CLOCK/15734)
#define VBL_START	(HSYNC*240)
#define VBL_END		(HSYNC*262)

void EmulationThreadEntryFunc(); 

class Psx {
public:
	Psx();
	~Psx();

	void Execute();
	void StartSaveStateEmulation(const char *filename);
	void StartEmulation();
	void PauseEmulation(BOOL pause);

	u32 GetHwRevision();
	void ResetPsx();

	/* for savestates */
	BOOL CreateSaveStateFile(const char *path);
	BOOL RestoreSaveStateFile(const char *path);
	void GetVramImage(u16 *image);
	void SetVramImage(u16 *image);
	void ClearVram();

	PsxMemory *mem;
	PsxCpu *cpu;
	PsxGpu *gpu;
	PsxSpu *spu;
	Console *csl;
	PsxInterpreter *interpreter;
	PsxGLRenderer *gl;
	PsxCounters *rcnt;

	Window *mMainWnd;
	Window *mDispWnd;
	
	const BiosInfo *mCurBios;
	PsxBreakpoints *mPPsxBreakpoints;
	PsxConfig *conf;


	HINSTANCE hInst;
	BOOL quit;
	HANDLE mEmuThread;
	DWORD mEmuThreadId;

	// 33.8688 MHz
	static const u32 CPU_CLOCK = 33868800;

#if defined (_DEBUG)
	PsxCpuDebugger *mCpuDbg;
#endif

	void SignalQuit();

	// singleton
	static Psx *GetInstance() {
		static Psx instance;
		return &instance;
	}
};

#endif /* PSX_H */