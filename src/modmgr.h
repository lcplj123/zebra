#ifndef _MODULE_MGR_H__
#define _MODULE_MGR_H__
#include <iostream>
#include <map>
#include <fstream>

class configure;
class module;
class modmgr
{
public:
	modmgr(configure* conf);
	~modmgr();

	bool load_modules(std::string path = ""); /* 加载模块 */
	bool collect_data(); /* 搜集数据 */
	bool save_file(std::ofstream& out); /* 将数据保存到文件 */
	bool write_db(); /* 保存到远程数据库 */
	bool print(int level); /* 输出到屏幕  */
	bool setEnable(std::string modulename,bool enable); /* 设置是否启用 */
	void freemodules();
	void print_modules();

private:
	std::map<std::string,void*> so_list; /* 模块名及其对于的lib地址 */
	std::map<std::string,module*> modules_list; /* 模块名及其对应的类地址 */
	configure* conf;

};

#endif  // _MODULE_MGR_H__
