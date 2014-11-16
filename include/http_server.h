/*************************************************************************
                  Copyright (C) 2012   Shanghai Surfing
                                           All Rights Reserved
                                           R&D Dept.                                            
*************************************************************************/
/*************************************************************************

Filename:         	http_prase.h

Description:    	http_prase Function Define Source File.

Author:          		gaoyang

Date:             	2012-11-22 

Version:   		V1.0.0

Modified:      

*************************************************************************/
#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "basic_type.h"
#include "c_headers.h"
#include "basic_define.h"
#include "base_server.h"

#include "http_connection.h"

using namespace pure_baselib;

typedef struct server_struct_s 
{
    	struct server_struct_s *pnext;
    	s32 sd;
    	s32 is_ssl;			//是否使用ssl
    	//SSL_CTX *ssl_ctx;
}SEST_S;

typedef struct conn_struct_s 
{
	struct conn_struct_s *pnext;
	s32 state;
	s32 reqtype;
	s32 networkdesc;
	s32 filedesc;

#ifdef _CONFIG_HTTP_DIRECTORIES_
    	DIR *dirp;
#endif

	time_t timeout;
	s8 actualfile[MAXREQUESTLENGTH];
	s8 filereq[MAXREQUESTLENGTH];
	s8 dirname[MAXREQUESTLENGTH];
	s8 server_name[MAXREQUESTLENGTH];
    s8 default_dir[MAXREQUESTLENGTH];
	s32 numbytes;
	s8 databuf[MAX_REQUEST_SIZE];
	u8 is_ssl;
	u8 close_when_done;
	time_t if_modified_since;

	//changed by simon 2012-12-12
	s32 range; 	//	o不用range，1，从前开始向后取 2，从后向前取
	s32 range_from;
	s32 range_end;
	//end
}CON_S;

struct http_event_queue_s
{
	epoll_event			ev;
	conn_struct_s			*con;
	http_event_queue_s	*next;
};



namespace pure_frame
{

	
/*
class server:public base_socket
{
	protected:
		connection		*con_head;		//connect头节点
		connection		*con_tail;		//connect尾节点
	public:

		virtual ~server(){}

		virtual s32 server_sockfd_create(void) = 0;
		virtual s32 server_sockfd_bind(socket_addr *addr, u32 addr_len) = 0;
		virtual s32 server_sockfd_listen(s32 listen_num) = 0;
		virtual connection* server_connect_create(s32 connfd) = 0;
		virtual bool server_connect_delete(connection *con) = 0;
		virtual bool server_connect_destroy(void) = 0;
}
*/
class CHttp_connection;
class CHttpServer:public server
{
	public:
		//s32		sock_fd;					//socket的描述符
		//u32		sock_type;				//socket描述符的类型。0表示connect描述符；1表示server的描述符。
		//virtual ~base_socket(){}
		
		s32 					m_http_fd;		//http的文件描述符
		s32 					m_https_fd;		//https的文件描述符,暂不用
		SEST_S 				*m_servers;		//server链表
		CON_S				*m_usedconns;		
		CON_S				*m_freeconns;
		s32 					m_count;		//记录服务器监听端口数量
		
		CHttpServer();
		virtual ~CHttpServer();

		virtual s32 server_sockfd_create(void);
		virtual s32 server_sockfd_bind(socket_addr *addr, u32 addr_len);
		virtual s32 server_sockfd_listen(s32 listen_num);
		//添加client连接
		virtual connection *server_connect_create(s32 connfd);
		//CHttp_connection* server_connect_create(s32 connfd);
		virtual bool server_connect_delete(connection *con);
		virtual bool server_connect_destroy(void);
		/*
		//请求或者响应中时间模块初始化
		void tdate_init(void);
		//请求或者响应中时间解析模块的比较模块
		static s32 day_mon_map_compare(const s8 *i_pv1, const s8 *i_pv2);
		*/	
		//把server节点添加到server链表中去，以便查找,同时开启多个服务器时，可以使用
		void addto_servers(s32 i_sd);
		//建立socket，设置监听端口
		s32 open_listen(s32 i_port);

		//关闭客户端连接
		void remove_connection(CON_S *o_pcn) ;
		//设置非阻塞状态
		s32 set_nonblocking(s32 i_sock);
	protected:	
		
		
		

};


}
#endif
