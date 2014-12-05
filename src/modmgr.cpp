#include <vector>
#include <cstring>
#include <string>
#include <cstdio>
#include <dlfcn.h>
#include "modmgr.h"
#include "config.h"
#include "module.h"
//#include "common.h"

//const char* DEFAULT_MODULES_PATH = "/usr/bin/zebra/";
const char* DEFAULT_MODULES_PATH = "/root/git/zebra/build/modules/";

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
	char buf[256] = {0};
	std::vector<std::string>::iterator iter = conf->enable_modules_list.begin();
	std::cout<<"加载模块中。。。。"<<std::endl;
	for(; iter != conf->enable_modules_list.end(); iter++)
	{
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%slib%s.so",DEFAULT_MODULES_PATH,(*iter).c_str());
		std::cout<<"当前加载的模块名字："<<buf<<std::endl;
		//load module lib
		void* lib = dlopen(buf,RTLD_NOW|RTLD_GLOBAL);
		if(!lib) 
		{
			std::cout<<"加载模块失败。。。"<<lib<<buf<<std::endl;
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

	std::cout<<"加载模块结束。。。。"<<std::endl;

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
	std::map<std::string,module*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		module* p = iter->second;
		if(p)
		{
			p->save_file(out);
		}
	}
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