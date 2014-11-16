/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* 文件名称：connection.h
*
* 摘要：epoll模型中connect对象的抽象类及接口
*/

//下面是原作者、版本、完成、日期和当前版本的信息
/* 当前版本：1.0.0
* 作者：程志鹏，完成日期：2012年11月23日
*/
#ifndef BASE_CONNECT_H
#define BASE_CONNECT_H

#include "base_socket.h"
//#include "base_server.h"
///////////////////////////// typedef definition /////////////////////////
typedef struct connect_buf_s	connect_buf_t;
typedef struct sockaddr_in		socket_addr;
///////////////////////////// struct definition //////////////////////////
struct connect_buf_s
{
	s8		*buffer;	//用来存储数据的缓冲区
	u32		pos;		//当前可读数据的位置。
	s32		buf_len;	//当前buffer共有的数据长度。
	s8		*last;		//临时用的指针。
	u32		end;		//指向缓冲区最后的位置。
};



namespace pure_baselib
{
	class server;

	class connection:public base_socket
	{
	protected:
		//connect_buf_t			read_data;		//buffer for read data;
		//connect_buf_t			write_data;		//buffer for write data;
	public:
		bool 					readable;		//read flag;
		bool 					writeable;		//write flag;
		bool 					exeuteable;		//execute flag;
		socket_addr				remote_addr;	//the client addr;
		server					*svr;			//没一个connection都有自己制定的server指针，用于server的connection链表管理。
		connection				*next;			//next node;

		virtual ~connection(){}

		/*********************************************
		 * 函数名：connect_handle_init
		 * 描述：connection初始化的处理函数。
		 * 输入：
		 * 		无
		 * 输出：
		 * 		无
		 * 返回值：bool类型。成功返回true，失败返回false。
		 *********************************************/
		virtual s32 connect_handle_init() = 0;


		/*********************************************
		 * 函数名：connect_handle_read
		 * 描述：connection对象读事件处理函数。
		 * 输入：
		 * 		无
		 * 输出：
		 * 		无
		 * 返回值：bool类型。成功返回true，失败返回false。
		 *********************************************/
		virtual s32 connect_handle_read() = 0;


		/*********************************************
		 * 函数名：connect_handle_write
		 * 描述：connection对象写事件处理函数。
		 * 输入：
		 * 		无
		 * 输出：
		 * 		无
		 * 返回值：bool类型。成功返回true，失败返回false。
		 *********************************************/
		virtual s32 connect_handle_write() = 0;


		/*********************************************
		 * 函数名：connect_handle_timeout
		 * 描述：connection对象定时事件处理函数。
		 * 输入：
		 * 		无
		 * 输出：
		 * 		无
		 * 返回值：bool类型。成功返回true，失败返回false。
		 *********************************************/
		//virtual bool connect_handle_timeout() = 0;


		/*********************************************
		 * 函数名：connect_handle_error
		 * 描述：connection对象连接出错处理函数。
		 * 输入：
		 * 		无
		 * 输出：
		 * 		无
		 * 返回值：bool类型。成功返回true，失败返回false。
		 *********************************************/
		virtual s32 connect_handle_error() = 0;


		/******************************************************
		 * 函数名: connect_read
		 * 描述: connect对象的读数据操作。
		 * 输入:
		 *		buf_len:u32类型，需要读的数据大小。
		 *		flag:u32类型，暂未使用。
		 * 输出:
		 * 		buf:s8类型指针，用于存储从socket读到的数据的缓冲区。
		 * 返回值: s32类型。成功返回读到的字节数，失败返回FAILURE。
		 *******************************************************/
		virtual s32 connect_read(s8* buf, u32 buf_len, u32 flag);


		/******************************************************
		 * 函数名: connect_write
		 * 描述: connect对象的读数据操作。
		 * 输入:
		 * 		buf:s8类型指针，用于向socket发送数据的缓冲区。
		 *		buf_len:u32类型，需要写的数据大小。
		 *		flag:u32类型，暂未使用。
		 * 输出:
		 * 返回值: s32类型。成功返回已写的字节数，失败返回FAILURE。
		 *******************************************************/
		virtual s32 connect_write(s8* buf, u32 buf_len, u32 flag);


		/******************************************************
		 * 函数名: connect_sendfile
		 * 描述: connect对象向sock_fd发送文件数据。
		 * 输入:
		 * 		file_fd:s32类型，已打开的待读的文件描述符。
		 *		offset:off_t类型指针，该指针的值表示从文件开始的偏移位置。
		 *		size:size_t类型大小，需要读的数据的大小。
		 *		flag:u32类型，暂未使用。
		 * 输出:
		 * 返回值: s32类型。成功返回已写的字节数，失败返回FAILURE。
		 *******************************************************/
		virtual s32 connect_sendfile(s32 file_fd, off_t *offset, size_t size, u32 flag);

	};
};

#endif /* connection_H_ */
