#pragma once
#ifndef PSX_INTERPRETER_H
#define PSX_INTERPRETER_H

#include "Common.h"

class CInterpreter {
public:
	CInterpreter();
	~CInterpreter();

	void Execute();
	void ExecuteInstruction();
	void CheckInterrupts();

	void LogBiosCall();

	static void unimplemented();

	static void psxNULL();
	static void psxSPECIAL();
	static void psxREGIMM();
	static void psxCOP0();
	static void psxCOP2();
	static void psxBASIC();

	static void psxJ();
	static void psxJAL();
	static void psxBEQ();
	static void psxBNE();
	static void psxBLEZ();
	static void psxBGTZ();
	static void psxADDI();
	static void psxADDIU();
	static void psxSLTI();
	static void psxSLTIU();
	static void psxANDI();
	static void psxORI();
	static void psxXORI();
	static void psxLUI();
	static void psxLB();
	static void psxLH();
	static void psxLWL();
	static void psxLW();
	static void psxLBU();
	static void psxLHU();
	static void psxLWR();
	static void psxSB();
	static void psxSH();
	static void psxSWL();
	static void psxSW();
	static void psxSWR();
	static void psxHLE();
	static void psxSLL();
	static void psxSRL();
	static void psxSRA();
	static void psxSLLV();
	static void psxSRLV();
	static void psxSRAV();
	static void psxJR();
	static void psxJALR();
	static void psxSYSCALL();
	static void psxBREAK();
	static void psxMFHI();
	static void psxMTHI();
	static void psxMFLO();
	static void psxMTLO();
	static void psxMULT();
	static void psxMULTU();
	static void psxDIV();
	static void psxDIVU();
	static void psxADD();
	static void psxADDU();
	static void psxSUB();
	static void psxSUBU();
	static void psxAND();
	static void psxOR();
	static void psxXOR();
	static void psxNOR();
	static void psxSLT();
	static void psxSLTU();
	static void psxBLTZ();
	static void psxBGEZ();
	static void psxBLTZAL();
	static void psxBGEZAL();
	static void psxMFC0();
	static void psxCFC0();
	static void psxMTC0();
	static void psxCTC0();
	static void psxRFE();
	static void MTC0(u32 reg, u32 val);

	static void gteLWC2();
	static void gteSWC2();

	static void gteMFC2();
	static void gteCFC2();
	static void gteMTC2();
	static void gteCTC2();

	static void gteRTPS();
	static void gteNCLIP();
	static void gteOP();
	static void gteDPCS();
	static void gteINTPL();
	static void gteMVMVA();
	static void gteNCDS();
	static void gteCDP();
	static void gteNCDT();
	static void gteNCCS();
	static void gteCC();
	static void gteNCS();
	static void gteNCT();
	static void gteSQR();
	static void gteDCPL();
	static void gteDPCT();
	static void gteAVSZ3();
	static void gteAVSZ4();
	static void gteRTPT();
	static void gteGPF();
	static void gteGPL();
	static void gteNCCT();

	BOOL bDoStep;
	BOOL bCpuTraceLog;
	BOOL bLogBiosCalls;

	FILE *fout;

	typedef void (*PsxCpuInstruction)();

	static PsxCpuInstruction psxInstruction[64];
	static PsxCpuInstruction psxSPC[64];
	static PsxCpuInstruction psxREG[32];
	static PsxCpuInstruction psxCP0[32];
	static PsxCpuInstruction psxCP2[64];
	static PsxCpuInstruction psxCP2BSC[32];

	static CpuState *cpu;
	static CMemory *mem;
	CGpu *gpu;
	Console *csl;
};

#endif /* PSX_INTERPRETER_H */