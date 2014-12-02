#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include "config.h"
#include "modmgr.h"
#include "common.h"

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

}

void run_cron(configure& conf,modmgr& mgr)
{
	std::ofstream out(DEFAULT_SAVE_FILENAME,std::ios::app);	

}

void run_live(configure& conf,modmgr& mgr)
{
	

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
