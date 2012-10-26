#include "Memory.h"
#include "Common.h"
#include "Console.h"
#include "Cpu.h"
#include "Spu.h"
#include "Gpu.h"
#include "Interrupts.h"

#if defined (_DEBUG)
#include "CpuDebugger.h"
#endif

#if defined (_DEBUG)
#define LOG_DMA	
#endif

/* 
	TODO: I need information on DMA timings. I can't seem to find
	any info online, perhaps some testing on the real hardware
	is in order?
*/

void CMemory::DmaExecute(int n) {
	if ( !(mDmaDPCR & (8 << (n * 4)) && (mDmaCHCR[n] & 0x01000000)) )  {
		return;
	}

	u32 madr = mDmaMADR[n] & 0x1fffff; 
	u32 bcr = mDmaBCR[n]; 
	u32 chcr = mDmaCHCR[n]; 

	switch (n) {
		// MDECin
		case 0: {
#if defined (LOG_DMA)
			csl->out(CGREEN, "DMA MDECin\n");
#endif
		} break;

		// MDECout
		case 1: {
#if defined (LOG_DMA)
			csl->out(CGREEN, "DMA MDECout\n");
#endif
		} break;

		// GPU (lists + image data)
		case 2: {
			switch (chcr) {
				// vram2mem
				case 0x01000200:
#if defined (LOG_DMA)
					csl->out(CRED, "DMA2 GPU: vram to mem\n");
#endif
					break;

				// mem2vram
				case 0x01000201: {
#if defined (LOG_DMA)
					csl->out(CGREEN, "DMA2 GPU: mem to vram\n");
#endif
					cpu->cycles += 1000;

					u32 *ptr = (u32*)(RAM + madr);
					u32 size = (bcr >> 16) * (bcr & 0xffff);
					gpu->WriteData(ptr, size);
				} break;

				// dma chain
				case 0x01000401: {
#if defined (LOG_DMA)
					csl->out(CGREEN, "DMA2 GPU: dma chain\n");
#endif
					u32 *wmem = (u32*)(RAM + madr);
					u32 nextaddr;
					u32 size;

					do {
						cpu->cycles += 10;

						size = wmem[0] >> 24;
						nextaddr = wmem[0] & 0xffffff;
						gpu->WriteData(wmem+1, size);
						wmem = (u32*)(RAM + (nextaddr & 0x1fffff));
					} while (nextaddr != 0xffffff);
				} break;

				default:
#if defined (LOG_DMA)
					csl->out(CRED, "DMA2 GPU: unknown. chcr = 0x%08x, madr = 0x%08x, bcr = 0x%08x\n", chcr, madr, bcr);
#endif
					break;
			}
		} break;

		// CD-ROM
		case 3: {
#if defined (LOG_DMA)
			csl->out(CGREEN, "DMA CD-ROM\n");
#endif
		} break;

		// SPU
		case 4: {
#if defined (LOG_DMA)
			csl->out(CGREEN, "DMA SPU\n");
#endif
		} break;

		// PIO
		case 5: {
#if defined (LOG_DMA)
			csl->out(CGREEN, "DMA PIO\n");
#endif
		} break;

		// GPU OTC (reverse clear the Ordering Table)
		case 6: {
#if defined (LOG_DMA)
			csl->out(CGREEN, "DMA GPU OTC\n");
#endif
			if (mDmaCHCR[n] == 0x11000002) {
				u32 *mem = (u32*)&RAM[madr & 0x1fffff];
				while (bcr--) {
					*mem-- = (madr - 4) & 0xffffff;
					madr -= 4;
				}
				mem++; 
				*mem = 0xffffff;
			} else {
#if defined (LOG_DMA)
				csl->out(CRED, "DMA6 OT: unknown. chcr = 0x%08x, madr = 0x%08x, bcr = 0x%08x\n", mDmaCHCR[n], madr, bcr);
#endif
			}
		} break;
	}

	/* cheap emulation of dma timing. */
	//cpu->cycles += 80000;

	//mDmaCHCR[n] &= ~0x01000000;
	//if (mDmaDICR & (1 << (16 + n))) { 
	//	mDmaDICR |= 0x80000000 | (1 << (24 + n)); 
	//}

	mDmaCHCR[n] &= ~( ( 1L << 0x18 ) | ( 1L << 0x1c ) );
	mDmaDICR |= 1 << ( 24 + n );

	int n_int;
	int n_mask;

	n_int = ( mDmaDICR >> 24 ) & 0x7f;
	n_mask = ( mDmaDICR >> 16 ) & 0xff;

	if( ( n_mask & 0x80 ) != 0 && ( n_int & n_mask ) != 0 ) {
		mDmaDICR |= 0x80000000;
		mIREG |= DMA_INTR;
	} else {
		mIREG &= ~DMA_INTR;
	}

	mDmaDICR &= 0x00ffffff | ( mDmaDICR << 8 );
}
