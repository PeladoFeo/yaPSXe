#include "Common.h"
#include "Interpreter.h"
#include "Cpu.h"
#include "Gte.h"
#include "Console.h"
#include "Memory.h"
#include "Windows.h"
#include "Breakpoints.h"
#include "Psx.h"
#include "Gpu.h"
#include "Spu.h"
#include "OpenGL.h"
#include "Interrupts.h"
#include "Bios.h"
#include "Console.h"
#include "CpuDebugger.h"

#if defined (_DEBUG)
#include "CpuDebugger.h"
#endif

#if 0
#define LOG_CPU_EXEC
#endif

#if 0
#define LOG_BIOS_CALLS
#endif


CInterpreter::CInterpreter() {
#if defined (LOG_CPU_EXEC)
	fout = fopen("../Logs/cpu-exec.txt", "wt+");
	bCpuTraceLog = TRUE;
#endif
	bLogBiosCalls = FALSE;
	bDoStep = FALSE;
	tFrameStart = tFrameEnd = 0;
	psx = CPsx::GetInstance();
	cpu = psx->cpu;
	mem = psx->mem;
	gpu = psx->gpu;
	csl = psx->csl;
}

CInterpreter::~CInterpreter() {
#if defined (LOG_CPU_EXEC)
	fclose(fout);
#endif
}

void CInterpreter::Execute() {
	CPsx *psx = CPsx::GetInstance();
	for (;;) {
#if defined(_DEBUG)
		switch (cpu->state) {
			case PSX_CPU_STEPPING: {
				if (bDoStep) {
					ExecuteInstruction();
					psx->mCpuDbg->UpdateDebugger();
					bDoStep = FALSE;
				}
				Sleep(10);
			} break;

			case PSX_CPU_RUNNING: {
				if (psx->mPCBreakpoints->CheckBreakpoint(cpu->pc)) {
					psx->csl->out(CRED, "Breakpoint detected @ 0x%08X\n", cpu->pc);
					cpu->SetCpuState(PSX_CPU_STEPPING);
					psx->mCpuDbg->OpenDebugger();
				} else {
					ExecuteInstruction();
				}
			} break;
				
			case PSX_CPU_HALTED:
				//Sleep(10);
				break;
		} 
#else if defined(_RELEASE)
		if (cpu->state == PSX_CPU_RUNNING) {
			ExecuteInstruction();
		} else {
			//Sleep(10);
		}
#endif
		CheckInterrupts();
		cpu->cycles += 1;
		cpu->mTotalCycles += 1;
	}
}

void CInterpreter::LogBiosCall() {
	int operation = cpu->GPR[9] & 0xff;
	int address = cpu->pc & 0x1fffff;

	if (address == 0xa0 || address == 0xb0 || address == 0xc0) {
		for (int i = 0; bioscalls[i].prototype; i++) {
			if (bioscalls[i].address == address && 
				bioscalls[i].operation == operation) {
				csl->out(CGREEN, "BIOS call: %s\n", bioscalls[i].prototype);
				break;
			}
		}
	}
}

void CInterpreter::ExecuteInstruction() {
#if defined (LOG_BIOS_CALLS)
	LogBiosCall();
#endif

	cpu->PsxOp.full = mem->Read32(cpu->pc);

#if defined (LOG_CPU_EXEC)
	if (bCpuTraceLog) {
		fprintf(fout, "%08x: %s\n", cpu->pc, CpuDebugger::DasmOne(cpu->PsxOp, cpu->pc));
		fflush(fout);
	}
#endif

	cpu->GPR[0] = 0;
	psxInstruction[cpu->PsxOp.op]();

	switch (cpu->delay_type) {
		case PSX_NO_DELAY: 
			cpu->pc += 4;
			break;

		case PSX_DELAY_SLOT: 
			cpu->pc += 4;
			cpu->delay_type = PSX_DELAY_JUMP;
			break;

		case PSX_DELAY_JUMP: 
			cpu->pc = cpu->delay_pc;
			cpu->delay_type = PSX_NO_DELAY;
			break;
	}
}