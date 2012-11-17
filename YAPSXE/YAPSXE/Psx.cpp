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
#include "Memory.h"
#include "Interpreter.h"
#include "Cpu.h"
#include "Gpu.h"
#include "Spu.h"
#include "Psx.h"
#include "Console.h"
#include "Windows.h"
#include "Breakpoints.h"
#include "OpenGL.h"
#include "Bios.h"
#include "Config.h"
#include "RootCounters.h"

#if defined (_DEBUG)
#include "CpuDebugger.h"
#endif

Psx::Psx() {
	hInst = GetModuleHandle(0);

	quit = FALSE;

	// init console first
	csl = new Console;

	mMainWnd = new Window;
	mDispWnd = new Window;
	mPPsxBreakpoints = new PsxBreakpoints;
	gl = new PsxGLRenderer;
#if defined (_DEBUG)
	mCpuDbg = new PsxCpuDebugger;
#endif
	gpu = new PsxGpu;
	spu = new PsxSpu;
	cpu = new PsxCpu;
	mem = new PsxMemory;
	interpreter = new PsxInterpreter;
	rcnt = new PsxCounters;

	/* InitClassPointers must be called, but only when 
	   all objects are allocated */
	gpu->InitClassPointers();
	spu->InitClassPointers();
	cpu->InitClassPointers();
	mem->InitClassPointers();
	interpreter->InitClassPointers();
	rcnt->InitClassPointers();

	// must be called after InitClassPointers
	conf = new PsxConfig;
}

Psx::~Psx() {
	// stop the emulation thread first
	SuspendThread(mEmuThread);

	if (conf) {
		delete conf; 
		conf = 0;
	}
	if (mPPsxBreakpoints) {
		delete mPPsxBreakpoints; 
		mPPsxBreakpoints = 0;
	}
	if (cpu) {
		delete cpu; 
		cpu = 0;
	}
	if (spu) {
		delete spu;
		spu = 0;
	}
	if (interpreter) {
		delete interpreter; 
		interpreter = 0;
	}
	if (mem) {
		delete mem; 
		mem = 0;
	}
	if (gl) {
		delete gl; 
		gl = 0;
	}
	if (gpu) {
		delete gpu; 
		gpu = 0;
	}
	if (rcnt) {
		delete rcnt;
		rcnt = 0;
	}
#if defined (_DEBUG)
	if (mCpuDbg) {
		delete mCpuDbg; 
		mCpuDbg = 0;
	}
#endif
	if (mMainWnd) {
		delete mMainWnd; 
		mMainWnd = 0;
	}
	if (mDispWnd) {
		delete mDispWnd; 
		mDispWnd = 0;
	}
	if (csl) {
		delete csl; 
		csl = 0;
	}
}

void Psx::PauseEmulation(BOOL pause) {
	if (pause) {
		CheckMenuItem(mMainWnd->GetHMenu(), IDM_EMULATION_PAUSE, MF_CHECKED);
		cpu->SetCpuState(PSX_CPU_HALTED);
	} else {
		CheckMenuItem(mMainWnd->GetHMenu(), IDM_EMULATION_PAUSE, MF_UNCHECKED);
		cpu->SetCpuState(PSX_CPU_RUNNING);
	}
}

u32 Psx::GetHwRevision() {
	return (mCurBios ? mCurBios->hwRevision : -1);
}

void Psx::SignalQuit() {
	quit = TRUE;
	SendMessage(mMainWnd->GetHwnd(), WM_CLOSE, 0, 0);
}

void Psx::StartEmulation() {
	/* enable and uncheck the pause menu item */
	EnableMenuItem(mMainWnd->GetHMenu(), IDM_EMULATION_PAUSE, MF_BYCOMMAND | MF_ENABLED);
	CheckMenuItem(mMainWnd->GetHMenu(), IDM_EMULATION_PAUSE, MF_UNCHECKED);

	/* show the video output window */
	mDispWnd->ShowWnd(TRUE);
	SetForegroundWindow(mDispWnd->GetHwnd());
	SetFocus(mDispWnd->GetHwnd());

#if defined (_DEBUG)
	if (mCpuDbg->mmMainWnd->IsOpen() && !mDispWnd->mFullscreen) {
		cpu->SetCpuState(PSX_CPU_STEPPING);
		mCpuDbg->UpdateDebugger();
	} else {
		cpu->SetCpuState(PSX_CPU_RUNNING);
	}
#else
	cpu->SetCpuState(PSX_CPU_RUNNING);
#endif
}

void Psx::Execute() {
	if (!conf->bBiosLoaded) {
		MessageBox(mMainWnd->GetHwnd(), "BIOS not loaded", "Error", MB_ICONERROR);
		return;
	}
	StartEmulation();
}

void Psx::StartSaveStateEmulation(const char *filename) {
	if (!RestoreSaveStateFile(filename)) {
		return;
	}
	StartEmulation();
}

void Psx::ResetPsx() {
	cpu->Reset();
	gpu->InitGpu();
	spu->InitSpu();
	mem->ResetMem();
}

void Psx::GetVramImage(u16 *image) {
	gpu->mVramImagePtr = image;
	gpu->mSaveVram = TRUE;
	while (gpu->mSaveVram) Sleep(50);
}

void Psx::SetVramImage(u16 *image) {
	gpu->mVramImagePtr = image;
	gpu->mRestoreVram = TRUE;
	while (gpu->mRestoreVram) Sleep(50);
}

void Psx::ClearVram() {
	static u16 mem[1024*512];
	memset(mem,0,1024*512*sizeof(u16));
	SetVramImage(mem);
}

static const char const *gSaveStateID = "PSX-SAVE-STATE-FILE";

BOOL Psx::CreateSaveStateFile(const char *filename) {
	std::ofstream fSaveState(filename, std::ios::out | std::ios::binary);

	if (!fSaveState.is_open()) {
		MessageBox(mMainWnd->GetHwnd(), "Failed to open file for writing\n", "Error", MB_ICONERROR);
		fSaveState.close();
		return FALSE;
	}

	char timebuf[18];
	time_t rawtime;
	struct tm *timeinfo;

	/* write the file id */
	fSaveState.write(gSaveStateID, strlen(gSaveStateID)+1);

	/* write the date and time */
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	sprintf_s(timebuf, 18, "%02d:%02d:%02d %02d/%02d/%02d", 
				timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
				timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year % 100);
	fSaveState.write(timebuf, 18);

	/* write the emulation state */
	fSaveState.write((char*)cpu, sizeof(PsxCpu));
	fSaveState.write((char*)mem, sizeof(PsxMemory));
	fSaveState.write((char*)gpu, sizeof(PsxGpu));
	fSaveState.write((char*)spu, sizeof(PsxSpu));
	fSaveState.write((char*)rcnt, sizeof(PsxCounters));

	/* and the vram image */
	u16 *vram = new u16 [PsxGpu::VRAM_WIDTH*PsxGpu::VRAM_HEIGHT*2];
	GetVramImage(vram);
	fSaveState.write((char*)vram, PsxGpu::VRAM_WIDTH*PsxGpu::VRAM_HEIGHT*2);
	delete [] vram;

	fSaveState.close();

	csl->out(CGREEN, "Savestate created:\n\tfilename '%s'\n\tcreated %s\n", filename, timebuf);

	return TRUE;
}

BOOL Psx::RestoreSaveStateFile(const char *filename) {
	std::ifstream fSaveState;
	char timebuf[18];

	if (!filename) return FALSE;

	fSaveState.open(filename, std::ios::in | std::ios::binary);
	if (!fSaveState.is_open()) {
		MessageBox(0, "Failed to open file", "Error", MB_ICONERROR);
		fSaveState.close();
		return FALSE;
	}

	/* check the file ID */
	char *id = new char [strlen(gSaveStateID)+1];
	fSaveState.read((char*)id, strlen(gSaveStateID)+1);
	if (strcmp(id, gSaveStateID) != 0) {
		MessageBox(0, "Savestate file is not valid", "Error", MB_ICONERROR);
		fSaveState.close();
		return FALSE;
	}
	delete [] id;

	cpu->SetCpuState(PSX_CPU_HALTED);

	/* read the date and time */
	fSaveState.read((char*)timebuf, 18);

	/* now restore the emulation state */
	fSaveState.read((char*)cpu, sizeof(PsxCpu));
	fSaveState.read((char*)mem, sizeof(PsxMemory));
	fSaveState.read((char*)gpu, sizeof(PsxGpu));
	fSaveState.read((char*)spu, sizeof(PsxSpu));	
	fSaveState.read((char*)rcnt, sizeof(PsxCounters));	

	/* re-init all the pointers we invalidated */
	cpu->InitClassPointers();
	mem->InitClassPointers();
	gpu->InitClassPointers();
	spu->InitClassPointers();
	rcnt->InitClassPointers();

	/* and the vram image */
	u16 *vram = new u16 [PsxGpu::VRAM_WIDTH*PsxGpu::VRAM_HEIGHT*2];
	fSaveState.read((char*)vram, PsxGpu::VRAM_WIDTH*PsxGpu::VRAM_HEIGHT*2);
	SetVramImage(vram);
	delete [] vram;

	fSaveState.close();

	csl->out(CGREEN, "Savestate restored:\n\tfilename '%s'\n\tcreated %s\n", filename, timebuf);

	return TRUE;
}

void EmulationThreadEntryFunc() {
	Psx *psx = Psx::GetInstance();

	/* has to be called in this thread */
	if (!psx->gl->InitOpenGLWindow(psx->mDispWnd)) {
		MessageBox(0, "OpenGL failed to initialise\n", "Error", MB_ICONERROR);
		psx->SignalQuit();
		return;
	}

	psx->cpu->SetCpuState(PSX_CPU_HALTED);
	psx->ResetPsx();
	psx->interpreter->Execute();
}
