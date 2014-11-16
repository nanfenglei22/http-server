/*************************************************************************
                  Copyright (C) 2012   Shanghai Surfing
                                All Rights Reserved
                                      R&D Dept.                                            
*************************************************************************/
/*************************************************************************

Filename:         	pure_http_prase.cpp

Description:    	http_prase Function Define Source File.

Author:          		gaoyang

Date:             	2012-11-17 

Version:   		V1.0.0

Modified:      

*************************************************************************/
#include "http_server.h"


//week
DMM_S wday_tab1[] = 
{
    { "Sun", 0 }, { "Mon", 1 }, { "Tue", 2 }, { "Wed", 3 },
    { "Thu", 4 }, { "Fri", 5 }, { "Sat", 6 }, 
};

//month
DMM_S mon_tab1[] = 
{
    { "Jan", 0 }, { "Feb", 1 }, { "Mar", 2 }, { "Apr", 3 },
    { "May", 4 }, { "Jun", 5 }, { "Jul", 6 }, { "Aug", 7 },
    { "Sep", 8 }, { "Oct", 9 }, { "Nov", 10 }, { "Dec", 11 },
};
//end
using namespace pure_frame;


//public:
CHttpServer::CHttpServer()
{
	//m_http_fd = 0;
	//m_https_fd = 0;
	m_servers = NULL;
	m_usedconns = NULL;
	m_count = 0 ;
}

CHttpServer::~CHttpServer()
{
	//if (m_http_fd != 0)
		//close(m_http_fd);
	//if (m_https_fd != 0)
		//close(m_https_fd);
	m_usedconns = NULL;
	m_servers = NULL;
	
}


s32 CHttpServer::server_sockfd_create(void)
{
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket create error!");
		return FAILURE;
	}

	if (FAILURE == set_nonblocking(sock_fd))
	{
		printf(" set_nonblocking error!\n");
		return FAILURE;
	}
	sock_type =1;
}

//server_sockfd_bind(&server_addr,sizeof(server_addr));
s32 CHttpServer::server_sockfd_bind(socket_addr *addr, u32 addr_len)
{
	//struct sockaddr_in server_addr;
	struct sockaddr_in server_addr;

	u32 opt = 0;
	opt = SO_REUSEADDR;
	
	if (0 != setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		close(sock_fd);
		printf(" setsockopt error!\n");
		return FAILURE;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(CONFIG_HTTP_PORT);
	//server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);;

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		close(sock_fd);
		perror("socket bind error!");
		return FAILURE;
	}
	return SUCCESS;
}

s32 CHttpServer::server_sockfd_listen(s32 listen_num)
{
	if(listen_num>0)
	{
		listen(sock_fd, listen_num);
		return SUCCESS;
	}
	else
		return FAILURE;
	
}


//CHttp_connection* CHttpServer::server_connect_create(s32 connfd)
connection *CHttpServer::server_connect_create(s32 connfd)
{
	set_nonblocking(connfd);
	CHttp_connection* ptest = new CHttp_connection();
	if (ptest == NULL) {
        return NULL;
    }
	//ptest->sock_fd = connfd;
	CON_S *ptp;

	//Get ourselves a CON_S 
    //如果没有已经释放的节点，就新申请一个节点给新的连接
    //如果已经有释放的节点，就获取释放节点的头给新的连接
	if (m_freeconns == NULL) { 
		ptest->m_pcon = (CON_S *)calloc(1, sizeof(CON_S));
        if (ptest->m_pcon == NULL) {
            return NULL;
        }
    } else {
		ptest->m_pcon = m_freeconns;
		m_freeconns = ptest->m_pcon->pnext;
	}
	//changed by simon 2012-12-15
	ptest->tdate_init();
	//end
	//Attach it to the used list 
	ptest->m_pcon->pnext = m_usedconns;
	m_usedconns = ptest->m_pcon;
    //给新的客户端连接初始化相应的属性
	ptest->m_pcon->networkdesc = connfd;
    strcpy(ptest->m_pcon->default_dir, "/home/lizhiming/study/c++/network/httpserver/http-epoll/test");
	ptest->m_pcon->is_ssl = 0;
	ptest->m_pcon->filedesc = -1;
#ifdef _CONFIG_HTTP_HAS_DIRECTORIES_
	ptest->m_pcon->dirp = NULL;
#endif

	*ptest->m_pcon->actualfile = '\0';
	*ptest->m_pcon->filereq = '\0';
	ptest->m_pcon->state = STATE_WANT_TO_READ_HEAD;
	ptest->m_pcon->reqtype = TYPE_GET;
	ptest->m_pcon->close_when_done = 0;
	ptest->m_pcon->timeout = time(NULL) + CONFIG_HTTP_TIMEOUT;

	//changed by simon 2012-12-12
	ptest->m_pcon->range = 0;
	ptest->m_pcon->range_from = 0;
	ptest->m_pcon->range_end = 0;
	//end
	//ptest->m_pcon = ptp;
	return ptest;

}


bool CHttpServer::server_connect_delete(connection *con)
{
	CHttp_connection* ptest = (CHttp_connection*)con;
	if(ptest)
	{
		remove_connection(ptest->m_pcon);
		return SUCCESS;
	}
	else
		return FAILURE;
}


bool CHttpServer::server_connect_destroy(void)
{
	CON_S *ptp;
	s32 shouldret = 0;
	ptp = m_usedconns;
	while (ptp) 
	{
		remove_connection(ptp);
		ptp = ptp->pnext;
	}
	m_usedconns = NULL;
	return SUCCESS;
}
/*
//请求或者响应中时间模块初始化
void CHttpServer::tdate_init(void)
{
	qsort(wday_tab1, sizeof(wday_tab1)/sizeof(DMM_S),
		sizeof(DMM_S), 
            	(s32 (*)(const void *, const void *))day_mon_map_compare);
   	qsort(mon_tab1, sizeof(mon_tab1)/sizeof(DMM_S),
            	sizeof(DMM_S), 
            	(s32 (*)(const void *, const void *))day_mon_map_compare);
}

//请求或者响应中时间解析模块的比较模块
s32 CHttpServer::day_mon_map_compare(const s8 *i_pv1, const s8 *i_pv2)
{
    	return strcmp(((DMM_S*)i_pv1)->s, ((DMM_S*)i_pv2)->s);
}
*/

//把server节点添加到server链表中去，以便查找,同时开启多个服务器时，可以使用
void CHttpServer::addto_servers(s32 i_sd) 
{
	SEST_S *ptp = (SEST_S *)calloc(1, sizeof(SEST_S));
	ptp->pnext = NULL;
	ptp->sd = i_sd;
	m_count += 1;
	m_servers = ptp;
}



s32 CHttpServer::open_listen(s32 i_port)
{
	printf("<<-----open_listen------>>\n");
	s32 sfd;
	u32 opt = 0;

	#ifndef _IPV6_
	struct sockaddr_in server_addr;

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket create error!");
		return FAILURE;
	}

	#else
	struct sockaddr_in6 server_addr;
	if ((sd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) 
		return -1;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_port = htons(port);
	server_addr.sin6_addr.s_addr = INADDR_ANY;
	#endif
	
	if (FAILURE == set_nonblocking(sfd))
	{
		printf(" set_nonblocking error!\n");
		return FAILURE;
	}
	opt = SO_REUSEADDR;
	
	if (0 != setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		close(sfd);
		printf(" setsockopt error!\n");
		return FAILURE;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons((short)i_port);
	//server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);;

	if (bind(sfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		close(sfd);
		perror("socket bind error!");
		return FAILURE;
	}

	printf("<<-----open_listen----sfd=%d-->>\n",sfd);
	listen(sfd, MAX_LISTEN_SIZE);
	return sfd;

}


//关闭连接
void CHttpServer::remove_connection(CON_S *o_pcn) 
{
	//printf("[%s]----[%d]----------\n",__func__,__LINE__);
	CON_S *ptp;
	s32 shouldret = 0;
	/*
	ptp = m_usedconns;
	if (ptp == NULL || o_pcn == NULL) 
	{
		printf("[%s]----[%d]----------\n",__func__,__LINE__);
		shouldret = 1;
	}
	else if (ptp == o_pcn) 
	{
		printf("[%s]----[%d]----------\n",__func__,__LINE__);
		m_usedconns = ptp->pnext;
	}
	else 
	{
		printf("[%s]----[%d]----------\n",__func__,__LINE__);
		while (ptp != NULL) 
		{
			if (ptp->pnext == o_pcn) 
			{
				ptp->pnext = (ptp->pnext)->pnext;
				shouldret = 0;
				break;
			}
			ptp = ptp->pnext;
			shouldret = 1;
		}
		printf("[%s]----[%d]----------\n",__func__,__LINE__);
	}

	printf("[%s]----[%d]----------\n",__func__,__LINE__);
	if (shouldret) 
		return;
	
	printf("[%s]----[%d]----------\n",__func__,__LINE__);
	//If we did, add it to the free list 
	o_pcn->pnext = m_freeconns;
	m_freeconns = o_pcn;
	*/
	//printf("[%s]----[%d]----------\n",__func__,__LINE__);
	// Close it all down 
	if (o_pcn->networkdesc != -1) 
	{
		if (o_pcn->is_ssl) 
		{
			//ssl_free(o_pcn->ssl);//nedd to be finish 
			//o_pcn->ssl = NULL;
		}
		close(o_pcn->networkdesc);
	}
	//printf("[%s]----[%d]----------\n",__func__,__LINE__);

	if (o_pcn->filedesc != -1) 
		close(o_pcn->filedesc);
	//printf("[%s]----[%d]----------\n",__func__,__LINE__);
}


//-------------------------------------protected-----------------------------------------------	

//设置连接为非阻塞
s32 CHttpServer::set_nonblocking(s32 i_sock)
{
	s32 opts;
	opts=fcntl(i_sock,F_GETFL);

	if(opts<0)
	{
		perror("fcntl(sock.GETFL\n)");
		return -1;
	}

	opts = opts|O_NONBLOCK;

	if(fcntl(i_sock,F_SETFL,opts)<0)
	{
		perror("fcntl(sock.SETFL\n)");
		return FAILURE;
	}
	return SUCCESS;
}



