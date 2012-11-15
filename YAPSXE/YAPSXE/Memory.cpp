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
 
#include "Memory.h"
#include "Common.h"
#include "Console.h"
#include "Cpu.h"
#include "Windows.h"
#include "Spu.h"
#include "Gpu.h"
#include "Psx.h"

#if defined (_DEBUG)
#include "CpuDebugger.h"
#endif


/* 
	0x0000_0000 - 0x0000_ffff	Kernel (64K)
	0x0001_0000 - 0x001f_ffff	User Memory (1.9 Meg I-cache off)
	0x1f00_0000 - 0x1f00_ffff	Parallel Port (64K)
	0x1f80_0000 - 0x1f80_03ff	Scratch Pad (on chip D-cache) (1024 bytes)
	0x1f80_1000 - 0x1f80_2fff	Hardware Registers (8K)
	0x8000_0000 - 0x801f_ffff	Kernel and User Memory Mirror (2 Meg I-cache on)
	0xa000_0000 - 0xa01f_ffff	Kernel and User Memory Mirror (2 Meg I-cache off)
	0xbfc0_0000 - 0xbfc7_ffff	BIOS (512K)
	
	However, a “TLB”, which is the virtual memory management device for the R3000 CPU, is not mounted.
	Therefore, the relationship between the physical address and the logic address of the memory space is
	always fixed.
*/

PsxMemory::PsxMemory() {
	ResetMem();
}

void PsxMemory::InitClassPointers() {
	rcnt = CPsx::GetInstance()->rcnt;
	csl = CPsx::GetInstance()->csl;
	cpu = CPsx::GetInstance()->cpu;
	gpu = CPsx::GetInstance()->gpu;
	spu = CPsx::GetInstance()->spu;
}

void PsxMemory::ResetMem() {
	memset(mDmaMADR, 0, sizeof(int)*7); 
	memset(mDmaBCR, 0, sizeof(int)*7); 
	memset(mDmaCHCR, 0, sizeof(int)*7); 
	mDmaDPCR = 0;
	mDmaDICR = 0;
	mIREG = 0;
	mIMASK = 0;
	memset(RAM, 0, 0x200000); 
}

void PsxMemory::Write8(u32 addr, u8 data) {
	if (cpu->CP0[CP0_STATUS] & 0x10000) {
		return;
	}

	if (addr < 0x00200000) {
		RAM[addr] = data;
	} else if (addr >= 0x1f800000 && addr <= 0x1f8003ff) {
		// Scratch pad
		csl->out(CWHITE, "Scratch pad write byte\n");
	} else if (addr >= 0x1f801000 && addr <= 0x1f802fff) {
		HwWrite8(addr, data);
	} else if (addr >= 0x80000000 && addr <= 0x801fffff) {
		RAM[addr & 0x1fffffff] = data;
	} else if (addr >= 0xa0000000 && addr <= 0xa01fffff) {
		RAM[addr & 0x1fffffff] = data;
	} else {
		//csl->out(CWHITE, "Unknown memory in Write8: 0x%08x\n", addr);
	}
}

void PsxMemory::Write16(u32 addr, u16 data) {
	if (cpu->CP0[CP0_STATUS] & 0x10000) {
		return;
	}

	if (addr < 0x00200000) {
		*(u16*)&RAM[addr] = data;
	} else if (addr >= 0x1f800000 && addr <= 0x1f8003ff) {
		// Scratch pad
		csl->out(CWHITE, "Scratch pad write half word\n");
	} else if (addr >= 0x1f801000 && addr <= 0x1f802fff) {
		HwWrite16(addr, data);
	} else if (addr >= 0x80000000 && addr <= 0x801fffff) {
		*(u16*)&RAM[addr & 0x1fffffff] = data;
	} else if (addr >= 0xa0000000 && addr <= 0xa01fffff) {
		*(u16*)&RAM[addr & 0x1fffffff] = data;
	} else {
		//csl->out(CWHITE, "Unknown memory in Write16: 0x%08x\n", addr);
	}
}

void PsxMemory::Write32(u32 addr, u32 data) {
	if (cpu->CP0[CP0_STATUS] & 0x10000) {
		return;
	}

	if (addr < 0x00200000) {
		*(u32*)&RAM[addr] = data;
	} else if (addr >= 0x1f800000 && addr <= 0x1f8003ff) {
		// Scratch pad
		csl->out(CWHITE, "Scratch pad write word\n");
	} else if (addr >= 0x1f801000 && addr <= 0x1f802fff) {
		HwWrite32(addr, data);
	} else if (addr >= 0x80000000 && addr <= 0x801fffff) {
		*(u32*)&RAM[addr & 0x1fffffff] = data;
	} else if (addr >= 0xa0000000 && addr <= 0xa01fffff) {
		*(u32*)&RAM[addr & 0x1fffffff] = data;
	} else {
		//csl->out(CWHITE, "Unknown memory in Write32: 0x%08x\n", addr);
	}
}

u8 PsxMemory::Read8(u32 addr) {
	if (addr < 0x00200000) {
		return RAM[addr];
	} else if (addr >= 0x1f800000 && addr <= 0x1f8003ff) {
		// Scratch pad
		csl->out(CWHITE, "Scratch pad read byte\n");
	} else if (addr >= 0x1f801000 && addr <= 0x1f802fff) {
		return HwRead8(addr);
	} else if (addr >= 0x80000000 && addr <= 0x801fffff) {
		return RAM[addr & 0x1fffffff];
	} else if (addr >= 0xa0000000 && addr <= 0xa01fffff) {
		return RAM[addr & 0x1fffffff];
	} else if (addr >= 0xbfc00000 && addr <= 0xbfc7ffff) {
		return BIOS[addr & 0x7ffff];
	} else {
		//csl->out(CWHITE, "Unknown memory in Read8: 0x%08x\n", addr);
		return 0;
	}
}

u16 PsxMemory::Read16(u32 addr) {
	if (addr < 0x00200000) {
		return *(u16*)&RAM[addr];
	} else if (addr >= 0x1f800000 && addr <= 0x1f8003ff) {
		// Scratch pad
		csl->out(CWHITE, "Scratch pad read half word\n");
	} else if (addr >= 0x1f801000 && addr <= 0x1f802fff) {
		return HwRead16(addr);
	} else if (addr >= 0x80000000 && addr <= 0x801fffff) {
		return *(u16*)&RAM[addr & 0x1fffffff];
	} else if (addr >= 0xa0000000 && addr <= 0xa01fffff) {
		return *(u16*)&RAM[addr & 0x1fffffff];
	} else if (addr >= 0xbfc00000 && addr <= 0xbfc7ffff) {
		return *(u16*)&BIOS[addr & 0x7ffff];
	} else {
		//csl->out(CWHITE, "Unknown memory in Read16: 0x%08x\n", addr);
		return 0;
	}
}

u32 PsxMemory::Read32(u32 addr) {
	if (addr < 0x00200000) {
		return *(u32*)&RAM[addr];
	} else if (addr >= 0x1f800000 && addr <= 0x1f8003ff) {
		// Scratch pad
		csl->out(CWHITE, "Scratch pad read word\n");
	} else if (addr >= 0x1f801000 && addr <= 0x1f802fff) {
		return HwRead32(addr);
	} else if (addr >= 0x80000000 && addr <= 0x801fffff) {
		return *(u32*)&RAM[addr & 0x1fffffff];
	} else if (addr >= 0xa0000000 && addr <= 0xa01fffff) {
		return *(u32*)&RAM[addr & 0x1fffffff];
	} else if (addr >= 0xbfc00000 && addr <= 0xbfc7ffff) {
		return *(u32*)&BIOS[addr & 0x7ffff];
	} else {
		//csl->out(CWHITE, "Unknown memory in Read32: 0x%08x\n", addr);
		//CPsx::GetInstance()->cpu->SetPsxCpu(PSX_CPU_STEPPING);
		//OpenDebugger();
		return 0;
	}
}



