#include <vector>
#include <cstring>
#include <string>
#include <cstdio>
#include <dlfcn.h>
#include <ctime>
#include "modmgr.h"
#include "config.h"
#include "module.h"
#include "mysqlconn.h"
//#include "common.h"

const char* DEFAULT_MODULES_PATH = "/usr/local/zebra/modules/";
//const char* DEFAULT_MODULES_PATH = "/root/git/zebra/build/modules/";

modmgr::modmgr(configure* _conf):
	conf(NULL)
{
	conf = _conf;
	so_list.clear();
	modules_list.clear();
}

modmgr::~modmgr()
{
	conf = NULL;
	freemodules();
	so_list.clear();
	modules_list.clear();
}

bool modmgr::load_modules(std::string path)
{
	if(conf == NULL) return false;
	char buf[256] = {'\0'};
	std::vector<std::string>::iterator iter = conf->enable_modules_list.begin();
	//std::cout<<"加载模块中:"<<conf->modules_path<<std::endl;
	for(; iter != conf->enable_modules_list.end(); iter++)
	{
		memset(buf,'\0',sizeof(buf));
		sprintf(buf,"%slib%s.so",DEFAULT_MODULES_PATH,(*iter).c_str());
		//load module lib
		void* lib = dlopen(buf,RTLD_NOW|RTLD_GLOBAL);
		if(!lib) 
		{
			std::cout<<"加载模块失败。。。"<<buf<<std::endl;
			std::cout<<dlerror()<<std::endl;
			continue;
		}
		so_list.insert(make_pair(*iter,lib));
		
		//load module class
		new_module mod_xxx =(new_module)dlsym(lib,"create");
		if(dlerror())
		{
			std::cout<<"加载xxx失败..."<<*iter<<std::endl;
			continue;
		}
		module* p = mod_xxx();
		modules_list.insert(make_pair(*iter,p));

	}

	//std::cout<<"加载模块结束。。。。"<<std::endl;
	init_modules();

	return true;
}

bool modmgr::collect_data()
{
	std::map<std::string,module*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		module* p = iter->second;
		if(p)
		{
			p->collect_data();
		}
	}
	return true;
}

bool modmgr::save_file(std::ofstream& out)
{
	time_t cur_time = time(NULL);
	out<<cur_time;	
	std::map<std::string,module*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		module* p = iter->second;
		if(p)
		{
			p->save_file(out);
		}
	}
	out<<std::endl;
	return true;
}

bool modmgr::print(int level)
{
	std::map<std::string,module*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		module* p = iter->second;
		if(p)
		{
			p->print(level);
		}
	}
	return true;
}

bool modmgr::write_db()
{
	std::vector<std::string>::iterator iter = conf->output_interface.begin();
	for(; iter != conf->output_interface.end(); iter++)
	{
		if(*iter == "db")
		{
			write_to_mysql();
		}
		if(*iter == "url")
		{
			write_to_url();
		}
	}

	return true;
}
void modmgr::freemodules()
{
	std::map<std::string,void*>::iterator iter = so_list.begin();
	for(; iter != so_list.end();iter++)
	{
		if(iter->second)
		{
			release_module re = (release_module)dlsym(iter->second,"release");
			if(!re)
			{
				std::cout<<"release 没找到..."<<iter->first<<std::endl;
			}
			std::map<std::string,module*>::iterator finder = modules_list.find(iter->first);
			if(finder != modules_list.end() && re)
			{
				re(finder->second);
			}
			dlclose(iter->second);
			iter->second = NULL;
		}
	}
}

void modmgr::print_modules()
{
	std::map<std::string,void*>::iterator iter = so_list.begin();
	std::map<std::string,module*>::iterator iter1 = modules_list.begin();
	std::cout<<"----------------------"<<std::endl;
	for(; iter != so_list.end(); iter++)
	{
		std::cout<<"key= "<<iter->first<<" value= "<<iter->second<<std::endl;
	}
	for(; iter1 != modules_list.end(); iter1++)
	{
		std::cout<<"key = "<<iter1->first<<" value= "<<iter1->second<<std::endl;
	}
	std::cout<<"----------------------"<<std::endl;

}

bool modmgr::write_to_mysql()
{
	if(conf->db_module_list.size() <= 0) return false;
	MysqlConn conn;
	if(!conn.connect(conf->db_ip.c_str(),conf->db_port,conf->db_user.c_str(),conf->db_passwd.c_str(),conf->db_name.c_str()))
	{
		std::cout<<"connect mysql error!"<<std::endl;
		return false;
	}

	std::string sql = "update " + conf->db_tabname + " set  ";
	std::vector<std::string>::iterator iter = conf->db_module_list.begin();
	for(; iter != conf->db_module_list.end(); iter++)
	{
		std::map<std::string,module*>::iterator miter = modules_list.find(*iter);
		if(miter == modules_list.end())
			continue;
		module* p = miter->second;
		sql.append(p->get_dbstr());
		sql.append(",");
	}
	sql.erase(sql.length()-1,1);
	sql.append(" where " + conf->db_index + " = '" + conf->db_key + "'");
	std::cout<<"sql = "<<sql<<std::endl;

	const char* s = sql.c_str();
	conn.execute(s,strlen(s));
	
	return true;
}

bool modmgr::write_to_url()
{
	return true;
}

void modmgr::init_modules()
{
	std::map<std::string,module*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		if(iter->first == "module_process")
			iter->second->init_module(conf->process_list);
	}
}
