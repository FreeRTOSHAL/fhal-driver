#include <signal.h>
#include <errno.h>
int _kill (int  pid, int  sig) {
	errno = EINVAL;
	return -1;
}
