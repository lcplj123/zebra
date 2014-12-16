#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include "../module.h"

const char* load_howto = "load usage:-------------------";
const char* LOADINFO = "/proc/loadavg";

class module_load:public module
{

public:
	module_load():
		module("module_load",load_howto)
	{
		//std::cout<<"构造load模块。。。"<<std::endl;
	}
	virtual ~module_load()
	{
		//std::cout<<"析构load模块。。。"<<std::endl;
	}
	virtual void collect_data()
	{
		if(!enable()) return;
		read_load_stat();
	}
	virtual void save_file(std::ofstream& out)
	{
		if(!enable()) return;
		out<<"|load:"<<load.loadavg_1<<","<<load.loadavg_5<<","<<load.loadavg_15;
	}
	virtual void print(int level)
	{
		if(!enable()) return;
		if(PRINT_SUMMARY == level)
		{
			std::cout<<"  "<<load.loadavg_1;	
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}
	virtual std::string get_dbstr()
	{
		std::ostringstream ss;
		ss<<"load = ";
		ss<<load.loadavg_5;
		return ss.str();
	}

private:
	//如果每个核的进程数大于5，表示负载有点高
	struct load_status_s{
		float loadavg_1; //1分中之内的平均进程数
		float loadavg_5; //5分钟之内平均进程数量
		float loadavg_15; //15分钟内的平均进程数量
		unsigned int nr_running; //采用时刻，运行队列的任务数
		unsigned int nr_threads; //采样时候系统中活跃的任务个数
	};
	load_status_s load;

private:
	void read_load_stat()
	{
		std::ifstream fin(LOADINFO);
		memset(&load,0,sizeof(load_status_s));
		std::string ss;
		while(getline(fin,ss))
		{
			if(ss.length() > 0)
			{
				std::istringstream input(ss);
				input>>load.loadavg_1>>load.loadavg_5>>load.loadavg_15;
				break;
			}
		}

		fin.close();
	}

};


extern "C"{
//创建类
module* create()
{
	return new module_load;
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
