#include <cstdlib>
#include <sstream>
#include <cstring>
#include <vector>
#include <fcntl.h>
#include "../module.h"

const char* process_howto = "process usage:.................";


class module_process:public module
{
public:
	module_process():
		module("module_process",process_howto)
	{
		bad_list.clear();
		process_list.clear();
		//std::cout<<"构造process模块。。。"<<std::endl;
	}
	virtual ~module_process()
	{
		bad_list.clear();
		process_list.clear();
		//std::cout<<"析构process模块。。。"<<std::endl;
	}
	virtual void init_module(std::vector<std::string>& v)
	{
		process_list.clear();
		std::vector<std::string>::iterator iter = v.begin();
		for(; iter != v.end(); iter++)
			process_list.push_back(*iter);
	}
	virtual void collect_data()
	{
		if(!enable()) return;
		bad_list.clear();
		std::vector<std::string>::iterator iter = process_list.begin();
		for(; iter != process_list.end(); iter++)
		{
			if(read_process_stat(*iter) <= 0 )
				bad_list.push_back(*iter);
		}
	}
	virtual void save_file(std::ofstream& out)
	{
		if(!enable()) return;
		out<<"|process:";
		std::vector<std::string>::iterator iter = bad_list.begin();
		for(; iter != bad_list.end(); iter++)
			out<<*iter<<" ";
	}
	virtual void print(int level)
	{
		if(!enable()) return;
		if(PRINT_SUMMARY == level)
		{
			std::cout<<"  "<<bad_list.size()<<"/"<<process_list.size();
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}
	virtual std::string get_dbstr()
	{
		std::ostringstream ss;
		std::string result = "";
		std::vector<std::string>::iterator iter = bad_list.begin();
		for(; iter != bad_list.end(); iter++)
		{
			result += *iter;
			result += ',';
		}
		if(bad_list.size() > 0)
			result.erase(result.length()-1,1);
		ss<<"badproc = '";
		ss<<result;
		ss<<"'";
		return ss.str();
	}

private:
	std::vector<std::string> bad_list; /* stores the processes that are corrupt */	
	std::vector<std::string> process_list; /* stores the processes that are checked */

private:
	unsigned int read_process_stat(std::string proc)
	{
		FILE* fs = NULL;
		char buff[64] = {'\0'};
		unsigned int counter = 0;
		std::string cmd = "pgrep "+proc;
		cmd += " 2>/dev/null";
		fs = popen(cmd.c_str(),"r");
		if(NULL == fs)
		{
			return 0;
		}
		while(fgets(buff,sizeof(buff),fs))
		{
			counter++;
		}
		pclose(fs);
		return counter;
	}

	void debug_print()
	{

	}
};


extern "C"{
//创建类
module* create()
{
	return new module_process;
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
