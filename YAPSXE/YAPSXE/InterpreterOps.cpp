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
#include "Interpreter.h"
#include "Cpu.h"
#include "Gte.h"
#include "Console.h"
#include "Memory.h"
#include "Psx.h"

#if defined (_DEBUG)
#include "CpuDebugger.h"
#endif

#define RD		cpu->GPR[cpu->PsxOp.rd]
#define RS		cpu->GPR[cpu->PsxOp.rs]
#define RT		cpu->GPR[cpu->PsxOp.rt]
#define IMM		cpu->PsxOp.imm

/* necessary for static interpreter functions */
PsxCpu *PsxInterpreter::cpu;
PsxMemory *PsxInterpreter::mem;

PsxInterpreter::PsxCpuInstruction PsxInterpreter::psxInstruction[64] = {
	psxSPECIAL, psxREGIMM, psxJ   , psxJAL  , psxBEQ , psxBNE , psxBLEZ, psxBGTZ,
	psxADDI   , psxADDIU , psxSLTI, psxSLTIU, psxANDI, psxORI , psxXORI, psxLUI ,
	psxCOP0   , psxNULL  , psxCOP2, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL   , psxNULL  , psxNULL, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxLB     , psxLH    , psxLWL , psxLW   , psxLBU , psxLHU , psxLWR , psxNULL,
	psxSB     , psxSH    , psxSWL , psxSW   , psxNULL, psxNULL, psxSWR , psxNULL, 
	psxNULL   , psxNULL  , gteLWC2, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL   , psxNULL  , gteSWC2, psxHLE  , psxNULL, psxNULL, psxNULL, psxNULL 
};

PsxInterpreter::PsxCpuInstruction PsxInterpreter::psxSPC[64] = {
	psxSLL , psxNULL , psxSRL , psxSRA , psxSLLV   , psxNULL , psxSRLV, psxSRAV,
	psxJR  , psxJALR , psxNULL, psxNULL, psxSYSCALL, psxBREAK, psxNULL, psxNULL,
	psxMFHI, psxMTHI , psxMFLO, psxMTLO, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxMULT, psxMULTU, psxDIV , psxDIVU, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxADD , psxADDU , psxSUB , psxSUBU, psxAND    , psxOR   , psxXOR , psxNOR ,
	psxNULL, psxNULL , psxSLT , psxSLTU, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxNULL, psxNULL , psxNULL, psxNULL, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxNULL, psxNULL , psxNULL, psxNULL, psxNULL   , psxNULL , psxNULL, psxNULL
};

PsxInterpreter::PsxCpuInstruction PsxInterpreter::psxREG[32] = {
	psxBLTZ  , psxBGEZ  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL  , psxNULL  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxBLTZAL, psxBGEZAL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL  , psxNULL  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};

PsxInterpreter::PsxCpuInstruction PsxInterpreter::psxCP0[32] = {
	psxMFC0, psxNULL, psxCFC0, psxNULL, psxMTC0, psxNULL, psxCTC0, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxRFE , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};

PsxInterpreter::PsxCpuInstruction PsxInterpreter::psxCP2[64] = {
	psxBASIC, gteRTPS , psxNULL , psxNULL, psxNULL, psxNULL , gteNCLIP, psxNULL,
	psxNULL , psxNULL , psxNULL , psxNULL, gteOP  , psxNULL , psxNULL , psxNULL,
	gteDPCS , gteINTPL, gteMVMVA, gteNCDS, gteCDP , psxNULL , gteNCDT , psxNULL,
	psxNULL , psxNULL , psxNULL , gteNCCS, gteCC  , psxNULL , gteNCS  , psxNULL,
	gteNCT  , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL,
	gteSQR  , gteDCPL , gteDPCT , psxNULL, psxNULL, gteAVSZ3, gteAVSZ4, psxNULL, 
	gteRTPT , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL,
	psxNULL , psxNULL , psxNULL , psxNULL, psxNULL, gteGPF  , gteGPL  , gteNCCT
};

PsxInterpreter::PsxCpuInstruction PsxInterpreter::psxCP2BSC[32] = {
	gteMFC2, psxNULL, gteCFC2, psxNULL, gteMTC2, psxNULL, gteCTC2, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};


void PsxInterpreter::unimplemented() {
#if defined (_DEBUG)
	char *pstr = CpuDebugger::DasmOne(cpu->PsxOp,cpu->pc);
	int i = 0; 
	while (pstr[i] != ' ' && i < strlen(pstr)) i++; 
	pstr[i] = '\0';
	CPsx::GetInstance()->csl->out(CRED, "Unimplemented instruction '%s'\n", pstr);
	cpu->SetPsxCpu(PSX_CPU_STEPPING);
	CPsx::GetInstance()->mCpuDbg->OpenDebugger();
#endif
}

void PsxInterpreter::psxNULL() {
	CPsx::GetInstance()->csl->out(CRED, "Unknown instruction @ 0x%08x\n", cpu->pc);
}

void PsxInterpreter::psxSPECIAL() {
	psxSPC[cpu->PsxOp.funct]();
}

void PsxInterpreter::psxREGIMM() {
	psxREG[cpu->PsxOp.rt]();
}

void PsxInterpreter::psxCOP0() {
	psxCP0[cpu->PsxOp.rs]();
}

void PsxInterpreter::psxCOP2() {
	psxCP2[cpu->PsxOp.funct]();
}

void PsxInterpreter::psxBASIC() {
	psxCP2BSC[cpu->PsxOp.rs]();
}

void PsxInterpreter::psxJ() {
	cpu->delay_type = PSX_DELAY_SLOT;
	cpu->delay_pc  = (cpu->pc & 0xf0000000) + (cpu->PsxOp.target << 2);
}

void PsxInterpreter::psxJAL() {
	cpu->delay_type = PSX_DELAY_SLOT;
	cpu->delay_pc  = (cpu->pc & 0xf0000000) + (cpu->PsxOp.target << 2);
	cpu->GPR[31] = cpu->pc + 8;
}

void PsxInterpreter::psxBEQ() {
	cpu->delay_type = PSX_DELAY_SLOT;
	if (RS == RT) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

void PsxInterpreter::psxBNE() {
	cpu->delay_type = PSX_DELAY_SLOT;
	if (RS != RT) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

void PsxInterpreter::psxBLEZ() {
	cpu->delay_type = PSX_DELAY_SLOT;
	if ((s32)RS <= 0) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

void PsxInterpreter::psxBGTZ() {
	cpu->delay_type = PSX_DELAY_SLOT;
	if ((s32)RS > 0) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

// exception on overflow
void PsxInterpreter::psxADDI() {
	RT = (s32)((s32)RS + (s32)(s16)IMM);
}

void PsxInterpreter::psxADDIU() {
	RT = (s32)((s32)RS + (s32)(s16)IMM);
}

void PsxInterpreter::psxSLTI() {
	RT = ((s32)RS < (s32)(s16)IMM);
}

void PsxInterpreter::psxSLTIU() {
	RT = (RS < (u32)IMM);
}

void PsxInterpreter::psxANDI() {
	RT = (RS & (u32)IMM);
}

void PsxInterpreter::psxORI() {
	RT = (RS | (u32)IMM);
}

void PsxInterpreter::psxXORI() {
	RT = (RS ^ (u32)IMM);
}

void PsxInterpreter::psxLUI() {
	RT = (u32)IMM << 16;
}

void PsxInterpreter::psxLB() {
	RT = (s32)(s8)mem->Read8(RS + (s16)IMM);
}

void PsxInterpreter::psxLH() {
	RT = (s32)(s16)mem->Read16(RS + (s16)IMM);
}

// address error exception
void PsxInterpreter::psxLW() {
	RT = mem->Read32(RS + (s16)IMM);
}

void PsxInterpreter::psxLBU() {
	RT = (u32)mem->Read8(RS + (s16)IMM);
}

void PsxInterpreter::psxLHU() {
	RT = (u32)mem->Read16(RS + (s16)IMM);
}

void PsxInterpreter::psxLWL() {
	u32 address = RS + (s16)IMM;
	u32 val = mem->Read32(address & ~0x3);

	u32 reg = RT;

	switch (address % 4) {
        case 0: reg = val; break;
        case 1: reg = ((reg & 0x000000ff) | (val << 8));  break;
        case 2: reg = ((reg & 0x0000ffff) | (val << 16)); break;
        case 3: reg = ((reg & 0x00ffffff) | (val << 24)); break;
    }

	RT = reg;
}

void PsxInterpreter::psxLWR() {
	u32 address = RS + (s16)IMM;
	u32 val = mem->Read32(address & ~0x3);

	u32 reg = RT;

	switch (address % 4) {
        case 0: reg = (reg & 0xffffff00) | (val >> 24); break;
        case 1: reg = (reg & 0xffff0000) | (val >> 16); break;
        case 2: reg = (reg & 0xff000000) | (val >>  8); break;
        case 3: reg = val; break;
    }

	RT = reg;
}

void PsxInterpreter::psxSB() {
	mem->Write8(RS + (s16)IMM, (u8)RT);
}

void PsxInterpreter::psxSH() {
	mem->Write16(RS + (s16)IMM, (u16)RT);
}

void PsxInterpreter::psxSW() {
	mem->Write32(RS + (s16)IMM, RT);
}

void PsxInterpreter::psxSWL() {
	u32 newval;
	u32 address = RS + (s16)IMM;
	u32 treg = RT;
	u32 word = mem->Read32(address & ~0x3);

	switch (address % 4) {
		case 0:	newval = treg; break;
		case 1:	newval = (word & 0xff000000) | (treg >> 8 ); break;
		case 2:	newval = (word & 0xffff0000) | (treg >> 16); break;
		case 3:	newval = (word & 0xffffff00) | (treg >> 24); break;
	}

	mem->Write32(address & ~0x3, newval);
}

void PsxInterpreter::psxSWR() {
	u32 newval;
	u32 address = RS + (s16)IMM;
	u32 treg = RT;
	u32 word = mem->Read32(address & ~0x3);

	switch (address % 4) {
		case 0:	newval = (word & 0x00ffffff) | (treg << 24); break;
		case 1:	newval = (word & 0x0000ffff) | (treg << 16); break;
		case 2:	newval = (word & 0x000000ff) | (treg << 8); break;
		case 3:	newval = treg; break;
	}

	mem->Write32(address & ~0x3, newval);
}

void PsxInterpreter::psxHLE() {
	PsxInterpreter::unimplemented();
}

void PsxInterpreter::psxSLL() {
	RD = (s32)(RT << cpu->PsxOp.shamt);
}

void PsxInterpreter::psxSRL() {
	RD = (s32)(RT >> cpu->PsxOp.shamt);
}

void PsxInterpreter::psxSRA() {
	RD = (s32)((s32)RT >> cpu->PsxOp.shamt);
}

void PsxInterpreter::psxSLLV() {
	RD = (s32)((RT << (RS & 0x1f)) & 0xffffffff);
}

void PsxInterpreter::psxSRLV() {
	RD = (s32)(RT >> (RS & 0x1f));
}

void PsxInterpreter::psxSRAV() {
	RD = (s32)((s32)RT >> (RS & 0x1f));
}

void PsxInterpreter::psxJR() {
	cpu->delay_type = PSX_DELAY_SLOT;
	cpu->delay_pc = RS;
	//if (cpu->delay_pc & 0x3) { /* Address Error exception! */ }
}

void PsxInterpreter::psxJALR() {
	cpu->delay_type = PSX_DELAY_SLOT;
	RD = cpu->pc + 8;
	cpu->delay_pc = RS;
	//if (cpu->delay_pc & 0x3) { /* Address Error exception! */ }
}

void PsxInterpreter::psxSYSCALL() {
	cpu->Exception(EXC_SYS);
}

void PsxInterpreter::psxBREAK() {
	MessageBox(0, "psxBREAK\n", "0", 0);
}

void PsxInterpreter::psxMFHI() {
	RD = cpu->hi;
}

void PsxInterpreter::psxMTHI() {
	cpu->hi = RS;
}

void PsxInterpreter::psxMFLO() {
	RD = cpu->lo;
}

void PsxInterpreter::psxMTLO() {
	cpu->lo = RS;
}

void PsxInterpreter::psxMULT() {
	u64 res = (s64)((s64)(s32)RS * (s64)(s32)RT);
	cpu->lo = (u32)(res & 0xffffffff);
	cpu->hi = (u32)((res >> 32) & 0xffffffff);
}

void PsxInterpreter::psxMULTU() {
	u64 res = (u64)((u64)RS * (u64)RT);
	cpu->lo = (u32)(res & 0xffffffff);
	cpu->hi = (u32)((res >> 32) & 0xffffffff);
}

void PsxInterpreter::psxDIV() {
	if ((s32)RT != 0) {
		cpu->lo = (s32)((s32)RS / (s32)RT);
		cpu->hi = (s32)((s32)RS % (s32)RT);
	}
}

void PsxInterpreter::psxDIVU() {
	if (RT != 0) {
		cpu->lo = RS / RT;
		cpu->hi = RS % RT;
	}
}

// Overflow exception
void PsxInterpreter::psxADD() {
	RD = RS + RT;
}

void PsxInterpreter::psxADDU() {
	RD = RS + RT;
}

// Overflow exception
void PsxInterpreter::psxSUB() {
	RD = RS - RT;
}

void PsxInterpreter::psxSUBU() {
	RD = RS - RT;
}

void PsxInterpreter::psxAND() {
	RD = RS & RT;
}

void PsxInterpreter::psxOR() {
	RD = RS | RT;
}

void PsxInterpreter::psxXOR() {
	RD = RS ^ RT;
}

void PsxInterpreter::psxNOR() {
	RD = ~(RS | RT);
}

void PsxInterpreter::psxSLT() {
	RD = (s32)RS < (s32)RT;
}

void PsxInterpreter::psxSLTU() {
	RD = RS < RT;
}

void PsxInterpreter::psxBLTZ() {
	cpu->delay_type = PSX_DELAY_SLOT;
	if ((s32)RS < 0) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

void PsxInterpreter::psxBGEZ() {
	cpu->delay_type = PSX_DELAY_SLOT;
	if ((s32)RS >= 0) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

void PsxInterpreter::psxBLTZAL() {
	cpu->delay_type = PSX_DELAY_SLOT;
	cpu->GPR[31] = cpu->pc + 8;
	if ((s32)RS < 0) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

void PsxInterpreter::psxBGEZAL() {
	cpu->delay_type = PSX_DELAY_SLOT;
	cpu->GPR[31] = cpu->pc + 8;
	if ((s32)RS >= 0) {
		cpu->delay_pc = cpu->pc + ((s16)(IMM) << 2) + 4;
	} else {
		cpu->delay_pc = cpu->pc + 8;
	}
}

void PsxInterpreter::psxMFC0() {
	RT = cpu->CP0[cpu->PsxOp.rd];
}

void PsxInterpreter::psxCFC0() {
	RT = cpu->CP0[cpu->PsxOp.rd];
}

void PsxInterpreter::MTC0(u32 reg, u32 val) {
	switch (reg) {
		case 12: // Status
			cpu->CP0[CP0_STATUS] = val;
			//psxTestSWInts();
			break;
			
		case 13: // Cause
			cpu->CP0[CP0_CAUSE] = val & ~(0xfc00);
			//psxTestSWInts();
			break;
			
		default:
			cpu->CP0[cpu->PsxOp.rd] = val;
			break;
	}
}

void PsxInterpreter::psxMTC0() {
	MTC0(cpu->PsxOp.rd, RT);
}

// does this exist in the psx?
void PsxInterpreter::psxCTC0() {
	MTC0(cpu->PsxOp.rd, RT);
}

void PsxInterpreter::psxRFE() {
	cpu->CP0[CP0_STATUS] = (cpu->CP0[CP0_STATUS] & 0xfffffff0) |
						  ((cpu->CP0[CP0_STATUS] & 0x3c) >> 2);
}
