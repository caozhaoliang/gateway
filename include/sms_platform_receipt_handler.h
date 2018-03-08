/* ===========================================================================
*   Description: ����ƽ̨��ִHTTP����������
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

    //Ӧ��HTTP��Ϣ
    int responeHttpMsg(const char* buff, const int len);

	int getErrodeCode(std::string &errstr, const int errflag);

	//���Ż�ִ
	int reqSmsReceipt(const std::string & req_msg);

	//���ж���
	int reqSmsUplink(const std::string & req_msg);

private:
    void closeSocket();
    void freeMyself();

private:
    struct sockaddr_in client_addr;    // �Զ�ip��Ϣ
    TimerManager*  timer_manager;
    CEPoller*      bind_epoller;
    int            time_id;

    //���Ӵ���ʱ��
    time_t            create_sock_time;   

    //�ڲ�ά�ֵ�ʱ��
    time_t            time_tick;         
    
    //�����̹߳�����
    CSmsPlatformThreadManager* thread_manager;

    //HTTP������
    CHttpParserServer  http_parser;
};

#endif

