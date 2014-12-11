#include <cstdlib>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include "../module.h"

const char* mem_howto = "memory usage---------------";
const char* MEMINFO = "/proc/meminfo";

class module_mem:public module
{
public:
	module_mem():
		module("module_mem",mem_howto),
		mem_usage(0.0)
	{
		std::cout<<"构造mem模块。。。"<<std::endl;
	}

	virtual ~module_mem()
	{
		std::cout<<"析构mem模块。。。"<<std::endl;
	}

	virtual void collect_data()
	{
		if(!enable()) return;
		read_mem_stat();
		caculate_mem();
		//debug_print();
	}
	virtual void save_file(std::ofstream& out)
	{
		if(!enable()) return;
		out<<"|mem:"<<mem_status.total<<","<<mem_status.free<<","<<mem_status.buff<<","<<mem_status.cached;

	}
	virtual void print(int level)
	{
		if(!enable()) return;
		if(PRINT_SUMMARY == level)
		{
			std::cout<<"  "<<std::setprecision(3)<<mem_usage;
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}
	virtual std::string get_dbstr()
	{
		std::ostringstream ss;
		ss<<"memUsage = ";
		ss<<mem_usage;
		return ss.str();
	}

private:
	struct mem_status_s{  //in KB
		unsigned long total; //所有可用内存大小(去除一些预留位和内核二进制代码占用)
		unsigned long free; //未使用的内存
		unsigned long buff; //给文件做缓冲的大小
		unsigned long cached; //被高速缓冲存储器使用的内存大小
		unsigned long active;  //在活跃使用中的缓冲或高速缓冲存储器页面文件的大小
		unsigned long inactive; //在不经常使用中的缓冲或高速缓冲存储器页面文件的大小
		unsigned long slab;  //内核数据结构缓存的大小，可以减少申请和释放内存带来的大小
		unsigned long swaptotal;  //交换空间的总大小
		unsigned long swapfree;  //未被使用的交换空间的大小
		unsigned long swapcache;  //
		unsigned long committed;
	};
	mem_status_s mem_status;
	float mem_usage;	

private:
	void read_mem_stat()
	{
		std::ifstream fin(MEMINFO);
		memset(&mem_status,0,sizeof(mem_status_s));
		std::string ss;
		std::istringstream input;
		while(getline(fin,ss))
		{
			if(0 == ss.compare(0,9,"MemTotal:"))
			{
				input.clear();
				input.str(ss.substr(9));
				input>>mem_status.total;
			}
			else if(0 == ss.compare(0,8,"MemFree:"))
			{
				input.clear();
				input.str(ss.substr(8));
				input>>mem_status.free;
			}
			else if(0 == ss.compare(0,8,"Buffers:"))
			{
				input.clear();
				input.str(ss.substr(8));
				input>>mem_status.buff;
			}
			else if(0 == ss.compare(0,7,"Cached:"))
			{
				input.clear();
				input.str(ss.substr(7));
				input>>mem_status.cached;
			}
			else if(0 == ss.compare(0,7,"Active:"))
			{
				input.clear();
				input.str(ss.substr(7));
				input>>mem_status.active;
			}
			else if(0 == ss.compare(0,9,"Inactive:"))
			{
				input.clear();
				input.str(ss.substr(9));
				input>>mem_status.inactive;
			}
			else if(0 == ss.compare(0,5,"Slab:"))
			{
				input.clear();
				input.str(ss.substr(5));
				input>>mem_status.slab;
			}
			else if(0 == ss.compare(0,11,"SwapCached:"))
			{
				input.clear();
				input.str(ss.substr(11));
				input>>mem_status.swapcache;
			}
			else if(0 == ss.compare(0,10,"SwapTotal:"))
			{
				input.clear();
				input.str(ss.substr(10));
				input>>mem_status.swaptotal;
			}
			else if(0 == ss.compare(0,9,"SwapFree:"))
			{
				input.clear();
				input.str(ss.substr(9));
				input>>mem_status.swapfree;
			}
			else if(0 == ss.compare(0,13,"Committed_AS:"))
			{
				input.clear();
				input.str(ss.substr(13));
				input>>mem_status.committed;
			}
		}
		fin.close();
	}

	void caculate_mem()
	{
		unsigned int usedMem = mem_status.total - mem_status.free - mem_status.buff - mem_status.cached;
		mem_usage = 100.0*usedMem/mem_status.total;
	}

	void debug_print()
	{
		std::cout<<"total: "<<mem_status.total<<std::endl;
		std::cout<<"free: "<<mem_status.free<<std::endl;
		std::cout<<"buffers: "<<mem_status.buff<<std::endl;
		std::cout<<"cached: "<<mem_status.cached<<std::endl;
		std::cout<<"usage: "<<mem_usage<<std::endl;
	}


};

//so导出类的函数
extern  "C"{
//创建函数
module* create()
{
	return new module_mem;
}

//销毁函数
void release(module* p)
{
	if(p)
	{
		delete p;
		p = NULL;
	}
}
}
