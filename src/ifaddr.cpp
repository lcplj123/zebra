#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <iostream>
#include "ifaddr.h"

std::string getinterface()
//int main()
{
  struct ifaddrs * ifAddrStruct = NULL;
  struct ifaddrs * ifa = NULL;
  void * tmpAddrPtr = NULL;
  std::string result = "";

  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa ->ifa_addr->sa_family == AF_INET) //是否是IPv4 
	{ 
      //is a valid IPv4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN] = {'\0'};
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
	  if(strncmp(ifa->ifa_name,"lo",2))
	  {
		  std::string s(addressBuffer);
		  result = s;
		  break;
		  //std::cout<<ifa->ifa_name<<" IP Address is: "<<s<<std::endl; 
	  }
    }
	else if (ifa->ifa_addr->sa_family == AF_INET6) //是否是IPv6
	{ 
      //is a valid IPv6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      char addressBuffer[INET6_ADDRSTRLEN] = {'\0'};
      inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
	  if(strncmp(ifa->ifa_name,"lo",2))
	  {
		  //std::cout<<ifa->ifa_name<<" IP Address is: "<<addressBuffer<<std::endl; 
	  }
    } 
  }

  if (ifAddrStruct!=NULL) 
	  freeifaddrs(ifAddrStruct);

  //std::cout<<"result = "<<result<<std::endl;
  return result;
}
