#include <cstdlib>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include "../module.h"

const char* cpu_howto = "cpu usage:--------------------";
const char* STAT = "/proc/stat";
const char* CPUINFO = "/proc/cpuinfo";


class module_cpu:public module
{
public:
	module_cpu():
		module("module_cpu",cpu_howto),
		cpu_usage(0.0)
	{
		//std::cout<<"构造cpu模块。。。。"<<std::endl;
	}

	virtual	~module_cpu()
	{
		//std::cout<<"析构cpu模块。。。"<<std::endl;
	}
	virtual void collect_data()
	{
		if(!enable()) return;
		read_cpu_stat();
		caculate_cpu();
	}
	virtual void save_file(std::ofstream& out)
	{
		if(!enable()) return;
		out<<"|cpu:"<<cpu_status.cpu_user<<","<<cpu_status.cpu_nice<<","<<cpu_status.cpu_sys<<","<<cpu_status.cpu_idle<<","<<cpu_status.cpu_iowait<<","<<cpu_status.cpu_hardirq<<","<<cpu_status.cpu_softirq<<","<<cpu_status.cpu_steal<<","<<cpu_status.cpu_guest<<","<<cpu_status.cpu_number;
	}
	virtual void print(int level)
	{
		if(!enable()) return;
		if (PRINT_SUMMARY == level)
		{
			std::cout<<"  "<<std::setprecision(2)<<cpu_usage<<"/"<<cpu_status.cpu_number;
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}
	virtual std::string get_dbstr()
	{
		std::ostringstream ss;
		ss<<"cpuUsage = ";
		ss<<cpu_usage;
		return ss.str();
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
	cpu_status_s old_status;
	float cpu_usage;

private:
	void read_cpu_stat()
	{
		std::ifstream fin(STAT);
		memset(&old_status,0,sizeof(cpu_status_s));
		memset(&cpu_status,0,sizeof(cpu_status_s));
		std::string olds;
		std::string s;


		std::ifstream fcpu(CPUINFO);
		std::string ss;
		while(getline(fcpu,ss))
		{
			if(0 == ss.compare(0,9,"processor"))	
			{
				cpu_status.cpu_number++;
				old_status.cpu_number++;
			}
		}
		fcpu.close();
		while(getline(fin,olds))
		{
			if(0 == olds.compare(0,4,"cpu "))
			{
				//std::cout<<"1:"<<olds<<std::endl;
				std::istringstream input(olds.substr(5));
				input>>old_status.cpu_user>>old_status.cpu_nice>>old_status.cpu_sys>>old_status.cpu_idle>>old_status.cpu_iowait>>old_status.cpu_hardirq>>old_status.cpu_softirq>>old_status.cpu_steal>>old_status.cpu_guest;
				break;
			}
		}
	
		fin.close();
		sleep(2);
		fin.open(STAT);

		while(getline(fin,s))
		{
			if(0 == s.compare(0,4,"cpu "))	
			{
				std::istringstream input(s.substr(5));
				//std::cout<<"2:"<<s<<std::endl;
				input>>cpu_status.cpu_user>>cpu_status.cpu_nice>>cpu_status.cpu_sys>>cpu_status.cpu_idle>>cpu_status.cpu_iowait>>cpu_status.cpu_hardirq>>cpu_status.cpu_softirq>>cpu_status.cpu_steal>>cpu_status.cpu_guest;

				break;
			}
		}
		fin.close();
	}


	void caculate_cpu()
	{
		cpu_usage = 0.0;
		//total1 = user+nice+system+idle+iowait+irq+softirq+steal+guest
		//total2 = user+nice+system+idle+iowait+irq+softirq+steal+guest
		//cpu_usage = (idle[0]-idle[1])/(total1-total[2])
		unsigned int total1,total2;
		total1 = old_status.cpu_user + old_status.cpu_nice + old_status.cpu_sys + old_status.cpu_idle + old_status.cpu_iowait + old_status.cpu_hardirq + old_status.cpu_softirq + old_status.cpu_steal + old_status.cpu_guest;

		total2 = cpu_status.cpu_user + cpu_status.cpu_nice + cpu_status.cpu_sys + cpu_status.cpu_idle + cpu_status.cpu_iowait + cpu_status.cpu_hardirq + cpu_status.cpu_softirq + cpu_status.cpu_steal + cpu_status.cpu_guest;
		unsigned int total = total2 - total1;
		unsigned int idle = cpu_status.cpu_idle - old_status.cpu_idle;
		cpu_usage = 100.0*(total - idle) / total;
		//std::cout<<"total1 = "<<total1<<std::endl;
		//std::cout<<"total2 = "<<total2<<std::endl;
		//std::cout<<"idle = "<<idle<<std::endl;
		//std::cout<<"cpu_usage: "<<cpu_usage<<std::endl;

	}

	void debug_print(cpu_status_s& p)
	{
		std::cout<<"-------------------cpu----------------"<<std::endl;
		std::cout<<"-------old"<<std::endl;
		std::cout<<"cpu_user: "<<p.cpu_user<<std::endl;
		std::cout<<"cpu_nice: "<<p.cpu_nice<<std::endl;
		std::cout<<"cpu_sys: "<<p.cpu_sys<<std::endl;
		std::cout<<"cpu_idle: "<<p.cpu_idle<<std::endl;
		std::cout<<"cpu_iowait: "<<p.cpu_iowait<<std::endl;
		std::cout<<"cpu_softirq: "<<p.cpu_softirq<<std::endl;
		std::cout<<"cpu_hardirq: "<<p.cpu_hardirq<<std::endl;
		std::cout<<"cpu_number: "<<p.cpu_number<<std::endl;
		std::cout<<"-------------------end----------------"<<std::endl;

	}
};


extern "C"{
//创建类
module* create()
{
	return new module_cpu; 
}

//销毁类
void release(module* p)
{
	if(p)
	{
		delete p;
		p = NULL;
	}
}
}
