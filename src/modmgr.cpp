#include <vector>
#include <cstring>
#include <string>
#include <cstdio>
#include <dlfcn.h>
#include "modmgr.h"
#include "config.h"
#include "common.h"

modmgr::modmgr(configure* _conf):
	conf(NULL)
{
	conf = _conf;
	modules_list.clear();
}

modmgr::~modmgr()
{
	conf = NULL;
	freemodules();
	modules_list.clear();
}

bool modmgr::load_modules(std::string path)
{
	if(conf == NULL) return false;
	char buf[256] = {0};
	std::vector<std::string>::iterator iter = conf->enable_modules_list.begin();
	for(; iter != conf->enable_modules_list.end(); iter++)
	{
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%s/%s.so",DEFAULT_MODULES_PATH,(*iter).c_str());
		void* lib = dlopen(buf,RTLD_NOW|RTLD_GLOBAL);
		if(!lib) continue;
		modules_list.insert(make_pair(*iter,lib));

	}


	return true;
}

bool modmgr::collect_data()
{
	void (*collectdata)();
	std::map<std::string,void*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		collectdata = (void (*)())dlsym(iter->second,"collect_data"); /* 必须得加一个转换，否则呜啊编译 */
		if(dlerror())
		{
			return false;
		}
		collectdata();
	}
	return true;
}

bool modmgr::save_file(std::ofstream& out)
{
	typedef void (*sf_fn)(std::ofstream&);
	std::map<std::string,void*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		sf_fn savefile = (void (*)(std::ofstream&))dlsym(iter->second,"save_file");
		if(dlerror())
		{
			return false;
		}
		savefile(out);
	}
	return true;
}

bool modmgr::print(int level)
{
	typedef	void (*pr_fn)(int);
	std::map<std::string,void*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		pr_fn  print = (void (*)(int))dlsym(iter->second,"print");
		if(dlerror())
		{
			return false;
		}
		print(level);
	}
	return true;
}

void modmgr::freemodules()
{
	std::map<std::string,void*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end();iter++)
	{
		if(iter->second)
		{
			dlclose(iter->second);
			iter->second = NULL;
		}
	}
}
