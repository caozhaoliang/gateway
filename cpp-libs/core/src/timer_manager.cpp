/* ===========================================================================
*   Description: 时钟管理单元，向外提供定时器功能
*   Author:  huihai.shen@inin88.com
*   Date :  
*   histroy:
* ============================================================================
*/
#include <vector>
#include "timer_manager.h"
#include "simple_log.h"

using namespace std ;

TimerManager::TimerManager()
{
    max_node_count = 500000 ; //50W个
    initManager();
}

TimerManager::TimerManager(int max_count)
{
    max_node_count = max_count ;
    initManager();
}

void TimerManager::initManager()
{
    container_node = new TimerNode[max_node_count];
    idle_node_count = max_node_count;

    for(int index=0; index<max_node_count; index++)
    {
        container_node[index].time_id = index ;
        container_node[index].is_used = false ;
        if(index < max_node_count-1 )
        {
            container_node[index].next_node = &container_node[index+1];
        }

        if(index>0)
        {
            container_node[index].prev_node =  &container_node[index-1];
        }
    }

    idle_head.next_node = &container_node[0];
	container_node[0].prev_node = &idle_head ;

    trigger_id = -1;

}

TimerManager::~TimerManager()
{
    if(container_node)
    {
        delete [] container_node ;
    }
    container_node = NULL ;
}


// 从队列取一个结点
void TimerManager::getListNode(TimerNode* list_head , TimerNode* & current)
{
    current = list_head->next_node ;
    while( current )
    {
        if(current == NULL)
        {
            ERROR_LOG("fatal error,not idle time node\n");
            return ;
        }

        if( current->time_id == trigger_id)
        {
            current = current->next_node ;
            continue;
        }
        else
        {
            TimerNode* next = current->next_node ;
			TimerNode* prev = current->prev_node ;

			//prev肯定不会为空
			prev->next_node = next ;
            if(next)
            {
                next->prev_node = prev ;
            }
            current->next_node = NULL;
            current->prev_node = NULL ;
			break ;
        }
    }
}


// 将一个结点加入到队列
void TimerManager::listNodeAdd(TimerNode* list_head ,TimerNode* current)
{
    TimerNode* next = list_head->next_node ;
    if(next == NULL)
    {
        list_head->next_node = current;
        current->prev_node = list_head ;
    }
    else
    {
        list_head->next_node = current ;
        current->next_node = next ;

        current->prev_node = list_head ;
        next->prev_node = current ;
    }
}


// 删除一个结点
void TimerManager::listNodeDel(TimerNode* current)
{
    if(current == NULL)
    {
        return ;
    }

    TimerNode* prev = current->prev_node ;
    TimerNode* next = current->next_node ;


    if(prev)
    {
        prev->next_node = next ;
    }

    if(next)
    {
        next->prev_node = prev ;
    }

    current->prev_node = NULL ;
    current->next_node = NULL ;

}

int TimerManager::setTimer(BaseTimer* obj,int interval,bool type,int total)
{
    if(obj == NULL || total< 0 || ( type == false && total == 0 )  )
    {
        return -1;
    }

    if(idle_node_count <= 0)
    {
        ERROR_LOG("set timer error,no used timer.\n");
        return -1;
    }

    //申请一个时钟单元
    TimerNode* current = NULL;
    getListNode(&idle_head,current) ;
    if(current == NULL)
    {
        return -1;
    }

    idle_node_count--;

    //赋值
    current->is_used = true;
    current->start_time = time(NULL);
    current->interval_time = interval ;
    current->trigger_count = 0 ;
    current->trigger_total = total;
    current->loop_type = type;
    current->timer_obj = obj;

    listNodeAdd(&used_head,current);

    return current->time_id ;

}


bool TimerManager::killTimer(int id)
{
    if(id<0 || id >max_node_count || container_node[id].is_used == false)
    {
        return false ;
    }

    //加入到空闲列表
    TimerNode* current = &container_node[id];

    listNodeDel(current);

    idle_node_count++;

    //清空
    current->is_used = false;
    current->start_time = 0;
    current->interval_time = 0;
    current->trigger_count = 0 ;
    current->trigger_total = 0;
    current->loop_type = 0;
    current->timer_obj = 0;

    listNodeAdd(&idle_head ,current);

    return true ;
}


//定时检测
void TimerManager::loopCheck(time_t current_time )
{
    TimerNode* current_node ;
    if( idle_node_count == max_node_count )
    {
        return ;
    }

    //在这个函数里面有可能会调用KillTimer，SetTimer，所以首先要将所有的
    // 时钟id取出来
    vector<int> time_ids ;
    current_node = used_head.next_node ;
    while(current_node)
    {
        if(current_node->is_used)
        {
            time_ids.push_back(current_node->time_id);
        }
        current_node = current_node->next_node ;
    }

    for( int index = 0; index < (int)time_ids.size(); index++)
    {
        current_node = &container_node[time_ids[index]];
        if(!current_node->is_used)
        {
            continue;
        }

        // 到达了触发次数
        if(!current_node->loop_type && current_node->trigger_count == current_node->trigger_total )
        {
            continue;
        }

        // 没到点
        if(current_node->start_time + current_node->interval_time > current_time)
        {
            continue;
        }

        // 触发
        trigger_id = time_ids[index] ;
        current_node->timer_obj->onTimer(current_node->time_id,current_time);

        // OnTimer有可能KillTimer，SetTimer,
        // 要保证current_node这个时钟单元在OnTimer如果被KillTimer后，不能再次被SetTimer
        // 通过trigger_id保证
        if(current_node->is_used)
        {
            current_node->start_time = current_time;
            if(!current_node->loop_type)
            {
                current_node->trigger_count++;
            }
        }
    }

    trigger_id = -1 ;
}

