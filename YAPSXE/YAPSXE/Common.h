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
#include <shlobj.h>
#include <objbase.h>

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