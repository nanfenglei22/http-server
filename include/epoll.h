/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* �ļ����ƣ�epoll.h
*
* ժҪ��epoll����ģ�������ı����ͺ�������
*/

//������ԭ���ߡ��汾����ɡ����ں͵�ǰ�汾����Ϣ
/* ��ǰ�汾��1.0.0
* ���ߣ���־����������ڣ�2012��11��13��
*/
#ifndef EPOLL_H_
#define EPOLL_H_

#include "base_connect.h"
#include "base_server.h"
#include "http_connection.h"
#include "http_server.h"

#define PURE_EPOLL_BUFFER_SIZE		1024
#define PURE_EPOLL_MAX_EVENTS		20

/* pure_epoll events */
#define PURE_EPOLLIN				EPOLLIN
#define PURE_EPOLLPRI				EPOLLPRI
#define PURE_EPOLLOUT				EPOLLOUT
#define PURE_EPOLLRDNORM			EPOLLRDNORM
#define PURE_EPOLLRDBAND			EPOLLRDBAND
#define PURE_EPOLLWRNORM			EPOLLWRNORM
#define PURE_EPOLLWRBAND			EPOLLWRBAND
#define PURE_EPOLLMSG				EPOLLMSG
#define PURE_EPOLLERR				EPOLLERR
#define PURE_EPOLLHUP				EPOLLHUP
#define PURE_EPOLLRDHUP				EPOLLRDHUP
#define PURE_EPOLLONESHOT			EPOLLONESHOT
#define PURE_EPOLLET				EPOLLET

/* Valid opcodes ( "opt" parameter ) to issue to pure_epoll_ctl(). */
#define PURE_EPOLL_CTL_ADD			1	//Add a file decriptor to the interface.
#define PURE_EPOLL_CTL_DEL			2	//Remove a file decriptor from the interface.
#define PURE_EPOLL_CTL_MOD			3	//Change file decriptor epoll_event structure.

/////////////////////////////////////// typedef definition ///////////////////////////////////
typedef struct epoll_event			pure_epoll_event;

//////////////////////////////////////////////////////////////////////////////////////////////

namespace pure_baselib
{

	class pure_epoll_event_queue
	{
	public:
		connection					*con;		//connect node;
		pure_epoll_event			event;		//epoll event;
		pure_epoll_event_queue		*next;		//next node;
		pure_epoll_event_queue() : con(NULL), next(NULL) { memset(&event, 0, sizeof(pure_epoll_event)); }
		virtual ~pure_epoll_event_queue(){}
	};

	class pure_epoll
	{
	public:
		s32				max_fds;//max fd num
		s32				max_events;
		s32				epfd;//epoll fd
	private:
		pure_epoll_event_queue		ev_head;
		pure_epoll_event_queue		*ev_tail;

		/******************************************************************
		 * ������: pure_epoll_ctl
		 * ����: ����epoll���ӡ�ɾ�����޸�һ����������
		 * ����:
		 * 		epfd:s32���ͣ�ͨ��pure_epoll_create������������epoll�ļ���������
		 * 		opt:s32���ͣ�epoll�����ӡ�ɾ�����޸ĵ�ѡ�
		 * 		fd:s32���ͣ���Ҫepoll�����socket��������
		 * 		ev:pure_epoll_event����ָ�룬��ҪΪfdע����¼���ַ��
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 ******************************************************************/
		s32 pure_epoll_ctl(s32 epfd, s32 opt, s32 fd, pure_epoll_event *ev);


		/****************************************************************
		 * ������: pure_poll_wait
		 * ����: ����epoll��õ�ǰ��Ծ�������¼�������
		 * ����:
		 * 		epfd:s32���ͣ�ͨ��pure_epoll_create������������epoll�ļ���������
		 *		max_events:s32���ͣ��洢epoll�¼�����Ĵ�С��
		 * 		timeout:s32���ͣ�epollѭ���Ķ�ʱ������������ȡֵ��
		 * 				����:��Ҫ�ȴ�����ֵ��ʱ��ŷ��ء���λ(����)
		 * 				0:�������ء�
		 * 				-1:��Ҫ�ȴ���ֻҪ���¼���Ӧ�Ż᷵�أ�����һֱ�ڵȴ���
		 * ���:
		 * 		events:pure_epoll_event����ָ�룬���epollʱ��������׵�ַ��
		 * ����ֵ:�ɹ����ػ�û�Ծ�������ĸ�����ʧ�ܷ���-1��
		 ****************************************************************/
		s32 pure_poll_wait(s32 epfd, pure_epoll_event *events, s32 max_events, s32 timeout);


		/****************************************************************
		 * ������: pure_epoll_pwait
		 * ����: ����epoll��õ�ǰ��Ծ�������¼�������sigmaskΪNULLʱͬpure_poll_wait������
		 * 		��ΪNULLʱ��ʾ�ȴ�ĳ���ض����źŻ�����������ԭ�Ӳ���
		 * ����:
		 * 		epfd:s32���ͣ�ͨ��pure_epoll_create������������epoll�ļ���������
		 *		max_events:s32���ͣ��洢epoll�¼�����Ĵ�С��
		 * 		timeout:s32���ͣ�epollѭ���Ķ�ʱ������������ȡֵ��
		 * 				����:��Ҫ�ȴ�����ֵ��ʱ��ŷ��ء���λ(����)
		 * 				0:�������ء�
		 * 				-1:��Ҫ�ȴ���ֻҪ���¼���Ӧ�Ż᷵�أ�����һֱ�ڵȴ���
		 * 		sigmask:__sigset_t����ָ�롣
		 * ���:
		 * 		events:pure_epoll_event����ָ�룬���epollʱ��������׵�ַ��
		 * ����ֵ:�ɹ����ػ�û�Ծ�������ĸ�����ʧ�ܷ���-1��
		 ****************************************************************/
		s32 pure_epoll_pwait(s32 epfd, pure_epoll_event *events, s32 max_events, s32 timeout, __sigset_t *sigmask);


		/****************************************************
		 * ������: pure_epoll_event_add
		 * ����: ��epoll��event queue���������һ��event�ڵ㡣
		 * ����:
		 * 		event:pure_epoll_event_queue����ָ�룬��Ҫ��ӽڵ�ĵ�ַ��
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *****************************************************/
		s32 pure_epoll_event_add(pure_epoll_event_queue *event);


		/****************************************************
		 * ������: pure_epoll_event_del
		 * ����: ��epoll��event queue������ɾ��һ��ָ��fd�Ľڵ㡣
		 * ����:
		 * 		fd:s32���ͣ���Ҫɾ���ڵ��fdֵ��
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *****************************************************/
		s32 pure_epoll_event_del(connection *con);


		/****************************************************
		 * ������: pure_epoll_event_destroy
		 * ����: ����epoll��event_queue����
		 * ����:
		 * 		�ޡ�
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *****************************************************/
		s32 pure_epoll_event_destroy(void);


		/****************************************************
		 * ������: pure_epoll_get_event_head
		 * ����: ����epoll��event_queue����
		 * ����:
		 * 		�ޡ�
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *****************************************************/
		//inline pure_epoll_event_queue* pure_epoll_get_event_head(void) { return &ev_head; }

	public:
		pure_epoll();

		virtual ~pure_epoll();

		/****************************************************
		 * ������: pure_epoll_create
		 * ����: ����epoll�ļ���������
		 * ����:
		 * 		max_fds:s32���ͣ����Դ򿪵�socket����������������
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *****************************************************/
		s32 pure_epoll_create(s32 max_fds);



		/****************************************************************
		 * ������: pure_epoll_sockfd_regesiter
		 * ����: ��epoll��ע��һ��server���ʵ���ĵ�ַ��
		 * ����:
		 * 		sock:base_socket���ͣ�base_socketָ�룬����ʵ�ֲ�ͬserver�Ķ�̬��
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *****************************************************************/
		s32 pure_epoll_sockfd_regesiter(base_socket *sock);



		/****************************************************************
		 * ������: pure_epoll_process
		 * ����: epoll��ܵ���ѭ����������
		 * ����:
		 * 		�ޡ�
		 * ���:
		 * 		��
		 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
		 *****************************************************************/
		s32 pure_epoll_process(s32 timeout);	//
		/* ����������Ϣ�ĵ�һ��(ͷ)�ڵ㡣*/

		/* �����¼����еĵ�һ��(ͷ)�ڵ� */


		/* �����¼����е����һ��(β)�ڵ� ������һ���ڵ�ʱ��ֱ����β�ڵ������ӣ����ٴ�ͷ������ʱ�� */
		//inline pure_epoll_event_queue* pure_epoll_get_event_tail(void) { return ev_tail; }
	    /******************************************************************************
        *��������my_HandleNewConnect
        *******************************************************************************/
        int my_HandleNewConnect(base_socket *sock, s32 &connfd, socket_addr &client_addr, u32 &length);
        s32 my_epoll_wait(s32 epfd, pure_epoll_event events[], s32 timeout);
        int my_GetAllRequest(base_socket *sock,  pure_epoll_event events[], int i);   
        void my_HandleAllRequest();
    };
};


#endif /* EPOLL_H_ */
