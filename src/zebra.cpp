#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include <iostream>
//#include "common.h"
#include "config.h"
#include "modmgr.h"

enum{
	RUN_NULL,
	RUN_LIVE,
	RUN_CRON,
};
enum {
	PRINT_SUMMARY,
	PRINT_DETAIL,
};

const char* DEFAULT_SAVE_FILENAME = "/var/log/zebra.data";
//const char* conf_path = "/etc/zebra/zebra.conf";
const char* conf_path = "/root/git/zebra/conf/zebra.conf";

const char* shortopt = ":lci:";
struct option longopt[] = {
	{"help",no_argument,NULL,'h'},
	{"detail",no_argument,NULL,'d'},
	{"cron",no_argument,NULL,'c'},
	{"live",no_argument,NULL,'l'},
	{"version",no_argument,NULL,'V'},
	{0,0,0,0},
	
};

const char* CLIENT_VERSION = "0.1";

//参数用法
void usage()
{
	std::cout<<"用法如下："<<std::endl;
	std::cout<<"--help "<<"获取帮助信息"<<std::endl;
	std::cout<<"--cron "<<"以计划任务运行程序"<<std::endl;
	std::cout<<"--live "<<"以现场模式运行程序"<<std::endl;
	std::cout<<"--version "<<"获取客户端版本"<<std::endl;
	std::cout<<"--detail "<<"展示详细信息，默认是概要信息"<<std::endl;
	std::cout<<"-c "<<"以计划任务运行程序"<<std::endl;
	std::cout<<"-l "<<"以现场模式运行程序"<<std::endl;
}

//输入参数处理
bool opt_init(int argc,char** argv,configure& conf)
{
	int opt = 0;
	int long_index = 0;
	//std::cout<<"解析输入参数"<<std::endl;
	while((opt = getopt_long(argc,argv,shortopt,longopt,&long_index)) != -1)
	{
		switch(opt)
		{
			case 'c':
				conf.run_state = RUN_CRON;
				break;
			case 'l':
				conf.run_state = RUN_LIVE;
				break;
			case 'd':
				conf.print_mode = PRINT_DETAIL;
				break;
			case 'i':
				break;
			case 'h':
				usage();
				return false;
				break;
			case 'V':
				std::cout<<CLIENT_VERSION<<std::endl;
				return false;
				break;
			case ':': //缺少选项参数
				std::cout<<"缺少参数。。。"<<std::endl;
				conf.run_state = RUN_NULL;
				usage();
				return false;
				break;
			case '?': //包含无效选项
				std::cout<<"包含无效选项"<<std::endl;
				conf.run_state = RUN_NULL;
				usage();
				return false;
				break;
		}
	}
	//std::cout<<"解析输入参数 结束"<<std::endl;
	return true;
}

void run_cron(configure& conf,modmgr& mgr)
{
	std::ofstream out(conf.output_file_path.c_str(),std::ios::app);	
	mgr.collect_data();
	mgr.save_file(out);
	mgr.write_db();
	out.close();
}

void run_live(configure& conf,modmgr& mgr)
{
	while(1)
	{
		std::ofstream out(conf.output_file_path.c_str(),std::ios::app);
		mgr.collect_data();
		std::vector<std::string>::iterator iter = conf.enable_modules_list.begin();
		for(; iter != conf.enable_modules_list.end(); iter++)
		{
			std::cout<<"--"<<(*iter).substr(7,std::string::npos)<<"-- ";
		}
		std::cout<<std::endl;
		mgr.print(conf.print_mode);
		std::cout<<std::endl;
		mgr.save_file(out);
		mgr.write_db();
		out.close();
		sleep(conf.interval);
	}

}


int main(int argc,char** argv)
{
	//load config
	configure conf(conf_path);
	conf.parse_config_file();
	//conf.load_from_db(); //从远程数据库加载参数
	conf.load_from_url(); //从远程url加载参数
	//conf.debug_print();

	//parse opt
	if(!opt_init(argc,argv,conf))
		return 0;

	//load module manager
	modmgr mgr(&conf);
	mgr.load_modules(conf.modules_path);
	//mgr.print_modules();

	//start
	switch(conf.run_state)
	{
		case RUN_CRON:
			run_cron(conf,mgr);
			break;
		case RUN_LIVE:
			run_live(conf,mgr);
			break;
		case RUN_NULL:
			break;
	}
	return 0;
}
