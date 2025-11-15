#ifndef _PRINTK_H_
#define _PRINTK_H_

enum log_level {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
};

int printk(enum log_level log_level, const char *fmt, ...);


#endif