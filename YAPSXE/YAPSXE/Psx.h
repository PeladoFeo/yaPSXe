#ifndef PSX_H
#define PSX_H

#include "Common.h"

#define HSYNC   (CPsx::CPU_CLOCK/15734)
#define VSYNC   (CPsx::CPU_CLOCK/60)

void EmulationThreadEntryFunc(); 

class CPsx {
public:
	CPsx();
	~CPsx();

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

	CMemory *mem;
	CpuState *cpu;
	CGpu *gpu;
	CSpu *spu;
	Console *csl;
	CInterpreter *interpreter;
	CGLRenderer *gl;
	CWindow *mMainWnd;
	CWindow *mDispWnd;
	const BiosInfo *mCurBios;
	CBreakpoints *mPCBreakpoints;
	Config *conf;


	HINSTANCE hInst;
	BOOL quit;
	HANDLE mEmuThread;
	DWORD mEmuThreadId;

	// 33.8688 MHz
	static const u32 CPU_CLOCK = 33868800;

#if defined (_DEBUG)
	CpuDebugger *mCpuDbg;
#endif

	void SignalQuit();

	// singleton
	static CPsx *GetInstance() {
		static CPsx instance;
		return &instance;
	}
};

#endif /* PSX_H */