#ifndef _BASIC_DEFINE_H_
#define _BASIC_DEFINE_H_

namespace pure_baselib
{	
	//����ѡ��
	//@�ļ��нṹ��ʽ�������Ҫ��û������ҳ����������ʾĿ¼�ṹ
	//ʹ������ĺ꣬������ע�͵�
	#define _CONFIG_HTTP_HAS_DIRECTORIES_ 	1		
	#define _CONFIG_HTTP_DIRECTORIES_ 		1
	//@end

	//IPV6�Ķ�Ӧ����ѡ�ʹ��IPV6���
	//#define _IPV6_ 1

	//����ֵ�ĺ�
	#define WAIT									2
	#define REFILTER								1
	#define SUCCESS 								0
	#define FAILURE								~0

	//���ü����˿�
	#define CONFIG_HTTP_PORT 					8124	//http�����˿�
	#define CONFIG_HTTP_HTTPS_PORT 			443		//https�����˿�

	#define MAX_REQUEST_SIZE					4096	//һ���ܶ�ȡ����������

	/*
	*�����������������ӵĸ��������������������ʹ��ϵͳ����accept()Ӧ��֮ǰ
	*Ҫ�ڽ�������еȴ������ֵ�Ƕ�����������ӵ�е�����ĸ�����
	*/
	#define MAX_LISTEN_SIZE 					1024  	

	#define MAXREQUESTLENGTH                    		256		//Ӧ���ַ�������󳤶�
	#define INITIAL_CONNECTION_SLOTS           	10		//��ʼ�����������г���

	//���ӹ����еķ�����
	#define STATE_WANT_TO_READ_HEAD 			1		
	#define STATE_WANT_TO_SEND_HEAD 			2
	#define STATE_WANT_TO_READ_FILE 			3
	#define STATE_WANT_TO_SEND_FILE 			4
	#define STATE_DOING_DIR 					5

	//changed by simon 2012-12-18
	#define STATE_DONT_FINISH_SEND 			6
	//end

	//һ�����ӵĳ�ʱʱ�䣬��λ(s)
	#define CONFIG_HTTP_TIMEOUT 				300

	//��ͷHTTP_VERSION�ֽ�
	#define HTTP_VERSION        "HTTP/1.1"
	//changed by simon 2012-12-14
	const s8*  const tclose = "close";
	const s8*  const tkeep_alive ="keep_alive";
	//end	

	#define VERSION    "1.0.0"
	const s8 * const server_version = "purehttpd/"VERSION;

	static const s8 * index_file = "index.html";

	//http���񷽷���
	enum HTTP_TYPE_E
	{
	    TYPE_GET = 0,
	    TYPE_HEAD,
	    TYPE_POST
	};

	

}
#endif /* _BASIC_DEFINE_H_ */
