#include <fstream>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <curl/curl.h>
#include "config.h"
#include "ifaddr.h"
#include "mysqlconn.h"
//#include "common.h"

enum {
	RUN_NULL,
	RUN_LIVE,
	RUN_CRON,
};
const int COLLECT_INTERVAL = 10;
enum {
	INFO,
	DEBUG,
	WARNING,
	ERROR,
	CRITICAL,
};
enum{
	PRINT_SUMMARY,
	PRINT_DETAIL,
};

extern const char* DEFAULT_SAVE_FILENAME;
extern const char* DEFAULT_CONF_FILENAME;
extern const char* DEFAULT_MODULES_PATH;
extern const char* CLIENT_VERSION;
const char* process_url = "http://122.11.33.188:5678/monitor/request/proclist";

configure::configure(const char* filename):
	clientVersion(CLIENT_VERSION),
	run_state(RUN_LIVE),
	conf_name(filename),
	interval(COLLECT_INTERVAL),
	debug_level(CRITICAL),
	print_mode(PRINT_SUMMARY),
	output_file_path(DEFAULT_SAVE_FILENAME),
	modules_path(DEFAULT_MODULES_PATH),
	db_ip(""),
	db_port(3306),
	db_name(""),
	db_user(""),
	db_passwd(""),
	db_tabname(""),
	db_url(""),
	ip("127.0.0.1")
{
	enable_modules_list.clear();
	db_module_list.clear();
	confMap.clear();
	output_interface.clear();
	process_list.clear();	
	std::string _ip = getinterface();
	if(_ip != "")
		ip = _ip;
}

configure::~configure()
{
	enable_modules_list.clear();
	db_module_list.clear();
	output_interface.clear();
	process_list.clear();
}

void configure::parse_config_file(std::string path)
{
	std::string s;
	if (path != "")
		conf_name = path;

	//检查文件是否存在
	if(access(conf_name.c_str(),F_OK)!=0)
	{
		std::cout<<"文件不存在"<<conf_name<<std::endl;
		return;
	}

	std::ifstream fin(conf_name.c_str());
	while(getline(fin,s))
		getSplit(s);

	//parse every line
	parse_line();
	fin.close();
}

void configure::load_from_db()
{
	MysqlConn conn;
	if(!conn.connect(db_ip.c_str(),db_port,db_user.c_str(),db_passwd.c_str(),db_name.c_str()))
	{
		std::cout<<"load from db error!"<<std::endl;
		return;
	}
	std::string tmp;
	std::string sql = "select processList from machineinfo where ip = '" + ip + "'";
	const char* s = sql.c_str();
	conn.execute(s,strlen(s));
	conn.getResult();
	MYSQL_ROW row;
	if(row = conn.nextRow())
	{
		tmp = row[0];
	}
	//std::cout<<"ttttttttt "<<tmp<<std::endl;
	splitBy(tmp,',',process_list);
}

void configure::load_from_url()
{
	std::string tmp = "";
	CURL* easy_handle = NULL;
	CURLcode retcode;
	curl_global_init(CURL_GLOBAL_ALL);
	easy_handle = curl_easy_init();
	if(NULL == easy_handle)
	{
		std::cout<<"configure::init curl error!"<<std::endl;
		curl_global_cleanup();
		return;
	}
	curl_easy_setopt(easy_handle,CURLOPT_URL,process_url);
	curl_easy_setopt(easy_handle,CURLOPT_POST,1);
	std::string post = "ip="+ip;
	curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS,post.c_str());
	curl_easy_setopt(easy_handle,CURLOPT_CONNECTTIMEOUT,5);
	curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,&tmp);
	retcode = curl_easy_perform(easy_handle);
	if(CURLE_OK != retcode)
	{
		std::cout<<"configure:curl perform error.."<<post.c_str()<<std::endl;
		curl_easy_cleanup(easy_handle);
		curl_global_cleanup();
		return;
	
	}
	curl_easy_cleanup(easy_handle); 
	curl_global_cleanup();
	splitBy(tmp,',',process_list);	
	
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
			if(iter->second.find("db") != std::string::npos)
				output_interface.push_back("db");
			if(iter->second.find("url") != std::string::npos)
				output_interface.push_back("url");
		}
		else if(token == "interval")
		{
			int val = atoi(iter->second.c_str());
			interval = val >=5? val:5;
		}
		else if(token == "output_file_path")
		{
			output_file_path = iter->second;
		}
		else if(token == "modules_path")
		{
			//modules_path = iter->second;
		}
		else if(token == "output_db_modules")
		{
			if(iter->second == "all")
			{
				copy_to_db();
			}
			else
			{
				splitBy(iter->second,',',db_module_list);				
			}
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
		else if(token == "output_db_name")
		{
			db_name = iter->second;
		}
		else if(token == "output_db_user")
		{
			db_user = iter->second;
		}
		else if(token == "output_db_passwd")
		{
			db_passwd = iter->second;
		}
		else if(token == "output_db_tablename")
		{
			db_tabname = iter->second;
		}
		else if(token == "output_url")
		{
			db_url = iter->second;	
		}
		else if(token == "processes")
		{
			//splitBy(iter->second,',',process_list);
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
	std::cout<<"modules_path:  "<<modules_path<<std::endl;
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
		std::cout<<*iter3<<",";
	}
	std::cout<<std::endl<<"check process: ";
	for(iter3 = process_list.begin(); iter3 != process_list.end(); iter3++)
	{
		std::cout<<" "<<*iter3; 
	}
	std::cout<<std::endl;

	
}

bool configure::reload(const char* path)
{
	if (NULL != path)
		conf_name = std::string(path);
	run_state = RUN_NULL;
	interval = COLLECT_INTERVAL;
	confMap.clear();
	debug_level = CRITICAL;
	output_interface.clear();
	print_mode = PRINT_SUMMARY;
	output_file_path = DEFAULT_SAVE_FILENAME;
	modules_path = DEFAULT_MODULES_PATH;
	db_module_list.clear();
	db_ip = "";
	db_port = 3306;
	db_url = "";
	parse_config_file();
	return true;
}

void configure::copy_to_db()
{
	std::vector<std::string>::iterator iter = enable_modules_list.begin();
	for(; iter != enable_modules_list.end(); iter++)
	{
		db_module_list.push_back(*iter);	
	}
}

void configure::splitBy(std::string& str,char delim,std::vector<std::string>& v)
{
	size_t pos = 0;
	size_t index = 0;
	while((index = str.find_first_of(delim,pos)) != std::string::npos)
	{
		std::string tmp = removeTrim(str.substr(pos,index - pos));
		if(tmp.length() > 0)
			v.push_back(tmp);
		pos = index + 1;
	}
	std::string tmp = removeTrim(str.substr(pos));
	if(tmp.length() > 0)
		v.push_back(tmp);
}

size_t configure::write_callback(void* buff,size_t size,size_t nmemb,std::string* userdata)
{
	std::string tmp((char*)buff);
	userdata->append((char*)buff,size*nmemb);
	return size*nmemb;
}

// for test use
/*
int main()
{
	configure f("/root/zebra.conf");
	f.parse_config_file();	
	f.debug_print();
	return 0;
}
*/
