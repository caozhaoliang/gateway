/* ===========================================================================
*   Description: 工作任务事件反应线程
*   Author:  shenhuihai
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_

#include <queue>
#include <string>
#include "thread_base.h"
#include "epoller.h"
#include "timer_manager.h"

using namespace std;



class CWorkThread : public CThreadBase, public BaseTimer
{
public:
    //构造函数
    CWorkThread(const int timer_count, const int epoll_count, 
        const int thread_stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //析构函数
    virtual ~CWorkThread();

public:
    //初始化线程
    virtual int init();

    //启动线程
    virtual int start();

    //定时器处理函数
    //virtual void onTimer(int time_id, time_t current_time);

protected:
    //主处理流程
    virtual int process();

    //事件处理
    virtual int eventLoop();

protected:
    //epoll 事件对像
    CEPoller* event_epoll;

    //定是器管理
    TimerManager* timer_manager;

    //定时器最大个数
    int max_timer_count;

    //事件对像最大个数
    int  max_epoll_count;

};


#endif

