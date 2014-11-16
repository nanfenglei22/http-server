/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* 文件名称：server.h
*
* 摘要：epoll模型的server抽象类及接口
*/

//下面是原作者、版本、完成、日期和当前版本的信息
/* 当前版本：1.0.0
* 作者：程志鹏，完成日期：2012年11月23日
*/
#ifndef BASE_SERVER_H_
#define BASE_SERVER_H_

#include "base_socket.h"
#include "base_connect.h"

namespace pure_baselib
{

	class server:public base_socket
	{
	protected:
		connection		*con_head;		//connect头节点
		connection		*con_tail;		//connect尾节点
	public:

		virtual ~server(){}

		/*********************************************
		 * 函数名：server_sockfd_create
		 * 描述：创建server的socket描述符，将值赋值给
		 * 		base_socket的sock_fd，并设置类型。
		 * 输入：
		 * 		无
		 * 输出：
		 * 		无
		 * 返回值：s32类型。成功返回SUCCESS，失败返回FAILURE。
		 *********************************************/
		virtual s32 server_sockfd_create(void) = 0;


		/****************************************************************
		 * 函数名：server_sockfd_bind
		 * 描述：将server的sock_fd帮定至指定的端口号。
		 * 输入：
		 * 		addr：socket_addr类型指针。存有需要帮定的ip地址和端口号的地址。
		 * 		addr_len：u32类型。socket_addr结构体的大小
		 * 输出：
		 * 		无
		 * 返回值：s32类型。成功返回SUCCESS，失败返回FAILURE。
		 ***************************************************************/
		virtual s32 server_sockfd_bind(socket_addr *addr, u32 addr_len) = 0;


		/***************************************************
		 * 函数名：server_sockfd_listen
		 * 描述：监听server对象的sock_fd。
		 * 输入：
		 * 		listen_num：s32类型。最大可监听的个数大小。
		 * 输出：
		 * 		无
		 * 返回值：s32类型。成功返回SUCCESS，失败返回FAILURE。
		 ***************************************************/
		virtual s32 server_sockfd_listen(s32 listen_num) = 0;


		/*************************************************************
		 * 函数名：server_connect_create
		 * 描述：创建指定connfd的connection节点。
		 * 输入：
		 * 		connfd：s32类型。需要创建连接节点的socket_fd值。
		 * 输出：
		 * 		无
		 * 返回值：connection类型指针。成功返回新创建的节点的地址，失败返回NULL。
		 *************************************************************/
		virtual connection* server_connect_create(s32 connfd) = 0;


		/*************************************************************
		 * 函数名：server_connect_create
		 * 描述：删除指定connfd的connection节点。
		 * 输入：
		 * 		con：connection类型指针。需要删除连接节点地址。
		 * 输出：
		 * 		无
		 * 返回值：bool类型。成功返回true，失败返回false。
		 *************************************************************/
		virtual bool server_connect_delete(connection *con) = 0;


		/*************************************************************
		 * 函数名：server_connect_create
		 * 描述：销毁connection链表。
		 * 输入：
		 * 		无
		 * 输出：
		 * 		无
		 * 返回值：bool类型。成功返回true，失败返回false。
		 *************************************************************/
		virtual bool server_connect_destroy(void) = 0;

	};
};

#endif /* server_H_ */
