/* ===========================================================================
*   Description: 线程基类
*   Author:  shenhuihai
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _THREAD_BASE_H_
#define _THREAD_BASE_H_

#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define STRERROR(no) (strerror(no) != NULL ? strerror(no) : "Unkown error")

#define I_PTHREAD_DEFAULT_STACK_SIZE        (2 * 1024 * 1024)



class CThreadBase
{
public:
    //构造函数
    CThreadBase(const int stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //析构函数
    virtual ~CThreadBase();

public:
    //启动线程
    virtual int start();

    //停止线程(强制停止)
    virtual int stop();

    //停止线程(平滑停止)
    virtual void stopNormal()
    { 
        runing_flag = false;
    }

    //线程运行状态
    virtual bool isRunning()
    {
        return runing_flag;
    }

public:
    //初始化线程锁
    static int initPthreadLock(pthread_mutex_t* pthread_lock);

    //初始化线程属性
    static int initPthreadAttr(pthread_attr_t* pattr, int const stack_size);

protected:
    //线程入口点
    static void* runThread(void* arg);

    //主处理流程
    virtual int process();

protected:
    //线程标识
    pthread_t     thread_id;

    //线程栈大小
    unsigned int  thread_stack_size;

    //运行标识
    bool          runing_flag;
};


#endif






