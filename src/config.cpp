#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "common.h"

configure::configure(const char* filename):
	run_state(RUN_NULL),
	conf_name(filename),
	interval(COLLECT_INTERVAL),
	modules_num(0),
	debug_level(CRITICAL),
	print_mode(PRINT_SUMMARY),
	output_file_path(DEFAULT_SAVE_FILENAME),
	db_ip(""),
	db_port(3306),
	db_url("")
{
	enable_modules_list.clear();
	db_module_list.clear();
	confMap.clear();
	output_interface.clear();
	
}

configure::~configure()
{
	enable_modules_list.clear();
	db_module_list.clear();
}

void configure::parse_config_file(std::string path)
{
	std::string s;
	if (path != "")
		conf_name = path;
	std::ifstream fin(conf_name.c_str());
	while(getline(fin,s))
		getSplit(s);

	//parse every line
	parse_line();
	fin.close();
}

void configure::getSplit(const std::string& s)
{
	size_t pos = 0;
	std::string trimstr = removeTrim(s);
	std::string::const_iterator iter = trimstr.begin();
	for(; iter != trimstr.end(); iter++)
	{
		if(*iter == ' ' || *iter == '	')
			break;
		else
			pos++;
	}
	std::string key = trimstr.substr(0,pos);
	std::string value = removeTrim(trimstr.substr(pos,trimstr.length()-pos));
	if(key != "" && value != "")
	{
		confMap.insert(std::make_pair(key,value));
	}
}


std::string configure::removeTrim(const std::string& s)
{
	size_t startPos = getLeftTrimPos(s);
	size_t endPos = getRightTrimPos(s);
	return s.substr(startPos,endPos);
}

size_t configure::getLeftTrimPos(const std::string& s)
{
	size_t pos = 0;
	std::string::const_iterator iter = s.begin();
	for(; iter != s.end(); iter++)
	{
		if(*iter == ' ' || *iter == '	')
			pos++;
		else
			break;
	}
	return pos;
}

size_t configure::getRightTrimPos(const std::string& s)
{
	size_t pos = 0;
	std::string::const_reverse_iterator iter = s.rbegin();
	for(; iter != s.rend(); iter++)
	{
		if(*iter == ' ' || *iter == '	')
			pos++;
		else
			break;
	}
	return s.length() - pos;
}

void configure::parse_line()
{
	std::map<std::string,std::string>::iterator iter = confMap.begin();
	for(; iter != confMap.end(); iter++)
	{
		std::string token = iter->first;
		//std::cout<<"key = "<<token<<"  value = "<<iter->second<<std::endl;
		if (0 == token.find("#",0))
		{
			continue;
		}
		else if (token == "debug_level")
		{
			if(iter->second == "INFO")
				debug_level = INFO;
			else if(iter->second == "DEBUG")
				debug_level = DEBUG;
			else if(iter->second == "WARNING")
				debug_level = WARNING;
			else if(iter->second == "ERROR")
				debug_level = ERROR;
			else 
				debug_level = CRITICAL;
		}
		else if(std::string::npos != token.find("module_",0))
		{
			parse_module(token,iter->second);	
		}
		else if(token == "output_interface")
		{
			if(iter->second.find("file"))
				output_interface.push_back("file");
			if(iter->second.find("db"))
				output_interface.push_back("db");
			if(iter->second.find("url"))
				output_interface.push_back("url");
		}
		else if(token == "output_file_path")
		{
			output_file_path = iter->second;
		}
		else if(token == "output_db_modules")
		{
			
		}
		else if(token == "output_db_addr")
		{
			db_ip = iter->second;
		}
		else if(token == "output_db_port")
		{
			unsigned int dbport = 0;
			dbport = atoi(iter->second.c_str());
			if(dbport != 0)
			{
				db_port = dbport; 
			}
		}
		else if(token == "output_url")
		{
			db_url = iter->second;	
		}
		else if(token == "include")
		{
			parse_include_conf(iter->second);
		}


	}

}

void configure::parse_module(std::string token,std::string value)
{
	if(value == "enable" || value == "on")
	{
		enable_modules_list.push_back(token);
	}
}

void configure::parse_include_conf(std::string path)
{
	FILE* fs = NULL;
	char buff[1024]={'\0'};
	std::string cmd = "ls "+path;
	cmd += " 2>/dev/null";
	fs = popen(cmd.c_str(),"r");
	if(NULL == fs)
		return;
	while(fgets(buff,sizeof(buff),fs))
	{
		std::string fullpath = path + std::string(buff);
		parse_config_file(fullpath);
	}

}

void configure::debug_print()
{
	std::cout<<"name: "<<conf_name<<std::endl;
	std::cout<<"run_mode: "<<run_state<<std::endl;
	std::cout<<"interval: "<<interval<<std::endl;
	std::cout<<"enable_modules_list: ";
	std::vector<std::string>::iterator iter1 = enable_modules_list.begin();
	for(;iter1 != enable_modules_list.end(); iter1++)
		std::cout<<"  "<<*iter1;
	std::cout<<std::endl;
	std::cout<<"debug_level: "<<debug_level<<std::endl;
	std::cout<<"-----------mapConf----------"<<std::endl;
	std::map<std::string,std::string>::iterator iter2 = confMap.begin();
	for(; iter2 != confMap.end();iter2++)
	{
		std::cout<<iter2->first<<": "<<iter2->second<<std::endl;
	}
	std::cout<<"-----------mapConf end----------"<<std::endl;
	std::cout<<"db_ip: "<<db_ip<<std::endl;
	std::cout<<"db_port: "<<db_port<<std::endl;
	std::cout<<"db_url: "<<db_url<<std::endl;
	std::vector<std::string>::iterator iter3 = db_module_list.begin();
	std::cout<<"db_module_list: ";
	for(; iter3 != db_module_list.end(); iter3++)
	{
		std::cout<<"  "<<*iter3;
	}
	std::cout<<std::endl;

	
	

	
}
/*
int main()
{
	configure f("/root/zebra.conf");
	f.parse_config_file();	
	f.debug_print();
	return 0;
}
*/
