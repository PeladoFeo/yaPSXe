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
#ifndef ROOT_COUNTERS_H
#define ROOT_COUNTERS_H

#include "Common.h"
#include "Psx.h"

class RootCounters {
public:
	RootCounters();
	void InitClassPointers();

	u32 ReadCurrent(int n);
	u32 ReadMode(int n);
	u32 ReadTarget(int n);

	void WriteCurrent(int n, u32 data);
	void WriteMode(int n, u32 data);
	void WriteTarget(int n, u32 data);

private:
	u32 count[4];
	u32 mode[4];
	u32 target[4];

	CPsx *psx;
	Console *csl;
};

#endif /* ROOT_COUNTERS_H */