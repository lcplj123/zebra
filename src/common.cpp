const int RUN_NULL = 0;
const int RUN_LIVE = 1;
const int RUN_CRON = 2;

const int COLLECT_INTERVAL = 300;

enum {
	PRINT_SUMMARY = 0,
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
	INFO = 0,
	DEBUG,
	WARNING,
	ERROR,
	CRITICAL,
};

