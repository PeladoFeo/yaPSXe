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

#if defined (_DEBUG)
#include "CpuDebugger.h"
#endif

CPsx::CPsx() {
	hInst = GetModuleHandle(NULL);

	quit = FALSE;
	
	// init console first
	csl = new Console;

	mMainWnd = new CWindow;
	mDispWnd = new CWindow;
	mPCBreakpoints = new CBreakpoints;
	gl = new CGLRenderer;

#if defined (_DEBUG)
	mCpuDbg = new CpuDebugger;
#endif

	/* the order that these are allocated in matters. 
	   don't touch. */
	gpu = new CGpu;
	spu = new CSpu;
	cpu = new CpuState;
	mem = new CMemory;
	interpreter = new CInterpreter;
	/* ****************** */

	conf = new Config;
}

CPsx::~CPsx() {
	cpu->SetCpuState(PSX_CPU_HALTED);

	/* the order that these are deleted in matters. 
	   don't touch. */
	delete conf;
	delete mPCBreakpoints;
	delete cpu;
	delete spu;
	delete interpreter;
	delete mem;
	delete gl;
	delete gpu;
#if defined (_DEBUG)
	delete mCpuDbg;
#endif
	delete mMainWnd;
	delete mDispWnd;
	delete csl;
}

void CPsx::PauseEmulation(BOOL pause) {
	if (pause) {
		CheckMenuItem(mMainWnd->GetHMenu(), IDM_EMULATION_PAUSE, MF_CHECKED);
		cpu->SetCpuState(PSX_CPU_HALTED);
	} else {
		CheckMenuItem(mMainWnd->GetHMenu(), IDM_EMULATION_PAUSE, MF_UNCHECKED);
		cpu->SetCpuState(PSX_CPU_RUNNING);
	}
}

u32 CPsx::GetHwRevision() {
	return (mCurBios ? mCurBios->hwRevision : -1);
}

void CPsx::SignalQuit() {
	quit = TRUE;
	SendMessage(mMainWnd->GetHwnd(), WM_CLOSE, NULL, NULL);
}

void CPsx::StartEmulation() {
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

void CPsx::Execute() {
	if (!conf->bBiosLoaded) {
		MessageBox(mMainWnd->GetHwnd(), "BIOS not loaded", "Error", MB_ICONERROR);
		return;
	}
	StartEmulation();
}

void CPsx::StartSaveStateEmulation(const char *filename) {
	if (!RestoreSaveStateFile(filename)) {
		return;
	}
	StartEmulation();
}

void CPsx::ResetPsx() {
	cpu->Reset();
	gpu->InitGpu();
	spu->InitSpu();
	mem->ResetMem();
}

void CPsx::GetVramImage(u16 *image) {
	gpu->mVramImagePtr = image;
	gpu->mSaveVram = TRUE;
	cpu->SetCpuState(PSX_CPU_RUNNING);
	while (gpu->mSaveVram) Sleep(50);
	cpu->SetCpuState(PSX_CPU_HALTED);
}

void CPsx::SetVramImage(u16 *image) {
	gpu->mVramImagePtr = image;
	gpu->mRestoreVram = TRUE;
	cpu->SetCpuState(PSX_CPU_RUNNING);
	while (gpu->mRestoreVram) Sleep(50);
	cpu->SetCpuState(PSX_CPU_HALTED);
}

static const char const *gSaveStateID = "PSX-SAVE-STATE-FILE";

BOOL CPsx::CreateSaveStateFile(const char *filename) {
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
	fSaveState.write((char*)cpu, sizeof(CpuState));
	fSaveState.write((char*)mem, sizeof(CMemory));
	fSaveState.write((char*)gpu, sizeof(CGpu));
	fSaveState.write((char*)spu, sizeof(CSpu));
		
	/* and the vram image */
	u16 *vram = new u16 [CGpu::VRAM_WIDTH*CGpu::VRAM_HEIGHT*2];
	GetVramImage(vram);
	fSaveState.write((char*)vram, CGpu::VRAM_WIDTH*CGpu::VRAM_HEIGHT*2);
	delete [] vram;

	fSaveState.close();

	csl->out(CGREEN, "Savestate created:\n\tfilename '%s'\n\tcreated %s\n", filename, timebuf);

	return TRUE;
}

BOOL CPsx::RestoreSaveStateFile(const char *filename) {
	std::ifstream fSaveState;
	char timebuf[18];

	if (!filename) return FALSE;

	fSaveState.open(filename, std::ios::in | std::ios::binary);
	if (!fSaveState.is_open()) {
		MessageBox(NULL, "Failed to open file", "Error", MB_ICONERROR);
		fSaveState.close();
		return FALSE;
	}

	/* check the file ID */
	char *id = new char [strlen(gSaveStateID)+1];
	fSaveState.read((char*)id, strlen(gSaveStateID)+1);
	if (strcmp(id, gSaveStateID) != 0) {
		MessageBox(NULL, "Savestate file is not valid", "Error", MB_ICONERROR);
		fSaveState.close();
		return FALSE;
	}
	delete [] id;

	cpu->SetCpuState(PSX_CPU_HALTED);

	/* read the date and time */
	fSaveState.read((char*)timebuf, 18);

	/* now restore the emulation state */
	fSaveState.read((char*)cpu, sizeof(CpuState));
	fSaveState.read((char*)mem, sizeof(CMemory));
	fSaveState.read((char*)gpu, sizeof(CGpu));
	fSaveState.read((char*)spu, sizeof(CSpu));	

	/* re-init all the pointers we invalidated */
	cpu->SetClassPointers();
	mem->SetClassPointers();
	gpu->SetClassPointers();
	spu->SetClassPointers();

	/* and the vram image */
	u16 *vram = new u16 [CGpu::VRAM_WIDTH*CGpu::VRAM_HEIGHT*2];
	fSaveState.read((char*)vram, CGpu::VRAM_WIDTH*CGpu::VRAM_HEIGHT*2);
	SetVramImage(vram);
	delete [] vram;

	fSaveState.close();

	csl->out(CGREEN, "Savestate restored:\n\tfilename '%s'\n\tcreated %s\n", filename, timebuf);

	return TRUE;
}

void EmulationThreadEntryFunc() {
	CPsx *psx = CPsx::GetInstance();

	/* has to be called in this thread */
	if (!psx->gl->InitOpenGLWindow(psx->mDispWnd)) {
		MessageBox(NULL, "OpenGL failed to initialise\n", "Error", MB_ICONERROR);
		psx->SignalQuit();
		return;
	}

	psx->cpu->SetCpuState(PSX_CPU_HALTED);
	psx->ResetPsx();
	psx->interpreter->Execute();
}
