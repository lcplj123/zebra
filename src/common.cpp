#ifndef _ZEBRA_COMMON_H__
#define  _ZEBRA_COMMON_H__

enum {
	RUN_NULL,
	RUN_LIVE,
	RUN_CRON,
};

const int COLLECT_INTERVAL = 300;

enum {
	PRINT_SUMMARY,
	PRINT_DETAIL,
};

const char* DEFAULT_SAVE_FILENAME = "/var/log/zebra.data";
const char* DEFAULT_CONF_FILENAME = "/etc/zebra/zebra.conf";
const char* DEFAULT_MODULES_PATH = "/usr/bin/zebra/";
const char* STAT = "/proc/stat";
const char* MEMINFO = "/proc/meminfo";
const char* CPUINFO = "/proc/cpuinfo";
const char* DEFAULT_SO_PATH = "/usr/local/zebra/modules";

enum {
	INFO,
	DEBUG,
	WARNING,
	ERROR,
	CRITICAL,
};

#endif // _ZEBRA_COMMON_H__
