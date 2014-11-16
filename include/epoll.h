/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* 文件名称：epoll.h
*
* 摘要：epoll网络模型类对象的变量和函数声明
*/

//下面是原作者、版本、完成、日期和当前版本的信息
/* 当前版本：1.0.0
* 作者：程志鹏，完成日期：2012年11月13日
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
		 * 函数名: pure_epoll_ctl
		 * 描述: 用于epoll增加、删除、修改一个描述符。
		 * 输入:
		 * 		epfd:s32类型，通过pure_epoll_create方法创建出的epoll文件描述符。
		 * 		opt:s32类型，epoll的增加、删除、修改等选项。
		 * 		fd:s32类型，需要epoll处理的socket描述符。
		 * 		ev:pure_epoll_event类型指针，需要为fd注册的事件地址。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 ******************************************************************/
		s32 pure_epoll_ctl(s32 epfd, s32 opt, s32 fd, pure_epoll_event *ev);


		/****************************************************************
		 * 函数名: pure_poll_wait
		 * 描述: 用于epoll获得当前活跃描述符事件个数。
		 * 输入:
		 * 		epfd:s32类型，通过pure_epoll_create方法创建出的epoll文件描述符。
		 *		max_events:s32类型，存储epoll事件数组的大小。
		 * 		timeout:s32类型，epoll循环的定时器。具有以下取值：
		 * 				正数:需要等待正数值的时间才返回。单位(毫秒)
		 * 				0:立即返回。
		 * 				-1:需要等待，只要有事件相应才会返回，否则一直在等待。
		 * 输出:
		 * 		events:pure_epoll_event类型指针，存放epoll时间的数组首地址。
		 * 返回值:成功返回获得活跃描述符的个数，失败返回-1。
		 ****************************************************************/
		s32 pure_poll_wait(s32 epfd, pure_epoll_event *events, s32 max_events, s32 timeout);


		/****************************************************************
		 * 函数名: pure_epoll_pwait
		 * 描述: 用于epoll获得当前活跃描述符事件个数。sigmask为NULL时同pure_poll_wait函数。
		 * 		不为NULL时表示等待某个特定的信号或描述符进行原子操作
		 * 输入:
		 * 		epfd:s32类型，通过pure_epoll_create方法创建出的epoll文件描述符。
		 *		max_events:s32类型，存储epoll事件数组的大小。
		 * 		timeout:s32类型，epoll循环的定时器。具有以下取值：
		 * 				正数:需要等待正数值的时间才返回。单位(毫秒)
		 * 				0:立即返回。
		 * 				-1:需要等待，只要有事件相应才会返回，否则一直在等待。
		 * 		sigmask:__sigset_t类型指针。
		 * 输出:
		 * 		events:pure_epoll_event类型指针，存放epoll时间的数组首地址。
		 * 返回值:成功返回获得活跃描述符的个数，失败返回-1。
		 ****************************************************************/
		s32 pure_epoll_pwait(s32 epfd, pure_epoll_event *events, s32 max_events, s32 timeout, __sigset_t *sigmask);


		/****************************************************
		 * 函数名: pure_epoll_event_add
		 * 描述: 向epoll的event queue链表中添加一个event节点。
		 * 输入:
		 * 		event:pure_epoll_event_queue类型指针，需要添加节点的地址。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 *****************************************************/
		s32 pure_epoll_event_add(pure_epoll_event_queue *event);


		/****************************************************
		 * 函数名: pure_epoll_event_del
		 * 描述: 从epoll的event queue链表中删除一个指定fd的节点。
		 * 输入:
		 * 		fd:s32类型，需要删除节点的fd值。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 *****************************************************/
		s32 pure_epoll_event_del(connection *con);


		/****************************************************
		 * 函数名: pure_epoll_event_destroy
		 * 描述: 销毁epoll的event_queue链表。
		 * 输入:
		 * 		无。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 *****************************************************/
		s32 pure_epoll_event_destroy(void);


		/****************************************************
		 * 函数名: pure_epoll_get_event_head
		 * 描述: 销毁epoll的event_queue链表。
		 * 输入:
		 * 		无。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 *****************************************************/
		//inline pure_epoll_event_queue* pure_epoll_get_event_head(void) { return &ev_head; }

	public:
		pure_epoll();

		virtual ~pure_epoll();

		/****************************************************
		 * 函数名: pure_epoll_create
		 * 描述: 创建epoll文件描述符。
		 * 输入:
		 * 		max_fds:s32类型，可以打开的socket描述符的最大个数。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 *****************************************************/
		s32 pure_epoll_create(s32 max_fds);



		/****************************************************************
		 * 函数名: pure_epoll_sockfd_regesiter
		 * 描述: 向epoll中注册一个server类的实例的地址。
		 * 输入:
		 * 		sock:base_socket类型，base_socket指针，用于实现不同server的多态。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 *****************************************************************/
		s32 pure_epoll_sockfd_regesiter(base_socket *sock);



		/****************************************************************
		 * 函数名: pure_epoll_process
		 * 描述: epoll框架的主循环处理函数。
		 * 输入:
		 * 		无。
		 * 输出:
		 * 		无
		 * 返回值:成功返回SUCCESS，失败返回FAILURE。
		 *****************************************************************/
		s32 pure_epoll_process(s32 timeout);	//
		/* 返回连接信息的第一个(头)节点。*/

		/* 返回事件队列的第一个(头)节点 */


		/* 返回事件队列的最后一个(尾)节点 当新增一个节点时，直接在尾节点后面添加，减少从头遍历的时间 */
		//inline pure_epoll_event_queue* pure_epoll_get_event_tail(void) { return ev_tail; }
	    /******************************************************************************
        *函数名：my_HandleNewConnect
        *******************************************************************************/
        int my_HandleNewConnect(base_socket *sock, s32 &connfd, socket_addr &client_addr, u32 &length);
        s32 my_epoll_wait(s32 epfd, pure_epoll_event events[], s32 timeout);
        int my_GetAllRequest(base_socket *sock,  pure_epoll_event events[], int i);   
        void my_HandleAllRequest();
    };
};


#endif /* EPOLL_H_ */
