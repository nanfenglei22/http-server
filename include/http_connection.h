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
				s32		sock_fd;						//socket��������
				u32		sock_type;					//socket�����������͡�0��ʾconnect��������1��ʾserver����������
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
			server					*svr;			//ûһ��connection�����Լ��ƶ���serverָ�룬����server��connection�������
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
			virtual s32 connect_handle_error() ;//��������			
			virtual s32 connect_read(s8* buf, u32 buf_len, u32 flag);
			virtual s32 connect_write(s8* buf, u32 buf_len, u32 flag);
			virtual s32 connect_sendfile(s32 file_fd, off_t *offset, size_t size, u32 flag);
			

			//changed by simon 2012-12-18 
			/******************************************************
			 * ������: creat_send_option
			 * ����: �����Լ���Ҫ����Ӧ����
			 * ����:
			 * 		//i_pcn:CON_S����ָ�룬���������������еĲ���
			 *		buf:�ַ�����ָ�룬��ָ���ֵ��ʾ����ͷ֮����Ҫ��������ݡ�
			 *		buf_len:u32���ʹ�С��buf���ݵĴ�С��
			 *		res_type:u32����,��Ҫ��Ӧ����Ӧͷ���ͣ�eg:200,404,304,206
			 *		flag:u32���ͣ���δʹ�á�
			 * ���:
			 * ����ֵ: s32���͡��ɹ�����SUCCESS��ʧ�ܷ���FAILURE��
			 *******************************************************/
			virtual s32 creat_send_option(s8* buf, u32 buf_len, u32 res_type,u32 flag);

			s32 http_send_response(s8* buf, u32 buf_len, u32 res_type,u32 flag);
			//end
			
			size_t http_sendfile(CON_S *o_pcn);
			CON_S *get_cons();
			//end
			#ifdef _CONFIG_HTTP_DIRECTORIES_
			//չ�����г�������·��
			void show_main_dir(CON_S *o_pcn);
			#endif
			//��������ͷ
			void http_read_head(CON_S *o_pcn) ;
			//������Ӧ
			void http_send_head(CON_S *o_pcn);
			//��ȡ�ϴ����ļ�
			void http_read_file(CON_S *o_pcn);
			//����������ļ�
			void http_send_file(CON_S *o_pcn);

			//���������Ӧ��ʱ��ģ���ʼ��
			void tdate_init(void);
			//���������Ӧ��ʱ�����ģ��ıȽ�ģ��
			static s32 day_mon_map_compare(const s8 *i_pv1, const s8 *i_pv2);
		protected:
			//changed by simon 2012-12-18
			off_t m_offset;
			size_t m_sendlenth;
			//end
			#ifdef _CONFIG_HTTP_DIRECTORIES_
			//��û����ҳ�������£�����������������Ŀ¼
			void dir_listing(CON_S *o_pcn);
			#endif

			//ת��url����������ַ(%xx )����������
			void url_decode(s8 *i_pbuf);
			
			#ifdef _CONFIG_HTTP_DIRECTORIES_
			//ת��url����������ַ���(%xx )��ʽ
			void url_encode(const u8 *i_ps, s8 *o_pt); 
			#endif
			
			//A-Z,a-z,0-9��Щ�ַ���ʶ��
			s32 hex_it(s8 c);
			//�ַ���飬���ͽ�����һ��������Ϣ�Ƿ�Ϸ�
			s32 sanitize_file(const s8 *i_pbuf);
			//�ַ���飬����host��ַ��
			s32 sanitize_host(s8 *i_pbuf);
			//��װ��strncpy(),��ֹ���ַ�'\0'��β���ַ�������������
			s8 * my_strncpy(s8 *o_pdest, const s8 *i_psrc, size_t n);
			//���ͳ�����Ӧ������403��404��418
			void send_error(CON_S *o_pcn, s32 i_err);
			// һ��һ�еĽ���������Ϣ�����������Ҫ����Ϣ���Ѿ������꣬����1
			s32 head_elem(CON_S *o_pcn, s8 *i_pbuf);
			//socket ��д����
			s32 special_read(CON_S *o_pcn, void *i_pbuf, size_t i_count);
			s32 special_write(CON_S *o_pcn,const s8 *i_pbuf, size_t i_count);
			
			//ʹ��2�ֲ��ң��ҵ�str�����ʱ�����ܼ����߼��·�
			s8 day_mon_map_search(const s8* i_pstr,const DMM_S* i_ptab, s32 n);
			//���������Ӧ��ʱ��������ҳ����ܼ����߼��·�
			time_t tdate_parse(const s8* i_pstr);
			//����ͷ���͵��ļ�����
			const s8* get_mime_type(const s8 *i_pname);
			//�ж�tpbuf�����·���Ƿ���Ŀ¼
			s32 isdir(const s8 *i_pbuf);
			//ͨ��������ļ�ȫ·�����ҵ��ļ����ڵ��ļ��е�ַ
			void build_actual_file(CON_S *o_pcn);
			//��ȡhttp���е�ĳһ���ֶΣ�����findbuf��Ҫ���ҵ�buf������request head��ResponseHeader��
			s32 get_field(s8 *i_pfindbuf,s8 *i_phead_str, s8 *i_tail_str, s32 i_total_len,s32 o_pos,s32 &i_lastpos, s8 *o_ptbuf);
	};
}

#endif
