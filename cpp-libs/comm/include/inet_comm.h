
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
// ���� socket fd Ϊ������ģʽ
bool set_sock_nonblock(int sockfd);

bool set_addr_reuse(int sockfd, bool b_reuse);

int close_sock(int sockfd);

bool create_sock(const char* host_ip, unsigned short host_port,
                 short proto_type,		//UDP(0) or TCP(1) socket type?
                 int recv_buf_size, int send_buf_size,
                 int& sock_fd
                );

// �������ֽ����ʾ��IP��ַת��Ϊ���ʮ������ʽ
const char* inet_ntoa(unsigned int ulIP);

unsigned int inet_aton(const char* host_ip);


// ����һ�����ӵ�����˵Ŀͻ�������
bool create_tcp_client(const char *v_ip, unsigned short v_port, int &fd_out);

int send_tcp_data(int _sock_fd,char* buff, int len);

int recv_tcp_data(int dwSockFd, char* pcRecvBuf, size_t tCount, int dwTimeout);

unsigned int GetCurMilliSecond();

// ����������ip��ַ  
std::string GetDomainIp (std::string domain_name);

}

#endif
