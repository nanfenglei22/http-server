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
 * ������: connect_read
 * ����: connect����Ķ����ݲ�����
 * ����:
 *		buf_len:u32���ͣ���Ҫ�������ݴ�С��
 *		flag:u32���ͣ���δʹ�á�
 * ���:
 * 		buf:s8����ָ�룬���ڴ洢��socket���������ݵĻ�������
 * ����ֵ: s32���͡��ɹ����ض������ֽ�����ʧ�ܷ���FAILURE��
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
 * ������: connect_write
 * ����: connect����Ķ����ݲ�����
 * ����:
 * 		buf:s8����ָ�룬������socket�������ݵĻ�������
 *		buf_len:u32���ͣ���Ҫд�����ݴ�С��
 *		flag:u32���ͣ���δʹ�á�
 * ���:
 * ����ֵ: s32���͡��ɹ�������д���ֽ�����ʧ�ܷ���FAILURE��
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
 * ������: connect_sendfile
 * ����: connect������sock_fd�����ļ����ݡ�
 * ����:
 * 		file_fd:s32���ͣ��Ѵ򿪵Ĵ������ļ���������
 *		offset:off_t����ָ�룬��ָ���ֵ��ʾ���ļ���ʼ��ƫ��λ�á�
 *		size:size_t���ʹ�С����Ҫ�������ݵĴ�С��
 *		flag:u32���ͣ���δʹ�á�
 * ���:
 * ����ֵ: s32���͡��ɹ�������д���ֽ�����ʧ�ܷ���FAILURE��
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

