#pragma once
#ifndef SPU_H
#define SPU_H

#include "Common.h"
#include "Psx.h"

class CSpu {
public:
	CSpu();
	void InitSpu();

	void SetClassPointers();

	void WriteReg16(u32 addr, u16 val);
	u16 ReadReg16(u32 addr);

	// 512kb
	u16 mSpuMem[256*1024];

	u32 mSpuTransferAddr;
	u32 mSpuStatus;

private:
	Console *csl;
};

#endif /* SPU_H */