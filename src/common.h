
enum {
	RUN_NULL,
	RUN_PRINT,
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


enum {
	INFO,
	DEBUG,
	WARNING,
	ERROR,
	CRITICAL,
};
