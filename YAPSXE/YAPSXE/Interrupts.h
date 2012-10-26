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