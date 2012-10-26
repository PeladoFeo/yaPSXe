#pragma once
#ifndef BIOS_H
#define BIOS_H

#include "Common.h"

#define BIOS_SIZE	0x80000	/* 512KB */

enum {
	NTSC_JAP = 0,
	NTSC_USA,
	PAL_EURO,
};

#define REV_A	0
#define REV_B	1
#define REV_C	2

struct BiosCalls {
	int address;
	int operation;
	const char *prototype;
};

struct BiosInfo {
	char *name;
	u32 checksum;
	int region;
	u32 hwRevision;
};

extern const BiosInfo gBiosData[];
extern const BiosCalls bioscalls[];
static const u32 terminator = 0xdeadbeef;

u32 CheckSum(u8 *data, u32 size);

#endif /* BIOS_H */