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
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "Common.h"

#define VBL_INTR		 0x01
#define GPU_INTR		 0x02
#define CDROM_INTR		 0x04
#define DMA_INTR		 0x08
#define RCNT0_INTR		 0x10
#define RCNT1_INTR		 0x20
#define RCNT2_INTR		 0x40
#define CTRL_INTR		 0x80
#define SIO_INTR		 0x100
#define SPU_INTR		 0x200
#define PIO_INTR		 0x400

#endif /* INTERRUPTS_H */