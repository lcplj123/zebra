#ifndef __ZEBRA_CONFIG_H__
#define __ZEBRA_CONFIG_H__

#include <string>
#include <vector>
#include <map>

class configure
{
public:
	configure(const char* filename); /* construct */
	~configure(); /* destruct */
	void parse_config_file(std::string path = ""); /* analyze the conf file */
	bool reload(const char* filename = NULL); /* reload the config */
	void debug_print();

public:
	int run_state; /* running mode */
	std::string conf_name; /* configure file name */
	int interval;  /* how many seconds escape in every print  */
	std::vector<std::string> enable_modules_list;  /* enable modules */
	std::map<std::string,std::string> confMap; /* 读取配置使用的文件 */
	int debug_level; /* debug level */
	std::vector<std::string> output_interface; /* output form */
	std::string modules_path; /*modules path*/

	//output for print
	int print_mode; /* print summary or detail */

	//output for file. All enable modules should be saved by default
	std::string output_file_path; /* which file to save data */


	//output for database. We provide two ways
	//1) connect to database directly by using ip and port
	//2) using the http request
	std::vector<std::string> db_module_list; /* which module will output to database */
	std::string db_ip; /* database ip */
	unsigned int db_port; /* database port */
	std::string db_url; /* database url */

private:
	void parse_include_conf(std::string dirs); /* it allows the include item */
	size_t getLeftTrimPos(const std::string& s); /* get left character position */
	size_t getRightTrimPos(const std::string& s);  /* get right character position */
	std::string removeTrim(const std::string& s); /* remove the giving string trim */
	void getSplit(const std::string& s); /* get the item splited by space or tab */
	void parse_line();  /* parse every line option */
	void parse_module(std::string token,std::string value); /* parse module item  */
	void copy_to_db(); /* copy all enable module list to db_module_list */
	void split(std::string& str,char delim,std::vector<std::string>& v); /* split string by delim*/

};


#endif  //__ZEBRA_CONFIG_H__
