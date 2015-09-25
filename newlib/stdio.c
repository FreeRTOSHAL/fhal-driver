#include <newlib_stub.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <uart.h>
#define UART_PRV
#include <uart_prv.h>

static struct uart *in = NULL;
static struct uart *out = NULL;

int32_t newlib_init(struct uart *std_in, struct uart *std_out) {
	in = std_in;
	out = std_out;
	return 0;
}

/**
 * close - close a file descriptor
 * 
 * \param file File No
 * \return -1 not Supported
 */
int _close(int file) {
	(void) file;
	errno = EPERM;
	return -1;
}

/**
 * write - write to a file descriptor
 * 
 * \param file File No
 * \param ptr Pointer to data
 * \param len size of data
 * \return On success, the number of bytes written is returned (zero indicates nothing was written).
 * On error, -1 is returned, and errno is set appropriately.
 */
int _write(int file, char *data, int len) {
	int i;
	int32_t ret;
	struct uart *uart;
	if (file == 1 && out == NULL) {
		errno = EBADF;
		goto _write_error_0;
	}
	if (file == 1) {
		uart = out;
	} else {
		/* write to a anther file as stdout not implement now */
		errno = EBADF;
		goto _write_error_0;
	}
	ret = uart_lock(uart, portMAX_DELAY);
	if (ret != 1) {
		errno = EIO;
		goto _write_error_0;
	}
	for (i = 0; i < len; i++, data++) {
#ifdef CONFIG_NEWLIB_UART_NEWLINE
		/* replace \n with \r\n only if file == stdout */
		if (file == 1 && *data == '\n') {
			ret = uart_putc(uart, '\r', portMAX_DELAY);
			if (ret < 0 ) {
				errno = EIO;
				goto _write_error_1;
			}
		}
#endif
		ret = uart_putc(uart, *data, portMAX_DELAY);
		if (ret < 0 ) {
			errno = EIO;
			goto _write_error_1;
		}
	}
	ret = uart_unlock(uart);
	if (ret != 1) {
		errno = EIO;
		goto _write_error_0;
	}
	return i;
_write_error_1:
	(void) uart_unlock(uart);
_write_error_0:
	return -1;
}
/**
 * fstat - get file status
 * 
 * Status of an open file. For consistency with other minimal implementations in these examples, 
 * all files are regarded as character special devices. The sys/stat.h header file required is 
 * distributed in the include subdirectory for this C library.
 * 
 * \param file File No
 * \param st File Stat
 * \return 0 minimal Implementation
 */
int _fstat(int file, struct stat *st) {
	if (file == 1) {
		st->st_mode = S_IFCHR;
		return 0;
	} else {
		errno = EBADF;
		return -1;
	}
}
/**
 * isatty - test whether a file descriptor refers to a terminal
 * 
 * Query whether output stream is a terminal. For consistency with the other minimal implementations, 
 * which only support output to stdout, this minimal implementation is suggested
 * 
 * \param file File No
 * \return 
 */
int _isatty(int file) {
	if (file == 1) {
		return 1;
	} else {
		errno = EBADF;
		return -1;
	}
}
/**
 * lseek - reposition read/write file offset
 * 
 * Set position in a file.
 * 
 * Not implement yet
 *
 * \param file File No
 * \param ptr offset
 * \param dir SEEK_SET, SEEK_CUR, SEEK_END
 * \return new offset
 */
int _lseek(int file, int ptr, int dir) {
	(void) ptr;
	(void) dir;
	if (file == 1) {
		errno = ESPIPE;
		return -1;
	} else {
		errno = EBADF;
		return -1;
	}
}
/**
 * read - read from a file descriptor
 *
 * Not implement yet
 * 
 * \param File No
 * \param ptr char pointer
 * \param len count
 * \return On  success, the number of bytes read is returned (zero indicates end of file), and the file 
 *         position is advanced by this number.  It is not an error if this number is smaller than the 
 *         number of bytes requested; this may happen for example because fewer bytes are  actually  
 *         available  right  now  (maybe because we were close to end-of-file, or because we are reading 
 *         from a pipe, or from a terminal), or because read() was interrupted by a signal.  On error, 
 *         -1 is returned, and errno is set appropriately.  In this case it is  left  unspecified  
 *         whether the file position (if any) changes.
 */
int _read(int file, char *ptr, int len) {
	(void) file;
	(void) ptr;
	(void) len;
	(void) in;
	errno = EPERM;
	return -1;
}
