#include <cstdlib>
#include <sstream>
#include <cstring>
#include <mntent.h>
#include <sys/statfs.h>
#include <map>
#include "../module.h"

const char* disk_howto = "disk usage:------------";
const char* DISKINFO = "/etc/mtab";

class module_disk:public module
{
public:
	module_disk():
		module("module_disk",disk_howto),
		disk_usage(0.0)
	{
		disk_map.clear();
		std::cout<<"构造disk模块。。。"<<std::endl;
	}
	virtual ~module_disk()
	{
		std::cout<<"析构disk模块。。。"<<std::endl;
		disk_map.clear();
	}
	
	virtual void collect_data()
	{
		if(!enable()) return;
		read_disk_stat();
		caculate_disk();
	}
	virtual void save_file(std::ofstream& out)
	{
		if(!enable()) return;
	}
	virtual void print(int level)
	{
		if(!enable()) return;
		if(PRINT_SUMMARY == level)
		{
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}

private:
	struct disk_status_s{
		unsigned long long blocksize; /* 每个block里包含的字节数 */
		unsigned long long blocknum; /* block的个数 */
		unsigned long long freeblocks; /* 可用的块数 */
		unsigned long long availblocks; /* 非超级用户可获取的块数 */
		unsigned long long nodenum;  /* 文件节点总数 */
		unsigned long long freenodes;  /* 可用文件节点数 */
	};
	std::map<std::string,disk_status_s> disk_map;	
	float disk_usage;
private:
	void read_disk_stat()
	{
		FILE* fmnt = NULL;
		struct mntent* entry = NULL;
		/**
		 * struct mntent{
		 * char* mnt_fsname; //挂载的文件系统的名字
		 * char* mnt_dir;  //挂载点
		 * char* mnt_type; //文件系统类型如nfs，xfs等
		 * char* mnt_opts; //选项，以逗号位分隔符
		 * int   mnt_freq; //dump的频率，以天为单位
		 * int   mnt_passno; //fsck检查的次序
		 * }
		 **/
		struct statfs s;
		/**
		 * struct statfs{
		 * long f_type;  //文件系统类型
		 * long f_bsize;  //块大小，单位是字节
		 * long f_blocks;  //文件系统的块总数
		 * long f_bfree;  //可用块数
		 * long f_bavail;  //非超级用户可用的块数
		 * long f_files;  //文件节点总数
		 * long f_ffree;  //可用文件节点数
		 * fsid_t f_fsid;  //文件系统标识
		 * long f_namelen;  //文件名的最大长度
		 * }
		 **/
		disk_map.clear();
		fmnt = setmntent(DISKINFO,"r");
		if(NULL == fmnt) return;
		while((entry = getmntent(fmnt)) != NULL)
		{

		}

	}
	void caculate_disk()
	{
	}
	void debug_print()
	{
	}
};

extern "C"{
//创建类
module* create()
{
	return new module_disk;
}

//销毁类
void release(module* p)
{
	if(p)
	{
		delete p;
		p = NULL;
	}
}
}
