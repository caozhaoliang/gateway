/* ===========================================================================
*   Description: 短信平台回执TCP接受处理器
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include "inet_comm.h"
#include "simple_log.h"
#include "inet_comm.h"
#include "sms_platform_receipt_handler.h"
#include "sms_platform_accept_handler.h"


using namespace std ;

CSmsPlatformAcceptHandler::CSmsPlatformAcceptHandler(string ip, 
                                                     unsigned short port, 
                                                     TimerManager* manager, 
                                                     CEPoller* epoller, 
                                                     CSmsPlatformThreadManager* thread_mgr)
{
    _sock_fd = -1 ;

    thread_manager = thread_mgr ;

    tcp_ip = ip;
    tcp_port = port;
    timer_manager = manager ;
    bind_epoller = epoller;

    createTcpServer();

    // 每2秒检查一次句柄情况
    time_id = timer_manager->setTimer(this, 2, true, 0);
}

CSmsPlatformAcceptHandler::~CSmsPlatformAcceptHandler()
{
    if( time_id != INVALID_TIME_ID )
    {
        timer_manager->killTimer(time_id) ;
        time_id = INVALID_TIME_ID;
    }

    closeSocket();
}

void CSmsPlatformAcceptHandler::onTimer(int id,time_t current_time)
{
    if( id != time_id)
    {
        return ;
    }

    if( _sock_fd <= 0 )
    {
        createTcpServer();
    }

}

int  CSmsPlatformAcceptHandler::onRecv()
{
    int length = sizeof(struct sockaddr_in);
    int clientfd = accept(_sock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&length);
    if ( clientfd <= 0 )
    {
        ERROR_LOG("accept error,clientfd:%d,errno:%d,desc:%s.\n",clientfd,errno,strerror(errno));
        return -1;
    }

    if(! inet::set_sock_nonblock(clientfd))
    {
        close(clientfd);
        return -1;
    }

    //设置发送缓冲区和接收缓冲区大小
    int buff_size = 20 * 1024 * 1024;
    int set_result = 0 ;
    set_result = setsockopt(clientfd, SOL_SOCKET, SO_RCVBUF, (char*)&buff_size, sizeof(buff_size));
    set_result = setsockopt(clientfd, SOL_SOCKET, SO_SNDBUF, (char*)&buff_size, sizeof(buff_size));

    CSmsPlatformReceiptHandler* handler = new CSmsPlatformReceiptHandler(clientfd, 
        timer_manager, bind_epoller, thread_manager);
    handler->setClientAddr(client_addr);

    return 0;
}


void CSmsPlatformAcceptHandler::closeSocket()
{
    if( _sock_fd > 0 )
    {
        detachEpoller();
        close(_sock_fd);
        _sock_fd = -1;
    }
}

bool CSmsPlatformAcceptHandler::createTcpServer()
{
    //创建TCP服务器句柄
    _sock_fd = -1;

    int tcp_sock_server = -1;
    bool creat_tcp = inet::create_sock(tcp_ip.c_str(), tcp_port,1, 1024*1024*10, 1024*1024*10, tcp_sock_server);
    if(!creat_tcp)
    {
        printf("fatal error,create tcp server error,ip:%s,port:%d,errno:%d,desc:%s\n"
               ,tcp_ip.c_str(),tcp_port,errno,strerror(errno));
        return false ;
    }
    _sock_fd = tcp_sock_server;

    inet::set_sock_nonblock(_sock_fd);
    inet::set_addr_reuse(_sock_fd, true);

    if(listen(_sock_fd,1024) < 0)
    {
        printf("fatal error,listen on socket failed:errno %d,desc:%s\n" ,
            errno, strerror(errno));
        close(_sock_fd);
        _sock_fd = -1 ;
        return false;
    }

    //事件绑定
    //attachEpoller(bind_epoller, FD_RECV|FD_CLOSE|FD_ERROR);
    attachEpoller(bind_epoller);
    setEvent(FD_RECV|FD_CLOSE|FD_ERROR);

    return true ;

}


int CSmsPlatformAcceptHandler::onClose()
{
    WRITELOG("listen server OnClose,errno:%d,desc:%s\n",errno,strerror(errno));
    closeSocket();
    return 0 ;
}


int CSmsPlatformAcceptHandler::onError()
{
    WRITELOG("listen server OnError,errno:%d,desc:%s\n",errno,strerror(errno));
    closeSocket();
    return 0 ;
}

