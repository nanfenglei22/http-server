/*************************************************************************
                  Copyright (C) 2012   Shanghai Surfing
                                All Rights Reserved
                                      R&D Dept.                                            
*************************************************************************/
/*************************************************************************

Filename:         	http_connection.cpp

Description:    	http_connection Function Define Source File.

Author:          		gaoyang

Date:             	2012-11-17 

Version:   		V1.0.0

Modified:      

*************************************************************************/
#include "http_connection.h"
#include <iostream>

using namespace pure_baselib;
using namespace pure_frame;

//changed by simon 2012-12-18 如果type为0，表明用的是循环sendfile，如果为1表明是epollout模型
#define type 1
//end

//week
DMM_S wday_tab[] = 
{
    { "Sun", 0 }, { "Mon", 1 }, { "Tue", 2 }, { "Wed", 3 },
    { "Thu", 4 }, { "Fri", 5 }, { "Sat", 6 }, 
};

//month
DMM_S mon_tab[] = 
{
    { "Jan", 0 }, { "Feb", 1 }, { "Mar", 2 }, { "Apr", 3 },
    { "May", 4 }, { "Jun", 5 }, { "Jul", 6 }, { "Aug", 7 },
    { "Sep", 8 }, { "Oct", 9 }, { "Nov", 10 }, { "Dec", 11 },
};
//end

CHttp_connection::CHttp_connection()
{
	m_offset = 0;
	m_sendlenth = 0;
	
}

CHttp_connection::~CHttp_connection()
{
	m_offset = 0;
	m_sendlenth = 0;
}
s32 CHttp_connection::connect_handle_init() 
{
	
}

s32 CHttp_connection::connect_handle_read() 
{
	//printf("[%s]----[%d]----------\n",__func__,__LINE__);
	if (m_pcon != NULL) 
	{
		//printf("[%s]--m_pcon->networkdesc--0=%d-------------\n",__func__,m_pcon->networkdesc);
read_head:
		if (m_pcon->state == STATE_WANT_TO_READ_HEAD) 
		{
			http_read_head(m_pcon);
		} 

		if (m_pcon->state == STATE_WANT_TO_SEND_HEAD ) 
		{
			//printf("m_pcon->state == STATE_WANT_TO_SEND_HEAD\n");
			/*
			CON_S *test = get_cons();
			if(test !=NULL)
				printf("test->filereq=%s\n",test->filereq);
			else
				printf("test ==NULL\n");
			*/
			s8 *tbuf=NULL;
			u32 buf_len = 0;
			u32 res_type = 200;
			u32 flag =0;
			s32 aret= creat_send_option(tbuf,buf_len,res_type,flag);
			if(aret == FAILURE)
				return FAILURE;
			else
				http_send_response(tbuf,buf_len,res_type,flag);
				
			#if 0
			http_send_head(m_pcon);
			#endif
			if(m_pcon->state == STATE_WANT_TO_READ_HEAD)
				goto  read_head;
			//changed by simon 2012-12-18
			if(m_pcon->state == STATE_DONT_FINISH_SEND)
			{
				writeable = true;
				//return RETRY;
			}
			
		} 
		
//read_file:
		if (m_pcon->state == STATE_WANT_TO_READ_FILE ) 
		{
			http_read_file(m_pcon);
		} 

		if (m_pcon->state == STATE_WANT_TO_SEND_FILE)
		{
			http_send_file(m_pcon);
			if(m_pcon->state == STATE_WANT_TO_READ_FILE)
			{
				//goto  read_file;
				writeable = true;
				//return RETRY;
			}
		}

#ifdef _CONFIG_HTTP_DIRECTORIES_
		if (m_pcon->state == STATE_DOING_DIR)
		{
			//printf("[%s]----[%d]-----m_pcon->state == STATE_DOING_DIR-----\n",__func__,__LINE__);
			show_main_dir(m_pcon);
			//printf("--finish---STATE_DOING_DIR----\n");
		}
#endif
	}
	return SUCCESS;
}

s32 CHttp_connection::connect_handle_write()
{
	//printf("------------------connect_handle_write------------------------\n");
	if (m_pcon != NULL) 
	{
		if (m_pcon->state == STATE_WANT_TO_READ_FILE ) 
		{
			http_read_file(m_pcon);
		} 

		if (m_pcon->state == STATE_WANT_TO_SEND_FILE)
		{
			http_send_file(m_pcon);
			if(m_pcon->state == STATE_WANT_TO_READ_FILE)
			{
				writeable = true;
				//goto  read_file;
				//return RETRY;
			}
		}
		//changed by simon 2012-12-18
		if (m_pcon->state == STATE_DONT_FINISH_SEND ) 
		{
			//printf("------------------m_pcon->state == STATE_DONT_FINISH_SEND----------m_sendlenth=%d------m_offset=%d--------\n",m_sendlenth,m_offset);
			size_t size = 0;
			size = http_sendfile(m_pcon);
			//printf("http_sendfile---------size == %d-------------\n",size);
			if ((s32)size < 0)
			{
				//printf("---------errno == %d------(s32)size < 0-------\n",errno);
				if (errno == EAGAIN)
				{
					perror("errno == EAGAIN!");
					//changde by simon 2012-12-18
					m_pcon->state = STATE_DONT_FINISH_SEND;
					return SUCCESS;
					//end
				}
				//changde by simon 2012-12-18
				m_pcon->state = STATE_WANT_TO_READ_HEAD;
				close(m_pcon->filedesc);
				//return ;
				//end
			}
			
			if (size >= m_sendlenth)
			{
				//printf("---------size >= m_sendlenth-------------\n");
				if(m_pcon->close_when_done == 0)
					m_pcon->state = STATE_WANT_TO_READ_HEAD;
				//close(m_pcon->filedesc);
				return SUCCESS;
			}
			//else if(size < stbuf.st_size-off_set)
			else if(size < m_sendlenth)
			{
				//printf("---------size < m_sendlenth-------------\n");
				m_sendlenth -= size;
				//m_offset +=size;
				m_pcon->state = STATE_DONT_FINISH_SEND;
				return SUCCESS;
			}
			//changed by simon 2012-12-14
			close(m_pcon->filedesc);
		} 
		//end
	}
	return SUCCESS;
}

s32 CHttp_connection::connect_handle_error()
{

	return SUCCESS;
}

s32 CHttp_connection::connect_read(s8* buf, u32 buf_len, u32 flag)
{
	
}

 s32 CHttp_connection::connect_write(s8* buf, u32 buf_len, u32 flag)
 {
	
 }

 s32 CHttp_connection::connect_sendfile(s32 file_fd, off_t *offset, size_t size, u32 flag)  {


 }

 
//changed by simon 2012-12-18

s32 CHttp_connection::creat_send_option(s8* buf, u32 buf_len, u32 res_type,u32 flag) {

}


s32 CHttp_connection::http_send_response(s8* buf, u32 buf_len, u32 res_type,u32 flag) {
	s8 rbuf[MAXREQUESTLENGTH*4];
	memset(rbuf,'\0',MAXREQUESTLENGTH*4);
	s8 *ptitle;
	s8 *ptext;
	s8 date[32];
	s8 last_date[32];
	s8 *twday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	s8 *tmon[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	time_t now = m_pcon->timeout - CONFIG_HTTP_TIMEOUT;
	s32 file_exists;
	struct stat stbuf;
	
	if(m_pcon == NULL) {
		return FAILURE;
   	}
	if(res_type < 100 || res_type > 600 ) {
		return FAILURE;
	}

	file_exists = stat(m_pcon->actualfile, &stbuf);
    	std::cout<<__FILE__<<"\t"<<__LINE__<<"\t"<<m_pcon->actualfile<<std::endl;
	if (isdir(m_pcon->actualfile)) {
		s8 atbuf[MAXREQUESTLENGTH];
		snprintf(atbuf, MAXREQUESTLENGTH, "%s%s", m_pcon->actualfile, index_file);

		if ((file_exists = stat(atbuf, &stbuf)) != -1) {
			my_strncpy(m_pcon->actualfile, atbuf, MAXREQUESTLENGTH);
		} else {
#ifdef _CONFIG_HTTP_DIRECTORIES_
			//If not, we do a directory listing of it 
			dir_listing(m_pcon);
#else
			send_error(m_pcon, 404);
#endif
			return SUCCESS;
		}
	}
	if (file_exists == -1 && buf == NULL) {
		send_error(m_pcon, 404);
		return SUCCESS;
	}

	struct tm *p;
	p=gmtime(&now);
	sprintf(date,"%s, %d %s %d %02d:%02d:%02d GMT",twday[p->tm_wday],p->tm_mday,tmon[p->tm_mon],
		1900+p->tm_year,p->tm_hour, p->tm_min, p->tm_sec);

	if (file_exists != -1 ) {	
		struct tm *tp;
		tp=gmtime(&stbuf.st_mtime);
		sprintf(last_date,"%s, %d %s %d %02d:%02d:%02d GMT",twday[tp->tm_wday],tp->tm_mday,tmon[tp->tm_mon],
			1900+tp->tm_year,tp->tm_hour, tp->tm_min, tp->tm_sec);
		
		//res_type == 304  
		if (m_pcon->if_modified_since != -1 && (m_pcon->if_modified_since == 0 || 
			m_pcon->if_modified_since >= stbuf.st_mtime)) {
			snprintf(rbuf, sizeof(rbuf), HTTP_VERSION" 304 Not Modified\r\nServer: "
			    "%s\r\nDate: %s\r\nConnection: %s\r\n\r\n", server_version, date,m_pcon->close_when_done == 1 ? tclose:tkeep_alive);
			special_write(m_pcon, rbuf, strlen(rbuf));
			m_pcon->state = STATE_WANT_TO_READ_HEAD;
			return SUCCESS;
		}
		
		if(m_pcon->range == 1) {
			s32 flags = O_RDONLY;
			printf("o_pcn->actualfile ===%s\n",m_pcon->actualfile);
			m_pcon->filedesc = open(m_pcon->actualfile, flags);
			if (m_pcon->filedesc < 0) {
				send_error(m_pcon, 404);
				return SUCCESS;
			}
			//changed by simon 2012-12-14
			sl64 range_from = 0;
			sl64 range_end = 0;
			sl64 content_lenth = 0;
			
			if(m_pcon->range_from >= 0) {
				if(m_pcon->range_end == 0) {
					content_lenth = stbuf.st_size - m_pcon->range_from;
					range_from = m_pcon->range_from;
					range_end = stbuf.st_size;
				} else if(m_pcon->range_end > 0) {
					content_lenth = m_pcon->range_end - m_pcon->range_from;
					range_from = m_pcon->range_from;
					range_end = m_pcon->range_end;
				}
			}
			//end
			
			snprintf(rbuf, sizeof(rbuf), HTTP_VERSION" 206 Partial Content\r\nServer: %s\r\n"
				"Content-Type: %s\r\nContent-Length: %ld\r\n"
				"Date: %s\r\nLast-Modified: %s\r\nConnection: %s\r\n"
				"Content-Range: bytes %ld-%ld/%ld\r\n\r\n",
				server_version,
				get_mime_type(m_pcon->actualfile), (sl64)content_lenth,
				date, last_date,m_pcon->close_when_done == 1 ? tclose:tkeep_alive,
				(sl64)range_from,(sl64)range_end,(sl64)stbuf.st_size); 

			special_write(m_pcon, rbuf, strlen(rbuf));
			size_t size = 0;
			off_t off_set=0;
			#if type
			if(range_from > 0) {
				m_offset = range_from;
			} else{
				m_offset = 0;
			}
			if(content_lenth > 0) {
				m_sendlenth = content_lenth;
			} else{
				m_sendlenth = stbuf.st_size;
			}
			#else
			while(off_set<stbuf.st_size)
			#endif
			{
				#if type
				size = http_sendfile(m_pcon);
				#else
				size = sendfile(m_pcon->networkdesc, m_pcon->filedesc, &off_set,(stbuf.st_size-off_set));//
				#endif
				//printf("@@@@@@@@@@@2[sendfile]------stbuf.st_size=%d    size=%d\n",stbuf.st_size,size);
				if ((s32)size < 0) {
					//printf("---------errno == %d-------------\n",errno);
					if (errno == EAGAIN) {
						//perror("errno == EAGAIN!");
						#if type
						//changde by simon 2012-12-18
						m_pcon->state = STATE_DONT_FINISH_SEND;
						return SUCCESS;
						//end
						#else
						continue;
						#endif
					}
					//changde by simon 2012-12-18
					#if type
					m_pcon->state = STATE_WANT_TO_READ_HEAD;
					close(m_pcon->filedesc);
					#endif
					return SUCCESS;
					//end
				}
				//printf("---------off_set= %d-------------\n",off_set);
				#if type
				if (size >= m_sendlenth)
				#else
				if (size >= stbuf.st_size-off_set)
				#endif
				{
					//printf("---------size >= stbuf.st_size-off_set-------------\n");
					if(m_pcon->close_when_done == 0)
						m_pcon->state = STATE_WANT_TO_READ_HEAD;
					//close(o_pcn->filedesc);
					return SUCCESS;
				}
				#if type
				else if(size < m_sendlenth)
				#else
				else if(size < stbuf.st_size-off_set)
				#endif
				{
					#if type
					//printf("---------size < m_sendlenth-------------\n");
					m_sendlenth -= size;
					//m_offset =size;
					m_pcon->state = STATE_DONT_FINISH_SEND;
					return SUCCESS;
					#else
					continue;
					#endif
				}
			}
			//changed by simon 2012-12-14
			close(m_pcon->filedesc);
			//end
			//printf("------------------------------------------------------------------------\n\n\n");
			return SUCCESS;
		}
		//end

		if (m_pcon->reqtype == TYPE_HEAD) 
		{
			//printf("o_pcn->reqtype == TYPE_HEAD---\n");
			m_psvr->remove_connection(m_pcon);
			return SUCCESS;
		} 
		else 
		{
			s32 flags = O_RDONLY;
			//printf("o_pcn->actualfile]--1===%s\n",o_pcn->actualfile);
			m_pcon->filedesc = open(m_pcon->actualfile, flags);
			if (m_pcon->filedesc < 0) 
			{
				printf("o_pcn->filedesc  <0---\n");
				send_error(m_pcon, 404);
				return SUCCESS;
			}
			
			snprintf(rbuf, sizeof(rbuf), HTTP_VERSION" 200 OK\r\nServer: %s\r\n"
				"Content-Type: %s\r\nContent-Length: %ld\r\n"
				"Date: %s\r\nLast-Modified: %s\r\nConnection: %s\r\n"
				"Accept-Ranges: bytes\r\n\r\n", 
				//"Content-Range: bytes 0-%ld/%ld\r\n\r\n",
				server_version,
				get_mime_type(m_pcon->actualfile), (sl64) stbuf.st_size,
				date, last_date,m_pcon->close_when_done == 1 ? tclose : tkeep_alive); //ctime() has a \n on the end 
				//date, last_date,(sl64)stbuf.st_size-1,(sl64)stbuf.st_size); 

			special_write(m_pcon, rbuf, strlen(rbuf));

			size_t size = 0;
			off_t off_set=0;
			#if type
			//changed by simon 2012-12-18
			m_offset = 0;
			m_sendlenth = stbuf.st_size;
			//end
			#else
			while(off_set<stbuf.st_size)
			#endif	
			{
				#if type
				size = http_sendfile(m_pcon);
				#else
				size = sendfile(m_pcon->networkdesc, m_pcon->filedesc, &off_set,(stbuf.st_size-off_set));//
				#endif
				//printf("@@@@@@@@@@@2[sendfile]------stbuf.st_size=%d    size=%d\n",stbuf.st_size,size);
				if ((s32)size < 0)
				{
					//printf("---------errno == %d-------------\n",errno);
					if (errno == EAGAIN)
					{
						//perror("errno == EAGAIN!");
						#if type
						//changde by simon 2012-12-18
						m_pcon->state = STATE_DONT_FINISH_SEND;
						return SUCCESS;
						//end
						#else
						continue;
						#endif
					}
					//changde by simon 2012-12-18
					#if type
					m_pcon->state = STATE_WANT_TO_READ_HEAD;
					close(m_pcon->filedesc);
					#endif
					return SUCCESS;
					//end
				}

				#if type
				if (size >= m_sendlenth)
				#else
				if (size >= stbuf.st_size-off_set)
				#endif
				{
					//printf("---------size >= stbuf.st_size-off_set-------------\n");
					if(m_pcon->close_when_done == 0)
						m_pcon->state = STATE_WANT_TO_READ_HEAD;
					//close(o_pcn->filedesc);
					return SUCCESS;
				}
				#if type
				else if(size < m_sendlenth)
				#else
				else if(size < stbuf.st_size-off_set)
				#endif
				{
					#if type
					//printf("---------size < m_sendlenth-------------\n");
					m_sendlenth -= size;
					//m_offset +=size;
					m_pcon->state = STATE_DONT_FINISH_SEND;
					return SUCCESS;
					#else
					continue;
					#endif
				}
			}
			//changed by simon 2012-12-14
			close(m_pcon->filedesc);
			//end
			//printf("------------------------------------------------------------------------\n\n\n");
			return SUCCESS;
		}
	}	
	else //file_exists ==-1
	{
		snprintf(rbuf, sizeof(rbuf), "HTTP/1.1 %d %s\r\n"
			"Server: %s\r\nDate: %s\r\n"
			"Content-Length: %ld\r\n"
			"Connection: %s\r\n\r\n", 
			res_type, ptitle,server_version,date,buf_len>0?(sl64)buf_len:0,
			m_pcon->close_when_done == 1 ? tclose:tkeep_alive);
		
			special_write(m_pcon, rbuf, strlen(rbuf));
				
			if(buf!= NULL && buf_len > 0 ) {
				special_write(m_pcon, buf, buf_len);
			}
			m_psvr->remove_connection(m_pcon);
	}
	return SUCCESS;
}
//end


//changed by simon 2012-12-18 for sendfile
size_t CHttp_connection::http_sendfile(CON_S *o_pcn)
{
	return sendfile(o_pcn->networkdesc, o_pcn->filedesc, &m_offset,m_sendlenth);
}
//end

//changed by simon 2012-12-18 for 获得当前请求中的参数cons结构体
//此函数必须要在void CHttp_connection::http_read_head(CON_S *o_pcn) 函数之后调用
CON_S *CHttp_connection::get_cons()
{
	if(m_pcon != NULL )
		return m_pcon;
	else
		return NULL;
	
}
//end

//再找不到文件的时候，展现主目录
#ifdef _CONFIG_HTTP_DIRECTORIES_
void CHttp_connection::show_main_dir(CON_S *o_pcn) 
{
	struct dirent *dp;
	s8 abuf[MAXREQUESTLENGTH];
	s8 encbuf[1024];
	s8 *file;

	while(1)
	{
		abuf[0] = 0;
		if ((dp = readdir(o_pcn->dirp)) == NULL)  
		{
			snprintf(abuf, sizeof(abuf), "</body></html>\n");
			s32 pret = special_write(o_pcn, abuf, strlen(abuf));
			m_psvr->remove_connection(o_pcn);
			return;
		}
		file = dp->d_name;
		
		// if no index file, don't display the ".." directory 
		if (o_pcn->filereq[0] == '/' && o_pcn->filereq[1] == '\0' && strcmp(file, "..") == 0) 
			continue;

		//don't display files beginning with "." 
		if (file[0] == '.' && file[1] != '.')
			continue;

		// make sure a '/' is at the end of a directory 
		if (o_pcn->filereq[strlen(o_pcn->filereq)-1] != '/')
			strcat(o_pcn->filereq, "/");

		//see if the dir + file is another directory 
		snprintf(abuf, sizeof(abuf), "%s%s", o_pcn->actualfile, file);
		if (isdir(abuf))
			strcat(file, "/");

		url_encode((u8 *)file, encbuf);
		snprintf(abuf, sizeof(abuf), "<a href=\"%s%s\">%s</a><br />\n",o_pcn->filereq, encbuf, file);
		s32 tret = special_write(o_pcn, abuf, strlen(abuf));
		if(tret <=0)
			return;
	}
}
#endif

void CHttp_connection::http_read_head(CON_S *o_pcn) 
{
	s8 abuf[MAXREQUESTLENGTH*4], *ptp, *pnext;
	s32 rv;
	memset(abuf, 0, MAXREQUESTLENGTH*4);
    //call read function to read content from connected socketfd
	rv = special_read(o_pcn, abuf, sizeof(abuf)-1);
	if (rv <= 0) {
		if (rv < 0) {//really dead? 
			m_psvr->remove_connection(o_pcn);
		}
		return;
	}

	abuf[rv] = '\0';
	pnext = ptp = abuf;
        std::cout<<__FILE__<<"\t"<<__LINE__<<"\tRequest Contents:"<<std::endl;
        std::cout<<abuf<<std::endl;
	//Split up lines and send to head_elem() 
	while (*pnext != '\0') {
		//If we have a blank line, advance to next stage 
		//we have finished to process request head
		if (*pnext == '\r' || *pnext == '\n') {`
			build_actual_file(o_pcn);
			o_pcn->state = STATE_WANT_TO_SEND_HEAD;
			return;
		}
		while (*pnext != '\r' && *pnext != '\n' && *pnext != '\0') {
			pnext++;
		}
		if (*pnext == '\r'){
			*pnext = '\0';
			pnext += 2;
		} else if (*pnext == '\n') {
			*pnext++ = '\0';
		}
        	//process each request line
        	if (head_elem(o_pcn, ptp) == 0) {
            		return;
        	}
        	//ptp point to next request line 
        	ptp = pnext;
    }

    std::cout<<__FILE__<<"\t"<<__LINE__<<"\tFinish Process Request Contents:"<<std::endl;
    std::cout<<abuf<<std::endl;
}

// In this function we assume that the file has been checked for
// maliciousness (".."s, etc) and has been decoded
void CHttp_connection::http_send_head(CON_S *o_pcn) 
{
	s8 *twday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	s8 *tmon[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	s8 abuf[MAXREQUESTLENGTH];
	struct stat stbuf;
	time_t now = o_pcn->timeout - CONFIG_HTTP_TIMEOUT;
	s8 date[32];
	s8 last_date[32];
	s32 file_exists;

	file_exists = stat(o_pcn->actualfile, &stbuf);

	// look for "index.html"? 
	if (isdir(o_pcn->actualfile))
	{
		s8 atbuf[MAXREQUESTLENGTH];
		snprintf(atbuf, MAXREQUESTLENGTH, "%s%s", o_pcn->actualfile, index_file);

		//printf("  [tbuf]== %s---\n",tbuf);
		if ((file_exists = stat(atbuf, &stbuf)) != -1) 
		{
			my_strncpy(o_pcn->actualfile, atbuf, MAXREQUESTLENGTH);
		}
		else
		{
#ifdef _CONFIG_HTTP_DIRECTORIES_
			//If not, we do a directory listing of it 
			dir_listing(o_pcn);
#else
			send_error(o_pcn, 404);
#endif
			return;
		}
	}
	if (file_exists == -1)
	{
		//printf("file_exists == -1---\n");
		send_error(o_pcn, 404);
		return;
	}

	//changed by simon 2012-12-18
	m_offset = 0;
	m_sendlenth = stbuf.st_size;
	//end
	
	//changed by simon 2012-12-12
	struct tm *p;
	p=gmtime(&now);
	sprintf(date,"%s, %d %s %d %02d:%02d:%02d GMT",twday[p->tm_wday],p->tm_mday,tmon[p->tm_mon],
		1900+p->tm_year,p->tm_hour, p->tm_min, p->tm_sec);

	struct tm *tp;
	tp=gmtime(&stbuf.st_mtime);
	sprintf(last_date,"%s, %d %s %d %02d:%02d:%02d GMT",twday[tp->tm_wday],tp->tm_mday,tmon[tp->tm_mon],
		1900+tp->tm_year,tp->tm_hour, tp->tm_min, tp->tm_sec);
	//end

	//has the file been read before? 
	if (o_pcn->if_modified_since != -1 && (o_pcn->if_modified_since == 0 || 
		o_pcn->if_modified_since >= stbuf.st_mtime))
	{
		snprintf(abuf, sizeof(abuf), HTTP_VERSION" 304 Not Modified\r\nServer: "
		    "%s\r\nDate: %s\r\nConnection: %s\r\n\r\n", server_version, date,o_pcn->close_when_done == 1 ? tclose:tkeep_alive);
		s32 pret = special_write(o_pcn, abuf, strlen(abuf));
		o_pcn->state = STATE_WANT_TO_READ_HEAD;
		return;
	}
	
	//changed by simon 2012-12-12
	if(o_pcn->range == 1)
	{
		s32 flags = O_RDONLY;
		printf("o_pcn->actualfile ===%s\n",o_pcn->actualfile);
		o_pcn->filedesc = open(o_pcn->actualfile, flags);
		if (o_pcn->filedesc < 0) 
		{
			//printf("o_pcn->filedesc  <0---\n");
			send_error(o_pcn, 404);
			return;
		}

		//changed by simon 2012-12-14
		sl64 range_from = 0;
		sl64 range_end = 0;
		sl64 content_lenth = 0;
		
		if(o_pcn->range_from >= 0)
		{
			if(o_pcn->range_end == 0)
			{
				content_lenth = stbuf.st_size - o_pcn->range_from;
				range_from = o_pcn->range_from;
				range_end = stbuf.st_size;
			}
			else if(o_pcn->range_end > 0)
			{
				content_lenth = o_pcn->range_end - o_pcn->range_from;
				range_from = o_pcn->range_from;
				range_end = o_pcn->range_end;
			}
		}
		//end
		
		snprintf(abuf, sizeof(abuf), HTTP_VERSION" 206 Partial Content\r\nServer: %s\r\n"
			"Content-Type: %s\r\nContent-Length: %ld\r\n"
			"Date: %s\r\nLast-Modified: %s\r\nConnection: %s\r\n"
			//"Content-Range: bytes 0-%ld/%ld\r\n\r\n",
			"Content-Range: bytes %ld-%ld/%ld\r\n\r\n",
			server_version,
			//get_mime_type(o_pcn->actualfile), (sl64) stbuf.st_size,
			//date, last_date,(sl64)stbuf.st_size-1,(sl64)stbuf.st_size); 
			get_mime_type(o_pcn->actualfile), (sl64)content_lenth,
			date, last_date,o_pcn->close_when_done == 1 ? tclose:tkeep_alive,
			(sl64)range_from,(sl64)range_end,(sl64)stbuf.st_size); 

		s32 pret = special_write(o_pcn, abuf, strlen(abuf));
		size_t size = 0;
		off_t off_set=0;
		//changed by simon 2012-12-18
		#if type
		if(range_from > 0)
			m_offset = range_from;
		else
			m_offset = 0;
		if(content_lenth > 0)
			m_sendlenth = content_lenth;
		else
			m_sendlenth = stbuf.st_size;
		//end
		#else
		while(off_set<stbuf.st_size)
		#endif
		{
			#if type
			size = http_sendfile(o_pcn);
			#else
			size = sendfile(o_pcn->networkdesc, o_pcn->filedesc, &off_set,(stbuf.st_size-off_set));//
			#endif
			//printf("@@@@@@@@@@@2[sendfile]------stbuf.st_size=%d    size=%d\n",stbuf.st_size,size);
			if ((s32)size < 0)
			{
				//printf("---------errno == %d-------------\n",errno);
				if (errno == EAGAIN)
				{
					//perror("errno == EAGAIN!");
					#if type
					//changde by simon 2012-12-18
					o_pcn->state = STATE_DONT_FINISH_SEND;
					return;
					//end
					#else
					continue;
					#endif
				}
				//changde by simon 2012-12-18
				#if type
				o_pcn->state = STATE_WANT_TO_READ_HEAD;
				close(o_pcn->filedesc);
				#endif
				return;
				//end
			}
			//printf("---------off_set= %d-------------\n",off_set);
			#if type
			if (size >= m_sendlenth)
			#else
			if (size >= stbuf.st_size-off_set)
			#endif
			{
				//printf("---------size >= stbuf.st_size-off_set-------------\n");
				if(o_pcn->close_when_done == 0)
					o_pcn->state = STATE_WANT_TO_READ_HEAD;
				//close(o_pcn->filedesc);
				return;
			}
			#if type
			else if(size < m_sendlenth)
			#else
			else if(size < stbuf.st_size-off_set)
			#endif
			{
				#if type
				//printf("---------size < m_sendlenth-------------\n");
				m_sendlenth -= size;
				//m_offset =size;
				o_pcn->state = STATE_DONT_FINISH_SEND;
				return;
				#else
				continue;
				#endif
			}
		}
		//changed by simon 2012-12-14
		close(o_pcn->filedesc);
		//end
		//printf("------------------------------------------------------------------------\n\n\n");
		return;
	}
	//end

	if (o_pcn->reqtype == TYPE_HEAD) 
	{
		//printf("o_pcn->reqtype == TYPE_HEAD---\n");
		m_psvr->remove_connection(o_pcn);
		return;
	} 
	else 
	{
		s32 flags = O_RDONLY;
		//printf("o_pcn->actualfile]--1===%s\n",o_pcn->actualfile);
		o_pcn->filedesc = open(o_pcn->actualfile, flags);
		if (o_pcn->filedesc < 0) 
		{
			printf("o_pcn->filedesc  <0---\n");
			send_error(o_pcn, 404);
			return;
		}
		
		snprintf(abuf, sizeof(abuf), HTTP_VERSION" 200 OK\r\nServer: %s\r\n"
			"Content-Type: %s\r\nContent-Length: %ld\r\n"
			"Date: %s\r\nLast-Modified: %s\r\nConnection: %s\r\n"
			"Accept-Ranges: bytes\r\n\r\n", 
			//"Content-Range: bytes 0-%ld/%ld\r\n\r\n",
			server_version,
			get_mime_type(o_pcn->actualfile), (sl64) stbuf.st_size,
			date, last_date,o_pcn->close_when_done == 1 ? tclose : tkeep_alive); //ctime() has a \n on the end 
			//date, last_date,(sl64)stbuf.st_size-1,(sl64)stbuf.st_size); 

		special_write(o_pcn, abuf, strlen(abuf));

		size_t size = 0;
		off_t off_set=0;
		#if type
		//changed by simon 2012-12-18
		m_offset = 0;
		m_sendlenth = stbuf.st_size;
		//end
		#else
		while(off_set<stbuf.st_size)
		#endif	
		{
			#if type
			size = http_sendfile(o_pcn);
			#else
			size = sendfile(o_pcn->networkdesc, o_pcn->filedesc, &off_set,(stbuf.st_size-off_set));//
			#endif
			//printf("@@@@@@@@@@@2[sendfile]------stbuf.st_size=%d    size=%d\n",stbuf.st_size,size);
			if ((s32)size < 0)
			{
				//printf("---------errno == %d-------------\n",errno);
				if (errno == EAGAIN)
				{
					//perror("errno == EAGAIN!");
					#if type
					//changde by simon 2012-12-18
					o_pcn->state = STATE_DONT_FINISH_SEND;
					return;
					//end
					#else
					continue;
					#endif
				}
				//changde by simon 2012-12-18
				#if type
				o_pcn->state = STATE_WANT_TO_READ_HEAD;
				close(o_pcn->filedesc);
				#endif
				return;
				//end
			}

			#if type
			if (size >= m_sendlenth)
			#else
			if (size >= stbuf.st_size-off_set)
			#endif
			{
				//printf("---------size >= stbuf.st_size-off_set-------------\n");
				if(o_pcn->close_when_done == 0)
					o_pcn->state = STATE_WANT_TO_READ_HEAD;
				//close(o_pcn->filedesc);
				return;
			}
			#if type
			else if(size < m_sendlenth)
			#else
			else if(size < stbuf.st_size-off_set)
			#endif
			{
				#if type
				//printf("---------size < m_sendlenth-------------\n");
				m_sendlenth -= size;
				//m_offset +=size;
				o_pcn->state = STATE_DONT_FINISH_SEND;
				return;
				#else
				continue;
				#endif
			}
		}
		//changed by simon 2012-12-14
		close(o_pcn->filedesc);
		//end
		//printf("------------------------------------------------------------------------\n\n\n");
		return;
	}
}

//读取client端需要的文件
void CHttp_connection::http_read_file(CON_S *o_pcn) 
{
	memset(o_pcn->databuf,0,MAX_REQUEST_SIZE);
	s32 rv = read(o_pcn->filedesc, o_pcn->databuf, MAX_REQUEST_SIZE);
	if (rv <= 0) 
	{
		close(o_pcn->filedesc);
		o_pcn->filedesc = -1;

		if (o_pcn->close_when_done)        //close immediately 
		{
			//printf("o_pcn->close_when_done\n");
			m_psvr->remove_connection(o_pcn);
		}
		else 
		{     
			//keep socket open - HTTP 1.1 
			o_pcn->state = STATE_WANT_TO_READ_HEAD;
			o_pcn->numbytes = 0;
		}
		return;
	}
	o_pcn->numbytes = rv;
	o_pcn->state = STATE_WANT_TO_SEND_FILE;
}

//发送请求的文件
void CHttp_connection::http_send_file(CON_S *o_pcn) 
{
	s32 rv = special_write(o_pcn, o_pcn->databuf, o_pcn->numbytes);
	if (rv < 0)
		m_psvr->remove_connection(o_pcn);
	else if (rv == o_pcn->numbytes)
	{
		o_pcn->state = STATE_WANT_TO_READ_FILE;
	}
	else if (rv == 0)
	{ 
		//Do nothing  
	}
	else 
	{
		memmove(o_pcn->databuf, o_pcn->databuf + rv, o_pcn->numbytes - rv);
		o_pcn->numbytes -= rv;
	}
}

//请求或者响应中时间模块初始化
void CHttp_connection::tdate_init(void)
{
	qsort(wday_tab, sizeof(wday_tab)/sizeof(DMM_S),
		sizeof(DMM_S), 
            	(s32 (*)(const void *, const void *))day_mon_map_compare);
   	qsort(mon_tab, sizeof(mon_tab)/sizeof(DMM_S),
            	sizeof(DMM_S), 
            	(s32 (*)(const void *, const void *))day_mon_map_compare);
}

//请求或者响应中时间解析模块的比较模块
s32 CHttp_connection::day_mon_map_compare(const s8 *i_pv1, const s8 *i_pv2)
{
    	return strcmp(((DMM_S*)i_pv1)->s, ((DMM_S*)i_pv2)->s);
}


//-------------------------------------protected-----------------------------------------------	
//在没有主页面的情况下，返回主机程序所在目录
#ifdef _CONFIG_HTTP_DIRECTORIES_
void CHttp_connection::dir_listing(CON_S *o_pcn)
{
	s8 abuf[MAXREQUESTLENGTH];
	s8 actualfile[1024];

	if (o_pcn->reqtype == TYPE_HEAD) 
	{
		snprintf(abuf, sizeof(abuf), HTTP_VERSION
			" 200 OK\r\nContent-Type: text/html\r\n\r\n");
			write(o_pcn->networkdesc, abuf, strlen(abuf));//
			m_psvr->remove_connection(o_pcn);
		return;
	}

	strcpy(actualfile, o_pcn->actualfile);

	if ((o_pcn->dirp = opendir(actualfile)) == NULL) 
	{
		send_error(o_pcn, 404);
		return;
	}

	snprintf(abuf, sizeof(abuf), HTTP_VERSION
		" 200 OK\r\nContent-Type: text/html\r\n\r\n"
		"<html><body>\n<title>Directory Listing</title>\n"
		"<h3>Directory listing of %s://%s%s</h3><br />\n", 
		o_pcn->is_ssl ? "https" : "http", o_pcn->server_name, o_pcn->filereq);
	s32 pret = special_write(o_pcn, abuf, strlen(abuf));
	o_pcn->state = STATE_DOING_DIR;
}
#endif

//转换url里面的乱码地址(%xx )到正常符号
void CHttp_connection::url_decode(s8 *i_pbuf) 
{
	s32 v;
	s8 *p, *s, *w;
	w = p = i_pbuf;
	while (*p) 
	{
		v = 0;
		if (*p == '%') 
		{
			s = p;
			s++;
			if (isxdigit((s32) s[0]) && isxdigit((s32) s[1]))
			{
				v = hex_it(s[0])*16 + hex_it(s[1]);
				if (v) 
				{ 
					//do not decode %00 to null s8 
					*w = (s8)v;
					p = &s[1];
				}
			}
		}
		if (!v) *w=*p;
		p++; 
		w++;
	}
	*w='\0';
}

//转码url里面的正常字符到(%xx )形式
//Encode funny chars -> %xx in newly allocated storage  (preserves '/' !) 
#ifdef _CONFIG_HTTP_DIRECTORIES_
void CHttp_connection::url_encode(const u8 *i_ps, s8 *o_pt) 
{
	const u8 *p = i_ps;
	s8 *ptp = o_pt;

	for (; *p; p++) 
	{
		if ((*p > 0x00 && *p < ',') ||(*p > '9' && *p < 'A') ||
			(*p > 'Z' && *p < '_') ||(*p > '_' && *p < 'a') ||
			(*p > 'z' && *p < 0xA1)) 
		{
			sprintf((s8 *)ptp, "%%%02X", *p);
			ptp += 3; 
		} 
		else 
		{
			*ptp = *p;
			ptp++;
		}
	}
	*ptp='\0';
}
#endif

//A-Z,a-z,0-9这些字符的识别
s32 CHttp_connection::hex_it(s8 c) 
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return 0;
}

//字符检查，看送进来的一行请求信息是否合法
s32 CHttp_connection::sanitize_file(const s8 *i_pbuf) 
{
	s32 len, i;

	//Don't accept anything not starting with a / 
	if (*i_pbuf != '/') 
		return 0;

	len = strlen(i_pbuf);
	for (i = 0; i < len; i++) 
	{
		//Check for "/." i.e. don't send files starting with a .
		if (i_pbuf[i] == '/' && i_pbuf[i+1] == '.') 
			return 0;
	}
	return 1;
}

//字符检查，返回host地址。
//eg:Host: 127.0.0.1:8124
s32 CHttp_connection::sanitize_host(s8 *i_pbuf) {
    while (*i_pbuf != '\0') {
        //Handle the port 
        if (*i_pbuf == ':') {
            *i_pbuf = '\0';
            return 1;
        }
        //Enforce some basic URL rules... 
        if ((isalnum(*i_pbuf) == 0 && *i_pbuf != '-' && *i_pbuf != '.') ||(*i_pbuf == '.' && *(i_pbuf+1) == '.') ||
                    (*i_pbuf == '.' && *(i_pbuf+1) == '-') ||(*i_pbuf == '-' && *(i_pbuf+1) == '.')) {
            return 0;
        }
        i_pbuf++;
    }
    return 1;
}

//封装了strncpy(),防止出现非'\0'结尾的字符串带来的问题
s8 * CHttp_connection::my_strncpy(s8 *o_pdest, const s8 *i_psrc, size_t n) 
{
	strncpy(o_pdest, i_psrc, n);
	o_pdest[n-1] = '\0';
	return o_pdest;
}

//发送出错响应，包括403，404，418
void CHttp_connection::send_error(CON_S *o_pcn, s32 i_err)
{
	s8 buf[MAXREQUESTLENGTH*4];
	s8 *ptitle;
	s8 *ptext;

	switch (i_err)
	{
		case 403:
			ptitle = "Forbidden";
			ptext = "File is protected";
		break;

		case 404:
			ptitle = "Not Found";
			ptext = "Not Found---truse me ,i found hardly,but not find out!!!";
		break;

		case 418:
			ptitle = "POST data size is to large";
			ptext = ptitle;
		break;

		default:
			ptitle = "Unknown";
			ptext = "Unknown";
		break;
	}

	snprintf(buf, MAXREQUESTLENGTH*4, "HTTP/1.1 %d %s\r\n"
		"Content-Type: text/html\r\n"
		"Cache-Control: no-cache,no-store\r\n"
		"Connection: close\r\n\r\n"
		"<html>\n<head>\n<title>%d %s</title></head>\n"
		"<body><h1>%d %s</h1>\n</body></html>\n", 
		i_err, ptitle, i_err, ptitle, i_err, ptext);
		s32 pret = special_write(o_pcn, buf, strlen(buf));
		//printf("<special_write>    6-----------pret=%d      strlen(buf)=%d\n",pret,strlen(buf));
		m_psvr->remove_connection(o_pcn);
}

// 一行一行的解析请求消息，如果所有需要的消息都已经解析完，返回1
s32 CHttp_connection::head_elem(CON_S *o_pcn, s8 *i_pbuf) 
{
	s8 *pdelim, *pvalue;
	printf("FILE:%s\tfunc:%s\tLINE:%d\t%s\n",__FILE__,__func__,__LINE__,i_pbuf);
	if ((pdelim = strchr(i_pbuf, ' ')) == NULL) {
		return 0;
	}

	*pdelim = 0;
	pvalue = pdelim+1;

	if (strcmp(i_pbuf, "GET") == 0 || strcmp(i_pbuf, "HEAD") == 0 ||strcmp(i_pbuf, "POST") == 0) {
		if (i_pbuf[0] == 'H') {
			o_pcn->reqtype = TYPE_HEAD;
		}else if (i_pbuf[0] == 'P') {
			o_pcn->reqtype = TYPE_POST;
		}
		//expect HTTP type 
		if ((pdelim = strchr(pvalue, ' ')) == NULL) {
			return 0;
		}
		*pdelim = 0;
		url_decode(pvalue);
		//judge whether the url is legal
		if (sanitize_file(pvalue) == 0) {
			send_error(o_pcn, 403);
			return 0;
		}

		my_strncpy(o_pcn->filereq, pvalue, MAXREQUESTLENGTH);
		o_pcn->if_modified_since = -1;			
	} else if (strcmp(i_pbuf, "Host:") == 0) {
		//Judge whether host is legal
		if (sanitize_host(pvalue) == 0) {
			m_psvr->remove_connection(o_pcn);
			return 0;
		}

		my_strncpy(o_pcn->server_name, pvalue, MAXREQUESTLENGTH);
	} else if (strcmp(i_pbuf, "Connection:") == 0 && strcmp(pvalue, "close") == 0) {
		o_pcn->close_when_done = 1;//http is short connect
	} else if (strcmp(i_pbuf, "If-Modified-Since:") == 0) {//last time modify file time
		//printf("-------pvalue=%s-----\n",pvalue);
		o_pcn->if_modified_since = tdate_parse(pvalue);
		if(o_pcn->if_modified_since != -1) {
			o_pcn->if_modified_since += 8*3600;//时间修正
		}
	}else if (strcmp(i_pbuf, "Expect:") == 0) {
		send_error(o_pcn, 417); //expectation failed 
		return 0;
	}else if (strcmp(i_pbuf, "Range:") == 0){//changed by simon 2012-12-12
		//printf("[Range:]------pvalue=%s\n",pvalue);
		s8 from[256]={0,};
		s8 end[256]={0,};
		s32 last_pos =0;
		s32 last_pos1 =0;
		s32 pathlenth=get_field(pvalue,"bytes=","-",1024,0,last_pos,from);
		//printf("from =%s  strlen(from)=%d\n",from,strlen(from));
		o_pcn->range = 1;
		if(strlen(from)-1>=0){//range的开始字段是有值得，表明要从文件的偏移量开始传输
			o_pcn->range_from = atoi(from);
		}else{
			return 1;
		}
		pathlenth=get_field(pvalue,"-","\r\n",1024,0,last_pos1,end);
		//printf("end =%s  strlen(end)=%d\n",end,strlen(end));
		if(strlen(end)-1 >=0) {
			o_pcn->range_end = atoi(end);
		}
	}
	return 1;
}


//socket 读写函数
s32 CHttp_connection::special_read(CON_S *o_pcn, void *i_pbuf, size_t i_count)
{
	s32 res;
	res = read(o_pcn->networkdesc, i_pbuf, i_count);//
	return res;
}

s32 CHttp_connection::special_write(CON_S *o_pcn,const s8 *i_pbuf, size_t i_count)
{
	return write(o_pcn->networkdesc, i_pbuf, i_count);//
}


//使用2分查找，找到str代表的时间是周几或者几月份
s8 CHttp_connection::day_mon_map_search(const s8* i_pstr,const DMM_S* i_ptab, s32 n) {
	DMM_S *search = (DMM_S*)bsearch(&i_pstr, i_ptab, n,sizeof(DMM_S), 
			(s32 (*)(const void *, const void *))day_mon_map_compare);

	return search ? search->l : -1;
}

//
time_t CHttp_connection::tdate_parse(const s8* i_pstr) {
	struct tm tm;
	s8 str_mon[4], str_wday[4];
	s32 tm_sec, tm_min, tm_hour, tm_mday, tm_year;

	// Initialize. 
	memset(&tm, 0, sizeof(struct tm));
	//wdy, DD mth YY HH:MM:SS GMT
	if ((sscanf(i_pstr, "%3[a-zA-Z], %d %3[a-zA-Z] %d %d:%d:%d GMT",
					str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
					&tm_sec) == 7) ||
			                //wdy mth DD HH:MM:SS YY 
			                (sscanf(i_pstr, "%3[a-zA-Z] %3[a-zA-Z] %d %d:%d:%d %d",
				                      str_wday, str_mon, &tm_mday, &tm_hour, &tm_min, &tm_sec,
				        &tm_year) == 7)){
		s8 tm_wday = day_mon_map_search(str_wday, wday_tab, 
				sizeof(wday_tab)/sizeof(DMM_S));
		s8 tm_mon = day_mon_map_search(str_mon, mon_tab, 
				sizeof(mon_tab)/sizeof(DMM_S));

		if (tm_wday < 0 || tm_mon < 0) {
			return -1;
		}
		tm.tm_wday = tm_wday;
		tm.tm_mon = tm_mon;
		tm.tm_mday = tm_mday;
		tm.tm_hour = tm_hour;
		tm.tm_min = tm_min;
		tm.tm_sec = tm_sec;
		tm.tm_year = tm_year - 1900;
		//printf("[time]---->>>   tm_wday=%d ,tm_mon=%d ,tm_mday=%d ,tm_hour=%d ,tm_min=%d ,tm_sec=%d ,tm_year - 1900=%d \n",tm_wday,tm_mon,tm_mday,tm_hour,tm_min,tm_sec,tm_year - 1900);
		return mktime(&tm);
	}
	return -1;  //error 
}

//配置头发送的文件类型
const s8* CHttp_connection::get_mime_type(const s8 *i_pname)
{
    	//only bother with a few mime types - let the browser figure the rest out 
	if (strstr(i_pname, ".htm"))
		return "text/html";
	else if (strstr(i_pname, ".css"))
		return "text/css"; 
	else
		return "application/octet-stream";
}

//判断tpbuf代表的路径是否是目录
s32 CHttp_connection::isdir(const s8 *i_pbuf) 
{
	printf("%s  %s   %d  i_pbuf=%s\n",__FILE__,__func__,__LINE__,i_pbuf);
	struct stat st;
	s8 path[MAXREQUESTLENGTH];
	strcpy(path, i_pbuf);

	if (stat(path, &st) == -1) 
	{
		printf("%s  %s   %d  not dir\n",__FILE__,__func__,__LINE__);
		return 0;
	}

	if ((st.st_mode & S_IFMT) == S_IFDIR) //是否是目录
	{
		printf("%s  %s   %d  is  dir\n",__FILE__,__func__,__LINE__);
		return 1;
	}
	return 0;
}

//build actualfile or actualdir when meet /r/n(blan line)
void CHttp_connection::build_actual_file(CON_S *o_pcn) {
	s8 *pcp;
	snprintf(o_pcn->actualfile, MAXREQUESTLENGTH, "%s%s", o_pcn->default_dir, o_pcn->filereq);

	std::cout<<__FILE__<<"\t"<<__LINE__<<std::endl;
	printf("o_pcn->filereq=%s   o_pcn->actualfile=%s    o_pcn->default_dir=%s\n",o_pcn->filereq,o_pcn->actualfile,o_pcn->default_dir);
	//Add directory slash if not there 
	if (isdir(o_pcn->actualfile) &&o_pcn->actualfile[strlen(o_pcn->actualfile)-1] != '/') {
		strcat(o_pcn->actualfile, "/");
		std::cout<<__FILE__<<"\t"<<__LINE__<<"\t"<<__func__<<"\t"<<o_pcn->actualfile<<" is dir"<<std::endl;
	}else{
		std::cout<<__FILE__<<"\t"<<__LINE__<<"\t"<<__func__<<"\t"<<o_pcn->actualfile<<" is file"<<std::endl;
	}

	// work out the directory name 
	strncpy(o_pcn->dirname, o_pcn->actualfile, MAXREQUESTLENGTH);
	if ((pcp = strrchr(o_pcn->dirname, '/')) == NULL) {
		o_pcn->dirname[0] = 0;
	} else{
		*pcp = 0;
	}

	printf("[%s]----[%d]-------o_pcn->filereq=%s---o_pcn->dirname=%s\n",__func__,__LINE__,o_pcn->filereq,o_pcn->dirname);
}


//获取http包中的某一个字段，其中findbuf是要查找的buf，比如request head，ResponseHeader。
//head_str是要查找的字段的起始标志，tail_str是结束标志，
//total_len，是要查找的总长，tbuf是最终查找结果
//lastpos是上次查找的指针结束位置,pos是本次查找完指针的位置，返回值.
//当要查找Set-Cookie:这样的会在头中多次出现的字段时，才需要使用lastpos和pos，实现从上次查找之后的位置继续查找

//s8 psLocal[1024];
//s32 last_pos=0;s32 last_pos1=0;
//s32 pos =0;
//pc->get_field(sHerder,"Set-Cookie: ","\r\n",1024,pos,last_pos,psLocal);
//pc->get_field(sHerder,"Set-Cookie: ","\r\n",1024,last_pos,last_pos1,psLocal);
s32 CHttp_connection::get_field(s8 *i_pfindbuf,s8 *i_phead_str, s8 *i_tail_str, s32 i_total_len,s32 o_pos,s32 &i_lastpos, s8 *o_ptbuf)  
{  
	s32 t =0;
 	s32 i;  
	s32 http_offset;  
	s32 head_len, tail_len, val_len;  
	s8 head_tmp[MAX_LISTEN_SIZE], tail_tmp[MAX_LISTEN_SIZE];  
	//初始化  
	memset(head_tmp, 0, sizeof(head_tmp));  
	memset(tail_tmp, 0, sizeof(tail_tmp));  
	head_len = strlen(i_phead_str);  

	//changed by simon 2012-12-14
	if(i_tail_str == NULL)
	{
		tail_len = 0;
	}
	else
		tail_len = strlen(i_tail_str);  
	//end
	//查找 head_str  
	http_offset = 0;//记录下HTTP报文初始文件偏移  
	s32 p =o_pos;
	while((head_tmp[0] = i_pfindbuf[p]) != EOF) //逐个字节遍历  
	{  
		p++;
		if((p-http_offset) > i_total_len) //遍历完成  
		{  
			printf("can not find %s\n",i_phead_str);
			sprintf(o_ptbuf, "can not find %s \r\n", i_phead_str);  
			return 0;  
		}  
		if(head_tmp[0] == *i_phead_str) //匹配到第一个字符  
		{  
			for(i=1; i<head_len; i++) //匹配 head_str 的其他字符  
			{  
				head_tmp[i]=i_pfindbuf[p];
				p++;
				if(head_tmp[i] != *(i_phead_str+i))  
				{
					break;  
				}
			}  
			if(i == head_len) //匹配 head_str 成功，停止遍历 
			{
				break;  
			}
		}  
	}  
	//printf("head_tmp=%s \n", head_tmp);  
	//查找 tail_str  
	val_len = 0;  
	while((tail_tmp[0] = i_pfindbuf[p] ) != EOF)
	{  	
		//changed by simon 2012-12-14
		if(i_tail_str == NULL)
		{
			p++;
			if((p-http_offset) > i_total_len) 
			{
				return 0; 
			}
			o_ptbuf[val_len++] = tail_tmp[0];
		}
		else if(i_tail_str != NULL)
		{
			//end
			p++;
			if((p-http_offset) > i_total_len) 
			{  
				printf("can not find %s\n",i_tail_str);
				sprintf(o_ptbuf, "can not find %s \r\n", i_tail_str);  
				return 0; 
			}  
			o_ptbuf[val_len++] = tail_tmp[0]; //用buf 存储 value 直到查找到 tail_str  
			if(tail_tmp[0] == *i_tail_str) //匹配到第一个字符  
			{  
				for(i=1; i<tail_len; i++) //匹配 head_str 的其他字符  
				{  
					tail_tmp[i]=i_pfindbuf[p];
					p++;
					if(tail_tmp[i] != *(i_tail_str+i))  
						break;  
				}  
				if(i == tail_len) //匹配 head_str 成功，停止遍历  
				{  
					o_ptbuf[val_len-1] = 0; //清除多余的一个字符  
					break;  
				}  
			}  
		}
	}  
	i_lastpos = p;
	//printf("val=%s  val_len =%d,lastpos=%d\n", o_ptbuf,val_len,i_lastpos);  
	return val_len; 
}
