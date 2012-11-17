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
 
#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include "Common.h"

class PsxMemory {
public:
	PsxMemory();
	void ResetMem();

	void Write8(u32 addr, u8 data);
	void Write16(u32 addr, u16 data);
	void Write32(u32 addr, u32 data);

	void HwWrite8(u32 addr, u8 data);
	void HwWrite16(u32 addr, u16 data);
	void HwWrite32(u32 addr, u32 data);

	u8 Read8(u32 addr);
	u16 Read16(u32 addr);
	u32 Read32(u32 addr);

	u8 HwRead8(u32 addr);
	u16 HwRead16(u32 addr);
	u32 HwRead32(u32 addr);

	void DmaExecute(int n);
	void Dma2Chain(u32 madr);

	BOOL LoadBiosRom(std::string path);

	void InitClassPointers();

	u32 mIREG;
	u32 mIMASK;

	u32 mDmaMADR[7];
	u32 mDmaBCR[7];
	u32 mDmaCHCR[7];
	u32 mDmaDPCR;
	u32 mDmaDICR;

	u8 ram[0x200000];
	u8 bios[0x80000];
	u8 dcache[1024];

private:
	Console *csl;
	PsxCpu *cpu;
	PsxGpu *gpu;
	PsxSpu *spu;
	PsxCounters *rcnt;
};

#endif /* MEMORY_H */