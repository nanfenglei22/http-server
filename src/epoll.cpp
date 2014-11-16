/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* �ļ����ƣ�epoll.cpp
*
* ժҪ��epoll����ģ�������ı����ͺ�������
*/

//������ԭ���ߡ��汾����ɡ����ں͵�ǰ�汾����Ϣ
/* ��ǰ�汾��1.0.0
* ���ߣ���־����������ڣ�2012��11��13��
*/


#include "epoll.h"
#include <iostream>

#include "http_connection.h"
#include "http_server.h"

using namespace std;


namespace pure_baselib
{
	/*****************************************************************
	 * ��������pure_epoll
	 * ˵����pure_epoll�����캯��
	 * ���룺
	 * 		��
	 * �����
	 * 		��
	 * ����ֵ��
	 * 		��
	 *****************************************************************/
	pure_epoll::pure_epoll() : ev_tail(&ev_head)
	{
		max_fds = max_events = epfd = 0;
		memset(&ev_head, 0, sizeof(pure_epoll_event_queue));
	}

	/****************************************************
	 * ������: pure_epoll_create
	 * ����: ����epoll�ļ���������
	 * ����:
	 * 		max_fds:s32���ͣ����Դ򿪵�socket����������������
	 * ���:
	 * 		��
	 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
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
	s32 pure_epoll::pure_epoll_ctl(s32 epfd, s32 opt, s32 fd, pure_epoll_event *ev)
	{

		return epoll_ctl(epfd, opt, fd, ev);
	}

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
	 * ������: pure_epoll_sockfd_regesiter
	 * ����: ��epoll��ע��һ��server���ʵ���ĵ�ַ��
	 * ����:
	 * 		sock:base_socket���ͣ�base_socketָ�룬����ʵ�ֲ�ͬserver�Ķ�̬��
	 * ���:
	 * 		��
	 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
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
//EPOLLIN ����ʾ��Ӧ���ļ����������Զ��������Զ�SOCKET�����رգ���
//EPOLLOUT����ʾ��Ӧ���ļ�����������д��
//EPOLLPRI����ʾ��Ӧ���ļ��������н��������ݿɶ�������Ӧ�ñ�ʾ�д������ݵ�������
//EPOLLERR����ʾ��Ӧ���ļ���������������
//EPOLLHUP����ʾ��Ӧ���ļ����������Ҷϣ�
//EPOLLET�� ��EPOLL��Ϊ��Ե����(Edge Triggered)ģʽ�����������ˮƽ����(Level Triggered)��˵�ġ�
//EPOLLONESHOT��ֻ����һ���¼���������������¼�֮���������Ҫ�����������socket�Ļ�����Ҫ�ٴΰ����socket���뵽EPOLL������

		if (FAILURE == pure_epoll_ctl(epfd, PURE_EPOLL_CTL_ADD, sock->sock_fd, &ev))
		{
			cout<<"["<<__FILE__<<", "<<__LINE__<<"] "<<"function pure_epoll_ctl() called error!"<<endl;
			return FAILURE;
		}
		return SUCCESS;
	}

	/****************************************************
	 * ������: pure_epoll_event_add
	 * ����: ��epoll��event queue���������һ��event�ڵ㡣
	 * ����:
	 * 		event:pure_epoll_event_queue����ָ�룬��Ҫ��ӽڵ�ĵ�ַ��
	 * ���:
	 * 		��
	 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
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
	 * ������: pure_epoll_event_del
	 * ����: ��epoll��event queue������ɾ��һ��ָ��fd�Ľڵ㡣
	 * ����:
	 * 		fd:s32���ͣ���Ҫɾ���ڵ��fdֵ��
	 * ���:
	 * 		��
	 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
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
	 * ������: pure_epoll_event_destroy
	 * ����: ����epoll��event_queue����
	 * ����:
	 * 		�ޡ�
	 * ���:
	 * 		��
	 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
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
					perror("epfd ����һ����Ч���ļ�������");
				else if(errno == EFAULT )
					perror("events ָ����ڴ�û��дȨ��");
				else if(errno == EINTR )
					perror("�������¼������� timeout ����֮ǰ��������ñ��źŴ������ж�");
				else if(errno == EINVAL )
					perror("epfd ����һ�� epoll �ļ����������� maxevents С�ڻ������");

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

						con= dynamic_cast <connection*>(svr->server_connect_create(connfd));	//��server����һ���µ�connection�ڵ�
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
						if (FAILURE == pure_epoll_sockfd_regesiter(static_cast <base_socket*>(con)))	//���µ�connection�ڵ�ע�ᵽepoll��
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

					/* ����õ���epollʱ��Ϊ����ֵ��ʾĳһ��sock_fd�����ˣ���Ҫ�ֶ�������Ӧ������ڵ� */
					if ((events[i].events & PURE_EPOLLERR) || (events[i].events & PURE_EPOLLRDHUP) || (events[i].events & PURE_EPOLLHUP)) {
						if(events[i].events & PURE_EPOLLERR) {
							printf("events[i].events & PURE_EPOLLERR\n");
                        } else if(events[i].events & PURE_EPOLLRDHUP) {
							printf("events[i].events & PURE_EPOLLRDHUP\n");
                        } else if(events[i].events & PURE_EPOLLHUP) {
							printf("events[i].events & PURE_EPOLLHUP\n");
                        }
						perror("epoll errors!");
						/* ɾ��server��connection�����е�connect�ڵ� */
						con->svr->server_connect_delete(con);
						/* ɾ��event_queue�����е�connect�ڵ� */
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
	 * ������: pure_epoll_process
	 * ����: epoll��ܵ���ѭ����������
	 * ����:
	 * 		�ޡ�
	 * ���:
	 * 		��
	 * ����ֵ:�ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
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
				if (sock->sock_type)	//���sock_typeΪ1�����Ƿ�������sock_fd����ô��Ҫʹ��accept����������
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
	 * ��������~pure_epoll
	 * ˵����pure_epoll�����������
	 * ���룺
	 * 		��
	 * �����
	 * 		��
	 * ����ֵ��
	 * 		��
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


