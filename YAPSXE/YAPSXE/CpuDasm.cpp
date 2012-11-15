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
#include "CpuDebugger.h"

#if defined (_DEBUG)

/* Variadic macro to shorten sprintf_s calls */
#define SPF(...) sprintf_s(str, 256, __VA_ARGS__)

#define BOFFSET (pc + ((s16)(code.imm) << 2) + 4)

#define RS		PsxCpuDebugger::GetGprRegName(code.rs)
#define BASE	PsxCpuDebugger::GetGprRegName(code.rs)
#define RT		PsxCpuDebugger::GetGprRegName(code.rt)
#define RD		PsxCpuDebugger::GetGprRegName(code.rd)

static void dasmPsxSPECIAL(char *str, PsxOpcode code, u32 pc);
static void dasmPsxREGIMM(char *str, PsxOpcode code, u32 pc);
static void dasmPsxCOP0(char *str, PsxOpcode code, u32 pc);
static void dasmPsxCOP2(char *str, PsxOpcode code, u32 pc);
static void dasmPsxBASIC(char *str, PsxOpcode code, u32 pc);

static char *HexToSignedStr16(s16 hex) {
	static char str[256];
	if (hex < 0) {
		u32 ret = ~hex + 1;
		SPF("-0x%04x",ret);
	} else {
		SPF("0x%04x",hex);
	}
	return (char*)str;
}

static void dasmPsx0(char *str, PsxOpcode code, u32 pc) {
	SPF("???");
}

static void dasmPsxJ(char *str, PsxOpcode code, u32 pc) {
	SPF("J 0x%08x", (pc & 0xf0000000) + (code.target << 2));
}

static void dasmPsxJAL(char *str, PsxOpcode code, u32 pc) {
	SPF("JAL 0x%08x", (pc & 0xf0000000) + (code.target << 2));
}

static void dasmPsxBEQ(char *str, PsxOpcode code, u32 pc) {
	SPF("BEQ %s,%s,0x%08x", RS,RT,BOFFSET);
}

static void dasmPsxBNE(char *str, PsxOpcode code, u32 pc) {
	SPF("BNE %s,%s,0x%08x", RS,RT,BOFFSET);
}

static void dasmPsxBLEZ(char *str, PsxOpcode code, u32 pc) {
	SPF("BLEZ %s,%s,0x%08x", RS,RT,BOFFSET);
}

static void dasmPsxBGTZ(char *str, PsxOpcode code, u32 pc) {
	SPF("BGTZ %s,%s,0x%08x", RS,RT,BOFFSET);
}

static void dasmPsxADDI(char *str, PsxOpcode code, u32 pc) {
	SPF("ADDI %s,%s,%s", RT,RS,HexToSignedStr16(code.imm));
}

static void dasmPsxADDIU(char *str, PsxOpcode code, u32 pc) {
	SPF("ADDIU %s,%s,%s", RT,RS,HexToSignedStr16(code.imm));
}

static void dasmPsxSLTI(char *str, PsxOpcode code, u32 pc) {
	SPF("SLTI %s,%s,%s", RT,RS,HexToSignedStr16(code.imm));
}

static void dasmPsxSLTIU(char *str, PsxOpcode code, u32 pc) {
	SPF("SLTIU %s,%s,0x%04X", RT,RS,code.imm);
}

static void dasmPsxANDI(char *str, PsxOpcode code, u32 pc) {
	SPF("ANDI %s,%s,0x%04X", RT,RS,code.imm);
}

static void dasmPsxORI(char *str, PsxOpcode code, u32 pc) {
	SPF("ORI %s,%s,0x%04X", RT,RS,code.imm);
}

static void dasmPsxXORI(char *str, PsxOpcode code, u32 pc) {
	SPF("XORII %s,%s,0x%04X", RT,RS,code.imm);
}

static void dasmPsxLUI(char *str, PsxOpcode code, u32 pc) {
	SPF("LUI %s,0x%04X", RT,code.imm);
}

static void dasmPsxLB(char *str, PsxOpcode code, u32 pc) {
	SPF("LB %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxLH(char *str, PsxOpcode code, u32 pc) {
	SPF("LH %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxLWL(char *str, PsxOpcode code, u32 pc) {
	SPF("LWL %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxLW(char *str, PsxOpcode code, u32 pc) {
	SPF("LW %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxLBU(char *str, PsxOpcode code, u32 pc) {
	SPF("LBU %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxLHU(char *str, PsxOpcode code, u32 pc) {
	SPF("LHU %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxLWR(char *str, PsxOpcode code, u32 pc) {
	SPF("LWR %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxSB(char *str, PsxOpcode code, u32 pc) {
	SPF("SB %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxSH(char *str, PsxOpcode code, u32 pc) {
	SPF("SH %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxSWL(char *str, PsxOpcode code, u32 pc) {
	SPF("SWL %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxSW(char *str, PsxOpcode code, u32 pc) {
	SPF("SW %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxSWR(char *str, PsxOpcode code, u32 pc) {
	SPF("SWR %s,%s(%s)", RT,HexToSignedStr16(code.imm),BASE);
}

static void dasmPsxHLE(char *str, PsxOpcode code, u32 pc) {
	SPF("PCSX HLE ???");
}

static void dasmPsxSLL(char *str, PsxOpcode code, u32 pc) {
	if (code.rd == 0 && code.rt == 0 && code.shamt == 0) {
		SPF("NOP");
	} else {
		SPF("SLL %s,%s,%d", RD, RT, code.shamt);
	}
}

static void dasmPsxSRL(char *str, PsxOpcode code, u32 pc) {
	SPF("SRL %s,%s,%d", RD, RT, code.shamt);
}

static void dasmPsxSRA(char *str, PsxOpcode code, u32 pc) {
	SPF("SRA %s,%s,%d", RD, RT, code.shamt);
}

static void dasmPsxSLLV(char *str, PsxOpcode code, u32 pc) {
	SPF("SLLV %s,%s,%s", RD, RT, RS);
}

static void dasmPsxSRLV(char *str, PsxOpcode code, u32 pc) {
	SPF("SRLV %s,%s,%s", RD, RT, RS);
}

static void dasmPsxSRAV(char *str, PsxOpcode code, u32 pc) {
	SPF("SRAV %s,%s,%s", RD, RT, RS);
}

static void dasmPsxJR(char *str, PsxOpcode code, u32 pc) {
	SPF("JR %s", RS);
}

static void dasmPsxJALR(char *str, PsxOpcode code, u32 pc) {
	SPF("JALR %s,%s", RD, RS);
}

static void dasmPsxSYSCALL(char *str, PsxOpcode code, u32 pc) {
	SPF("SYSCALL");
}

static void dasmPsxBREAK(char *str, PsxOpcode code, u32 pc) {
	SPF("BREAK");
}

static void dasmPsxMFHI(char *str, PsxOpcode code, u32 pc) {
	SPF("MFHI %s", RD);
}

static void dasmPsxMTHI(char *str, PsxOpcode code, u32 pc) {
	SPF("MTHI %s", RS);
}

static void dasmPsxMFLO(char *str, PsxOpcode code, u32 pc) {
	SPF("MFLO %s", RD);
}

static void dasmPsxMTLO(char *str, PsxOpcode code, u32 pc) {
	SPF("MTLO %s", RS);
}

static void dasmPsxMULT(char *str, PsxOpcode code, u32 pc) {
	SPF("MULT %s,%s", RS, RT);
}

static void dasmPsxMULTU(char *str, PsxOpcode code, u32 pc) {
	SPF("MULTU %s,%s", RS, RT);
}

static void dasmPsxDIV(char *str, PsxOpcode code, u32 pc) {
	SPF("DIV %s,%s", RS, RT);
}

static void dasmPsxDIVU(char *str, PsxOpcode code, u32 pc) {
	SPF("DIVU %s,%s", RS, RT);
}

static void dasmPsxADD(char *str, PsxOpcode code, u32 pc) {
	SPF("ADD %s,%s,%s", RD, RS, RT);
}

static void dasmPsxADDU(char *str, PsxOpcode code, u32 pc) {
	SPF("ADDU %s,%s,%s", RD, RS, RT);
}

static void dasmPsxSUB(char *str, PsxOpcode code, u32 pc) {
	SPF("SUB %s,%s,%s", RD, RS, RT);
}

static void dasmPsxSUBU(char *str, PsxOpcode code, u32 pc) {
	SPF("SUBU %s,%s,%s", RD, RS, RT);
}

static void dasmPsxAND(char *str, PsxOpcode code, u32 pc) {
	SPF("AND %s,%s,%s", RD, RS, RT);
}

static void dasmPsxOR(char *str, PsxOpcode code, u32 pc) {
	SPF("OR %s,%s,%s", RD, RS, RT);
}

static void dasmPsxXOR(char *str, PsxOpcode code, u32 pc) {
	SPF("XOR %s,%s,%s", RD, RS, RT);
}

static void dasmPsxNOR(char *str, PsxOpcode code, u32 pc) {
	SPF("NOR %s,%s,%s", RD, RS, RT);
}

static void dasmPsxSLT(char *str, PsxOpcode code, u32 pc) {
	SPF("SLT %s,%s,%s", RD, RS, RT);
}

static void dasmPsxSLTU(char *str, PsxOpcode code, u32 pc) {
	SPF("SLTU %s,%s,%s", RD, RS, RT);
}

static void dasmPsxBLTZ(char *str, PsxOpcode code, u32 pc) {
	SPF("BLTZ %s,0x%08X", RS, BOFFSET);
}

static void dasmPsxBGEZ(char *str, PsxOpcode code, u32 pc) {
	SPF("BGEZ %s,0x%08X", RS, BOFFSET);
}

static void dasmPsxBLTZAL(char *str, PsxOpcode code, u32 pc) {
	SPF("BLTZAL %s,0x%08X", RS, BOFFSET);
}

static void dasmPsxBGEZAL(char *str, PsxOpcode code, u32 pc) {
	SPF("BGEZAL %s,0x%08X", RS, BOFFSET);
}

static void dasmPsxMFC0(char *str, PsxOpcode code, u32 pc) {
	SPF("MFCO %s,%s", RT, PsxCpuDebugger::GetCp0RegName(code.rd));
}

static void dasmPsxCFC0(char *str, PsxOpcode code, u32 pc) {
	SPF("CFC0 ");
}

static void dasmPsxMTC0(char *str, PsxOpcode code, u32 pc) {
	SPF("MTCO %s,%s", RT, PsxCpuDebugger::GetCp0RegName(code.rd));
}

static void dasmPsxCTC0(char *str, PsxOpcode code, u32 pc) {
	SPF("CTC0 %s,%s", RT, PsxCpuDebugger::GetCp0RegName(code.rd));
}

static void dasmPsxRFE(char *str, PsxOpcode code, u32 pc) {
	SPF("RFE");
}

static void dasmGteLWC2(char *str, PsxOpcode code, u32 pc) {
	SPF("LWC2 ");
}

static void dasmGteSWC2(char *str, PsxOpcode code, u32 pc) {
	SPF("SWC2 ");
}

static void dasmGteMFC2(char *str, PsxOpcode code, u32 pc) {
	SPF("MFC2 ");
}

static void dasmGteCFC2(char *str, PsxOpcode code, u32 pc) {
	SPF("CFC2 ");
}

static void dasmGteMTC2(char *str, PsxOpcode code, u32 pc) {
	SPF("MTC2 ");
}

static void dasmGteCTC2(char *str, PsxOpcode code, u32 pc) {
	SPF("CTC2 ");
}

static void dasmGteRTPS(char *str, PsxOpcode code, u32 pc) {
	SPF("RTPS ");
}

static void dasmGteNCLIP(char *str, PsxOpcode code, u32 pc) {
	SPF("NCLIP ");
}

static void dasmGteOP(char *str, PsxOpcode code, u32 pc) {
	SPF("OP ");
}

static void dasmGteDPCS(char *str, PsxOpcode code, u32 pc) {
	SPF("DPCS ");
}

static void dasmGteINTPL(char *str, PsxOpcode code, u32 pc) {
	SPF("INTPL ");
}

static void dasmGteMVMVA(char *str, PsxOpcode code, u32 pc) {
	SPF("MVMVA ");
}

static void dasmGteNCDS(char *str, PsxOpcode code, u32 pc) {
	SPF("NCDS ");
}

static void dasmGteCDP(char *str, PsxOpcode code, u32 pc) {
	SPF("CDP ");
}

static void dasmGteNCDT(char *str, PsxOpcode code, u32 pc) {
	SPF("NCDT ");
}

static void dasmGteNCCS(char *str, PsxOpcode code, u32 pc) {
	SPF("NCCS ");
}

static void dasmGteCC(char *str, PsxOpcode code, u32 pc) {
	SPF("CC ");
}

static void dasmGteNCS(char *str, PsxOpcode code, u32 pc) {
	SPF("NCS ");
}

static void dasmGteNCT(char *str, PsxOpcode code, u32 pc) {
	SPF("NCT ");
}

static void dasmGteSQR(char *str, PsxOpcode code, u32 pc) {
	SPF("SQR ");
}

static void dasmGteDCPL(char *str, PsxOpcode code, u32 pc) {
	SPF("DCPL ");
}

static void dasmGteDPCT(char *str, PsxOpcode code, u32 pc) {
	SPF("DPCT ");
}

static void dasmGteAVSZ3(char *str, PsxOpcode code, u32 pc) {
	SPF("AVSZ3 ");
}

static void dasmGteAVSZ4(char *str, PsxOpcode code, u32 pc) {
	SPF("AVSZ4 ");
}

static void dasmGteRTPT(char *str, PsxOpcode code, u32 pc) {
	SPF("RTPT ");
}

static void dasmGteGPF(char *str, PsxOpcode code, u32 pc) {
	SPF("GPF ");
}

static void dasmGteGPL(char *str, PsxOpcode code, u32 pc) {
	SPF("GPL ");
}

static void dasmGteNCCT(char *str, PsxOpcode code, u32 pc) {
	SPF("NCCT ");
}


void (*DasmPsxBSC[64])(char *str, PsxOpcode code, u32 pc) = {
	dasmPsxSPECIAL, dasmPsxREGIMM, dasmPsxJ   , dasmPsxJAL  , dasmPsxBEQ , dasmPsxBNE , dasmPsxBLEZ, dasmPsxBGTZ,
	dasmPsxADDI   , dasmPsxADDIU , dasmPsxSLTI, dasmPsxSLTIU, dasmPsxANDI, dasmPsxORI , dasmPsxXORI, dasmPsxLUI ,
	dasmPsxCOP0   , dasmPsx0  , dasmPsxCOP2, dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsx0   , dasmPsx0  , dasmPsx0, dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsxLB     , dasmPsxLH    , dasmPsxLWL , dasmPsxLW   , dasmPsxLBU , dasmPsxLHU , dasmPsxLWR , dasmPsx0,
	dasmPsxSB     , dasmPsxSH    , dasmPsxSWL , dasmPsxSW   , dasmPsx0, dasmPsx0, dasmPsxSWR , dasmPsx0, 
	dasmPsx0   , dasmPsx0  , dasmGteLWC2, dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsx0   , dasmPsx0  , dasmGteSWC2, dasmPsxHLE  , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0 
};

void (*dasmPsxSPC[64])(char *str, PsxOpcode code, u32 pc) = {
	dasmPsxSLL , dasmPsx0 , dasmPsxSRL , dasmPsxSRA , dasmPsxSLLV   , dasmPsx0 , dasmPsxSRLV, dasmPsxSRAV,
	dasmPsxJR  , dasmPsxJALR , dasmPsx0, dasmPsx0, dasmPsxSYSCALL, dasmPsxBREAK, dasmPsx0, dasmPsx0,
	dasmPsxMFHI, dasmPsxMTHI , dasmPsxMFLO, dasmPsxMTLO, dasmPsx0   , dasmPsx0 , dasmPsx0, dasmPsx0,
	dasmPsxMULT, dasmPsxMULTU, dasmPsxDIV , dasmPsxDIVU, dasmPsx0   , dasmPsx0 , dasmPsx0, dasmPsx0,
	dasmPsxADD , dasmPsxADDU , dasmPsxSUB , dasmPsxSUBU, dasmPsxAND    , dasmPsxOR   , dasmPsxXOR , dasmPsxNOR ,
	dasmPsx0, dasmPsx0 , dasmPsxSLT , dasmPsxSLTU, dasmPsx0   , dasmPsx0 , dasmPsx0, dasmPsx0,
	dasmPsx0, dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0   , dasmPsx0 , dasmPsx0, dasmPsx0,
	dasmPsx0, dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0   , dasmPsx0 , dasmPsx0, dasmPsx0
};

void (*dasmPsxREG[32])(char *str, PsxOpcode code, u32 pc) = {
	dasmPsxBLTZ  , dasmPsxBGEZ  , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsx0  , dasmPsx0  , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsxBLTZAL, dasmPsxBGEZAL, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsx0  , dasmPsx0  , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0
};

void (*dasmPsxCP0[32])(char *str, PsxOpcode code, u32 pc) = {
	dasmPsxMFC0, dasmPsx0, dasmPsxCFC0, dasmPsx0, dasmPsxMTC0, dasmPsx0, dasmPsxCTC0, dasmPsx0,
	dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsxRFE , dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0
};

void (*dasmPsxCP2[64])(char *str, PsxOpcode code, u32 pc) = {
	dasmPsxBASIC, dasmGteRTPS , dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0 , dasmGteNCLIP, dasmPsx0,
	dasmPsx0 , dasmPsx0 , dasmPsx0 , dasmPsx0, dasmGteOP  , dasmPsx0 , dasmPsx0 , dasmPsx0,
	dasmGteDPCS , dasmGteINTPL, dasmGteMVMVA, dasmGteNCDS, dasmGteCDP , dasmPsx0 , dasmGteNCDT , dasmPsx0,
	dasmPsx0 , dasmPsx0 , dasmPsx0 , dasmGteNCCS, dasmGteCC  , dasmPsx0 , dasmGteNCS  , dasmPsx0,
	dasmGteNCT  , dasmPsx0 , dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0 , dasmPsx0 , dasmPsx0,
	dasmGteSQR  , dasmGteDCPL , dasmGteDPCT , dasmPsx0, dasmPsx0, dasmGteAVSZ3, dasmGteAVSZ4, dasmPsx0, 
	dasmGteRTPT , dasmPsx0 , dasmPsx0 , dasmPsx0, dasmPsx0, dasmPsx0 , dasmPsx0 , dasmPsx0,
	dasmPsx0 , dasmPsx0 , dasmPsx0 , dasmPsx0, dasmPsx0, dasmGteGPF  , dasmGteGPL  , dasmGteNCCT
};

void (*dasmPsxCP2BSC[32])(char *str, PsxOpcode code, u32 pc) = {
	dasmGteMFC2, dasmPsx0, dasmGteCFC2, dasmPsx0, dasmGteMTC2, dasmPsx0, dasmGteCTC2, dasmPsx0,
	dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0,
	dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0, dasmPsx0
};


static void dasmPsxSPECIAL(char *str, PsxOpcode code, u32 pc) {
	dasmPsxSPC[code.funct](str, code, pc);
}

static void dasmPsxREGIMM(char *str, PsxOpcode code, u32 pc) {
	dasmPsxREG[code.rt](str, code, pc);
}

static void dasmPsxCOP0(char *str, PsxOpcode code, u32 pc) {
	dasmPsxCP0[code.rs](str, code, pc);
}

static void dasmPsxCOP2(char *str, PsxOpcode code, u32 pc) {
	dasmPsxCP2[code.funct](str, code, pc);
}

static void dasmPsxBASIC(char *str, PsxOpcode code, u32 pc) {
	dasmPsxCP2BSC[code.rs](str, code, pc);
}

char *PsxCpuDebugger::DasmOne(PsxOpcode code, u32 pc) {
	static char str[256];

	memset(str,'\0',256);

	if (code.full == 0) {
		SPF("NOP");
	}  else {
		DasmPsxBSC[code.op](str, code, pc);
	}

	std::string tmp(str);

	for (unsigned int i = 0; i < tmp.length(); i++) {
		if (tmp[i] == ' ') {
			while (i < 9) {
				tmp.insert(tmp.begin()+(i++),' ');
			}
			break;
		}
	}

	for (unsigned int i = 0; i < tmp.length(); i++) {
		tmp[i] = tolower(tmp[i]);
	}

	strcpy(str,tmp.c_str());

	return (char*)str;
}

#endif