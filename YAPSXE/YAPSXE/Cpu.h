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
#ifndef CPU_H
#define CPU_H

#include "Common.h"

enum { 
	PSX_NO_DELAY = 0,
	PSX_DELAY_SLOT,
	PSX_DELAY_JUMP
};

enum {
	PSX_CPU_STEPPING = 0,
	PSX_CPU_RUNNING,
	PSX_CPU_HALTED
};

#define Cause	(*(CauseReg*)&CP0[CP0_CAUSE])
#define Status	(*(StatusReg*)&CP0[CP0_STATUS])

/* Cause register exception codes */
#define EXC_INT		0
#define EXC_MOD		1
#define EXC_TLBL	2
#define EXC_TLBS	3
#define EXC_ADEL	4
#define EXC_ADES	5
#define EXC_IBE		6
#define EXC_DBE		7
#define EXC_SYS		8
#define EXC_BP		9
#define EXC_RI		10
#define EXC_CPU		11
#define EXC_OVF		12

/* COP0 registers */
#define CP0_INDEX           0
#define CP0_RANDOM          1
#define CP0_ENTRYLO0        2
#define CP0_ENTRYLO1        3
#define CP0_CONTEXT         4
#define CP0_PAGEMASK        5
#define CP0_WIRED           6
#define CP0_BADADDRESS      8
#define CP0_COUNT           9
#define CP0_ENTRYHI         10
#define CP0_COMPARE         11
#define CP0_STATUS          12
#define CP0_CAUSE           13
#define CP0_EPC             14
#define CP0_PRID            15
#define CP0_CONFIG          16
#define CP0_LLADDR          17
#define CP0_WATCHLO         18
#define CP0_WATCHHI         19
#define CP0_XCONTEXT        20
#define CP0_ECC             26
#define CP0_CACHEERR        27
#define CP0_TAGLO           28
#define CP0_TAGHI           29
#define CP0_ERROREPC        30

union PsxOpcode {
	u32 full;

	// I-Type (Immediate)
	// | 31 ... 26 | 25 ... 21 | 20 ... 16 | 15 ... 0  |
	// |     op    |    rs     |    rt     | immediate |
	struct {
		unsigned imm : 16;
		unsigned rt : 5;
		unsigned rs : 5;
		unsigned op : 6;
	};

	// J-Type (Jump)
	// | 31 ... 26 | 25 ... 0 |
	// |     op    |  target  |
	struct {
		unsigned target : 26;
		unsigned : 6;
	};

	// R-Type (Register)
	// | 31 ... 26 | 25 ... 21 | 20 ... 16 | 15 ... 11 | 10 ... 6 | 5 ... 0 | 
	// |     op    |    rs     |    rt     |     rd    |   shamt  |  funct  |
	struct {
		unsigned funct : 6;
		unsigned shamt : 5;
		unsigned rd : 5;
		unsigned : 5;
		unsigned : 5;
		unsigned : 6;
	};
};

union CauseReg {
	u32 full;
	struct {
		unsigned : 2;
		unsigned exc_code : 5;
		unsigned : 1;
		unsigned sw : 2;
		unsigned ip : 6;
		unsigned : 12;
		unsigned ce : 2;
		unsigned : 1;
		unsigned bd : 1;
	};
};

union StatusReg {
	u32 full;
	struct {
		unsigned ie_c : 1;
		unsigned ku_c : 1;
		unsigned ie_p : 1;
		unsigned ku_p : 1;
		unsigned ie_o : 1;
		unsigned ku_o : 1;
		unsigned : 2;
		unsigned int_mask : 8;
		unsigned is_c : 1;
		unsigned sw_c : 1;
		unsigned pz : 1;
		unsigned cm : 1;
		unsigned pe : 1;
		unsigned ts : 1;
		unsigned bev : 1;
		unsigned : 2;
		unsigned re : 1;
		unsigned : 2;
		unsigned cu : 4;
	};
};

class CpuState {
public:
	CpuState();

	u32 GPR[32];
	u32 CP0[32];
	u32 pc;
	u32 hi,lo;

	u32 delay_pc;
	int delay_type;
	PsxOpcode PsxOp;
	int state;

	void SetCpuState(int state) { 
		this->state = state; 
	}
	void Reset();
	void Exception(u32 code);

	u32 cycles;
	u64 mTotalCycles;

	void SetClassPointers();

private:
	Console *csl;
	CGpu *gpu;
	CSpu *spu;
};

#endif /* CPU_H */