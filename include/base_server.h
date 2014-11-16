/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* �ļ����ƣ�server.h
*
* ժҪ��epollģ�͵�server�����༰�ӿ�
*/

//������ԭ���ߡ��汾����ɡ����ں͵�ǰ�汾����Ϣ
/* ��ǰ�汾��1.0.0
* ���ߣ���־����������ڣ�2012��11��23��
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
		connection		*con_head;		//connectͷ�ڵ�
		connection		*con_tail;		//connectβ�ڵ�
	public:

		virtual ~server(){}

		/*********************************************
		 * ��������server_sockfd_create
		 * ����������server��socket����������ֵ��ֵ��
		 * 		base_socket��sock_fd�����������͡�
		 * ���룺
		 * 		��
		 * �����
		 * 		��
		 * ����ֵ��s32���͡��ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *********************************************/
		virtual s32 server_sockfd_create(void) = 0;


		/****************************************************************
		 * ��������server_sockfd_bind
		 * ��������server��sock_fd�ﶨ��ָ���Ķ˿ںš�
		 * ���룺
		 * 		addr��socket_addr����ָ�롣������Ҫ�ﶨ��ip��ַ�Ͷ˿ںŵĵ�ַ��
		 * 		addr_len��u32���͡�socket_addr�ṹ��Ĵ�С
		 * �����
		 * 		��
		 * ����ֵ��s32���͡��ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 ***************************************************************/
		virtual s32 server_sockfd_bind(socket_addr *addr, u32 addr_len) = 0;


		/***************************************************
		 * ��������server_sockfd_listen
		 * ����������server�����sock_fd��
		 * ���룺
		 * 		listen_num��s32���͡����ɼ����ĸ�����С��
		 * �����
		 * 		��
		 * ����ֵ��s32���͡��ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 ***************************************************/
		virtual s32 server_sockfd_listen(s32 listen_num) = 0;


		/*************************************************************
		 * ��������server_connect_create
		 * ����������ָ��connfd��connection�ڵ㡣
		 * ���룺
		 * 		connfd��s32���͡���Ҫ�������ӽڵ��socket_fdֵ��
		 * �����
		 * 		��
		 * ����ֵ��connection����ָ�롣�ɹ������´����Ľڵ�ĵ�ַ��ʧ�ܷ���NULL��
		 *************************************************************/
		virtual connection* server_connect_create(s32 connfd) = 0;


		/*************************************************************
		 * ��������server_connect_create
		 * ������ɾ��ָ��connfd��connection�ڵ㡣
		 * ���룺
		 * 		con��connection����ָ�롣��Ҫɾ�����ӽڵ��ַ��
		 * �����
		 * 		��
		 * ����ֵ��bool���͡��ɹ�����true��ʧ�ܷ���false��
		 *************************************************************/
		virtual bool server_connect_delete(connection *con) = 0;


		/*************************************************************
		 * ��������server_connect_create
		 * ����������connection����
		 * ���룺
		 * 		��
		 * �����
		 * 		��
		 * ����ֵ��bool���͡��ɹ�����true��ʧ�ܷ���false��
		 *************************************************************/
		virtual bool server_connect_destroy(void) = 0;

	};
};

#endif /* server_H_ */
