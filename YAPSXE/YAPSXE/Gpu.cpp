#include "Psx.h"
#include "Gpu.h"
#include "Common.h"
#include "Console.h"
#include "Cpu.h"
#include "OpenGL.h"
#include "Windows.h"

#if defined (_DEBUG)
#define LOG_GPU_STATUS_WRITES
#define LOG_GPU_COMMANDS
#endif

CGpu::CGpu(){
	SetClassPointers();
}

CGpu::~CGpu(){
	glDeleteTextures(1, &mCacheTexID);
}

void CGpu::SetClassPointers() {
	psx = CPsx::GetInstance();
	csl = psx->csl;
	gl = psx->gl;
}

void CGpu::ResetState() {
	mStatusReg = 0x14802000;
	mGpuBufferOffset = 0;
	mVramPosX = 0;
	mVramPosY = 0;
	mGpuVersion = psx->GetHwRevision();
	mGpuInfo = 0;
	mDispStartX = 0;
	mDispStartY = 0;
	mHorDispRangeStart = 0;
	mHorDispRangeEnd = 0;
	mVertDispRangeStart = 0;
	mVertDispRangeEnd = 0;
	mDrawOffsetX = 0;
	mDrawOffsetY = 0;
	mDrawAreaX1 = 0;
	mDrawAreaY1 = 0;
	mDrawAreaX2 = 0;
	mDrawAreaY2 = 0;
}

void CGpu::InitGpu() {
	if (!gl->mInitialised) {
		MessageBox(psx->mMainWnd->GetHwnd(), 
			"Error: OpenGL must be initialised before calling 'CGpu::InitGpu()'", "Error", MB_ICONERROR);
		psx->SignalQuit();
		return;
	}

	ResetState();

	mSaveVram = FALSE;
	mRestoreVram = FALSE;

	glEnable(GL_TEXTURE_2D);

	static BOOL bTexturesCreated = FALSE;
	if (!bTexturesCreated) {
		glGenTextures(1, &mCacheTexID);
		glBindTexture(GL_TEXTURE_2D, mCacheTexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, 1024, 512, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, NULL);
		bTexturesCreated = TRUE;
	}
}

u32 CGpu::ReadStatus() {
	/* hack for interlaced scan */
	mStatusReg ^= 0x80000000;

	return mStatusReg;
}

void CGpu::WriteStatus(u32 data) {
	u32 command = (data >> 24) & 0xff;
	switch (command) {
		// reset gpu
		case 0x00:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: reset GPU 0x%08x\n", data);
#endif
			ResetState();
			break;

		// reset command buffer
		case 0x01:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: reset command buffer 0x%08x\n", data);
#endif
			mGpuBufferOffset = 0;
			break;

		// reset irq
		case 0x02:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: reset irq 0x%08x\n", data);
#endif
			break;

		// display enable
		case 0x03:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: display enable 0x%08x\n", data);
#endif
			mStatusReg &= ~(1 << 0x17);
			mStatusReg |= (data & 0x01) << 0x17;
			break;

		// dma transfer mode
		case 0x04:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: DMA transfer mode 0x%08x\n", data);
#endif
			mStatusReg &= ~(0x03 << 29);
			mStatusReg |= (data & 0x03) << 29;
			mStatusReg &= ~( 1 << 0x19);
			if((data & 3) == 1 || (data & 3) == 2) {
				mStatusReg |= ( 1L << 0x19 );
			}
			break;

		// display position
		case 0x05:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: display position 0x%08x\n", data);
#endif
			mDispStartX = data & 1023;
			if(mGpuVersion == 2) {
				mDispStartY = (data >> 10) & 1023;
			} else {
				mDispStartY = (data >> 12) & 1023;
			}
			break;

		// horizontal display range
		case 0x06:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: horizontal display range 0x%08x (start: %x, end: %x)\n", data, data & 4095, (data >> 12) & 4095);
#endif
			mHorDispRangeStart = data & 4095;
			mHorDispRangeEnd = (data >> 12) & 4095;
			break;

		// vertical display range
		case 0x07:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: vertical display range 0x%08x\n", data, data & 1023, (data >> 10) & 2047);
#endif
			mVertDispRangeStart = data & 1023;
			mVertDispRangeEnd = (data >> 10) & 2047;
			break;

		// display mode
		case 0x08:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out("GPU command: display mode 0x%08x\n", data);
#endif
			mStatusReg &= ~( 127 << 0x10 );
			mStatusReg |= (data & 0x3f) << 0x11; /* width 0 + height + videmode + isrgb24 + isinter */
			mStatusReg |= ((data & 0x40) >> 0x06) << 0x10; /* width 1 */
			//if(mGpuVersion == 1) {
			//	mReverseFlag = (data >> 7) & 1;
			//}
			break;

		// gpu info
		case 0x10:
			switch(data & 0xff) {
				case 0x03:
					if (mGpuVersion == 2) {
						//mGpuInfo = n_drawarea_x1 | ( n_drawarea_y1 << 10 );
					} else {
						//mGpuInfo = n_drawarea_x1 | ( n_drawarea_y1 << 12 );
					}
#if defined (LOG_GPU_STATUS_WRITES)
					csl->out("GPU Info: draw area top left %08x\n", mGpuInfo);
#endif
					break;

				case 0x04:
					if (mGpuVersion == 2) {
						//mGpuInfo = n_drawarea_x2 | ( n_drawarea_y2 << 10 );
					} else {
						//mGpuInfo = n_drawarea_x2 | ( n_drawarea_y2 << 12 );
					}
#if defined (LOG_GPU_STATUS_WRITES)
					csl->out("GPU Info: Draw area bottom right %08x\n", mGpuInfo);
#endif
					break;

				case 0x05:
					if (mGpuVersion == 2) {
						//mGpuInfo = ( n_drawoffset_x & 2047 ) | ( ( n_drawoffset_y & 2047 ) << 11 );
					} else {
						//mGpuInfo = ( n_drawoffset_x & 2047 ) | ( ( n_drawoffset_y & 2047 ) << 12 );
					}
#if defined (LOG_GPU_STATUS_WRITES)
					csl->out("GPU Info: Draw offset %08x\n", mGpuInfo);
#endif
					break;

				case 0x07:
					mGpuInfo = mGpuVersion;
#if defined (LOG_GPU_STATUS_WRITES)
					csl->out("GPU Info: gpu type %08x\n", mGpuInfo);
#endif
					break;

				case 0x08:
					//n_gpuinfo = n_lightgun_x | ( n_lightgun_y << 16 );
#if defined (LOG_GPU_STATUS_WRITES)
					csl->out("GPU Info: lightgun position %08x\n", mGpuInfo);
#endif
					break;

				default:
#if defined (LOG_GPU_STATUS_WRITES)
					csl->out("GPU Info: unknown\n");
#endif
					mGpuInfo = 0;
					break;
			}
			break;

		default:
#if defined (LOG_GPU_STATUS_WRITES)
			csl->out(CRED, "GPU command: unhandled 0x%08x\n", data);
#endif
			break;
	}
}

u32 CGpu::ReadData() {
	u32 ret;
	if((mStatusReg & (1 << 0x1b))) {
		static int curPixel = 0;
		ret = mImageBuffer[curPixel++];
		ret |= ((u32)mImageBuffer[curPixel++] << 16);
		for (int pix = 0; pix < 2; pix++) {
			mVramPosX++;
			if(mVramPosX >= (mCommandBuffer[2] & 0xffff)) {
				mVramPosX = 0;
				mVramPosY++;
				if(mVramPosY >= (mCommandBuffer[2] >> 16)) {
					mGpuBufferOffset = 0;
					mVramPosX = 0;
					mVramPosY = 0;
					curPixel = 0;
					break;
				}
			}
		}
	} else {
		ret = mGpuInfo;
	}
	return ret;
}

void CGpu::WriteData(u32 *pData, u32 size) {
	for (int i = 0; i < size; i++, pData++) {
		mCommandBuffer[mGpuBufferOffset] = *pData;
		switch (mCommandBuffer[0] >> 24) {
			case 0x00:
#if defined (LOG_GPU_COMMANDS)
				csl->out("** unhandled GPU command 0x00\n");
#endif
				break;

			case 0x01:
#if defined (LOG_GPU_COMMANDS)
				csl->out("** Unhandled GPU command 0x01: clear cache\n");
#endif
				break;

			case 0x02: {
				if (mGpuBufferOffset < 2) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x02: framebuffer rectangle (x:%u, y:%u, w:%u, h:%u)\n",
						mCommandBuffer[1] & 0xffff, mCommandBuffer[1] >> 16, 
						mCommandBuffer[2] & 0xffff, mCommandBuffer[2] >> 16);
#endif
					FillFramebufferRect();
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x20:
			case 0x21:
			case 0x22:
			case 0x23: {
				if (mGpuBufferOffset < 3) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x20: monochrome 3 point polygon\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x24:
			case 0x25:
			case 0x26:
			case 0x27: {
				if (mGpuBufferOffset < 6) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x24: textured 3 point polygon\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x28:
			case 0x29:
			case 0x2a:
			case 0x2b: {
				if (mGpuBufferOffset < 4) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x28: monochrome 4 point polygon\n");
#endif
					Mono4PointPoly();
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x2c:
			case 0x2d:
			case 0x2e:
			case 0x2f: {
				if (mGpuBufferOffset < 8) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					VertexT *v = (VertexT*)&mCommandBuffer[1];
					csl->out("GPU command 0x2c: textured 4 point polygon (bgr=0x%06x, clut[x=%d, y=%d], "
						"tpage=0x%x, v0[x=%d, y=%d, u=%d, v=%d], v1[x=%d, y=%d, u=%d, v=%d], v2[x=%d, y=%d, "
						"u=%d, v=%d], v3[x=%d, y=%d, u=%d, v=%d])\n", 
						mCommandBuffer[0] & 0xffffff, (v[0].txinfo & 0x3f) << 4, (v[0].txinfo >> 6) & 0x3ff, v[1].txinfo, v[0].x, v[0].y, v[0].u, v[0].v, 
						v[1].x, v[1].y, v[1].u, v[1].v, v[2].x, v[2].y, v[2].u, v[2].v, v[3].x, v[3].y, v[3].u, v[3].v);
					//csl->out("Flat textured 4-point poly: %08x %08x %08x %08x %08x %08x %08x %08x %08x\n",
					//			mCommandBuffer[0], mCommandBuffer[1], mCommandBuffer[2], mCommandBuffer[3], mCommandBuffer[4],
					//			mCommandBuffer[5], mCommandBuffer[6], mCommandBuffer[7], mCommandBuffer[8]);
#endif
					Textured4PointPoly();
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x30:
			case 0x31:
			case 0x32:
			case 0x33: {
				if (mGpuBufferOffset < 5) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x30: gouraud 3 point polygon\n");
#endif
					Gouraud3PointPoly();
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x34:
			case 0x35:
			case 0x36:
			case 0x37: {
				if (mGpuBufferOffset < 8) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x34: gouraud textured 3 point polygon\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x38:
			case 0x39:
			case 0x3a:
			case 0x3b: {
				if (mGpuBufferOffset < 7) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x38: gouraud 4 point polygon\n");
#endif
					Gouraud4PointPoly();
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x3c:
			case 0x3d:
			case 0x3e:
			case 0x3f: {
				if (mGpuBufferOffset < 11) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x3c: gouraud textured 4 point polygon\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x40:
			case 0x41:
			case 0x42: {
				if (mGpuBufferOffset < 2) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x40: monochrome line\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x48:
			case 0x4a:
			case 0x4c:
			case 0x4e: {
#if defined (LOG_GPU_COMMANDS)
				if (mGpuBufferOffset == 0) {
					csl->out("GPU command 0x48: single color polyline\n");
				}
#endif
				if (mGpuBufferOffset < 3) {
					mGpuBufferOffset++;
				} else {
					// DrawLine()
					if((mCommandBuffer[3] & 0xf000f000) != 0x50005000) {
						mCommandBuffer[1] = mCommandBuffer[2];
						mCommandBuffer[2] = mCommandBuffer[3];
						mGpuBufferOffset = 3;
					} else {
						mGpuBufferOffset = 0;
					}
				}
			} break;

			case 0x50:
			case 0x51:
			case 0x52:
			case 0x53: {
				if (mGpuBufferOffset < 3) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x50: gouraud line\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x58:
			case 0x5a:
			case 0x5c:
			case 0x5e: {
#if defined (LOG_GPU_COMMANDS)
				if (mGpuBufferOffset == 0) {
					csl->out(CRED, "** Unimplemented GPU command 0x58: gouraud polyline\n");
				}
#endif
				psx->cpu->SetCpuState(PSX_CPU_HALTED);
			} break;

			case 0x60:
			case 0x61:
			case 0x62:
			case 0x63: {
				if (mGpuBufferOffset < 2) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x60: rectangle\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x64:
			case 0x65:
			case 0x66:
			case 0x67: {
				if (mGpuBufferOffset < 3) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x64: sprite (x: %d, y: %d, w: %d h: %d, CLUT: %x, v: %d, u: %d\n",
						mCommandBuffer[1] & 0xffff, mCommandBuffer[1] >> 16,
						mCommandBuffer[3] & 0xffff, mCommandBuffer[3] >> 16,
						mCommandBuffer[2] >> 16, 
						(mCommandBuffer[2] >> 8) & 0xff, mCommandBuffer[2] & 0xff);
#endif
					Sprite();
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x68:
			case 0x6a: {
				if (mGpuBufferOffset < 1) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x68: dot\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x70:
			case 0x71: {
				if (mGpuBufferOffset < 1) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x70: 8X8 rectangle\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x74:
			case 0x75: 
			case 0x76: 
			case 0x77: {
				if (mGpuBufferOffset < 2) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x74: 8X8 sprite\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x78:
			case 0x79: {
				if (mGpuBufferOffset < 1) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x78: 16X16 rectangle\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x7c:
			case 0x7d: 
			case 0x7e: 
			case 0x7f: {
				if (mGpuBufferOffset < 2) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x7c: 16X16 sprite\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			case 0x80: {
				if (mGpuBufferOffset < 3) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0x80: move image in framebuffer\n");
#endif
					mGpuBufferOffset = 0;
				}
			} break;

			/* send image to vram */
			case 0xa0: {
				static u32 curPixel = 0;

#if defined (LOG_GPU_COMMANDS)
				if (mGpuBufferOffset == 2) {
					csl->out("GPU command 0xa0: send image to framebuffer (x:%u, y:%u, w:%u, h:%u)\n",
						mCommandBuffer[1] & 0xffff, mCommandBuffer[1] >> 16, 
						mCommandBuffer[2] & 0xffff, mCommandBuffer[2] >> 16);
				}
#endif
				if (mGpuBufferOffset < 3) {
					mGpuBufferOffset++;
				} else {
					u32 data = *pData;
					for (int pix = 0; pix < 2; pix++) {
						mImageBuffer[curPixel++] = data;
						mVramPosX++;
						if(mVramPosX >= (mCommandBuffer[2] & 0xffff)) {
							mVramPosX = 0;
							mVramPosY++;
							if(mVramPosY >= (mCommandBuffer[2] >> 16)) {
								WriteFramebufferImage();
								mGpuBufferOffset = 0;
								mVramPosX = 0;
								mVramPosY = 0;
								curPixel = 0;
								break;
							}
						}
						data >>= 16;
					}
				}
			} break;

			/* send vram image to mem */
			case 0xc0: {
				if(mGpuBufferOffset < 2) {
					mGpuBufferOffset++;
				} else {
#if defined (LOG_GPU_COMMANDS)
					csl->out("GPU command 0xc0: send vram image\n");
#endif
					//csl->out("mGpuBufferOffset 1 =%d %x\n", mGpuBufferOffset, mCommandBuffer[2]);
					glReadPixels(mCommandBuffer[1] & 0xffff,
								 mCommandBuffer[1] >> 16,
								 mCommandBuffer[2] & 0xffff,
								 mCommandBuffer[2] >> 16, 
								 GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 
								 mImageBuffer);
					//csl->out("mGpuBufferOffset 2 =%d\n", mGpuBufferOffset);
					// ready to send image
					mStatusReg |= (1 << 0x1b);
				}
			} break;

			case 0xe1: {
#if defined (LOG_GPU_COMMANDS)
				csl->out("GPU command 0xe1: draw mode setting 0x%08x\n", mCommandBuffer[0]);
#endif
				UpdateTexturePage(mCommandBuffer[0]);
			} break;

			case 0xe2: {
#if defined (LOG_GPU_COMMANDS)
				csl->out("GPU command 0xe2: texture window setting 0x%08x\n", mCommandBuffer[0]);
#endif
			} break;

			case 0xe3: {
				mDrawAreaX1 = mCommandBuffer[0] & 0x3ff;
				if (mGpuVersion == 2) {
					mDrawAreaY1 = (mCommandBuffer[0] >> 10) & 0x3ff;
				} else {
					mDrawAreaY1 = (mCommandBuffer[0] >> 12) & 0x3ff;
				}
#if defined (LOG_GPU_COMMANDS)
				csl->out("GPU command 0xe3: drawing area top left %d,%d\n", mDrawAreaX1, mDrawAreaY1);
#endif
			} break;

			case 0xe4: {
				mDrawAreaX2 = mCommandBuffer[0] & 1023;
				if (mGpuVersion == 2) {
					mDrawAreaY2 = (mCommandBuffer[0] >> 10) & 0x3ff;
				} else {
					mDrawAreaY2 = (mCommandBuffer[0] >> 12) & 0x3ff;
				}
#if defined (LOG_GPU_COMMANDS)
				csl->out("GPU command 0xe4: drawing area bottom right %d,%d\n", mDrawAreaX2, mDrawAreaY2);
#endif
			} break;

			case 0xe5: {
				mDrawOffsetX = mCommandBuffer[0] & 0x7ff;
				if (mGpuVersion == 2) {
					mDrawOffsetY = (mCommandBuffer[0] >> 11) & 0x7ff;
				} else {
					mDrawOffsetY = (mCommandBuffer[0] >> 12) & 0x7ff;
				}
#if defined (LOG_GPU_COMMANDS)
				csl->out("GPU command 0xe5: drawing offset %d,%d\n", mDrawOffsetX, mDrawOffsetY);
#endif
			} break;

			case 0xe6: {
				mStatusReg &= ~(3 << 0xb);
				mStatusReg |= (mCommandBuffer[0] & 0x03) << 11;			
#if defined (LOG_GPU_COMMANDS)
				csl->out("GPU command 0xe6: mask setting (mask2: %d, Mask1: %d)\n", 
					(mCommandBuffer[0] & 2) >> 1, mCommandBuffer[0] & 1);
#endif
			} break;

			default:
#if defined (LOG_GPU_COMMANDS)
				csl->out(CRED, "Unimplemented GPU command 0x%02x\n", mCommandBuffer[0] >> 24);
#endif
				mGpuBufferOffset = 0;
				break;
		}
	}
}

void CGpu::WriteFramebufferImage() {
	glDisable(GL_TEXTURE_2D);

	u16 width = mCommandBuffer[2] & 0xffff;
	u16 height = mCommandBuffer[2] >> 16;
	u16 cx = mCommandBuffer[1] & 0xffff;
	u16 cy = mCommandBuffer[1] >> 16;

	glRasterPos2d(cx,cy);
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, mImageBuffer);
}

void CGpu::FillFramebufferRect() {
	glDisable(GL_TEXTURE_2D);

	u32 bgr = mCommandBuffer[0]; 
	u16 cx = mCommandBuffer[1] & 0x3ff;
	u16 cy = (mCommandBuffer[1] >> 16) & 0x1ff;
	u16 width = mCommandBuffer[2] & 0xffff;
	u16 height = mCommandBuffer[2] >> 16;

	if (cx == 0x3ff) cx++;
	if (cy == 0x1ff) cy++;

	glBegin(GL_QUADS);
		glColor3ub(bgr & 0xff,
				  (bgr >> 8) & 0xff,
				  (bgr >> 16) & 0xff);
		glVertex2s(cx,cy);
		glVertex2s(cx+width,cy);
		glVertex2s(cx+width,cy+height);
		glVertex2s(cx,cy+height);
	glEnd();
}

void CGpu::Gouraud3PointPoly() {
	glDisable(GL_TEXTURE_2D);

	VertexG *v = (VertexG*)&mCommandBuffer[0];
	
	vtxg[0].col = v[0].col;
	vtxg[1].col = v[1].col;
	vtxg[2].col = v[2].col;
	vtxg[0].x = v[0].x + mDrawOffsetX;
	vtxg[0].y = v[0].y + mDrawOffsetY;
	vtxg[1].x = v[1].x + mDrawOffsetX;
	vtxg[1].y = v[1].y + mDrawOffsetY;
	vtxg[2].x = v[2].x + mDrawOffsetX;
	vtxg[2].y = v[2].y + mDrawOffsetY;

	glBegin(GL_TRIANGLES);
		glColor3ubv((GLubyte*)&vtxg[0].col);
		glVertex2sv((GLshort*)&vtxg[0].xy);
		glColor3ubv((GLubyte*)&vtxg[1].col);
		glVertex2sv((GLshort*)&vtxg[1].xy);
		glColor3ubv((GLubyte*)&vtxg[2].col);
		glVertex2sv((GLshort*)&vtxg[2].xy);
	glEnd();
}

/* processed as two 3 point polygons */
void CGpu::Gouraud4PointPoly() {
	glDisable(GL_TEXTURE_2D);

	VertexG *v = (VertexG*)&mCommandBuffer[0];

	vtxg[0].col = v[0].col;
	vtxg[1].col = v[1].col;
	vtxg[2].col = v[2].col;
	vtxg[3].col = v[3].col;
	vtxg[0].x = v[0].x + mDrawOffsetX;
	vtxg[0].y = v[0].y + mDrawOffsetY;
	vtxg[1].x = v[1].x + mDrawOffsetX;
	vtxg[1].y = v[1].y + mDrawOffsetY;
	vtxg[2].x = v[2].x + mDrawOffsetX;
	vtxg[2].y = v[2].y + mDrawOffsetY;
	vtxg[3].x = v[3].x + mDrawOffsetX;
	vtxg[3].y = v[3].y + mDrawOffsetY;

	glBegin(GL_TRIANGLES);
		glColor3ubv((GLubyte*)&vtxg[0].col);
		glVertex2sv((GLshort*)&vtxg[0].xy);
		glColor3ubv((GLubyte*)&vtxg[1].col);
		glVertex2sv((GLshort*)&vtxg[1].xy);
		glColor3ubv((GLubyte*)&vtxg[2].col);
		glVertex2sv((GLshort*)&vtxg[2].xy);
		glColor3ubv((GLubyte*)&vtxg[2].col);
		glVertex2sv((GLshort*)&vtxg[2].xy);
		glColor3ubv((GLubyte*)&vtxg[1].col);
		glVertex2sv((GLshort*)&vtxg[1].xy);
		glColor3ubv((GLubyte*)&vtxg[3].col);
		glVertex2sv((GLshort*)&vtxg[3].xy);
	glEnd();
}

/* processed as two 3 point polygons */
void CGpu::Mono4PointPoly() {
	glDisable(GL_TEXTURE_2D);

	Vertex *v = (Vertex*)&mCommandBuffer[1];
	u32 bgr = mCommandBuffer[0];

	glBegin(GL_TRIANGLES);
		glColor3ub( bgr & 0xff,
					(bgr >> 8) & 0xff,
					(bgr >> 16) & 0xff);
		glVertex2sv((GLshort*)&v[0].xy);
		glVertex2sv((GLshort*)&v[1].xy);
		glVertex2sv((GLshort*)&v[2].xy);
		glVertex2sv((GLshort*)&v[2].xy);
		glVertex2sv((GLshort*)&v[1].xy);
		glVertex2sv((GLshort*)&v[3].xy);
	glEnd();
}

void CGpu::Textured4PointPoly() {
	glEnable(GL_TEXTURE_2D);

	VertexT *v = (VertexT*)&mCommandBuffer[1];
	static u16 teximage[256*256];
	static u16 resimage[256*256];

	UpdateTexturePage(v[1].txinfo);

	vtxt[0].u = v[0].u; vtxt[0].v = v[0].v;
	vtxt[1].u = v[1].u; vtxt[1].v = v[1].v;
	vtxt[2].u = v[2].u; vtxt[2].v = v[2].v;
	vtxt[3].u = v[3].u; vtxt[3].v = v[3].v;
	vtxt[0].x = v[0].x + mDrawOffsetX;
	vtxt[0].y = v[0].y + mDrawOffsetY;
	vtxt[1].x = v[1].x + mDrawOffsetX;
	vtxt[1].y = v[1].y + mDrawOffsetY;
	vtxt[2].x = v[2].x + mDrawOffsetX;
	vtxt[2].y = v[2].y + mDrawOffsetY;
	vtxt[3].x = v[3].x + mDrawOffsetX;
	vtxt[3].y = v[3].y + mDrawOffsetY;

	/* 4-bit clut */
	if (mTexPageInfo.tp == 0) {
		u16 palette[256];
		u16 clutx  = (v[0].txinfo & 0x3f) << 4;
		u16 cluty = (v[0].txinfo >> 6 ) & 0x3ff;

		/* read palette and texture data */
		glReadPixels(clutx, cluty, 16, 1, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, palette);
		glReadPixels(mTexPageInfo.tx, mTexPageInfo.ty, 64, 64, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, teximage);		

		/* do colour lookup conversion */
		for (int i = 0; i < 256*256; i += 4) {
			resimage[i] = palette[teximage[i >> 2] & 0xf];
			resimage[i+1] = palette[(teximage[i >> 2] >> 4) & 0xf];
			resimage[i+2] = palette[(teximage[i >> 2] >> 8) & 0xf];
			resimage[i+3] = palette[(teximage[i >> 2] >> 12) & 0xf];
		}

		/* update texture */
		glBindTexture(GL_TEXTURE_2D, mCacheTexID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, resimage);

		//csl->out("%d %d\n", mTexPageInfo.tx, mTexPageInfo.ty);

		/*
		glBegin(GL_TRIANGLES);
			glTexCoord2f(0.0, 0.0);
			glVertex2sv((GLshort*)&vtxt[0].xy);

			glTexCoord2f(0.25, 0.0);
			glVertex2sv((GLshort*)&vtxt[1].xy);

			glTexCoord2f(0.25, 0.25);
			glVertex2sv((GLshort*)&vtxt[2].xy);

			glTexCoord2f(0.25, 0.25);
			glVertex2sv((GLshort*)&vtxt[2].xy);

			glTexCoord2f(0.25, 0.0);
			glVertex2sv((GLshort*)&vtxt[1].xy);

			glTexCoord2f(0.0, 0.25);
			glVertex2sv((GLshort*)&vtxt[3].xy);
		glEnd();
		*/
		//for (int i = 0; i < 4; i++) {
		//	csl->out("u%d=%d, v%d=%d\n", i, vtxt[i].u, i, vtxt[i].v);
		//}

		//glDisable(GL_TEXTURE_2D);
		//glPointSize(3.0);
		//glBegin(GL_POINTS);
		//	glColor4f(1.0f,0.0f,0.0f,1.0f);
		//	glVertex2i(vtxt[0].u, vtxt[0].v);
		//	glVertex2i(vtxt[1].u, vtxt[1].v);
		//	glVertex2i(vtxt[2].u, vtxt[2].v );
		//	glVertex2i(vtxt[3].u, vtxt[3].v);
		//glEnd();

		//Sleep(100);

		//UpdateScreen();

	} 
	/* 8-bit clut */
	else if (mTexPageInfo.tp == 1) {
		MessageBox(NULL, "8-bit clut\n", "", NULL);
	} 
	/* 15-bit direct */
	else if (mTexPageInfo.tp == 2) {
		MessageBox(NULL, "15-bit direct\n", "", NULL);
	}
}

void CGpu::Sprite() {
	glEnable(GL_TEXTURE_2D);

	u16 x = (mCommandBuffer[1] & 0x3ff) + mDrawOffsetX;
	u16 y = ((mCommandBuffer[1] >> 16) & 0x1ff) + mDrawOffsetY;
	u16 width = mCommandBuffer[3] & 0x3ff;
	u16 height = (mCommandBuffer[3] >> 16) & 0x1ff;
	u32 imgsize = width*height;
	static u16 teximage[256*256];
	static u16 resimage[256*256];

	/* 4-bit clut */
	if (mTexPageInfo.tp == 0) {
		u16 palette[256];
		u16 clutx  = ((mCommandBuffer[2] >> 16) & 0x3f) << 4;
		u16 cluty = ((mCommandBuffer[2] >> 16) >> 6 ) & 0x3ff;

		/* read palette and texture data */
		glReadPixels(clutx, cluty, 16, 1, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, palette);
		glReadPixels(mTexPageInfo.tx, mTexPageInfo.ty, width >> 2, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, teximage);

		/* do colour lookup conversion */
		for (int i = 0; i < imgsize; i += 4) {
			resimage[i] = palette[teximage[i >> 2] & 0xf];
			resimage[i+1] = palette[(teximage[i >> 2] >> 4) & 0xf];
			resimage[i+2] = palette[(teximage[i >> 2] >> 8) & 0xf];
			resimage[i+3] = palette[(teximage[i >> 2] >> 12) & 0xf];
		}

		/* update texture */
		glBindTexture(GL_TEXTURE_2D, mCacheTexID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, resimage);

		glBegin(GL_QUADS);
			glColor4f(1.0f,1.0f,1.0f,1.0f);

			glTexCoord2f(0,(float)height/CGpu::VRAM_HEIGHT);
			glVertex2i(x,y+height);

			glTexCoord2f((float)width/CGpu::VRAM_WIDTH,(float)height/CGpu::VRAM_HEIGHT);
			glVertex2i(x+width,y+height);

			glTexCoord2f((float)width/CGpu::VRAM_WIDTH,0);
			glVertex2i(x+width,y);

			glTexCoord2f(0,0);
			glVertex2i(x,y);
		glEnd();
	} 
	/* 8-bit clut */
	else if (mTexPageInfo.tp == 1) {
		//MessageBox(NULL, "8-bit clut", "", NULL);
	} 
	/* 15-bit direct */
	else if (mTexPageInfo.tp == 2) {
		//MessageBox(NULL, "15-bit direct", "", NULL);
	}
}

/* save state related stuff */
void CGpu::CheckVramTransferRequest() {
	if (mSaveVram) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gl->mRenderTextureID);
		glReadPixels(0, 0, CGpu::VRAM_WIDTH, CGpu::VRAM_HEIGHT, 
			GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 
			mVramImagePtr);
		mSaveVram = FALSE;
	}
	if (mRestoreVram) {
		glBindTexture(GL_TEXTURE_2D, gl->mRenderTextureID);
		glEnable(GL_TEXTURE_2D);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
			CGpu::VRAM_WIDTH, CGpu::VRAM_HEIGHT, 
			GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 
			mVramImagePtr);
		mRestoreVram = FALSE;
	}
}

void CGpu::UpdateScreen() {
	CheckVramTransferRequest();

	gl->RenderFrameBufferObject();

	glViewport(0,0, gl->mFboWidth, gl->mFboHeight);
	glOrtho(0, gl->mFboWidth, 0, gl->mFboHeight, 0, 1);

	glDisable(GL_DEPTH_TEST);
}

/*
	type 1
	f  e| d| c  b| a  9| 8  7| 6  5| 4| 3  2  1  0
		|ti|     |   tp|  abr|   ty|  |         tx

	type 2
	f  e| d  c| b| a  9| 8  7| 6  5| 4| 3  2  1  0
		|iy|ix|ty|     |   tp|  abr|ty|         tx
*/
void CGpu::UpdateTexturePage(u16 data) {
	if(mGpuVersion == 2) {
		mStatusReg = (mStatusReg & 0xfffff800 ) | (data & 0x7ff);

		mTexPageInfo.tx = (data & 0x0f) << 6;
		mTexPageInfo.ty = ((data & 0x10) << 4) | ((data & 0x800) >> 2);
		mTexPageInfo.abr = (data & 0x60) >> 5;
		mTexPageInfo.tp = (data & 0x180) >> 7;
		mTexPageInfo.ix = (data & 0x1000) >> 12;
		mTexPageInfo.iy = (data & 0x2000) >> 13;
		mTexPageInfo.ti = 0;
		//if( ( tpage & ~0x39ff ) != 0 )
		//{
		//	verboselog( machine(), 1, "not handled: draw mode %08x\n", tpage & ~0x39ff );
		//}
		//if( n_tp == 3 )
		//{
		//	verboselog( machine(), 0, "not handled: tp == 3\n" );
		//}
	} else {
		mStatusReg = (mStatusReg & 0xffffe000) | (data & 0x1fff);

		mTexPageInfo.tx = (data & 0x0f) << 6;
		mTexPageInfo.ty = ((data & 0x60) << 3 );
		mTexPageInfo.abr = (data & 0x180) >> 7;
		mTexPageInfo.tp = (data & 0x600) >> 9;
		mTexPageInfo.ti = (data & 0x2000) >> 13;
		mTexPageInfo.ix = 0;
		mTexPageInfo.iy = 0;
		//if( ( tpage & ~0x27ef ) != 0 )
		//{
		//	verboselog( machine(), 1, "not handled: draw mode %08x\n", tpage & ~0x27ef );
		//}
		//if( n_tp == 3 )
		//{
		//	verboselog( machine(), 0, "not handled: tp == 3\n" );
		//}
		//else if( n_tp == 2 && n_ti != 0 )
		//{
		//	verboselog( machine(), 0, "not handled: interleaved 15 bit texture\n" );
		//}
	}
}

#if 0
#pragma pack(push, 1)
struct TGA_HEADER {
	u8  identsize;		  // size of ID field that follows 18 u8 header (0 usually)
	u8  colourmaptype;	  // type of colour map 0=none, 1=has palette
	u8  imagetype;		  // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	s16 colourmapstart;	 // first colour map entry in palette
	s16 colourmaplength;	// number of colours in palette
	u8  colourmapbits;	  // number of bits per palette entry 15,16,24,32

	s16 xstart;			 // image x origin
	s16 ystart;			 // image y origin
	s16 width;			  // image width in pixels
	s16 height;			 // image height in pixels
	u8  bits;			   // image bits per pixel 8,16,24,32
	u8  descriptor;		 // image descriptor bits (vh flip bits)
	
	// pixel data follows header
};
#pragma pack(pop)


bool SaveTGA(const char *filename, int width, int height, void *pdata) {
	TGA_HEADER hdr;
	FILE* f = fopen(filename, "wb");
	if( f == NULL )
		return false;

	if ( sizeof(TGA_HEADER) != 18 || sizeof(hdr) != 18) {
		MessageBox(NULL, "", "", NULL);
	}
	
	memset(&hdr, 0, sizeof(hdr));
	hdr.imagetype = 2;
	hdr.bits = 32;
	hdr.width = width;
	hdr.height = height;
	hdr.descriptor |= 8|(1<<5); // 8bit alpha, flip vertical

	fwrite(&hdr, sizeof(hdr), 1, f);
	fwrite(pdata, width*height*4, 1, f);
	fclose(f);
	return true;
}


void CGpu::SaveVramTga(const char *path) {
	u32 *pVramImage = new u32 [1024*512];

	glBindTexture(GL_TEXTURE_2D, gl->mRenderTextureID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pVramImage); 

	SaveTGA(path, 1024, 512, pVramImage);

	delete [] pVramImage;
}
#endif