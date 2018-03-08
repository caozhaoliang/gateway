/* ===========================================================================
*   Description: 短信平台线程管理器
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include "simple_log.h"
#include "comm_def.h"
#include "thread_base.h"
#include "sms_platform_send_thread.h"
#include "sms_platform_update_thread.h"
#include "sms_platform_query_thread.h"
#include "sms_platform_thread_manager.h"



//构造函数
CSmsPlatformThreadManager::CSmsPlatformThreadManager(const unsigned int thread_count) :
    send_thread_count(0),
    query_thread(NULL)
{
    //启线程数的合法性校验
    if((thread_count > 0) && (thread_count < I_MAX_THREAD_COUNT))
    {
        send_thread_count   = thread_count;
        update_thread_count = thread_count;
    }
    else
    {
        send_thread_count = I_DEFAULT_THREAD_COUNT;
        update_thread_count = thread_count;
    }
}

//析构函数
CSmsPlatformThreadManager::~CSmsPlatformThreadManager()
{
    unsigned int thread_count =  send_thread_pool.size();
    for(unsigned int index = 0; index < thread_count; ++index)
    {
        if(send_thread_pool[index]->isRunning())
        {
            send_thread_pool[index]->stop();
        }

        delete send_thread_pool[index];
        send_thread_pool[index] = NULL;
    }

    thread_count =  update_thread_pool.size();
    for(unsigned int index = 0; index < thread_count; ++index)
    {
        if(update_thread_pool[index]->isRunning())
        {
            update_thread_pool[index]->stop();
        }

        delete update_thread_pool[index];
        update_thread_pool[index] = NULL;
    }
}

//启动发送线程
int CSmsPlatformThreadManager::fireSendThreads()
{
    int iRet = 0;

    //启动发送线程
    //add by czl for init send_active_count at 20180203 begin
    send_active_count = 0;
    //add by czl for init send_active_count at 20180203 begin
    for(unsigned int i = 0; i < send_thread_count; ++i)
    {
        CThreadBase* pThread = NULL;
        NEW(pThread, CSmsPlatformSendThread(this, I_PTHREAD_DEFAULT_STACK_SIZE));
        if(NULL == pThread)
        {
            ERROR_LOG("new CSmsPlatformSendThread object fail.\n");
            continue;
        }

        iRet = pThread->start();
        if(iRet != 0)
        {
            DELETE(pThread);
            continue;
        }

        ++send_active_count;
        send_thread_pool.push_back(pThread);
    }

    WRITELOG("start send thread count:%d\n", (int)send_active_count);

    return (int)send_active_count;
}

//启动更新线程
int CSmsPlatformThreadManager::fireUpdateThreads()
{
    int iRet = 0;

    //启动发送线程
    //add by czl for init update_active_count at 20180203 begin
    update_active_count = 0;
    //add by czl for init update_active_count at 20180203 begin
    for(unsigned int i = 0; i < update_thread_count; ++i)
    {
        CThreadBase* pThread = NULL;
        NEW(pThread, CSmsPlatformUpdateThread(I_PTHREAD_DEFAULT_STACK_SIZE));
        if(NULL == pThread)
        {
            ERROR_LOG("new CSmsPlatformUpdateThread object fail.\n");
            continue;
        }

        iRet = pThread->start();
        if(iRet != 0)
        {
            DELETE(pThread);
            continue;
        }

        ++update_active_count;
        update_thread_pool.push_back(pThread);
    }

    WRITELOG("start update thread count:%d\n", (int)update_active_count);

    return (int)update_active_count;

    return 1;
}

//启动查询线程
int CSmsPlatformThreadManager::fireQueryThreads()
{
    NEW(query_thread, CSmsPlatformQueryThread(this, I_PTHREAD_DEFAULT_STACK_SIZE));
    if(NULL == query_thread)
    {
        ERROR_LOG("new CSmsPlatformQueryThread object fail.\n");
        return 0;
    }

    int iRet = query_thread->start();
    if(iRet != 0)
    {
        ERROR_LOG("Start query thread fail.\n");
        DELETE(query_thread);
        return 0;
    }

    WRITELOG("Start query thread success.\n");

    return 1;
}

//启动发送线程
CThreadBase* CSmsPlatformThreadManager::getSendThread()
{
    if(0 == send_thread_pool.size())
    {
        ERROR_LOG("Send thread pool not usable thread.\n");
        return NULL;
    }
   
    unsigned int  index = ++send_robin_count % send_thread_pool.size();
    if(send_thread_pool[index]->isRunning())
    {
        return (CThreadBase*)send_thread_pool[index];
    }

    return NULL;
}

//获取更新线程
CThreadBase* CSmsPlatformThreadManager::getUpdateThread()
{
    if(0 == update_thread_pool.size())
    {
        ERROR_LOG("Update thread pool not usable thread.\n");
        return NULL;
    }

    unsigned int  index = ++update_robin_count % update_thread_pool.size();
    if(update_thread_pool[index]->isRunning())
    {
        return (CThreadBase*)update_thread_pool[index];
    }

    return NULL;
}

//获取查询线程
CThreadBase* CSmsPlatformThreadManager::getQueryThreads()
{
    return query_thread;
}


