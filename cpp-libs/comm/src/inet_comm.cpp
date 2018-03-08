
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <iostream>
#include <string>
#include "inet_comm.h"

using namespace std ;

// 设置句柄阻塞模式
bool inet::set_sock_nonblock(int sockfd)
{

    int val = fcntl(sockfd, F_GETFL, 0);
    if(fcntl(sockfd, F_SETFL, val | O_NONBLOCK) == -1)
    {
        return false;
    }
    return true;
}


// 设置句柄复用
bool inet::set_addr_reuse(int sockfd, bool b_reuse)
{
    int reuse = 0;
    if(b_reuse)
    {
        reuse=1;
    }
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
    if(ret == 0)
    {
        return true;
    }
    return false;

}

// 关闭句柄
int inet::close_sock(int sockfd)
{
    return close(sockfd);

}


//********************************************************************
// 功能：	创建 socket fd
// 参数：
//	ccp_host_ip:		服务器主机 ip 地址(点分十进制)
//	us_host_port:		服务器绑定端口
//	si_proto_type:		创建 socket 类型(0: UDP, 1: TCP)
//	i_recv_buf_size:	最大接收数据 buf
//	i_send_buf_size:	最大发送数据 buf
//	p_sock_fd:		创建成功后返回给调用者的 socket fd
// 返回值：
//	类型：	bool
// 	true(成功, false(失败)
//********************************************************************
bool inet::create_sock(const char* host_ip,
                       unsigned short host_port,
                       short proto_type,
                       int recv_buf_size,
                       int send_buf_size,
                       int &  sock_fd
                      )
{
    bool b_ret = false;
    int new_sockfd = -1;
    struct sockaddr_in svr_addr= {0};

    if(proto_type == 0)
    {
        //UDP type
        new_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //建立UDP套接字
    }
    else if(proto_type == 1)
    {
        //TCP type
        new_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //建立TCP套接字
    }

    if(new_sockfd == -1)
    {
        return false;
    }

    struct in_addr	ip_addr;
    if(inet_aton(host_ip, &ip_addr) == 0)
    {
        printf("gen ip addr error,errno:%d,desc:%s\n ",errno,strerror(errno));
        close_sock(new_sockfd);
        return false;
    }
    svr_addr.sin_addr.s_addr = ip_addr.s_addr;
    svr_addr.sin_port =htons(host_port);
    svr_addr.sin_family = AF_INET;


    if(bind(new_sockfd, (struct sockaddr*)&svr_addr, sizeof(struct sockaddr_in))== -1)
    {
        printf("bind error.\n");
        close_sock(new_sockfd);
        return false;
    }
    else
    {

    }

    if(recv_buf_size>0)
    {
        if (setsockopt(new_sockfd, SOL_SOCKET, SO_RCVBUF, (char*)&recv_buf_size, sizeof(recv_buf_size))!=0)
        {
            printf("setsockopt SO_RCVBUF error.\n ");
        }
    }
    if(send_buf_size>0)
    {
        if(setsockopt(new_sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&send_buf_size, sizeof(send_buf_size))!=0)
        {
            printf("setsockopt SO_SNDBUF error.\n ");
        }
    }

    if(new_sockfd != -1 && new_sockfd>0 )
    {
        b_ret = true;
        sock_fd = new_sockfd;
    }

    return b_ret;
}


// ip地址转换
const char* inet::inet_ntoa(unsigned int ulIP)
{
    struct in_addr stIPAddr = {0};
    stIPAddr.s_addr = ulIP;
    return ::inet_ntoa(stIPAddr);
}


unsigned int inet::inet_aton(const char* host_ip)
{
    struct in_addr	ip_addr;

    ::inet_aton(host_ip, &ip_addr);

    return ip_addr.s_addr;
}



//创建一个连接到服务器的客户端连接
bool inet::create_tcp_client(const char *v_ip, unsigned short v_port, int &fd_out)
{
    struct sockaddr_in addr;

    fd_out = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd_out <= 0 )
    {
        fd_out = -1 ;
        return false;
    }

    int recv_buf_size=1024*1024;
    int send_buf_size=1024*1024;

    setsockopt(fd_out, SOL_SOCKET, SO_RCVBUF, (char*)&recv_buf_size, sizeof(recv_buf_size));
    setsockopt(fd_out, SOL_SOCKET, SO_SNDBUF, (char*)&send_buf_size, sizeof(send_buf_size));

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(v_port);
    addr.sin_addr.s_addr = inet_addr(v_ip);

    if(addr.sin_addr.s_addr == INADDR_NONE)
    {
        close(fd_out);
        fd_out = -1 ;
        return false;
    }

    /*设置为非阻塞*/
    int arg = fcntl(fd_out, F_GETFL, 0);
    fcntl(fd_out, F_SETFL, arg | O_NONBLOCK);

    time_t start_time = time(NULL);
    int ret = connect(fd_out,(struct sockaddr*)&addr,sizeof(addr));
    if( ret < 0 )
    {
        bool bConnected = false;
        struct pollfd _conncet_client[1];
        int _nfd = 1;
        memset(&_conncet_client[0],0,sizeof(pollfd));
        _conncet_client[0].fd = fd_out;
        _conncet_client[0].events = POLLOUT;

        int can_write =  ::poll(_conncet_client, _nfd, (int)(2*1000));
        if( can_write > 0)
        {
            bConnected = true;
        }

        if(!bConnected)
        {
            time_t end_time = time(NULL);
            //printf("connect to :%s fail, time cost:%llu, ret:%d,errno:%d,desc:%s\n",
            //          v_ip, end_time-start_time, ret,errno,strerror(errno));
            close(fd_out);
            fd_out = -1 ;
            return false;
        }
    }
    return true;
}


//向外发送tcp数据
int inet::send_tcp_data(int _sock_fd,char* buff, int len)
{
    int ret;
    int _snd=0;
    int _len = len;
    errno = 0;
    while(_len>0)
    {
        ret = ::send(_sock_fd,(void*)((char*)buff+_snd),_len,0);
        if ( ret <= 0 )
        {
            if ( errno == EINTR || errno == EAGAIN )
            {
                usleep(10);
                continue;
            }
            //TRACE_LOG("send fail,ret:%d errno:%d,desc:%s\n", ret, errno,strerror(errno) );
            break;
        }
        _len -= ret;
        _snd += ret;
    }

    return _snd;
}


unsigned int inet::GetCurMilliSecond()
{
    time_t dwPreciseTime;
#ifdef _WIN32
    struct _timeb	tbNow;
    _ftime(&tbNow);
    dwPreciseTime = tbNow.millitm;
#else
    struct timeval	tvNow;
    gettimeofday(&tvNow, NULL);
    dwPreciseTime = tvNow.tv_usec / 1000;
#endif
    return (unsigned int)dwPreciseTime;
}




//阻塞接收tcp数据
int inet::recv_tcp_data(int dwSockFd, char* pcRecvBuf, size_t tCount, int dwTimeout)
{
    char*  szRecvBuf = pcRecvBuf;
    size_t dwBytesRead = 0;
    int dwThisRead;
    unsigned int dwMilliSecT = 0;
    unsigned int dwTimeoutT = dwTimeout*1000;

    while(dwBytesRead < tCount  && dwMilliSecT < dwTimeoutT)
    {
        do
        {
            unsigned long tStart = GetCurMilliSecond();
            time_t dwTime = time(NULL);

            //printf("insight recv_tcp_data :begin recv\n ");
            dwThisRead = (int)recv(dwSockFd, szRecvBuf, tCount-dwBytesRead, 0);
            //printf("insight recv_tcp_data :after recv,dwThisRead=%d\n ",dwThisRead);

            dwTime = time(NULL) - dwTime;
            unsigned long tEnd = GetCurMilliSecond();
            dwMilliSecT += (unsigned int)(dwTime*1000 + tEnd - tStart);
        }
        while((dwThisRead < 0) && (errno == EINTR));

        if(dwThisRead > 0)
        {
            dwBytesRead += dwThisRead;
            szRecvBuf += dwThisRead;
        }
        else if(dwThisRead <= 0)
        {
            return dwBytesRead;
        }
    }

    if (dwMilliSecT >= dwTimeoutT)
        return dwBytesRead;
    else
        return (int)tCount;
}



std::string inet::GetDomainIp (std::string domain_name)
{
    struct hostent* he = gethostbyname(domain_name.c_str());
    if (he)
    {
        for (char** p = he->h_addr_list; p!= NULL; ++p)
        {
            in_addr a;
            if(*p != NULL)
            {
                memcpy(&a, *p, sizeof(a));
                return string(inet_ntoa(a));
            }
            else
            {
                break ;
            }
        }
    }
    return "";
}


