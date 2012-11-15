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
 
#include "RootCounters.h"
#include "Psx.h"
#include "Console.h"

/* 
	todo
*/

RootCounters::RootCounters() {
}

void RootCounters::InitClassPointers() {
	psx = CPsx::GetInstance();
	csl = psx->csl;
}

u32 RootCounters::ReadCurrent(int n) {
	return 0;
}

u32 RootCounters::ReadMode(int n) {
	return 0;
}

u32 RootCounters::ReadTarget(int n) {
	return 0;
}

void RootCounters::WriteCurrent(int n, u32 data) {
	count[n] = data;
}

void RootCounters::WriteMode(int n, u32 data) {
}

void RootCounters::WriteTarget(int n, u32 data) {
}