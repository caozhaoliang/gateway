/* ===========================================================================
*   Description: 短信平台回执HTTP工作处理器
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _SMS_PLATFORM_RECEIPT_HANDLER_H_
#define _SMS_PLATFORM_RECEIPT_HANDLER_H_


#include "comm_type.h"
#include "simple_log.h"
#include "epoller.h"
#include "timer_manager.h"
#include "sms_platform_thread_manager.h"
#include "http_parser.h"
#include "http_parser_server.h"


#define MAX_BUFF_LEN (1024*1024)
#define MAX_PKG_LEN  (1024*10)


class CSmsPlatformReceiptHandler : public CEpollSocket, public BaseTimer
{
public:
    CSmsPlatformReceiptHandler(int handle, TimerManager* manager, CEPoller* epoller, 
        CSmsPlatformThreadManager* thread_mgr);
    ~CSmsPlatformReceiptHandler();
    void setClientAddr(struct sockaddr_in & address);
    sockaddr_in getClientAddr();
    int onRecv();
    int onClose();
    int onError();
    void onTimer(int time_id, time_t current_time);

private:
    int processMsg(const STHttpMsg & http_msg);

    //应答HTTP消息
    int responeHttpMsg(const char* buff, const int len);

	int getErrodeCode(std::string &errstr, const int errflag);

	//短信回执
	int reqSmsReceipt(const std::string & req_msg);

	//上行短信
	int reqSmsUplink(const std::string & req_msg);

private:
    void closeSocket();
    void freeMyself();

private:
    struct sockaddr_in client_addr;    // 对端ip信息
    TimerManager*  timer_manager;
    CEPoller*      bind_epoller;
    int            time_id;

    //连接创建时间
    time_t            create_sock_time;   

    //内部维持的时钟
    time_t            time_tick;         
    
    //工作线程管理器
    CSmsPlatformThreadManager* thread_manager;

    //HTTP解析器
    CHttpParserServer  http_parser;
};

#endif

