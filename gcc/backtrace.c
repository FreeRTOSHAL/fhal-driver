#include <stdio.h>
#include <unwind.h>
_Unwind_Reason_Code trace_fcn(_Unwind_Context *ctx, void *d)
{
	static uintptr_t lastpc = 0x0;
	static uint32_t counter = 0x0;
	int *depth = (int*)d;
	uintptr_t pc = _Unwind_GetIP(ctx);
	if (pc == lastpc) {
		if (counter++ >= 1) {
			printf("\t#%d: Endless Loop\n", *depth);
			return _URC_END_OF_STACK;
		}
	} else {
		counter = 0;
	}
	printf("\t#%d: program counter at 0x%x\n", *depth, pc);
	(*depth)++;
	lastpc = pc;
	return _URC_NO_REASON;
}
void backtrace() {
	int depth = 0;
	printf("Backtrace:\n");
	_Unwind_Backtrace(&trace_fcn, &depth);
}
