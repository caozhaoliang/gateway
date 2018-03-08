/* ===========================================================================
*   Description: ʱ�ӹ���Ԫ�������ṩ��ʱ������
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
    max_node_count = 500000 ; //50W��
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


// �Ӷ���ȡһ�����
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

			//prev�϶�����Ϊ��
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


// ��һ�������뵽����
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


// ɾ��һ�����
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

    //����һ��ʱ�ӵ�Ԫ
    TimerNode* current = NULL;
    getListNode(&idle_head,current) ;
    if(current == NULL)
    {
        return -1;
    }

    idle_node_count--;

    //��ֵ
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

    //���뵽�����б�
    TimerNode* current = &container_node[id];

    listNodeDel(current);

    idle_node_count++;

    //���
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


//��ʱ���
void TimerManager::loopCheck(time_t current_time )
{
    TimerNode* current_node ;
    if( idle_node_count == max_node_count )
    {
        return ;
    }

    //��������������п��ܻ����KillTimer��SetTimer����������Ҫ�����е�
    // ʱ��idȡ����
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

        // �����˴�������
        if(!current_node->loop_type && current_node->trigger_count == current_node->trigger_total )
        {
            continue;
        }

        // û����
        if(current_node->start_time + current_node->interval_time > current_time)
        {
            continue;
        }

        // ����
        trigger_id = time_ids[index] ;
        current_node->timer_obj->onTimer(current_node->time_id,current_time);

        // OnTimer�п���KillTimer��SetTimer,
        // Ҫ��֤current_node���ʱ�ӵ�Ԫ��OnTimer�����KillTimer�󣬲����ٴα�SetTimer
        // ͨ��trigger_id��֤
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

