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
    //启动
    bool fire();

	void onTimer(int time_id, time_t current_time);

	void process();

	void addMsgCount() { total_msg_count++ ;}

    static CSmsPlatformConfig* GetConfig() { return sms_config; }

private:
    //网络事件对象
    CEPoller conn_epoll;

    //定时器管理器
    TimerManager* timer_manager;

    //线程管理器
    CSmsPlatformThreadManager* thread_manager;

    //监听回执请求
    CSmsPlatformAcceptHandler*  receipt_accept_handler;

    //配置文件
    static CSmsPlatformConfig* sms_config;

    //消息统计
	unsigned long long total_msg_count;

};

extern CSmsPlatformSvr* g_sms_svr;


#endif

