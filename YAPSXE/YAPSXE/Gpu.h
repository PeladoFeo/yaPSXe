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
#ifndef GPU_H
#define GPU_H

#include "Common.h"
#include "Psx.h"

/* Gouraud vertex */
struct VertexG {
	union {
		u32 col;
		struct {
			unsigned r : 8;
			unsigned g : 8;
			unsigned b : 8;
			unsigned : 8;
		};
	};
	union {
		u32 xy;
		struct {
			signed x : 16;
			signed y : 16;
		};
	};
};

/* textured vertex */
struct VertexT {
	union {
		u32 xy;
		struct {
			signed x : 16;
			signed y : 16;
		};
	};
	union {
		u32 tex;
		struct {
			unsigned u : 8;
			unsigned v : 8;
			unsigned txinfo : 16;	// Clut ID/Texture Page
		};
	};
};

/* monochrome vertex */
struct Vertex {
	union {
		u32 xy;
		struct {
			unsigned x : 16;
			unsigned y : 16;
		};
	};
};

struct TexPageInfo {
	u16 tx;
	u16 ty;
	u16 abr;
	u16 tp;
	u16 ix;
	u16 iy;
	u16 ti;
};

class PsxGpu {
public:
	PsxGpu();
	~PsxGpu();

	void CheckVramTransferRequest();
	void InitClassPointers();
	void InitGpu();
	void ResetState();
	u32 ReadStatus();
	void WriteStatus(u32 data);
	u32 ReadData();
	void WriteData(u32 *pData, u32 size);
	void UpdateScreen();
	void UpdateTexturePage(u16 data);

	void WriteFramebufferImage();
	void FillFramebufferRect();

	void Gouraud3PointPoly();
	void Gouraud4PointPoly();

	void Mono4PointPoly();
	void Textured4PointPoly();

	void Sprite();

#if 0
	void SaveVramTga(const char *path);
#endif 

	static const u32 VRAM_WIDTH = 1024;
	static const u32 VRAM_HEIGHT = 512;

	BOOL mSaveVram;
	BOOL mRestoreVram;
	u16 *mVramImagePtr;

	u32 mCacheTexID;

	u32 mStatusReg;
	u32 mGpuBufferOffset;
	u32 mCommandBuffer[16];

	u32 mDrawOffsetX;
	u32 mDrawOffsetY;

	u32 mDrawAreaX1;
	u32 mDrawAreaY1;
	u32 mDrawAreaX2;
	u32 mDrawAreaY2;

	u32 mDispStartX;
	u32 mDispStartY;

	u32 mHorDispRangeStart;
	u32 mHorDispRangeEnd;
	u32 mVertDispRangeStart;
	u32 mVertDispRangeEnd;

	// for vram transfer operations
	u16 mImageBuffer[1024*512];
	u32 mVramPosX;
	u32 mVramPosY;

	u32 mGpuVersion;
	u32 mGpuInfo;

	TexPageInfo mTexPageInfo;

	Vertex vtx[4];
	VertexG vtxg[4];
	VertexT vtxt[4];

	Console *csl;
	CGLRenderer *gl;
	CPsx *psx;
};

#endif /* GPU_H */