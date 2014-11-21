#include <cstdlib>
#include "../module.h"

const char* cpu_usage = "cpu usage:--------------------";

class module_cpu:public module
{
public:
	module_cpu():
		module("module_cpu",cpu_usage)
	{
	}

	~module_cpu()
	{
	}
	virtual void collect_data()
	{
	}
	virtual void save_file(ofstream& out)
	{
	}
	virtual void print(int level)
	{
	}


private:
	struct cpu_status_s{
		unsigned long long cpu_user; /* user cpu time */
		unsigned long long cpu_nice; /* cpu priority time */
		unsigned long long cpu_sys;  /* cpu system time */
		unsigned long long cpu_idle;  /* cpu idle time */
		unsigned long long cpu_iowait; /* hardware io waiting time */
		unsigned long long cpu_hardirq; /* hard interrupt*/
		unsigned long long cpu_softirq; /* soft interrupt */
		unsigned long long cpu_steal; /* vm time */
		unsigned long long cpu_guest; /* guest time */
		unsigned long long cpu_number; /* cpu numbers */
	};
	cpu_status_s cpu_status;


private:
	void read_cpu_stat()
	{
	}
};