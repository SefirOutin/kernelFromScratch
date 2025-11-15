#include "printk.h"
#include "lib.h"

const char *log_level_str[] = {
    "INFO",
    "WARN",
    "ERROR"
};

int printk(enum log_level log_level, const char *fmt, ...)
{
	int		len = 0;
	va_list	args;

	va_start(args, fmt);

	printf("[%s]: ", log_level_str[log_level]);

	len = vprintf(fmt, args);

	va_end(args);
	return (len);
}