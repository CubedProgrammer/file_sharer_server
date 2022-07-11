#ifndef Included_logging_h
#define Included_logging_h

void logstr(const char *str);
void logfmt(const char *fmt, ...);
void log_endmsg(void);
int init_logger(const char *logfn);
void free_logger(void);

#endif
