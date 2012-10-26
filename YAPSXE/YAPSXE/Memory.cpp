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
	0x0001_0000 - 0x001f_ffff	User Memory (1.9 Meg)
	0x1f00_0000 - 0x1f00_ffff	Parallel Port (64K)
	0x1f80_0000 - 0x1f80_03ff	Scratch Pad (1024 bytes)
	0x1f80_1000 - 0x1f80_2fff	Hardware Registers (8K)
	0x8000_0000 - 0x801f_ffff	Kernel and User Memory Mirror (2 Meg Cached)
	0xa000_0000 - 0xa01f_ffff	Kernel and User Memory Mirror (2 Meg Uncached)
	0xbfc0_0000 - 0xbfc7_ffff	BIOS (512K)
*/

CMemory::CMemory() {
	SetClassPointers();
	ResetMem();
}

void CMemory::SetClassPointers() {
	csl = CPsx::GetInstance()->csl;
	cpu = CPsx::GetInstance()->cpu;
	gpu = CPsx::GetInstance()->gpu;
	spu = CPsx::GetInstance()->spu;
}

void CMemory::ResetMem() {
	memset(mDmaMADR, 0, sizeof(int)*7); 
	memset(mDmaBCR, 0, sizeof(int)*7); 
	memset(mDmaCHCR, 0, sizeof(int)*7); 
	mDmaDPCR = 0;
	mDmaDICR = 0;
	mIREG = 0;
	mIMASK = 0;
	memset(RAM, 0, 0x200000); 
}

void CMemory::Write8(u32 addr, u8 data) {
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

void CMemory::Write16(u32 addr, u16 data) {
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

void CMemory::Write32(u32 addr, u32 data) {
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

u8 CMemory::Read8(u32 addr) {
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

u16 CMemory::Read16(u32 addr) {
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

u32 CMemory::Read32(u32 addr) {
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
		//CPsx::GetInstance()->cpu->SetCpuState(PSX_CPU_STEPPING);
		//OpenDebugger();
		return 0;
	}
}



