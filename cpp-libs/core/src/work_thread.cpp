/* ===========================================================================
*   Description: ���������߳�
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


//���캯��
CWorkThread::CWorkThread(const int timer_count, const int epoll_count, const int thread_stack_size) :
    event_epoll(NULL),
    timer_manager(NULL),
    max_timer_count(timer_count),
    max_epoll_count(epoll_count)
{

}

//��������
CWorkThread::~CWorkThread()
{
    DELETE(event_epoll);
    DELETE(timer_manager);
}

//��ʼ������
int CWorkThread::init()
{
    //��ʼ�������¼�����
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

    //��ʼ����ʱ���������
    NEW(timer_manager, TimerManager(max_timer_count));
    if(NULL == timer_manager)
    {
        ERROR_LOG("new TimerManager error.\n");
        return -1;
    }

    return 0;
}

//�����߳�
int CWorkThread::start()
{
    return 0;
}

/*
//��ʱ��������
void CWorkThread::onTimer(int time_id, time_t current_time)
{
    return;
}
*/

//����������
int CWorkThread::process()
{
    return 0;
}

//����������
int CWorkThread::eventLoop()
{
    time_t current_time  = time(NULL);
    time_t last_check_time = current_time;

    while(runing_flag)
    {
        //����������Ϣ
        event_epoll->waitForEvent(200);

        //����ʱ��
        current_time = time(NULL);
        if(current_time - last_check_time >= 1)
        {
            //������ʱ��
            last_check_time = current_time;
            timer_manager->loopCheck(current_time); 
        }
    }

    return 0;
}








