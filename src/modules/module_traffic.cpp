#include <cstdlib>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <map>
#include "../module.h"

const char* traffic_howto = "traffic usage:----------------";
const char* NET_DEV = "/proc/net/dev";

unsigned long long B  = 1;
unsigned long long KB = 1024*B;
unsigned long long MB = 1024*KB;
unsigned long long GB = 1024*MB;
unsigned long long TB = 1024*GB;
unsigned long long PB = 1024*TB;
unsigned long long EB = 1024*PB;

class module_traffic:public module
{
public:
	module_traffic():
		module("module_traffic",traffic_howto)
	{
		traffic_map.clear();	
		//std::cout<<"构造traffic模块。。。"<<std::endl;
	}
	virtual ~module_traffic()
	{
		traffic_map.clear();
		//std::cout<<"析构traffic模块。。。"<<std::endl;
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
			out<<iter->first<<":"<<iter->second.byteIn<<","<<iter->second.byteOut<<","<<iter->second.packetIn<<","<<iter->second.packetOut<<" ";
		}
	}

	virtual void print(int level)
	{
		if(!enable()) return;
		if(PRINT_SUMMARY == level)
		{
			std::cout<<"  "<<to_verbose(total_traffic.byteIn)<<" "<<to_verbose(total_traffic.byteOut);
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}
	virtual std::string get_dbstr()
	{
		std::ostringstream ss;
		ss<<"trafficIn = '";
		ss<<to_verbose(total_traffic.byteIn);
		ss<<"',trafficOut = '";
		ss<<to_verbose(total_traffic.byteOut);
		ss<<"'";
		return ss.str();
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
		std::string s;
		traffic_map.clear();
		while(getline(fin,s))
		{
			std::string ss = removeLeftTrim(s); 
			if(0==ss.compare(0,3,"eth")||0==ss.compare(0,2,"em")||0==ss.compare(0,3,"eno")||0==ss.compare(0,5,"venet"))
			{
				int index = ss.find(':',0);
				traffic_status_s ts;
				memset(&ts,0,sizeof(traffic_status_s));
				std::istringstream input(ss.substr(index+1,std::string::npos));
				unsigned long long tmp = 0;
				input>>ts.byteIn>>ts.packetIn>>tmp>>tmp>>tmp>>tmp>>tmp>>tmp>>ts.byteOut>>ts.packetOut;
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

	std::string to_verbose(unsigned long long traffic)
	{
		std::ostringstream result;
		if(traffic >= EB)
			result<<traffic/EB<<"EB";
		else if(traffic >= PB && traffic < EB)
			result<<traffic/PB<<"PB";
		else if(traffic >= GB && traffic < PB)
			result<<traffic/GB<<"GB";
		else if(traffic >= MB && traffic < GB)
			result<<traffic/MB<<"MB";
		else if(traffic >= KB && traffic <MB)
			result<<traffic/KB<<"KB";
		else
			result<<traffic<<"B";
		return result.str();
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

	std::string removeLeftTrim(const std::string& s)
	{
		size_t startPos  = getLeftTrimPos(s);
		return s.substr(startPos,std::string::npos);
	}
	size_t getLeftTrimPos(const std::string& s)
	{
		size_t pos = 0;
		std::string::const_iterator iter = s.begin();
		for(; iter != s.end(); iter++)
		{
			if(*iter == ' '|| *iter == '	')
				pos++;
			else
				break;

		}
		return pos;
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
