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


//#define LOG_CPU_EXEC
//#define LOG_BIOS_CALLS
#define LOG_PSX_BIOS_OUTPUT


PsxInterpreter::PsxInterpreter() {
#if defined (LOG_CPU_EXEC)
	fout = fopen("../Logs/cpu-exec.txt", "wt+");
	bCpuTraceLog = TRUE;
#endif
	bLogBiosCalls = FALSE;
	bDoStep = FALSE;
	tFrameStart = tFrameEnd = 0;
}

PsxInterpreter::~PsxInterpreter() {
#if defined (LOG_CPU_EXEC)
	fclose(fout);
#endif
}

void PsxInterpreter::InitClassPointers() {
	psx = Psx::GetInstance();
	cpu = psx->cpu;
	mem = psx->mem;
	gpu = psx->gpu;
	csl = psx->csl;
}

void PsxInterpreter::Execute() {
	Psx *psx = Psx::GetInstance();
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
				if (psx->mPPsxBreakpoints->CheckBreakpoint(cpu->pc)) {
					psx->csl->out(CRED, "Breakpoint detected @ 0x%08X\n", cpu->pc);
					cpu->SetPsxCpu(PSX_CPU_STEPPING);
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
		}
#endif
		/* keep ticking regardless of the cpu state */
		CheckInterrupts();
		cpu->cycles += 1;
		cpu->mTotalCycles += 1;
	}
}

void PsxInterpreter::LogBiosCall() {
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

void PsxInterpreter::ExecuteInstruction() {
#if defined (LOG_PSX_BIOS_OUTPUT)
	// catch calls to putchar, called by printf
	int addr = cpu->pc & 0x1fffff, op = cpu->GPR[9] & 0xff;
	if (addr == 0xb0 && op == 0x3d) {
		psx->csl->out(CCYAN, "%c", psx->cpu->GPR[4] & 0xff);
	}
#endif

#if defined (LOG_BIOS_CALLS)
	LogBiosCall();
#endif

	cpu->PsxOp.full = mem->Read32(cpu->pc);

#if defined (LOG_CPU_EXEC)
	if (bCpuTraceLog) {
		fprintf(fout, "%08x: %s\n", cpu->pc, PsxCpuDebugger::DasmOne(cpu->PsxOp, cpu->pc));
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