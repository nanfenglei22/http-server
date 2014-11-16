########################## README ##############################
	/* 该版本中实现了epoll模块的整体架构和epoll模块的网络模型功能。该版本epoll使用的是ET触发，非阻塞IO。*/

1、本程序是采用epoll模型的http客户端和client端。在linux下编译通过。
2、支持get、文件下载（客户端），页面展示。
3、可支持IPV6（未测）
4、支持hls协议。
说明：
	1，epoll模块说明：
    epoll目录中包含了epoll模块的相关代码，其中以下几个目录。
    src:有所.cpp的源文件存放路径
    include:所有的.h文件存放路径
    obj:所有临时的.obj文件存放路径
    bin:所有的可执行文件存放路径
    2，epoll使用说明：
    epoll模块中，有几个主要的基类：
    										base_socket类
												  ｜
						－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－						  
						｜						  			    			｜							
    				server类												connection类
    使用时，需要自定一个服务器类继承server类，重写server类中的纯虚函数，然后注册到epoll类中。使用epoll的处理函数进行epoll的逻辑处理。具体的使用方式轻参考epoll的详细设计文档。
    				
管理：
    整个epoll模块使用GNU的makefile管理，通过使用make, make clean等命令完成编译和删除。

make步骤：
	先使用make clean清除原有的.o和可执行文件
    #make clean
    使用make编译epoll
    #make
   	make后会在bin路径下生成可执行文件。

	


