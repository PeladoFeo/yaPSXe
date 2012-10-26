#include "Interpreter.h"
#include "Interrupts.h"
#include "Cpu.h"
#include "Memory.h"
#include "Console.h"
#include "Gpu.h"
#include "Common.h"
#include "Psx.h"
#include "CpuDebugger.h"


void CpuState::Exception(u32 code) {
	Cause.exc_code = code;

	if (delay_type == PSX_DELAY_SLOT) {
		csl->out(CRED, "BD set!\n");
		Cause.bd = 1;
		CP0[CP0_EPC] = pc - 4;
	} else {
		Cause.bd = 0;
		CP0[CP0_EPC] = pc;
	}

	delay_type = PSX_DELAY_JUMP;

	if (Status.bev) {
		delay_pc = 0xbfc00180;
	} else {
		delay_pc = 0x80000080;
	}

	CP0[CP0_STATUS] = (CP0[CP0_STATUS] & ~0x3f) | 
					  ((CP0[CP0_STATUS] & 0xf) << 2);
}

void CInterpreter::CheckInterrupts() {
	if (cpu->cycles >= (HSYNC*240)) {
		mem->mIREG |= VBL_INTR;
	}

	if (cpu->cycles >= (HSYNC*262)) {
		//cpu->mTotalCycles += cpu->cycles;
		cpu->cycles = 0;
		gpu->UpdateScreen();
		mem->mIREG &= ~VBL_INTR;	// clear vbl?
	}

	//if (mem->mDmaDICR & 0x7f000000) { 
	//	mem->mIREG |= DMA_INTR;
	//}

	if (mem->mIREG & mem->mIMASK) {
		if ((cpu->CP0[CP0_STATUS] & 0x401) == 0x401) {
			cpu->Exception(EXC_INT);
		}
	}
}