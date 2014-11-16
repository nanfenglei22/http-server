#ifndef _BASIC_DEFINE_H_
#define _BASIC_DEFINE_H_

namespace pure_baselib
{	
	//编译选项
	//@文件夹结构方式，如果需要在没有请求页面的情况下显示目录结构
	//使用下面的宏，否则请注释掉
	#define _CONFIG_HTTP_HAS_DIRECTORIES_ 	1		
	#define _CONFIG_HTTP_DIRECTORIES_ 		1
	//@end

	//IPV6的对应编译选项，使用IPV6则打开
	//#define _IPV6_ 1

	//返回值的宏
	#define WAIT									2
	#define REFILTER								1
	#define SUCCESS 								0
	#define FAILURE								~0

	//配置监听端口
	#define CONFIG_HTTP_PORT 					8124	//http监听端口
	#define CONFIG_HTTP_HTTPS_PORT 			443		//https监听端口

	#define MAX_REQUEST_SIZE					4096	//一次能读取的最大包长度

	/*
	*进入队列中允许的连接的个数。进入的连接请求在使用系统调用accept()应答之前
	*要在进入队列中等待。这个值是队列中最多可以拥有的请求的个数。
	*/
	#define MAX_LISTEN_SIZE 					1024  	

	#define MAXREQUESTLENGTH                    		256		//应答字符串的最大长度
	#define INITIAL_CONNECTION_SLOTS           	10		//初始化的连接序列长度

	//连接过程中的方法宏
	#define STATE_WANT_TO_READ_HEAD 			1		
	#define STATE_WANT_TO_SEND_HEAD 			2
	#define STATE_WANT_TO_READ_FILE 			3
	#define STATE_WANT_TO_SEND_FILE 			4
	#define STATE_DOING_DIR 					5

	//changed by simon 2012-12-18
	#define STATE_DONT_FINISH_SEND 			6
	//end

	//一次链接的超时时间，单位(s)
	#define CONFIG_HTTP_TIMEOUT 				300

	//包头HTTP_VERSION字节
	#define HTTP_VERSION        "HTTP/1.1"
	//changed by simon 2012-12-14
	const s8*  const tclose = "close";
	const s8*  const tkeep_alive ="keep_alive";
	//end	

	#define VERSION    "1.0.0"
	const s8 * const server_version = "purehttpd/"VERSION;

	static const s8 * index_file = "index.html";

	//http服务方法宏
	enum HTTP_TYPE_E
	{
	    TYPE_GET = 0,
	    TYPE_HEAD,
	    TYPE_POST
	};

	

}
#endif /* _BASIC_DEFINE_H_ */
