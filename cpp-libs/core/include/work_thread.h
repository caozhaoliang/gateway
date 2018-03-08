/* ===========================================================================
*   Description: ���������¼���Ӧ�߳�
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
    //���캯��
    CWorkThread(const int timer_count, const int epoll_count, 
        const int thread_stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //��������
    virtual ~CWorkThread();

public:
    //��ʼ���߳�
    virtual int init();

    //�����߳�
    virtual int start();

    //��ʱ��������
    //virtual void onTimer(int time_id, time_t current_time);

protected:
    //����������
    virtual int process();

    //�¼�����
    virtual int eventLoop();

protected:
    //epoll �¼�����
    CEPoller* event_epoll;

    //����������
    TimerManager* timer_manager;

    //��ʱ��������
    int max_timer_count;

    //�¼�����������
    int  max_epoll_count;

};


#endif

