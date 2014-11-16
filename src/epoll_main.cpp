/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* 文件名称：epoll_main.cpp
*
* 摘要：pure_epoll测试
*/

//下面是原作者、版本、完成、日期和当前版本的信息
/* 当前版本：1.0.0
* 作者：程志鹏，完成日期：2012年11月13日
*/


#include "epoll.h"
#include <iostream>


#include "base_server.h"
#include "base_connect.h"

#include "http_connection.h"
#include "http_server.h"
using namespace std;
using namespace pure_baselib;
using namespace pure_frame;
s32 main(s32 argc, s8* argv[])
{
	#if 0
	/* step1:*/
	pure_epoll *epoll = new pure_epoll();
	if (NULL == epoll)
	{
		exit(EXIT_FAILURE);
	}

	if (FAILURE == epoll->pure_epoll_create(256))
	{
		exit(EXIT_FAILURE);
	}
	 

	/* step2:*/
	server *svr = new httpserver();
	svr->server_sockfd_create();
	svr->server_sockfd_bind(&addr, sizeof(addr));
	svr->server_sockfd_listen(1024);
	

	/* step3:*/
	 epoll->pure_epoll_sockfd_regesiter(static_cast <base_socket*> (svr));
	

	/* step4:*/
	 for(; ;)
	 {
	 	 //TODO:定时处理模块，得到一个定时的时间值timeout。
	 	 if (FAILURE == (epoll->pure_epoll_process(timeout))
	 	 {
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"function pure_epoll_process() called error!"<<endl;
	 	 }
	 }
	 #endif
	cout<<"\t\t***********************************************"<<endl;
	cout<<"\t\t   >(~_~)< Hello, this is a epoll test!!!"<<endl;
    cout<<"\t\t           listen port is on  "<<CONFIG_HTTP_PORT<<" !!!"<<endl;
	cout<<"\t\t***********************************************"<<endl;

	pure_epoll *epoll = new pure_epoll();
	if (NULL == epoll) {
		printf("<><><><><>     exit     1<><><><><>\n");
		exit(EXIT_FAILURE);
	}

	if (FAILURE == epoll->pure_epoll_create(256)) {
		printf("<><><><><>     exit     2<><><><><>\n");
		exit(EXIT_FAILURE);
	}

	/* step2:*/
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(CONFIG_HTTP_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //create server socket, then bind it to a server  	
	CHttpServer *svr = new CHttpServer();
	svr->server_sockfd_create();
	svr->server_sockfd_bind(&server_addr, sizeof(sockaddr_in));
	svr->server_sockfd_listen(CONFIG_HTTP_PORT);
    
    //CHttpServer Class inherit server class
    //server class inherit base_socket class
	/* step3:*/
    //register listen socket in epoll set
	epoll->pure_epoll_sockfd_regesiter(dynamic_cast <base_socket*> (svr));
	/* step4:*/
	for(; ;)
	{
		//TODO:定时处理模块，得到一个定时的时间值timeout。
		if (FAILURE == (epoll->pure_epoll_process(300)))
		{
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"function pure_epoll_process() called error!"<<endl;
		}
	}
	 
	cout<<"\t\t***********************************************"<<endl;
	cout<<"\t\t   >(~_~)< Hello, this is a epoll test!!!"<<endl;
    cout<<"\t\t listen port is on"<<CONFIG_HTTP_PORT<<endl;
	cout<<"\t\t***********************************************"<<endl;
	return 0;
}



