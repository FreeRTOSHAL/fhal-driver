#include <FreeRTOS.h>
#include <stdio.h>
#include <unwind.h>
_Unwind_Reason_Code trace_fcn(_Unwind_Context *ctx, void *d)
{
	int *depth = (int*)d;
	uintptr_t pc = _Unwind_GetIP(ctx);
#ifdef CONFIG_ARCH_JUMP_ADDR_ODD
	if (pc == (((uintptr_t) &taskReturnFunction) - 1)) {
#else
	if (pc == ((uintptr_t) &taskReturnFunction)) {
#endif
		printf("\t#%d: End of Stack\n", *depth);
		return _URC_END_OF_STACK;
	}
	printf("\t#%d: program counter at 0x%x\n", *depth, pc);
	(*depth)++;
	return _URC_NO_REASON;
}
void backtrace() {
	int depth = 0;
	printf("Backtrace:\n");
	_Unwind_Backtrace(&trace_fcn, &depth);
}
