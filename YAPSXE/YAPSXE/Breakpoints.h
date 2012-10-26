#ifndef CBreakpoints_H
#define CBreakpoints_H

#include "Common.h"

class CBreakpoints {
public:
	void AddBreakpoint(u32 addr);
	void RemoveBreakpoint(u32 addr);
	BOOL CheckBreakpoint(u32 addr);

	std::vector<u32> vBreakpoints;
};

#endif /* CBreakpoints_H */
