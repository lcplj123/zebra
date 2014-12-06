#include <cstdlib>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <map>
#include "../module.h"

const char* traffic_howto = "traffic usage:----------------";
const char* NET_DEV = "/proc/net/dev";


class module_traffic:public module
{
public:
	module_traffic():
		module("module_traffic",traffic_howto)
	{
		traffic_map.clear();	
		std::cout<<"构造traffic模块。。。"<<std::endl;
	}
	virtual ~module_traffic()
	{
		traffic_map.clear();
		std::cout<<"析构traffic模块。。。"<<std::endl;
	}

	virtual void collect_data()
	{
		if(!enable()) return;
		read_traffic_stat();
		caculate_traffic();
		//debug_traffic();
	}

	virtual void save_file(std::ofstream& out)
	{
		if(!enable()) return;
		out<<"|traffic:";
		std::map<std::string,traffic_status_s>::iterator iter = traffic_map.begin();
		for(; iter != traffic_map.end(); iter++)
		{
			out<<iter->first<<":"<<iter->second.packetIn<<","<<iter->second.byteOut<<","<<iter->second.packetIn<<","<<iter->second.packetOut<<" ";
		}
	}

	virtual void print(int level)
	{
		if(!enable()) return;
		if(PRINT_SUMMARY == level)
		{
			std::cout<<"  "<<total_traffic.byteIn<<" "<<total_traffic.byteOut;
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}


private:
	struct traffic_status_s{
		unsigned long long byteIn;
		unsigned long long byteOut;
		unsigned long long packetIn;
		unsigned long long packetOut;
	};
	std::map<std::string,traffic_status_s> traffic_map;
	traffic_status_s total_traffic;

private:
	void read_traffic_stat()
	{
		std::ifstream fin(NET_DEV);
		std::string ss;
		traffic_map.clear();
		while(getline(fin,ss))
		{
			if(0==ss.compare(0,3,"eth")||0==ss.compare(0,2,"em")||0==ss.compare(0,3,"eno")||0==ss.compare(0,5,"venet"))
			{
				int index = ss.find(':',0);
				traffic_status_s ts;
				memset(&ts,0,sizeof(traffic_status_s));
				std::istringstream input(ss.substr(index+1,std::string::npos));
				//std::cout<<"XXXXXXXXXXX"<<input.str()<<std::endl;
				unsigned long long tmp = 0;
				input>>ts.byteIn>>ts.packetIn>>tmp>>tmp>>tmp>>tmp>>tmp>>tmp>>ts.byteOut>>ts.packetOut;
				//std::cout<<"YYYYYYYYYYY"<<ts.byteIn<<"  "<<ts.byteOut<<"  "<<ts.packetIn<<"  "<<ts.packetOut<<std::endl;
				traffic_map.insert(std::make_pair(ss.substr(0,index),ts));
			}
		}
		fin.close();
	}

	void caculate_traffic()
	{
		memset(&total_traffic,0,sizeof(traffic_status_s));
		std::map<std::string,traffic_status_s>::iterator iter = traffic_map.begin();
		for(; iter != traffic_map.end(); iter++)
		{
			total_traffic.byteIn += iter->second.byteIn;
			total_traffic.byteOut += iter->second.byteOut;
			total_traffic.packetIn += iter->second.packetIn;
			total_traffic.packetOut += iter->second.packetOut;
		}
	}

	void debug_traffic()
	{
		std::map<std::string,traffic_status_s>::iterator iter = traffic_map.begin();
		std::cout<<"size = "<<traffic_map.size()<<std::endl;
		for(; iter != traffic_map.end(); iter++)
		{
			std::cout<<iter->first<<":"<<iter->second.byteIn<<" "<<iter->second.byteOut<<" "<<iter->second.packetIn<<" "<<iter->second.packetOut<<std::endl;
		}
		std::cout<<"total:"<<total_traffic.byteIn<<" "<<total_traffic.byteOut<<" "<<total_traffic.packetIn<<" "<<total_traffic.packetOut<<std::endl;
	}
};

extern "C"{
//创建类
module* create()
{
	return new module_traffic;
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
