/*
 * base_connect.cpp
 *
 *  Created on: 2012-11-28
 *      Author: chengzhip
 */

#include "base_connect.h"
#include <iostream>
using namespace std;

namespace pure_baselib
{

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
s32 connection::connect_read(s8* buf, u32 buf_len, u32 flag)
{
	if (NULL == buf)
	{
		cout<<"["<<__FILE__<<", "<<__LINE__<<"]"<<" argument error, point is NULL!"<<endl;
		return FAILURE;
	}

	if (flag)
	{
		//TODO: flag unused...
	}

	return read(this->sock_fd, buf, buf_len);
}

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
s32 connection::connect_write(s8* buf, u32 buf_len, u32 flag)
{
	if (NULL == buf)
	{
		cout<<"["<<__FILE__<<", "<<__LINE__<<"]"<<" argument error, point is NULL!"<<endl;
		return FAILURE;
	}

	if (flag)
	{
		//TODO: flag unused...
	}

	return write(this->sock_fd, buf, buf_len);
}


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
s32 connection::connect_sendfile(s32 file_fd, off_t *offset, size_t size, u32 flag)
{
	if (flag)
	{
		//TODO: flag unused...
	}

	return sendfile(this->sock_fd, file_fd, offset, size);
}

};

