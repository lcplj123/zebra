#ifndef __MYSQLCONN_H__
#define __MYSQLCONN_H__

#include <string>
#include <strings.h>
#include <vector>
#include <map>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>


/**
*  Mysql的连接定义
*/

class MysqlConn
{
public:
	//构造函数和析构函数
	MysqlConn();
	~MysqlConn();

	//init之后connect之前设置的参数， 具体请参照mysql_options中的说明
	void setOption(enum mysql_option option, const char* arg);

	//连接动作
	MYSQL* connect(const std::string &conf);
	MYSQL* connect(const char* db_ip,unsigned int db_port,const char* db_user,const char* db_pwd,const char* db_name);
	
	//连接是否正确
	bool good()
	{
		return 0 == mysql_errno(_mysql);
	}

	//关闭连接
	void close()
	{
		if(_mysql)
		{
			mysql_close(_mysql);
			_mysql = NULL;
		}
	}

	//获取最后一个错误
	unsigned int getLastError()
	{
		return mysql_errno(_mysql);
	}

	bool lostConnection()
	{
		return CR_SERVER_LOST == mysql_errno(_mysql);
	}

	bool hasGoneway()
	{
		return CR_SERVER_GONE_ERROR == mysql_errno(_mysql);
	}

	//重连
	void reConnect();

	/**
	*  执行sql语句
	*/
	void execute(const char* sql,size_t len);
	void execute(const std::string& sql);

	/**
	*  从数据库中取查询结果
	*/
	void getResult();

	/**
	 * @brief    返回上次UPDATE更改的行数，上次DELETE删除的行数，或上次INSERT语句插入的行数。
	 * 对于UPDATE、DELETE或INSERT语句，可在mysql_query()后立刻调用。对于SELECT语句，
	 * mysql_affected_rows()的工作方式与mysql_num_rows()类似。
	 *
	 * @return    大于0的整数表明受影响或检索的行数。“0”表示UPDATE语句未更新记录，
	 * 在查询中没有与WHERE匹配的行，或未执行查询。“-1”表示查询返回错误，或者，
	 * 对于SELECT查询，在调用mysql_store_result()之前调用了mysql_affected_rows()。
	 * 由于mysql_affected_rows()返回无符号值，通过比较返回值和“(my_ulonglong)-1”
	 * 或等效的“(my_ulonglong)~0”，检查是否为“-1”。
	 */
	long getAffectedRows()
	{
		return mysql_affected_rows(_mysql);
	}

	/**
	*  获取查询结果的行数
	*/
	my_ulonglong getRowsNum()
	{
		return _result == NULL ? 0 : mysql_num_rows(_result);
	}

	/**
	 * @brief    getResult之后可以调用该函数遍历结果集.
	 *
	 * @return   NULL表示到行尾.
	 */
	MYSQL_ROW nextRow();

	/**
	*  获取最近一次查询的字段数(列数)
	*/
	unsigned int getFieldsNum()
	{
		return _result == NULL ? 0 : mysql_num_fields(_result); 
	}

	/**
	*  获取最近一次查询第pos字段的名字
	*/
	const char* getFieldNameAt(int pos)
	{
		return _field[pos].name;
	}

	/**
	*  获取最近一次查询第pos字段的类型
	*/
	enum enum_field_types getFieldTypeAt(int pos)
	{
		return _field[pos].type;
	};

	/**
	*  获取当前结果行的第pos字段的值
	*/
	const char* getFieldValueAt(int pos)
	{
		return _row[pos];
	}

	/**
	*  根据当前结果行的列名字得到对应的值
	*/
	const char* getFieldValueByName(const char* name);

	/**
	*  获取当前结果第pos字段值的长度
	*/
	unsigned long getFieldValueLenAt(int pos)
	{
		return _fieldLens[pos];
	}

	/**
	*  根据当前结果行的列名字得到对应字段的长度
	*/
	unsigned long getFieldValueLenByName(const char* name)
	{
		std::map<const char*,int,ptrcmp>::iterator iter = _fieldNamePos.find(name);
		if(iter != _fieldNamePos.end())
		{
			return _fieldLens[iter->second];
		}
		return 0;
	}

	/**
	*  获取当前数据库的所有的表名
	*/
	void getAllTableNames(std::vector<std::string> &tables);

	/**
	*  获取当前数据库的所有的表名
	*/
	void getTableFields(std::vector<std::string> &fields, std::string table);

private:
	MYSQL*         _mysql;  //数据库连接句柄
	MYSQL_RES*     _result;  //查询返回的结果集
	MYSQL_ROW      _row;  //当前行对象
	MYSQL_FIELD*   _field;  //本次查询的列数组信息,包括列的名字和类型等
	unsigned long* _fieldLens;  //当前游标行字段值的长度数组

	std::string    _host;  //mysql所在主机的ip
	unsigned int   _port;  //连接mysql的端口
	std::string    _user;  //连接mysql时的用户
	std::string    _pwd;  //连接mysql时的密码
	std::string    _dbname;  //连接mysql的数据库名字
	std::string    _unixSocket;
	std::string    _conf;
	unsigned long  _clientFlag;
	//如果想要char*做为key，必须要重载一下操作符，否则每次查找的时候
	//总是找不到,因为它比较的是指针，而不是指针指针指向的内容
	struct  ptrcmp
	{
		bool operator()(const char* left,const char* right)
		{
			return strcasecmp(left,right) < 0;  //忽略大小写比较字符串
		}
	};
	std::map<const char*,int,ptrcmp> _fieldNamePos;  //字段(列名)map

};

#endif  //__MYSQLCONN_H__
