
#ifndef _SVR_COMM_LIB_H
#define	_SVR_COMM_LIB_H

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <poll.h>

#include <time.h>
#include <string>
namespace inet
{
// 设置 socket fd 为非阻塞模式
bool set_sock_nonblock(int sockfd);

bool set_addr_reuse(int sockfd, bool b_reuse);

int close_sock(int sockfd);

bool create_sock(const char* host_ip, unsigned short host_port,
                 short proto_type,		//UDP(0) or TCP(1) socket type?
                 int recv_buf_size, int send_buf_size,
                 int& sock_fd
                );

// 将网络字节序表示的IP地址转换为点分十进制形式
const char* inet_ntoa(unsigned int ulIP);

unsigned int inet_aton(const char* host_ip);


// 创建一个连接到服务端的客户端连接
bool create_tcp_client(const char *v_ip, unsigned short v_port, int &fd_out);

int send_tcp_data(int _sock_fd,char* buff, int len);

int recv_tcp_data(int dwSockFd, char* pcRecvBuf, size_t tCount, int dwTimeout);

unsigned int GetCurMilliSecond();

// 解析域名的ip地址  
std::string GetDomainIp (std::string domain_name);

}

#endif
