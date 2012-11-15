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
#ifndef SPU_H
#define SPU_H

#include "Common.h"
#include "Psx.h"

class PsxSpu {
public:
	PsxSpu();
	void InitSpu();

	void InitClassPointers();

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