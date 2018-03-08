/* ===========================================================================
*   Description: 工作任务线程
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include <sys/time.h>
//#include "isgw_def.h"
#include "comm_def.h"
#include "simple_log.h"
#include "work_thread.h"


//构造函数
CWorkThread::CWorkThread(const int timer_count, const int epoll_count, const int thread_stack_size) :
    event_epoll(NULL),
    timer_manager(NULL),
    max_timer_count(timer_count),
    max_epoll_count(epoll_count)
{

}

//析构函数
CWorkThread::~CWorkThread()
{
    DELETE(event_epoll);
    DELETE(timer_manager);
}

//初始化函数
int CWorkThread::init()
{
    //初始化网络事件对象
    NEW(event_epoll, CEPoller);
    if(NULL == event_epoll)
    {
        ERROR_LOG("new CEPoller error.\n");
        return -1;
    }

    if(0 != event_epoll->create(max_epoll_count))
    {
        ERROR_LOG("Create epoll event error.\n");
        return -1;
    }

    //初始化定时器管理对象
    NEW(timer_manager, TimerManager(max_timer_count));
    if(NULL == timer_manager)
    {
        ERROR_LOG("new TimerManager error.\n");
        return -1;
    }

    return 0;
}

//启动线程
int CWorkThread::start()
{
    return 0;
}

/*
//定时器处理函数
void CWorkThread::onTimer(int time_id, time_t current_time)
{
    return;
}
*/

//主处理流程
int CWorkThread::process()
{
    return 0;
}

//主处理流程
int CWorkThread::eventLoop()
{
    time_t current_time  = time(NULL);
    time_t last_check_time = current_time;

    while(runing_flag)
    {
        //处理网络消息
        event_epoll->waitForEvent(200);

        //触发时钟
        current_time = time(NULL);
        if(current_time - last_check_time >= 1)
        {
            //触发定时器
            last_check_time = current_time;
            timer_manager->loopCheck(current_time); 
        }
    }

    return 0;
}








