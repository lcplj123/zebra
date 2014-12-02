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
	typedef void (*cd_fn)();
	std::map<std::string,void*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		cd_fn collectdata = dlsym(iter->second,"collect_data");
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
	typedef void (*sf_fn)(ofstream&);
	std::map<std::string,void*>::iterator iter = modules_list.begin();
	for(; iter != modules_list.end(); iter++)
	{
		sf_fn savefile = dlsym(iter->second,"save_file");
		if(dlerror())
		{
			return false;
		}
		savefile(out);
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
