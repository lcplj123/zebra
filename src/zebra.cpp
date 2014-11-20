#include "config.h"


const char* conf_path = "/root/zebra.conf";
int main(int argc,char** argv)
{
	configure conf(conf_path);
	conf.parse_config_file();
	conf.debug_print();


	return 0;
}
