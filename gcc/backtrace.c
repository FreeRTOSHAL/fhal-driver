#include <stdio.h>
#include <unwind.h>
_Unwind_Reason_Code trace_fcn(_Unwind_Context *ctx, void *d)
{
	int *depth = (int*)d;
	printf("\t#%d: program counter at 0x%x\n", *depth, _Unwind_GetIP(ctx));
	(*depth)++;
	return _URC_NO_REASON;
}
void backtrace() {
	int depth = 0;
	printf("Backtrace:\n");
	_Unwind_Backtrace(&trace_fcn, &depth);
}
