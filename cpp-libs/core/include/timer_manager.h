#ifndef _TIMER_MANAGER_H
#define _TIMER_MANAGER_H

#include <time.h>

#define INVALID_TIME_ID -1

class BaseTimer
{
public:
    BaseTimer() {}
    virtual ~BaseTimer() {}
    virtual void onTimer(const int timer_id,time_t current_time) {}
};

// ʱ�ӵ�Ԫ
struct TimerNode
{
    bool   is_used ;
    int    time_id ;                    // ��ʱ��ID
    time_t start_time ;                 // ��ǰʱ��
    int interval_time ;                 // ���ʱ��
    int            trigger_count ;      // ��ǰ��������
    int            trigger_total ;      // �ܵĴ�������
    bool           loop_type     ;      // �Ƿ�����ѭ��
    BaseTimer*     timer_obj     ;      // �������

    TimerNode*     next_node ;
    TimerNode*     prev_node ;

    TimerNode()
    {
        initNode();
    }

    void initNode()
    {
        is_used = false ;
        time_id = 0 ;
        start_time = 0 ;
        interval_time = 0 ;
        trigger_count = 0 ;
        trigger_total = 0 ;
        loop_type = 0 ;
        timer_obj = 0 ;

        next_node = 0 ;
        prev_node = 0 ;
    }
};


class TimerManager
{
private:
    TimerNode  idle_head ;
    TimerNode  used_head ;

    TimerNode*  container_node ;

    int         max_node_count ;
    int         idle_node_count ;

	int         trigger_id ;   //��ǰ����ִ��OnTimer��id�������ontimer��KillTimer SetTimer��������
private:
    void initManager() ;

    void getListNode(TimerNode* list_head , TimerNode* & current);

    void listNodeAdd(TimerNode* list_head, TimerNode* current);

    void listNodeDel(TimerNode* current);


    //void LoopCheck();  // ��ⶨʱ��
public:
    TimerManager() ;

    TimerManager(int max_count) ;

    ~TimerManager() ;

    // obj:����ָ��
    // interval: ��ʱ��� ��λ��
    // type :�Ƿ�����ѭ������
    // total :������ѭ������ʱ,��������
    // return :��ʱ��id
    int setTimer(BaseTimer* obj, int interval, bool type = true, int total = 0);

    bool killTimer(int id);

    void loopCheck(time_t current_time);  // ��ⶨʱ��

};

#endif

