/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* 文件名称：epoll.cpp
*
* 摘要：epoll网络模型类对象的变量和函数定义
*/

//下面是原作者、版本、完成、日期和当前版本的信息
/* 当前版本：1.0.0
* 作者：程志鹏，完成日期：2012年11月13日
*/


#include "epoll.h"
#include <iostream>

#include "http_connection.h"
#include "http_server.h"

using namespace std;


namespace pure_baselib
{
	/*****************************************************************
	 * 函数明：pure_epoll
	 * 说明：pure_epoll对象构造函数
	 * 输入：
	 * 		无
	 * 输出：
	 * 		无
	 * 返回值：
	 * 		无
	 *****************************************************************/
	pure_epoll::pure_epoll() : ev_tail(&ev_head)
	{
		max_fds = max_events = epfd = 0;
		memset(&ev_head, 0, sizeof(pure_epoll_event_queue));
	}

	/****************************************************
	 * 函数名: pure_epoll_create
	 * 描述: 创建epoll文件描述符。
	 * 输入:
	 * 		max_fds:s32类型，可以打开的socket描述符的最大个数。
	 * 输出:
	 * 		无
	 * 返回值:成功返回SUCCESS，失败返回FAILURE。
	 *****************************************************/
	s32 pure_epoll::pure_epoll_create(s32 max_fds)
	{
		this->epfd = epoll_create(max_fds);
		if (-1 == epfd)
		{
			perror("epoll create error!");
			return FAILURE;
		}
		this->max_fds = max_fds;

		return SUCCESS;
	}

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
	s32 pure_epoll::pure_epoll_ctl(s32 epfd, s32 opt, s32 fd, pure_epoll_event *ev)
	{

		return epoll_ctl(epfd, opt, fd, ev);
	}

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
	s32 pure_epoll::pure_poll_wait(s32 epfd, pure_epoll_event *events, s32 max_events, s32 timeout)
	{
		if (NULL == events)
		{
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"argument error, point is NULL!"<<endl;
			return FAILURE;
		}

		this->max_events = max_events;

		return epoll_wait(epfd, events, max_events, timeout);
	}

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
	s32 pure_epoll::pure_epoll_pwait(s32 epfd, pure_epoll_event *events, s32 max_events, s32 timeout, __sigset_t *sigmask)
	{
		if (NULL == events)
		{
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"argument error, point is NULL!"<<endl;
			return FAILURE;
		}

		this->max_events = max_events;

		return epoll_pwait(epfd, events, max_events, timeout, sigmask);
	}

	/****************************************************************
	 * 函数名: pure_epoll_sockfd_regesiter
	 * 描述: 向epoll中注册一个server类的实例的地址。
	 * 输入:
	 * 		sock:base_socket类型，base_socket指针，用于实现不同server的多态。
	 * 输出:
	 * 		无
	 * 返回值:成功返回SUCCESS，失败返回FAILURE。
	 *****************************************************************/
	s32 pure_epoll::pure_epoll_sockfd_regesiter(base_socket *sock)
	{
		//printf("----sock-1->sock_fd=%d------\n",sock->sock_fd);
		pure_epoll_event ev;

		if (NULL == sock)
		{
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"argument error, point is NULL!"<<endl;
			return FAILURE;
		}

		memset(&ev, 0, sizeof(pure_epoll_event));
		ev.data.ptr = sock;
		ev.events = PURE_EPOLLIN | PURE_EPOLLET | PURE_EPOLLRDHUP | PURE_EPOLLOUT;
//EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
//EPOLLOUT：表示对应的文件描述符可以写；
//EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
//EPOLLERR：表示对应的文件描述符发生错误；
//EPOLLHUP：表示对应的文件描述符被挂断；
//EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
//EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

		if (FAILURE == pure_epoll_ctl(epfd, PURE_EPOLL_CTL_ADD, sock->sock_fd, &ev))
		{
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"function pure_epoll_ctl() called error!"<<endl;
			return FAILURE;
		}
		return SUCCESS;
	}

	/****************************************************
	 * 函数名: pure_epoll_event_add
	 * 描述: 向epoll的event queue链表中添加一个event节点。
	 * 输入:
	 * 		event:pure_epoll_event_queue类型指针，需要添加节点的地址。
	 * 输出:
	 * 		无
	 * 返回值:成功返回SUCCESS，失败返回FAILURE。
	 *****************************************************/
	s32 pure_epoll::pure_epoll_event_add(pure_epoll_event_queue *event)
	{
		if (NULL == event)
		{
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"argument error, point is NULL!"<<endl;
			return FAILURE;
		}

		this->ev_tail->next = event;
		this->ev_tail = this->ev_tail->next;

		return SUCCESS;
	}

	/****************************************************
	 * 函数名: pure_epoll_event_del
	 * 描述: 从epoll的event queue链表中删除一个指定fd的节点。
	 * 输入:
	 * 		fd:s32类型，需要删除节点的fd值。
	 * 输出:
	 * 		无
	 * 返回值:成功返回SUCCESS，失败返回FAILURE。
	 *****************************************************/
	s32 pure_epoll::pure_epoll_event_del(connection *con)
	{
		pure_epoll_event_queue *pre_node = &this->ev_head;
		pure_epoll_event_queue *cur_node = this->ev_head.next;

		while (cur_node != NULL)
		{
			if (cur_node->con == con)
			{
				if (cur_node == this->ev_tail)
				{
					this->ev_tail = pre_node;
				}
				pre_node->next = cur_node->next;
				delete cur_node;
				cur_node = NULL;
				break;
			}
			pre_node = cur_node;
			cur_node = cur_node->next;
		}

		return SUCCESS;
	}

	/****************************************************
	 * 函数名: pure_epoll_event_destroy
	 * 描述: 销毁epoll的event_queue链表。
	 * 输入:
	 * 		无。
	 * 输出:
	 * 		无
	 * 返回值:成功返回SUCCESS，失败返回FAILURE。
	 *****************************************************/
	s32 pure_epoll::pure_epoll_event_destroy(void)
	{
		pure_epoll_event_queue *pre_node = &this->ev_head;
		pure_epoll_event_queue *cur_node = this->ev_head.next;

		while (cur_node != NULL)
		{
			pre_node->next = cur_node->next;
			delete cur_node;
			cur_node = pre_node->next;
		}

		this->ev_head.next = NULL;

		return SUCCESS;
	}
    s32 pure_epoll::my_epoll_wait(s32 epfd, pure_epoll_event events[], s32 timeout) {
            if (this->max_fds >= PURE_EPOLL_MAX_EVENTS) { 
                s32 nfds = pure_poll_wait(epfd, events, PURE_EPOLL_MAX_EVENTS, timeout);
            else {
                s32 nfds = pure_poll_wait(epfd, events, this->max_fds, timeout);
            }
			if(nfds == -1)
			{
				if(errno == EBADF )
					perror("epfd 不是一个有效的文件描述符");
				else if(errno == EFAULT )
					perror("events 指向的内存没有写权限");
				else if(errno == EINTR )
					perror("在请求事件就绪或 timeout 到期之前，这个调用被信号处理器中断");
				else if(errno == EINVAL )
					perror("epfd 不是一个 epoll 文件描述符，或 maxevents 小于或等于零");

				printf("<><><><><>     exit     <><><><><>\n");
				exit(EXIT_FAILURE);
			}
            return nfds;

    }
    int pure_epoll::my_HandleNewConnect(base_socket *sock, s32 &connfd, socket_addr &client_addr, u32 &length)  {
                    server *svr = dynamic_cast <server*> (sock);
					if (NULL == svr)
						return true;
					//printf("[%s]----[%d]-----svr->sock_fd=%d-----\n",__func__,__LINE__,svr->sock_fd);
					while ((connfd = accept(svr->sock_fd,(sockaddr*)&client_addr, &length)) > 0)
					{
						//printf("[%s]----[%d]------connfd=%d----\n",__func__,__LINE__,connfd);
						connection *con = NULL;
						//CHttp_connection *con = NULL;
						pure_epoll_event ev;

						con= dynamic_cast <connection*>(svr->server_connect_create(connfd));	//由server创建一个新的connection节点
						if (NULL == con)
						{
							cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"function server_connect_create() error!"<<endl;
							return FAILURE;
						}
						//
						con->svr = svr;
						con->sock_fd = connfd;
						con->sock_type = 0;
						//
						if (FAILURE == pure_epoll_sockfd_regesiter(static_cast <base_socket*>(con)))	//将新的connection节点注册到epoll中
						{
							cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"function pure_epoll_sockfd_regesiter() error!"<<endl;
							return FAILURE;
						}
					}
                    if (connfd < 0)
					{
						if ((errno != EAGAIN) && (errno != ECONNABORTED)
								&& (errno != EPROTO) && (errno != EINTR))
						{
							perror("accept error!");
						}
						else if (errno == EMFILE || errno == ENFILE)
						{
							perror("WARNING: file descriptors is run out!");
						}
					}
                    return true;
    }
    int pure_epoll::my_GetAllRequest(base_socket *sock,  pure_epoll_event events[], int i){
                    connection *con = dynamic_cast <connection*> (sock);
					if (NULL == con)
					{
						cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"dynamic_cast error! point is NULL!"<<endl;
						return true;
					}

					/* 如果得到的epoll时间为以下值表示某一个sock_fd出错了，需要手动处理相应的链表节点 */
					if ((events[i].events & PURE_EPOLLERR) || (events[i].events & PURE_EPOLLRDHUP) || (events[i].events & PURE_EPOLLHUP)) {
						if(events[i].events & PURE_EPOLLERR) {
							printf("events[i].events & PURE_EPOLLERR\n");
                        } else if(events[i].events & PURE_EPOLLRDHUP) {
							printf("events[i].events & PURE_EPOLLRDHUP\n");
                        } else if(events[i].events & PURE_EPOLLHUP) {
							printf("events[i].events & PURE_EPOLLHUP\n");
                        }
						perror("epoll errors!");
						/* 删除server的connection队列中的connect节点 */
						con->svr->server_connect_delete(con);
						/* 删除event_queue队列中的connect节点 */
						pure_epoll_event_del(con);
					}
					else
					{
						//printf("[%s]----[%d]----------\n",__func__,__LINE__);
						if (!(events[i].events & PURE_EPOLLIN) && !(events[i].events & PURE_EPOLLOUT))
						{
							perror("Unknow!");
							return true;
						}

						pure_epoll_event_queue *ev_event = new pure_epoll_event_queue();
						if (NULL == ev_event)
						{
							cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"dynamic_cast error! point is NULL!"<<endl;
							return FAILURE;
						}

						if (events[i].events & PURE_EPOLLIN)
						{
							con->readable = true;
						}
						else
						{
							con->writeable = true;
						}
						ev_event->con = con;
						ev_event->event = events[i];
						if (FAILURE == pure_epoll_event_add(ev_event))
						{
							cout<<"["<<__FILE__<<", "<<__LINE__<<"]"<<" ##function pure_epoll_event_add() error!"<<endl;
							exit(EXIT_FAILURE);
						}
					}
                    return true;
    }

    void pure_epoll::my_HandleAllRequest() {
            pure_epoll_event_queue *ev_cur = &ev_head;
			while ((ev_cur = ev_cur->next) != NULL) {
				bool is_delete = false;
				if (ev_cur->event.events & PURE_EPOLLIN) {
					//printf("[%s]----[%d]------PURE_EPOLLIN----\n",__func__,__LINE__);
					if (ev_cur->con->readable) {
						//printf("[%s]----[%d]------ev_cur->con->readable----\n",__func__,__LINE__);
						s32 retValue;
						retValue =  ev_cur->con->connect_handle_read();
						if (FAILURE == retValue) {
							is_delete = true;
							goto epollin_clear;
						} else if (RETRY == retValue) {
							continue;
						} else if(SUCCESS == retValue) {
							//close(ev_cur->con->sock_fd);
							pure_epoll_event_del(ev_cur->con);
							//ev_cur->con->svr->server_connect_delete(ev_cur->con);
						}
						//TODO:If success, what do you want to do? Please write by yourself.
					}
epollin_clear:
					if (is_delete) {
						perror("connect handler read error");
						close(ev_cur->con->sock_fd);
						ev_cur->con->svr->server_connect_delete(ev_cur->con);
						pure_epoll_event_del(ev_cur->con);
					}
				}
            
				else if (ev_cur->event.events & PURE_EPOLLOUT) {
					//printf("[%s]----[%d]-------PURE_EPOLLOUT-ev_cur->con->writeable=%d--\n",__func__,__LINE__,ev_cur->con->writeable);
					if (ev_cur->con->writeable) {
						s32 retValue;
						retValue =  ev_cur->con->connect_handle_write();
						if (FAILURE == retValue) {
							is_delete = true;
							goto epollin_clear;
						} else if (RETRY == retValue) {
							continue;
						} else if(SUCCESS == retValue) {
							//close(ev_cur->con->sock_fd);
							pure_epoll_event_del(ev_cur->con);
							//ev_cur->con->svr->server_connect_delete(ev_cur->con);
						}
					}
				}
            }
    }
	/****************************************************************
	 * 函数名: pure_epoll_process
	 * 描述: epoll框架的主循环处理函数。
	 * 输入:
	 * 		无。
	 * 输出:
	 * 		无
	 * 返回值:成功返回SUCCESS，失败返回FAILURE。
	 *****************************************************************/
	s32 pure_epoll::pure_epoll_process(s32 timeout)
	{
		s32 connfd = 0;
		u32 length = 0;
		s32 fd = 0;
		socket_addr client_addr;
		pure_epoll_event events[PURE_EPOLL_MAX_EVENTS] = { 0 };
			
		for (; ;)
		{
			s32 i = 0;
            s32 nfds = my_epoll_wait(epfd,events,timeout);
			for (; i<nfds; i++)
			{
			    base_socket *sock = static_cast <base_socket*>(events[i].data.ptr);
				//printf("[%s]----[%d]-----sock->sock_fd=%d--sock->sock_type=%d---\n",__func__,__LINE__,sock->sock_fd,sock->sock_type);
				if (sock->sock_type)	//如果sock_type为1，则是服务器的sock_fd，那么需要使用accept来接受请求。
				{
			        if (my_HandleNewConnect(sock, connfd, client_addr, length) == FAILURE) {
                         return FAILURE;
                    } 	    
				}
				else
				{  
                    if (my_GetAllRequest(sock,  events, i) == FAILURE) {
                       return FAILURE;
                    }		
				}
			}
            my_HandleAllRequest();
		}
		return SUCCESS;
	}

	/*****************************************************************
	 * 函数明：~pure_epoll
	 * 说明：pure_epoll类的析构函数
	 * 输入：
	 * 		无
	 * 输出：
	 * 		无
	 * 返回值：
	 * 		无
	 *****************************************************************/
	pure_epoll::~pure_epoll()
	{
		cout<<"~pure_epoll()!"<<endl;
		if (0 != epfd)
		{
			close(epfd);
			cout<<"close epoll fd!"<<endl;
		}
		pure_epoll_event_destroy();
	}
}


