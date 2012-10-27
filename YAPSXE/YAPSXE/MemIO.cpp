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
#include "Console.h"
#include "Cpu.h"
#include "CpuDebugger.h"
#include "Spu.h"
#include "Gpu.h"
#include "Psx.h"
#include "Interpreter.h"
#include "Interrupts.h"

#if defined (_DEBUG)
#define LOG_MEM_HW_READ8
//#define LOG_MEM_HW_READ16
#define LOG_MEM_HW_READ32
#define LOG_MEM_HW_WRITE8
#define LOG_MEM_HW_WRITE16
#define LOG_MEM_HW_WRITE32
#endif

u8 CMemory::HwRead8(u32 addr) {
	switch (addr) {
		//case 0x1f801040: break; // sio
		//case 0x1f801050: break; //serial port
		case 0x1f801800: 
#if defined (LOG_MEM_HW_READ8)
			csl->out("CDREG0 8-bit read\n"); 
#endif
			break;

		case 0x1f801801: 
#if defined (LOG_MEM_HW_READ8)
			csl->out("CDREG1 8-bit read\n");
#endif
			break;

		case 0x1f801802: 
#if defined (LOG_MEM_HW_READ8)
			csl->out("CDREG2 8-bit read\n"); 
#endif
			break;

		case 0x1f801803: 
#if defined (LOG_MEM_HW_READ8)
			csl->out("CDREG3 8-bit read\n");
#endif
			break;

		default:
#if defined (LOG_MEM_HW_READ8)
			csl->out("Unknown 8-bit read from 0x%08x\n", addr);
#endif
			return 0;
	}
	return 0;
}

u16 CMemory::HwRead16(u32 addr) {
	switch (addr) {
		case 0x1f801070:
#if defined (LOG_MEM_HW_READ16)
			csl->out("IREG 16-bit read\n");
#endif
			return mIREG;

		case 0x1f801074:
#if defined (LOG_MEM_HW_READ16)
			csl->out("IMASK 16-bit read\n");
#endif
			return mIMASK;

		case 0x1f801040:
#if defined (LOG_MEM_HW_READ16)
			csl->out("SIO 16-bit read\n");
#endif
			return 0;

		case 0x1f801044:
#if defined (LOG_MEM_HW_READ16)
			csl->out("SIO 16-bit read\n");
#endif
			return 0;

		case 0x1f801048:
#if defined (LOG_MEM_HW_READ16)
			csl->out("SIO 16-bit read\n");
#endif
			return 0;

		case 0x1f80104a:
#if defined (LOG_MEM_HW_READ16)
			csl->out("SIO 16-bit read\n");
#endif
			return 0;

		case 0x1f80104e:
#if defined (LOG_MEM_HW_READ16)
			csl->out("SIO 16-bit read\n");
#endif
			return 0;

		case 0x1f801050:
#if defined (LOG_MEM_HW_READ16)
			csl->out("Serial 16-bit read\n");
#endif
			return 0;

		case 0x1f801054:
#if defined (LOG_MEM_HW_READ16)
			csl->out("Serial 16-bit read\n");
#endif
			return 0;

		case 0x1f80105a:
#if defined (LOG_MEM_HW_READ16)
			csl->out("Serial 16-bit read\n");
#endif
			return 0;

		case 0x1f80105e:
#if defined (LOG_MEM_HW_READ16)
			csl->out("Serial 16-bit read\n");
#endif
			return 0;
	
		case 0x1f801100:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 0 COUNT 16-bit read\n");
#endif
			return 0;

		case 0x1f801104:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 0 MODE 16-bit read\n");
#endif
			return 0;

		case 0x1f801108:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 0 TARGET 16-bit read\n");
#endif
			return 0;

		case 0x1f801110:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 1 COUNT 16-bit read\n");
#endif
			return 0;

		case 0x1f801114:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 1 MODE 16-bit read\n");
#endif
			return 0;

		case 0x1f801118:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 1 TARGET 16-bit read\n");
#endif
			return 0;

		case 0x1f801120:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 2 COUNT 16-bit read\n");
#endif
			return 0;

		case 0x1f801124:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 2 MODE 16-bit read\n");
#endif
			return 0;

		case 0x1f801128:
#if defined (LOG_MEM_HW_READ16)
			csl->out("COUNTER 2 TARGET 16-bit read\n");
#endif
			return 0;

	
		//case 0x1f802030: hard =   //int_2000????
		//case 0x1f802040: hard =//dip switches...??

		default: {
			if (addr >= 0x1f801c00 && addr < 0x1f801e00) {
#if defined (LOG_MEM_HW_READ16)
            	//csl->out("SPU 16-bit read\n");
#endif
			} else {
#if defined (LOG_MEM_HW_READ16)
				csl->out("Unknown 16-bit read\n");
#endif
			}
            return 0;
		}
	}
}

u32 CMemory::HwRead32(u32 addr) {
	switch (addr) {
		case 0x1f801014:
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "SPU DELAY (???) 32-bit read\n");
#endif
			return 0;

	    case 0x1f801040:
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "SIO 32-bit read\n");
#endif
			return 0;

		case 0x1f801050: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "Serial port 32-bit read\n");
#endif
			return 0;

		case 0x1f801060:
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "RAM size 32-bit read\n");
#endif
			return 0;

		case 0x1f801070: 
#if defined (LOG_MEM_HW_READ32)
			//csl->out(CWHITE, "IREG 32-bit read\n");
#endif
			//CPsx::GetInstance()->mCpuDbg->OpenDebugger();
			//cpu->SetCpuState(PSX_CPU_STEPPING);
			return mIREG;

		case 0x1f801074: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "IMASK 32-bit read\n");
#endif
			//CPsx::GetInstance()->mCpuDbg->OpenDebugger();
			//CPsx::GetInstance()->mCpuDbg->UpdateDebugger();
			//cpu->SetCpuState(PSX_CPU_STEPPING);
			return mIMASK;

		case 0x1f801080: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA0 MADR 32-bit read\n");
#endif
			return 0;

		case 0x1f801084: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA0 BCR 32-bit read\n");
#endif
			return 0;

		case 0x1f801088: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA0 CHCR 32-bit read\n");
#endif
			return 0;

		case 0x1f801090: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA1 MADR 32-bit read\n");
#endif
			return 0;

		case 0x1f801094: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA1 BCR 32-bit read\n");
#endif
			return 0;

		case 0x1f801098: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA1 CHCR 32-bit read\n");
#endif
			return 0;
		
		case 0x1f8010a0: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA2 MADR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010a4: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA2 BCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010a8: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA2 CHCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010b0: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA3 MADR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010b4: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA3 BCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010b8: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA3 CHCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010c0: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA4 MADR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010c4: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA4 BCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010c8: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA4 CHCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010d0: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA5 MADR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010d4: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA5 BCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010d8: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA5 CHCR 32-bit read\n");
#endif
			return 0; 

		case 0x1f8010e0: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA6 MADR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010e4: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA6 BCR 32-bit read\n");
#endif
			return 0;

		case 0x1f8010e8: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA6 CHCR 32-bit read\n");
#endif
			return 0; 

		case 0x1f8010f0: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA PCR 32-bit read\n");
#endif
			return mDmaDPCR;

		case 0x1f8010f4: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "DMA ICR 32-bit read\n");
#endif
			return mDmaDICR;

		case 0x1f801810: {
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "GPU DATA 32-bit read\n");
#endif
			return gpu->ReadData();
		}

		case 0x1f801814: {
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "GPU STATUS 32-bit read\n");
#endif
			return gpu->ReadStatus();
		}

		case 0x1f801820: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "MDEC0 32-bit read\n");
#endif
			return 0;

		case 0x1f801824: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "MDEC1 32-bit read\n");
#endif
			return 0;

		case 0x1f801100: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 0 COUNT 32-bit read\n");
#endif
			return 0;

		case 0x1f801104: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 0 MODE 32-bit read\n");
#endif
			return 0;

		case 0x1f801108: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 0 TARGET 32-bit read\n");
#endif
			return 0;

		case 0x1f801110: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 1 COUNT 32-bit read\n");
#endif
			//CPsx::GetInstance()->mCpuDbg->OpenDebugger();
			//cpu->SetCpuState(PSX_CPU_STEPPING);
			return 0;

		case 0x1f801114: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 1 MODE 32-bit read\n");
#endif
			return 0;

		case 0x1f801118: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 1 TARGET 32-bit read\n");
#endif
			return 0;

		case 0x1f801120: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 2 COUNT 32-bit read\n");
#endif
			return 0;

		case 0x1f801124: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 2 MODE 32-bit read\n");
#endif
			return 0;

		case 0x1f801128: 
#if defined (LOG_MEM_HW_READ32)
			csl->out(CWHITE, "COUNTER 2 TARGET 32-bit read\n");
#endif
			return 0;

		default: 
#if defined (LOG_MEM_HW_READ32)
			csl->out("Unknown 32-bit read from 0x%08x\n", addr);
#endif
			return 0;
	}
}

void CMemory::HwWrite8(u32 addr, u8 data) {
	switch (addr) {
		//case 0x1f801040: break; // sio
		//case 0x1f801050: break; //serial port
		case 0x1f801800: 
#if defined (LOG_MEM_HW_WRITE8)
			csl->out("CDREG0 8-bit write 0x%02x\n", data); 
#endif
			break;

		case 0x1f801801:  
#if defined (LOG_MEM_HW_WRITE8)
			csl->out("CDREG1 8-bit write 0x%02x\n", data);  
#endif
			//mIREG |= CDROM_INTR;
			break;

		case 0x1f801802:  
#if defined (LOG_MEM_HW_WRITE8)
			csl->out("CDREG2 8-bit write 0x%02x\n", data);  
#endif
			break;

		case 0x1f801803:  
#if defined (LOG_MEM_HW_WRITE8)
			csl->out("CDREG3 8-bit write 0x%02x\n", data);  
#endif
			break;

		default: 
#if defined (LOG_MEM_HW_WRITE8)
			csl->out("Unknown 8-bit write to 0x%08x\n", addr); 
#endif
			return;
	}
}

void CMemory::HwWrite16(u32 addr, u16 data) {
	switch (addr) {
		case 0x1f801040:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "SIO 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801044:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "SIO 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801048:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "SIO 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f80104a:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "SIO 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f80104e:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "SIO 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801070: 
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "IREG 16-bit write 0x%04x\n", data);
#endif
			mIREG &= mIMASK & data;
			return;

		case 0x1f801074:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "IMASK 16-bit write 0x%04x\n", data);
#endif
			mIMASK = data;
			return;

		case 0x1f801100:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 0 COUNT 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801104:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 0 MODE 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801108:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 0 TARGET 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801110:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 1 COUNT 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801114:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 1 MODE 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801118:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 1 TARGET 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801120:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 2 COUNT 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801124:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 2 MODE 16-bit write 0x%04x\n", data);
#endif
			return;

		case 0x1f801128:
#if defined (LOG_MEM_HW_WRITE16)
			csl->out(CWHITE, "COUNTER 2 TARGET 16-bit write 0x%04x\n", data);
#endif
			return;

		default:
			if (addr >= 0x1f801c00 && addr < 0x1f801e00) {
				spu->WriteReg16(addr,data);
			} else {
#if defined (LOG_MEM_HW_WRITE16)
				//csl->out("Unknown 16-bit write to 0x%08x\n", addr);
#endif
			}
			return;
	}
}

void CMemory::HwWrite32(u32 addr, u32 data) {
	switch (addr) {
		case 0x1f801014:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "SPU DELAY (???) 32-bit write 0x%08x\n", data);
#endif
			return;

	    case 0x1f801040:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "SIO 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801050: 
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "Serial port 32-bit write 0x%08x\n", data);
#endif
			break;

		case 0x1f801060: {
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "RAM size 32-bit write 0x%08x\n", data);
#endif
		} return;

		case 0x1f801070: 
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "IREG 32-bit write 0x%08x\n", data);
#endif
			mIREG &= mIMASK & data;
			return;

		case 0x1f801074:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "IMASK 32-bit write 0x%08x\n", data);
#endif
			mIMASK = data;
			return;

		case 0x1f801080:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA0 MADR 32-bit write 0x%08x\n", data);
#endif
			mDmaMADR[0] = data;
			return;

		case 0x1f801084:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA0 BCR 32-bit write 0x%08x\n", data);
#endif
			mDmaBCR[0] = data;
			return;

		case 0x1f801088:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA0 CHCR 32-bit write 0x%08x\n", data);
#endif
			mDmaCHCR[0] = data;
			DmaExecute(0);
			return;

		case 0x1f801090:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA1 MADR 32-bit write 0x%08x\n", data);
#endif
			mDmaMADR[1] = data;
			return;

		case 0x1f801094:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA1 BCR 32-bit write 0x%08x\n", data);
#endif
			mDmaBCR[1] = data;
			return;

		case 0x1f801098:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA1 CHCR 32-bit write 0x%08x\n", data);
#endif
			mDmaCHCR[1] = data;
			DmaExecute(1);
			return;
		
		case 0x1f8010a0:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA2 MADR 32-bit write 0x%08x\n", data);
#endif
			mDmaMADR[2] = data;
			return;

		case 0x1f8010a4:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA2 BCR 32-bit write 0x%08x\n", data);
#endif
			mDmaBCR[2] = data;
			return;

		case 0x1f8010a8:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA2 CHCR 32-bit write 0x%08x\n", data);
#endif
			mDmaCHCR[2] = data;
			DmaExecute(2);
			return;

		case 0x1f8010b0:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA3 MADR 32-bit write 0x%08x\n", data);
#endif
			mDmaMADR[3] = data;
			return;

		case 0x1f8010b4:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA3 BCR 32-bit write 0x%08x\n", data);
#endif
			mDmaBCR[3] = data;
			return;

		case 0x1f8010b8:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA3 CHCR 32-bit write 0x%08x\n", data);
#endif
			mDmaCHCR[3] = data;
			DmaExecute(3);
			return;

		case 0x1f8010c0:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA4 MADR 32-bit write 0x%08x\n", data);
#endif
			mDmaMADR[4] = data;
			return;

		case 0x1f8010c4:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA4 BCR 32-bit write 0x%08x\n", data);
#endif
			mDmaBCR[4] = data;
			return;

		case 0x1f8010c8:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA4 CHCR 32-bit write 0x%08x\n", data);
#endif
			mDmaCHCR[4] = data;
			DmaExecute(4);
			return;

		case 0x1f8010d0:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA5 MADR 32-bit write 0x%08x\n", data);
#endif
			mDmaMADR[5] = data;
			return;

		case 0x1f8010d4:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA5 BCR 32-bit write 0x%08x\n", data);
#endif
			mDmaBCR[5] = data;
			return;

		case 0x1f8010d8:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA5 CHCR 32-bit write 0x%08x\n", data);
#endif
			mDmaCHCR[5] = data;
			DmaExecute(5);
			return; 

		case 0x1f8010e0:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA6 MADR 32-bit write 0x%08x\n", data);
#endif
			mDmaMADR[6] = data;
			return;

		case 0x1f8010e4:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA6 BCR 32-bit write 0x%08x\n", data);
#endif
			mDmaBCR[6] = data;
			return;

		case 0x1f8010e8:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA6 CHCR 32-bit write 0x%08x\n", data);
#endif
			mDmaCHCR[6] = data;
			DmaExecute(6);
			return; 

		case 0x1f8010f0:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA PCR 32-bit write 0x%08x\n", data);
#endif
			mDmaDPCR |= data;
			return;

		case 0x1f8010f4:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "DMA ICR 32-bit write 0x%08x\n", data);
#endif
			mDmaDICR = ( mDmaDICR & ( 0x80000000 /*| ~mem_mask*/ ) ) |
				( mDmaDICR & ~data & 0x7f000000 /*& mem_mask*/ ) |
				( data & 0x00ffffff /*& mem_mask*/ );

			if((mDmaDICR & 0x80000000) != 0 && (mDmaDICR & 0x7f000000) == 0) {
				//verboselog( machine(), 2, "dma interrupt cleared\n" );
				mDmaDICR &= ~0x80000000;
			}
			return;

		case 0x1f801810:
//#if defined (LOG_MEM_HW_WRITE32)
			//csl->out(CWHITE, "GPU DATA 32-bit write 0x%08x %d\n", data);
//#endif
			gpu->WriteData(&data, 1);
			return;

		case 0x1f801814:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "GPU STATUS 32-bit write 0x%08x\n", data);
#endif
			gpu->WriteStatus(data);
			return;

		case 0x1f801820:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "MDEC0 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801824:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "MDEC1 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801100:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 0 COUNT 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801104:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 0 MODE 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801108:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 0 TARGET 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801110:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 1 COUNT 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801114:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 1 MODE 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801118:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 1 TARGET 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801120:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 2 COUNT 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801124:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 2 MODE 32-bit write 0x%08x\n", data);
#endif
			return;

		case 0x1f801128:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out(CWHITE, "COUNTER 2 TARGET 32-bit write 0x%08x\n", data);
#endif
			return;

		default:
#if defined (LOG_MEM_HW_WRITE32)
			csl->out("Unknown 32-bit write to 0x%08x\n", addr);
#endif
			return;
	}
}