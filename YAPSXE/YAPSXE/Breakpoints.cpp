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