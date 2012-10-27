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
 
#include "Spu.h"
#include "Common.h"
#include "Console.h"
#include "Psx.h"


//#define LOG_SPU_REGS


CSpu::CSpu() {
	SetClassPointers();
}

void CSpu::SetClassPointers() {
	csl = CPsx::GetInstance()->csl;
}

void CSpu::InitSpu() {
	memset(mSpuMem, 0, 0x80000);
	mSpuTransferAddr = 0;
	mSpuStatus = 0;
}

void CSpu::WriteReg16(u32 addr, u16 val) {
	/* voices */
	if (addr >= 0x1f801c00 && addr < 0x1f801d80) {
		int voiceNum = ((addr >> 4) & 0xff) - 0xc0;
		switch (addr & 0xf) {
			case 0x0:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d Volume Left write 0x%04x\n", voiceNum, val);
#endif
				break;

			case 0x02:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d Volume Right write 0x%04x\n", voiceNum, val);
#endif
				break;

			case 0x04:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d Pitch write 0x%04x\n", voiceNum, val);
#endif
				break;

			case 0x06:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d Start Addr write 0x%04x\n", voiceNum, val);
#endif
				break;

			case 0x08:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d ADSL write 0x%04x\n", voiceNum, val);
#endif
				break;

			case 0x0a:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d Sus_Rel write 0x%04x\n", voiceNum, val);
#endif
				break;

			case 0x0c:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d ADSR Volume write 0x%04x\n", voiceNum, val);
#endif
				break;

			case 0x0e:
#if defined (LOG_SPU_REGS)
				csl->out("Voice %d Repeat Addr write 0x%04x\n", voiceNum, val);
#endif
				break;
		}
	} else {
		switch (addr) {
			case 0x1f801d80:
#if defined (LOG_SPU_REGS)
				csl->out("Main Volume Left write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d82:
#if defined (LOG_SPU_REGS)
				csl->out("Main Volume Right write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d84:
#if defined (LOG_SPU_REGS)
				csl->out("Reverberation Depth Left write 0x%04x\n", val);
#endif
				break;
			
			case 0x1f801d86:
#if defined (LOG_SPU_REGS)
				csl->out("Reverberation Depth Right write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d88:
#if defined (LOG_SPU_REGS)
				csl->out("Voice ON (0-15) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d8a:
#if defined (LOG_SPU_REGS)
				csl->out("Voice ON (16-23) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d8c:
#if defined (LOG_SPU_REGS)
				csl->out("Voice OFF (0-15) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d8e:
#if defined (LOG_SPU_REGS)
				csl->out("Voice OFF (16-23) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d90:
#if defined (LOG_SPU_REGS)
				csl->out("Channel FM (pitch lfo) Mode (0-15) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d92:
#if defined (LOG_SPU_REGS)
				csl->out("Channel FM (pitch lfo) Mode (16-23) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d94:
#if defined (LOG_SPU_REGS)
				csl->out("Channel Noise Mode (0-15) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d96:
#if defined (LOG_SPU_REGS)
				csl->out("Channel Noise Mode (16-23) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d98:
#if defined (LOG_SPU_REGS)
				csl->out("Channel Reverb Mode (0-15) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d9a:
#if defined (LOG_SPU_REGS)
				csl->out("Channel Reverb Mode (16-23) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d9c:
#if defined (LOG_SPU_REGS)
				csl->out("Channel ON/OFF (0-15) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801d9e:
#if defined (LOG_SPU_REGS)
				csl->out("Channel ON/OFF (16-23) write 0x%04x\n", val);
#endif
				break;

			case 0x1f801da2:
#if defined (LOG_SPU_REGS)
				csl->out("Reverb Work Area Start write 0x%04x\n", val);
#endif
				break;

			case 0x1f801da4:
#if defined (LOG_SPU_REGS)
				csl->out("Sound buffer IRQ Address write 0x%04x\n", val);
#endif
				break;

			case 0x1f801da6:
#if defined (LOG_SPU_REGS)
				csl->out("Sound buffer Data Transfer Addr write 0x%04x\n", val);
#endif
				mSpuTransferAddr = (u32)val << 3;
				break;

			case 0x1f801da8:
#if defined (LOG_SPU_REGS)
				csl->out("SPU Data write 0x%04x\n", val);
#endif
				mSpuMem[mSpuTransferAddr >> 1] = val;
				mSpuTransferAddr += 2;
				if (mSpuTransferAddr > 0x7ffff) 
					mSpuTransferAddr = 0;
				break;

			case 0x1f801daa:
#if defined (LOG_SPU_REGS)
				csl->out("SPU Control write 0x%04x\n", val);
#endif
				break;

			case 0x1f801dac:
#if defined (LOG_SPU_REGS)
				csl->out("SPU Status write 0x%04x\n", val);
#endif
				break;

			case 0x1f801dae:
#if defined (LOG_SPU_REGS)
				csl->out("SPU Status (2?) write 0x%04x\n", val);
#endif
				mSpuStatus = val & 0xf800;
				break;

			default:
#if defined (LOG_SPU_REGS)
				csl->out("SPU unknown reg write 0x%08x\n", addr);
#endif
				break;
		}
	}
}

u16 CSpu::ReadReg16(u32 addr) {
	return 0;
}