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

// 时钟单元
struct TimerNode
{
    bool   is_used ;
    int    time_id ;                    // 定时器ID
    time_t start_time ;                 // 当前时间
    int interval_time ;                 // 间隔时间
    int            trigger_count ;      // 当前触发次数
    int            trigger_total ;      // 总的触发次数
    bool           loop_type     ;      // 是否无限循环
    BaseTimer*     timer_obj     ;      // 处理对象

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

	int         trigger_id ;   //当前正在执行OnTimer的id，规避在ontimer中KillTimer SetTimer错误隐患
private:
    void initManager() ;

    void getListNode(TimerNode* list_head , TimerNode* & current);

    void listNodeAdd(TimerNode* list_head, TimerNode* current);

    void listNodeDel(TimerNode* current);


    //void LoopCheck();  // 检测定时器
public:
    TimerManager() ;

    TimerManager(int max_count) ;

    ~TimerManager() ;

    // obj:对象指针
    // interval: 超时间隔 单位秒
    // type :是否无限循环触发
    // total :非无限循环触发时,触发次数
    // return :定时器id
    int setTimer(BaseTimer* obj, int interval, bool type = true, int total = 0);

    bool killTimer(int id);

    void loopCheck(time_t current_time);  // 检测定时器

};

#endif

