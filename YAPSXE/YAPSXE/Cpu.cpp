#include "Common.h"
#include "Cpu.h"
#include "Psx.h"
#include "Console.h"
#include "Gpu.h"
#include "Spu.h"


CpuState::CpuState() {
	state = PSX_CPU_STEPPING;
	SetClassPointers();
	Reset();
}

void CpuState::SetClassPointers() {
	csl = CPsx::GetInstance()->csl;
	gpu = CPsx::GetInstance()->gpu;
	spu = CPsx::GetInstance()->spu;
}

void CpuState::Reset() {
	memset(this, 0, sizeof(CpuState));

	delay_type = PSX_NO_DELAY;

	CP0[CP0_STATUS] = 0x10900000;	// COP0 enabled | BEV = 1 | TS = 1
	CP0[CP0_PRID] = 0x00000002;		// PRevID = Revision ID, same as Cpu

	pc = 0xbfc00000;
}
