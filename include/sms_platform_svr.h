#ifndef _SMS_PLATFORM_SVR_H_
#define _SMS_PLATFORM_SVR_H_

#include "epoller.h"
#include "sms_platform_config.h"


struct CSmsPlatformThreadManager;
class TimerManager;
class CSmsPlatformAcceptHandler;
class CSmsPlatformSvr
{
public:
      CSmsPlatformSvr(CSmsPlatformConfig* oConfig);
 
	  ~CSmsPlatformSvr();	
public:
    //����
    bool fire();

	void onTimer(int time_id, time_t current_time);

	void process();

	void addMsgCount() { total_msg_count++ ;}

    static CSmsPlatformConfig* GetConfig() { return sms_config; }

private:
    //�����¼�����
    CEPoller conn_epoll;

    //��ʱ��������
    TimerManager* timer_manager;

    //�̹߳�����
    CSmsPlatformThreadManager* thread_manager;

    //������ִ����
    CSmsPlatformAcceptHandler*  receipt_accept_handler;

    //�����ļ�
    static CSmsPlatformConfig* sms_config;

    //��Ϣͳ��
	unsigned long long total_msg_count;

};

extern CSmsPlatformSvr* g_sms_svr;


#endif

