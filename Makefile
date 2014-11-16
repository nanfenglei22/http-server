############################# makefile ############################
EPOLL_OBJECTS=obj/epoll.o \
              obj/epoll_main.o \
              obj/http_server.o \
              obj/http_connection.o \
              obj/connection.o

CXX=g++
CXXFLAG=-c -g -o 
GDB=-ggdb3
LIB_PTHREAD=-lpthread
PATH_INCLUDE=-Iinclude

################ make epoll ###################
epoll:$(EPOLL_OBJECTS)
	$(CXX) -o $@ $^ $(GDB) $(PATH_INCLUDE)
	mv epoll bin/epoll
	@echo "/* 正在离开epoll模块...... */"
obj/epoll.o:src/epoll.cpp
	@echo "/* 正在编译epoll模块...... */"
	$(CXX) $(CXXFLAG) $@ $< $(GDB) $(PATH_INCLUDE)
obj/epoll_main.o:src/epoll_main.cpp
	$(CXX) $(CXXFLAG) $@ $< $(GDB) $(PATH_INCLUDE)
obj/connection.o:src/base_connect.cpp
	$(CXX) $(CXXFLAG) $@ $< $(GDB) $(PATH_INCLUDE)	
obj/http_server.o:src/http_server.cpp
	$(CXX) $(CXXFLAG) $@ $< $(GDB) $(PATH_INCLUDE)	
obj/http_connection.o:src/http_connection.cpp
	$(CXX) $(CXXFLAG) $@ $< $(GDB) $(PATH_INCLUDE)	

.PHONY:clean
clean:
	@echo "/* 正在删除bin目录和obj目录下的所有文件... */"
	rm -rf obj/* bin/*
