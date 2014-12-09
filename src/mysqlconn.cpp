#include <unistd.h>
#include <iostream>
#include "mysqlconn.h"

MysqlConn::MysqlConn():
	_mysql(NULL),
	_result(NULL),
	_field(NULL),
	_host(""),
	_port(0),
	_user(""),
	_pwd(""),
	_dbname(""),
	_unixSocket(""),
	_conf(""),
	_clientFlag(0)
{
	_fieldNamePos.clear();
	_mysql = mysql_init(NULL);
	std::cout<<"MysqlConn::init........"<<std::endl;
}

MysqlConn::~MysqlConn()
{
	if(_result)
		mysql_free_result(_result);
	if(_mysql)
		mysql_close(_mysql);
	std::cout<<"MysqlConn::~MysqlConn.........xigou"<<std::endl;
}

void MysqlConn::setOption(enum mysql_option option, const char* arg)
{
	int ret = mysql_options(_mysql,option,arg);
	if(ret != 0)
	{
		std::cout<<"setOption error!"<<option<<arg<<std::endl;

	}
}

MYSQL* MysqlConn::connect(const std::string &conf)
{
	return NULL;
}

MYSQL* MysqlConn::connect(const char* db_ip,unsigned int db_port,const char* db_user,const char* db_pwd,const char* db_name)
{
	_host = db_ip;
	_port = db_port;
	_user = db_user;
	_pwd = db_pwd;
	_dbname = db_name;

	my_bool reconnect = 1;
	unsigned int read_timeout = 60;
	unsigned int write_timeout = 5;
	setOption(MYSQL_OPT_RECONNECT,(char*)&reconnect);
	setOption(MYSQL_OPT_READ_TIMEOUT,(char*)&read_timeout);
	setOption(MYSQL_OPT_WRITE_TIMEOUT,(char*)&write_timeout);
	MYSQL* result = NULL;
	result = mysql_real_connect(_mysql,_host.c_str(),_user.c_str(),_pwd.c_str(),
		_dbname.c_str(),_port,_unixSocket.empty()?NULL:_unixSocket.c_str(),_clientFlag);
	if(NULL == result)
	{
		std::cout<<"connect failed!"<<_host<<_port<<_user<<_pwd<<_dbname<<std::endl;
		std::cout<<"error msg:"<<mysql_error(_mysql)<<std::endl;
	}
	else
	{
		std::cout<<"connect succeed!...."<<std::endl;
	}
	return result;
}

void MysqlConn::execute(const char *sql, size_t len)
{
	int retry = 0;
	int result = -1;
	while((result = mysql_real_query(_mysql,sql,len)) != 0)  //执行语句失败
	{
		retry++;
		std::cout<<"execute error... reConnect "<<retry<<std::endl;
		if(retry > 3)  //重试3次，不行退出
		{
			break;
		}
		switch(result)
		{
		case CR_COMMANDS_OUT_OF_SYNC:
			std::cout<<"eeexecute error..."<<"Commands were executed in an improper order."<<std::endl;
			break;
		case CR_SERVER_GONE_ERROR:
			std::cout<<"eeexecute error..."<<"The MySQL server has gone away."<<std::endl;
			break;
		case CR_SERVER_LOST:
			std::cout<<"eeexecute error..."<<"The connection to the server was lost during the query."<<std::endl;
			break;
		case CR_UNKNOWN_ERROR:
			std::cout<<"eeexecute error..."<<"An unknown error occurred."<<std::endl;
			break;
		default:
			std::cout<<"eeeeeeeeeeeeeeeee..."<<result<<std::endl;
		}
		sleep(retry);  //休眠多少秒 如果是微秒用usleep函数
		reConnect(); //重新连接
	}

}

void MysqlConn::execute(const std::string& sql)
{
	return execute(sql.c_str(),sql.length());
}

void MysqlConn::getResult()
{
	//释放本地结果集空间
	if(_result != NULL)
	{
		mysql_free_result(_result);
		_result = NULL;
		_field = NULL;
	}

	//从服务器端取回结果并存储下来
	_result = mysql_store_result(_mysql);
	if(_result == NULL)
	{
		std::cout<<"getResult error! "<<mysql_error(_mysql)<<std::endl;
		return;
	}

	//给字段指针赋值
	_field = _result->fields;
	_fieldNamePos.clear();
	unsigned int fieldNum = mysql_num_fields(_result);
	for(unsigned int i = 0; i < fieldNum; i++)
	{
		_fieldNamePos.insert(std::pair<const char*,unsigned int>(getFieldNameAt(i),i));
	}
	//std::cout<<"mmmmmmmmmmmmmmmmmmmmmmmmmmm"<<fieldNum<<std::endl;

}

const char* MysqlConn::getFieldValueByName(const char* name)
{
	std::map<const char*,int,ptrcmp>::iterator iter = _fieldNamePos.find(name);
	if(iter == _fieldNamePos.end())
		return NULL;
	return _row[iter->second];
}

void MysqlConn::getAllTableNames(std::vector<std::string> &tables)
{
	std::string sql = "SHOW TABLES";
	if(good())
	{
		execute(sql);
		getResult();
		while((_row = mysql_fetch_row(_result)) != NULL)
		{
			tables.push_back(_row[0]);
		}
	}
}

void MysqlConn::getTableFields(std::vector<std::string> &fields,std::string table)
{
	std::string sql = "SHOW COLUMNS FROM " + table;
	if(good())
	{
		execute(sql);
		getResult();
		while((_row = mysql_fetch_row(_result)) != NULL)
		{
			fields.push_back(_row[0]);
		}
	}
}

void MysqlConn::reConnect()
{
	if(good()) return;
	close();
	_mysql = mysql_init(NULL);

	my_bool reconnect = 1;
	unsigned int read_timeout = 60;
	unsigned int write_timeout = 5;
	setOption(MYSQL_OPT_RECONNECT,(char*)&reconnect);
	setOption(MYSQL_OPT_READ_TIMEOUT,(char*)&read_timeout);
	setOption(MYSQL_OPT_WRITE_TIMEOUT,(char*)&write_timeout);
	MYSQL* result = NULL;
	result = mysql_real_connect(_mysql,_host.c_str(),_user.c_str(),_pwd.c_str(),
		_dbname.c_str(),_port,_unixSocket.empty()?NULL:_unixSocket.c_str(),_clientFlag);
	if(NULL == result)
	{
		std::cout<<"reConnect failed! "<<std::endl;
	}
	else
	{
		std::cout<<" reConnect succeed! "<<std::endl;
	}

}

MYSQL_ROW MysqlConn::nextRow()
{
	if(_result == NULL) return NULL;

	//取出下一行
	_row = mysql_fetch_row(_result);

	//获取字段值的长度序列
	if(_row != NULL)
	{
		_fieldLens = mysql_fetch_lengths(_result);
	}
	return _row;

}
