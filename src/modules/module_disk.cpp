#include <cstdlib>
#include <sstream>
#include <cstring>
#include <mntent.h>
#include <sys/statfs.h>
#include <map>
#include <vector>
#include "../module.h"

const char* disk_howto = "disk usage:------------";
const char* DISKINFO = "/etc/mtab";
//可通过下述路径获取磁盘分区列表(不过linux一般都是以文件系统来分的)
//const char* PARTITIONINFO = "/proc/partitions";
unsigned long long MB = 1024*1024;
unsigned long long GB = MB*1024;

class module_disk:public module
{
public:
	module_disk():
		module("module_disk",disk_howto),
		total_disk(0),
		free_disk(0),
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
		//debug_print();
	}
	virtual void save_file(std::ofstream& out)
	{
		if(!enable()) return;
		out<<"|disk:"<<disk_usage;
	}
	virtual void print(int level)
	{
		if(!enable()) return;
		if(PRINT_SUMMARY == level)
		{
			std::cout<<"  "<<disk_usage<<"/"<<total_disk/1024<<"GB";
		}
		else if(PRINT_DETAIL == level)
		{
		}
	}
	virtual std::string get_dbstr()
	{
		std::ostringstream ss;
		ss<<"disk = ";
		ss<<disk_usage;
		return ss.str();
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
	unsigned long long total_disk; /* in MB */
	unsigned long long free_disk; /* in MB */
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

		//获取磁盘分区信息
		disk_map.clear();
		fmnt = setmntent(DISKINFO,"r");
		if(NULL == fmnt) return;
		while((entry = getmntent(fmnt)) != NULL)
		{
			if(!strncmp(entry->mnt_fsname,"/",1))
			{
				disk_status_s tmp;
				memset(&tmp,0,sizeof(disk_status_s));
				read_partition_stat(entry->mnt_dir,&tmp);
				disk_map.insert(std::make_pair(entry->mnt_dir,tmp));
			}
		}
		endmntent(fmnt);

	}

	void read_partition_stat(const char* fsname,disk_status_s* p)
	{
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
		struct statfs buf;
		if(!statfs(fsname,&buf))
		{
			p->blocksize = buf.f_bsize;
			p->blocknum = buf.f_blocks;
			p->freeblocks = buf.f_bfree;
			p->availblocks = buf.f_bavail;
			p->nodenum = buf.f_files;
			p->freenodes = buf.f_ffree;
		}
	}
	void caculate_disk()
	{
		total_disk = 0;
		free_disk = 0;
		std::map<std::string,disk_status_s>::iterator iter = disk_map.begin();
		for(; iter != disk_map.end(); iter++)
		{
			total_disk += iter->second.blocksize*iter->second.blocknum/MB;
			free_disk += iter->second.freeblocks*iter->second.blocksize/MB;
		}
		disk_usage =1.0*(total_disk - free_disk)/total_disk*100.0;
	}
	void debug_print()
	{
		std::cout<<std::endl<<"磁盘总使用率 = "<<disk_usage<<"磁盘总量= "<<total_disk<<"已使用量="<<total_disk-free_disk<<std::endl;
		std::map<std::string,disk_status_s>::iterator iter = disk_map.begin();
		for(; iter != disk_map.end(); iter++)
		{
			std::cout<<std::endl<<iter->first<<std::endl;
			std::cout<<"blocksize  = "<<iter->second.blocksize<<std::endl;
			std::cout<<"blocknum   = "<<iter->second.blocknum<<std::endl;
			std::cout<<"freeblocks = "<<iter->second.freeblocks<<std::endl;
			std::cout<<"availblocks= "<<iter->second.availblocks<<std::endl;
			std::cout<<"nodenum    = "<<iter->second.nodenum<<std::endl;
			std::cout<<"freenodes  = "<<iter->second.freenodes<<std::endl;
		}
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
