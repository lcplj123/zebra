#include <fstream>
#include <iostream>
#include "config.h"
#include "common.h"

configure::configure(const char* filename):
	run_state(RUN_NULL),
	conf_name(filename),
	interval(COLLECT_INTERVAL),
	modules_num(0),
	print_mode(PRINT_SUMMARY),
	save_file_name(DEFAULT_SAVE_FILENAME),
	db_ip(""),
	db_port(0),
	db_url("")
{
	enable_modules_list.clear();
	db_module_list.clear();
	confMap.clear();
	
}

configure::~configure()
{
	enable_modules_list.clear();
	db_module_list.clear();
}

void configure::parse_config_file()
{
	std::string s;
	std::ifstream fin(conf_name.c_str());
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
		if(token.find("module_",0) != std::string::npos)
		{
			parse_module(token,iter->second);	
		}
		else if (0 == token.find("#",0))
		{
			continue;
		}
	}

}





