/* ===========================================================================
*   Description: 短信平台回执TCP接受处理器
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _SMS_PLATFORM_ACCEPT_HANDLER_H_
#define _SMS_PLATFORM_ACCEPT_HANDLER_H_

#include "epoller.h"
#include "timer_manager.h"
#include "sms_platform_thread_manager.h"

using namespace std;

#define MAX_CONNECT_FREQUENCY 200

class CSmsPlatformAcceptHandler : public CEpollSocket, public BaseTimer
{
public:
    CSmsPlatformAcceptHandler(string ip, unsigned short port, TimerManager* manager, 
        CEPoller* epoller, CSmsPlatformThreadManager* thread_manager);
    ~CSmsPlatformAcceptHandler();
    int onRecv();
    int onClose();
    int onError();
    void onTimer(int time_id, time_t current_time);

private:
    TimerManager*      timer_manager;
    CEPoller*          bind_epoller;
	struct sockaddr_in client_addr;
    string         tcp_ip;
    unsigned short tcp_port;
    int            time_id;

    std::map<unsigned int, unsigned short> frequency_info;
	
    //工作线程管理器
    CSmsPlatformThreadManager*  thread_manager;
private:
    bool createTcpServer();
	void closeSocket();
};


#endif