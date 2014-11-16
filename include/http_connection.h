/*************************************************************************
                  Copyright (C) 2012   Shanghai Surfing
                                           All Rights Reserved
                                           R&D Dept.                                            
*************************************************************************/
/*************************************************************************

Filename:         	http_connection.h

Description:    	http_connection Function Define Source File.

Author:          		gaoyang

Date:             	2012-11-22 

Version:   		V1.0.0

Modified:      

*************************************************************************/
#ifndef __CHTTP_CONNECTION_H__
#define __CHTTP_CONNECTION_H__

#include "base_connect.h"
#include "basic_type.h"
#include "c_headers.h"
#include "basic_define.h"

#include "http_server.h"

using namespace pure_baselib;

//data & time module
typedef struct day_mon_map_s 
{
	const s8* s;
	u8 l;
}DMM_S;


typedef struct conn_struct_s	CON_S;

namespace pure_frame
{
	class CHttpServer;
	class CHttp_connection:public connection
	{	
		/*
		class base_socket
		{
			public:
				s32		sock_fd;						//socket的描述符
				u32		sock_type;					//socket描述符的类型。0表示connect描述符；1表示server的描述符。
				virtual ~base_socket(){}
		};
		*/
		/*connection
		protected:

		public:
			bool 					readable;			//read flag;
			bool 					writeable;		//write flag;
			bool 					exeuteable;		//execute flag;
			socket_addr				remote_addr;		//the client addr;
			server					*svr;			//没一个connection都有自己制定的server指针，用于server的connection链表管理。
			connection				*next;			//next node;

		*/
		public:
			CHttpServer     *m_psvr;
			CON_S 		  *m_pcon;
			
			CHttp_connection();
			virtual ~CHttp_connection();
			virtual s32 connect_handle_init() ;
			
			s32 connect_handle_read() ;
			virtual s32 connect_handle_write() ;
			virtual s32 connect_handle_error() ;//错误处理函数			
			virtual s32 connect_read(s8* buf, u32 buf_len, u32 flag);
			virtual s32 connect_write(s8* buf, u32 buf_len, u32 flag);
			virtual s32 connect_sendfile(s32 file_fd, off_t *offset, size_t size, u32 flag);
			

			//changed by simon 2012-12-18 
			/******************************************************
			 * 函数名: creat_send_option
			 * 描述: 配置自己需要的响应内容
			 * 输入:
			 * 		//i_pcn:CON_S类型指针，解析出来的请求中的参数
			 *		buf:字符类型指针，该指针的值表示除了头之外需要传输的内容。
			 *		buf_len:u32类型大小，buf数据的大小。
			 *		res_type:u32类型,需要回应的响应头类型，eg:200,404,304,206
			 *		flag:u32类型，暂未使用。
			 * 输出:
			 * 返回值: s32类型。成功返回SUCCESS，失败返回FAILURE。
			 *******************************************************/
			virtual s32 creat_send_option(s8* buf, u32 buf_len, u32 res_type,u32 flag);

			s32 http_send_response(s8* buf, u32 buf_len, u32 res_type,u32 flag);
			//end
			
			size_t http_sendfile(CON_S *o_pcn);
			CON_S *get_cons();
			//end
			#ifdef _CONFIG_HTTP_DIRECTORIES_
			//展现运行程序所在路径
			void show_main_dir(CON_S *o_pcn);
			#endif
			//分析请求头
			void http_read_head(CON_S *o_pcn) ;
			//发送相应
			void http_send_head(CON_S *o_pcn);
			//读取上传的文件
			void http_read_file(CON_S *o_pcn);
			//发送请求的文件
			void http_send_file(CON_S *o_pcn);

			//请求或者响应中时间模块初始化
			void tdate_init(void);
			//请求或者响应中时间解析模块的比较模块
			static s32 day_mon_map_compare(const s8 *i_pv1, const s8 *i_pv2);
		protected:
			//changed by simon 2012-12-18
			off_t m_offset;
			size_t m_sendlenth;
			//end
			#ifdef _CONFIG_HTTP_DIRECTORIES_
			//在没有主页面的情况下，返回主机程序所在目录
			void dir_listing(CON_S *o_pcn);
			#endif

			//转换url里面的乱码地址(%xx )到正常符号
			void url_decode(s8 *i_pbuf);
			
			#ifdef _CONFIG_HTTP_DIRECTORIES_
			//转码url里面的正常字符到(%xx )形式
			void url_encode(const u8 *i_ps, s8 *o_pt); 
			#endif
			
			//A-Z,a-z,0-9这些字符的识别
			s32 hex_it(s8 c);
			//字符检查，看送进来的一行请求信息是否合法
			s32 sanitize_file(const s8 *i_pbuf);
			//字符检查，返回host地址。
			s32 sanitize_host(s8 *i_pbuf);
			//封装了strncpy(),防止出现非'\0'结尾的字符串带来的问题
			s8 * my_strncpy(s8 *o_pdest, const s8 *i_psrc, size_t n);
			//发送出错响应，包括403，404，418
			void send_error(CON_S *o_pcn, s32 i_err);
			// 一行一行的解析请求消息，如果所有需要的消息都已经解析完，返回1
			s32 head_elem(CON_S *o_pcn, s8 *i_pbuf);
			//socket 读写函数
			s32 special_read(CON_S *o_pcn, void *i_pbuf, size_t i_count);
			s32 special_write(CON_S *o_pcn,const s8 *i_pbuf, size_t i_count);
			
			//使用2分查找，找到str代表的时间是周几或者几月份
			s8 day_mon_map_search(const s8* i_pstr,const DMM_S* i_ptab, s32 n);
			//请求或者响应中时间解析，找出是周几或者几月份
			time_t tdate_parse(const s8* i_pstr);
			//配置头发送的文件类型
			const s8* get_mime_type(const s8 *i_pname);
			//判断tpbuf代表的路径是否是目录
			s32 isdir(const s8 *i_pbuf);
			//通过请求的文件全路径，找到文件所在的文件夹地址
			void build_actual_file(CON_S *o_pcn);
			//获取http包中的某一个字段，其中findbuf是要查找的buf，比如request head，ResponseHeader。
			s32 get_field(s8 *i_pfindbuf,s8 *i_phead_str, s8 *i_tail_str, s32 i_total_len,s32 o_pos,s32 &i_lastpos, s8 *o_ptbuf);
	};
}

#endif
