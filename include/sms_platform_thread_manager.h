/* ===========================================================================
*   Description: 短信平台线程管理器
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _SMS_PLATFORM_THREAD_MANAGER_H_
#define _SMS_PLATFORM_THREAD_MANAGER_H_

#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//默认线程数
const  unsigned int I_DEFAULT_THREAD_COUNT      = 4;

//最大线程数
const  unsigned int I_MAX_THREAD_COUNT          = 128;


class CThreadBase;
class CSmsPlatformThreadManager
{
public:
    //构造函数
    CSmsPlatformThreadManager(const unsigned int thread_count);

    //析构函数
    virtual ~CSmsPlatformThreadManager();

public:
    //启动发送线程
    int fireSendThreads();

    //启动更新线程
    int fireUpdateThreads();

    //启动查询线程
    int fireQueryThreads();

    //获取发送线程
    CThreadBase* getSendThread();

    //获取更新线程
    CThreadBase* getUpdateThread();

    //获取查询线程
    CThreadBase* getQueryThreads();

private:
    //发送线程数
    unsigned int send_thread_count;

    //发送线程轮循总数
    unsigned int send_robin_count;

    //发送线程线程池
    std::vector<CThreadBase*> send_thread_pool;

    //发送线程活动线程数
    unsigned int send_active_count;

    //更新回执线程线程数
    unsigned int update_thread_count;

    //更新回执线程轮循总数
    unsigned int update_robin_count;

    //更新回执线程池
    std::vector<CThreadBase*> update_thread_pool;

    //更新回执线程活动线程数
    unsigned int update_active_count;

    //查询线程
    CThreadBase* query_thread;
};


#endif






