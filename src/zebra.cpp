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

const char* DEFAULT_SAVE_FILENAME = "/root/zebra.data";
const char* conf_path = "/root/zebra.conf";
const char* shortopt = ":lci:";
struct option longopt[] = {
	{"help",no_argument,NULL,'h'},
	{"detail",no_argument,NULL,'d'},
	{"cron",no_argument,NULL,'c'},
	{"live",no_argument,NULL,'l'},
	{0,0,0,0},
	
};


//参数用法
void usage()
{
	std::cout<<"用法："<<std::endl;
}

//输入参数处理
void opt_init(int argc,char** argv,configure& conf)
{
	int opt = 0;
	int long_index = 0;
	std::cout<<"解析输入参数"<<std::endl;
	while((opt = getopt_long(argc,argv,shortopt,longopt,&long_index)) != -1)
	{
		switch(opt)
		{
			case 'c':
				conf.run_state = RUN_CRON;
				std::cout<<"run in cron..."<<std::endl;
				break;
			case 'l':
				conf.run_state = RUN_LIVE;
				std::cout<<"run in live....."<<std::endl;
				break;
			case 'd':
				conf.print_mode = PRINT_DETAIL;
				std::cout<<"run in detail....."<<std::endl;
				break;
			case 'i':
				break;
			case 'h':
				std::cout<<"run in help...."<<std::endl;
				usage();
				break;
			case ':': //缺少选项参数
				std::cout<<"缺少参数。。。"<<std::endl;
				conf.run_state = RUN_NULL;
				usage();
				break;
			case '?': //包含无效选项
				std::cout<<"包含无效选项"<<std::endl;
				conf.run_state = RUN_NULL;
				usage();
				break;
		}
	}
	std::cout<<"解析输入参数 结束"<<std::endl;

}

void run_cron(configure& conf,modmgr& mgr)
{
	std::ofstream out(DEFAULT_SAVE_FILENAME,std::ios::app);	
	mgr.collect_data();
	mgr.save_file(out);
	out.close();
}

void run_live(configure& conf,modmgr& mgr)
{
	while(1)
	{
		std::ofstream out(DEFAULT_SAVE_FILENAME,std::ios::app);
		mgr.collect_data();
		
		//mgr.save_file(out);
		out.close();
		sleep(conf.interval);
	}

}


int main(int argc,char** argv)
{
	//load config
	configure conf(conf_path);
	conf.parse_config_file();
	conf.debug_print();

	//parse opt
	opt_init(argc,argv,conf);

	//load module manager
	modmgr mgr(&conf);
	mgr.load_modules();
	mgr.print_modules();

	//start
	switch(conf.run_state)
	{
		case 2:
			run_cron(conf,mgr);
			break;
		case 1:
			run_live(conf,mgr);
			break;
		case 0:
			break;
	}
	return 0;
}
