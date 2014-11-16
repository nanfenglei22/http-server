/*
 * base_socket.h
 *
 *  Created on: 2012-11-23
 *      Author: chengzhip
 */

#ifndef BASE_SOCKET_H_
#define BASE_SOCKET_H_

#include "basic_type.h"
#include "c_headers.h"
#include "return_values.h"
//#include "public_func.h"
using namespace pure_baselib;

namespace pure_baselib
{
	class base_socket
	{
	public:
		s32		sock_fd;	//socket��������
		u32		sock_type;	//socket�����������͡�0��ʾconnect��������1��ʾserver����������
		virtual ~base_socket(){}
	};
};

#endif /* BASE_SOCKET_H_ */
