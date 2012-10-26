#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <time.h>
#include <gl\glew.h>

//#define GLOBAL_DISABLE_CSL_OUT

class CPsx;
class CMemory;
class CpuState;
class CGpu;
class CSpu;
class Console;
class CInterpreter;
class CWindow;
class CBreakpoints;
class CpuDebugger;
class CGLRenderer;
class Config;
struct BiosInfo;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned __int64 vcu64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef __int64 vcs64;

#endif /* COMMON_H */