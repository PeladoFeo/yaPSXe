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
 
#include "Interpreter.h"
#include "Interrupts.h"
#include "Cpu.h"
#include "Memory.h"
#include "Console.h"
#include "Gpu.h"
#include "Common.h"
#include "Psx.h"
#include "CpuDebugger.h"
#include "Config.h"


void PsxCpu::Exception(u32 code) {
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

void PsxInterpreter::CheckInterrupts() {
	if (cpu->cycles >= VBL_START) {
		mem->mIREG |= VBL_INTR;
	}

	if (cpu->cycles >= VBL_END) {
		cpu->cycles = 0;

		gpu->UpdateScreen();
		
		/* limit to about 60fps if wanted */
		if (psx->conf->bLimitFps) {
			tFrameEnd = timeGetTime();

			while ((tFrameEnd-tFrameStart) <= 16)
				tFrameEnd = timeGetTime();

			tFrameStart = timeGetTime();
		}
	}

#if 0
	if (mem->mDmaDICR & 0x7f000000) { 
		mem->mIREG |= DMA_INTR;
	}
#endif

	if (mem->mIREG & mem->mIMASK) {
		if ((cpu->CP0[CP0_STATUS] & 0x401) == 0x401) {
			cpu->Exception(EXC_INT);
		}
	}
}