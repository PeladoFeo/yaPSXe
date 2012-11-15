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
#include "Cpu.h"
#include "Psx.h"
#include "Console.h"
#include "Gpu.h"
#include "Spu.h"


PsxCpu::PsxCpu() {
	state = PSX_CPU_STEPPING;
	Reset();
}

void PsxCpu::InitClassPointers() {
	csl = Psx::GetInstance()->csl;
	gpu = Psx::GetInstance()->gpu;
	spu = Psx::GetInstance()->spu;
}

void PsxCpu::Reset() {
	memset(this, 0, sizeof(PsxCpu));

	delay_type = PSX_NO_DELAY;

	CP0[CP0_STATUS] = 0x10900000;	// COP0 enabled | BEV = 1 | TS = 1
	CP0[CP0_PRID] = 0x00000002;		// PRevID = Revision ID, same as Cpu

	pc = 0xbfc00000;
}
