#include <string>
#include <vector>
#include <map>
#include <fstream>

class configure
{
public:
	void parse_config_file(const std::string filename); /* analyze the conf file */
	void get_include_conf(); /* it allows the include item */
	size_t getLeftTrimPos(const std::string& s); /* get left character position */
	size_t getRightTrimPos(const std::string& s);  /* get right character position */
	std::string removeTrim(std::string& s); /* remove the giving string trim */
	void getSplit(const std::string& s,std::map<std::string,std::string>& mapconf); /* get the item splited by space or tab */

private:
	int run_state; /* running mode */
	std::string conf_name; /* configure file name */
	int interval;  /* how many seconds escape in every print  */

	//output for print
	int print_mode; /* print summary or detail */

	//output for file. All enable modules should be saved by default
	std::string save_file_name; /* which file to save data */


	//output for database. We provide two ways
	//1) connect to database directly by using ip and port
	//2) using the http request
	std::vector<std::string> db_module_list; /* which module will output to database */
	std::string db_ip; /* database ip */
	unsigned int db_port; /* database port */
	std::string db_url; /* database url */

};
