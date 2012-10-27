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
 
#include "Breakpoints.h"
#include "Common.h"

void CBreakpoints::AddBreakpoint(u32 addr) {
	vBreakpoints.push_back(addr);
}

void CBreakpoints::RemoveBreakpoint(u32 addr) {
	int size;
	if ((size = vBreakpoints.size()) == 0) 
		return;
	for (int i = 0; i < size; i++) {
		if (vBreakpoints[i] == addr) {
			vBreakpoints.erase(vBreakpoints.begin()+i);
			size--;
		}
	}
}

BOOL CBreakpoints::CheckBreakpoint(u32 addr) {
	int size;
	if ((size = vBreakpoints.size()) == 0) 
		return FALSE;
	for (int i = 0; i < size; i++) {
		if (vBreakpoints[i] == addr)
			return TRUE;
	}
	return FALSE;
}