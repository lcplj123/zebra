#ifndef __MODULE_H__
#define __MODULE_H__
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

enum{
	PRINT_SUMMARY,
	PRINT_DETAIL,
};

class module
{
public:
	module():
		name(""),
		_usage(""),
		_enable(true)
	{
	}
	module(const char* modulename,const char* usage):
		name(modulename),
		_usage(usage),
		_enable(true)
	{
	}
	virtual	~module()
	{
	}
	void usage()
	{
		std::cout<<_usage<<std::endl;
	}
	virtual void collect_data() /* collect data */
	{
	}
	virtual void save_file(std::ofstream& out) /*output to file*/
	{
	}
	virtual void print(int level) /* output to screen */
	{
	}
	virtual void write_db() /* output to db or url */
	{
	}
	void setEnable(bool enable) /* enable or disable the module */
	{
		_enable == enable? _enable:!_enable;
	}
	bool enable()
	{
		return _enable;
	}

public:

private:
	std::string name; /* the name of the module */	
	std::string _usage; /* describe how to use module */
	bool _enable; /* if use this module */

};

extern "C"{
module* create();
void release(module*);
typedef module* (*new_module)();
typedef void (*release_module)(module*);

}

#endif //__MODULE_H__
