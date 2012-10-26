#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include "Common.h"

class CMemory {
public:
	CMemory();
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

	void SetClassPointers();

	u32 mIREG;
	u32 mIMASK;

	u32 mDmaMADR[7];
	u32 mDmaBCR[7];
	u32 mDmaCHCR[7];
	u32 mDmaDPCR;
	u32 mDmaDICR;

	u8 RAM[0x200000];
	u8 BIOS[0x80000];

private:
	Console *csl;
	CpuState *cpu;
	CGpu *gpu;
	CSpu *spu;
};

#endif /* MEMORY_H */