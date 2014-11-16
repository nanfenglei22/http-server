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
		s32		sock_fd;	//socket的描述符
		u32		sock_type;	//socket描述符的类型。0表示connect描述符；1表示server的描述符。
		virtual ~base_socket(){}
	};
};

#endif /* BASE_SOCKET_H_ */
