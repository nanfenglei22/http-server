/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* �ļ����ƣ�connection.h
*
* ժҪ��epollģ����connect����ĳ����༰�ӿ�
*/

//������ԭ���ߡ��汾����ɡ����ں͵�ǰ�汾����Ϣ
/* ��ǰ�汾��1.0.0
* ���ߣ���־����������ڣ�2012��11��23��
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
	s8		*buffer;	//�����洢���ݵĻ�����
	u32		pos;		//��ǰ�ɶ����ݵ�λ�á�
	s32		buf_len;	//��ǰbuffer���е����ݳ��ȡ�
	s8		*last;		//��ʱ�õ�ָ�롣
	u32		end;		//ָ�򻺳�������λ�á�
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
		server					*svr;			//ûһ��connection�����Լ��ƶ���serverָ�룬����server��connection�������
		connection				*next;			//next node;

		virtual ~connection(){}

		/*********************************************
		 * ��������connect_handle_init
		 * ������connection��ʼ���Ĵ�������
		 * ���룺
		 * 		��
		 * �����
		 * 		��
		 * ����ֵ��bool���͡��ɹ�����true��ʧ�ܷ���false��
		 *********************************************/
		virtual s32 connect_handle_init() = 0;


		/*********************************************
		 * ��������connect_handle_read
		 * ������connection������¼���������
		 * ���룺
		 * 		��
		 * �����
		 * 		��
		 * ����ֵ��bool���͡��ɹ�����true��ʧ�ܷ���false��
		 *********************************************/
		virtual s32 connect_handle_read() = 0;


		/*********************************************
		 * ��������connect_handle_write
		 * ������connection����д�¼���������
		 * ���룺
		 * 		��
		 * �����
		 * 		��
		 * ����ֵ��bool���͡��ɹ�����true��ʧ�ܷ���false��
		 *********************************************/
		virtual s32 connect_handle_write() = 0;


		/*********************************************
		 * ��������connect_handle_timeout
		 * ������connection����ʱ�¼���������
		 * ���룺
		 * 		��
		 * �����
		 * 		��
		 * ����ֵ��bool���͡��ɹ�����true��ʧ�ܷ���false��
		 *********************************************/
		//virtual bool connect_handle_timeout() = 0;


		/*********************************************
		 * ��������connect_handle_error
		 * ������connection�������ӳ���������
		 * ���룺
		 * 		��
		 * �����
		 * 		��
		 * ����ֵ��bool���͡��ɹ�����true��ʧ�ܷ���false��
		 *********************************************/
		virtual s32 connect_handle_error() = 0;


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
		virtual s32 connect_read(s8* buf, u32 buf_len, u32 flag);


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
		virtual s32 connect_write(s8* buf, u32 buf_len, u32 flag);


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
		virtual s32 connect_sendfile(s32 file_fd, off_t *offset, size_t size, u32 flag);

	};
};

#endif /* connection_H_ */
