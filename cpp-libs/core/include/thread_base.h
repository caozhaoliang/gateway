/* ===========================================================================
*   Description: �̻߳���
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
    //���캯��
    CThreadBase(const int stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //��������
    virtual ~CThreadBase();

public:
    //�����߳�
    virtual int start();

    //ֹͣ�߳�(ǿ��ֹͣ)
    virtual int stop();

    //ֹͣ�߳�(ƽ��ֹͣ)
    virtual void stopNormal()
    { 
        runing_flag = false;
    }

    //�߳�����״̬
    virtual bool isRunning()
    {
        return runing_flag;
    }

public:
    //��ʼ���߳���
    static int initPthreadLock(pthread_mutex_t* pthread_lock);

    //��ʼ���߳�����
    static int initPthreadAttr(pthread_attr_t* pattr, int const stack_size);

protected:
    //�߳���ڵ�
    static void* runThread(void* arg);

    //����������
    virtual int process();

protected:
    //�̱߳�ʶ
    pthread_t     thread_id;

    //�߳�ջ��С
    unsigned int  thread_stack_size;

    //���б�ʶ
    bool          runing_flag;
};


#endif






