#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include "config.h"
#include "common.h"

configure::configure(const char* filename):
	run_state(RUN_NULL),
	conf_name(filename),
	interval(COLLECT_INTERVAL),
	modules_num(0),
	print_mode(PRINT_SUMMARY),
	output_file_path(DEFAULT_SAVE_FILENAME),
	db_ip(""),
	db_port(0),
	db_url(""),
	debug_level(CRITICAL)
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
	std::ifstream fin(path.c_str());
	while(getline(fin,s))
		getSplit(s,confMap);

	fin.close();
}

void configure::getSplit(const std::string& s,std::map<std::string,std::string>& mapconf)
{
	size_t pos = 0;
	std::string trimstr = removeTrim(s);
	std::string::const_iterator iter = trimstr.begin();
	for(; iter != trimstr.begin(); iter++)
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
		mapconf.insert(std::make_pair(key,value));
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
	return pos;
}

void configure::parse_line()
{
	std::map<std::string,std::string>::iterator iter = confMap.begin();
	for(; iter != confMap.end(); iter++)
	{
		std::string token = iter->first;
		if(std::string::npos != token.find("module_",0))
		{
			parse_module(token,iter->second);	
		}
		else if (0 == token.find("#",0))
		{
			continue;
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
		else if(token == "output_db_module")
		{

		}
		else if(token == "output_db_addr")
		{
			db_ip = iter->second;
		}
		else if(token == "output_db_port")
		{
			unsigned int dbport;
			std::stringstream ss(iter->second);
			ss>>dbport;
			db_port = dbport; 
		}
		else if(token == "output_url")
		{
			db_url = iter->second;	
		}
		else if(token == "include")
		{
			get_include_conf(iter->second);
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



