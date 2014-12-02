#ifndef _MODULE_MGR_H__
#define _MODULE_MGR_H__
#include <iostream>
#include <map>
#include <fstream>

class configure;
class modmgr
{
public:
	modmgr(configure* conf);
	~modmgr();

	bool load_modules(std::string path = ""); /* 加载模块 */
	bool collect_data(); /* 搜集数据 */
	bool save_file(std::ofstream& out); /* 将数据保存到文件 */
	bool setEnable(std::string modulename,bool enable); /* 设置是否启用 */
	void freemodules();

private:
	std::map<std::string,void*> modules_list;
	configure* conf;

};

#endif  // _MODULE_MGR_H__
