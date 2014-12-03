#ifndef _ZEBRA_COMMON_H__
#define _ZEBRA_COMMON_H__

extern int RUN_NULL;
extern int RUN_LIVE;
extern int RUN_CRON;
extern const int COLLECT_INTERVAL;
extern int PRINT_SUMMARY;
extern int PRINT_DETAIL;

extern const char* DEFAULT_SAVE_FILENAME;
extern const char* DEFAULT_CONF_FILENAME;
extern const char* DEFAULT_MODULES_PATH;
extern const char* STAT;
extern const char* MEMINFO;
extern const char* CPUINFO;
extern const char* DEFAULT_SO_PATH;

extern int INFO;
extern int DEBUG;
extern int WARNING;
extern int ERROR;
extern int CRITICAL;

#endif
