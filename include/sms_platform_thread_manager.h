/* ===========================================================================
*   Description: ����ƽ̨�̹߳�����
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

//Ĭ���߳���
const  unsigned int I_DEFAULT_THREAD_COUNT      = 4;

//����߳���
const  unsigned int I_MAX_THREAD_COUNT          = 128;


class CThreadBase;
class CSmsPlatformThreadManager
{
public:
    //���캯��
    CSmsPlatformThreadManager(const unsigned int thread_count);

    //��������
    virtual ~CSmsPlatformThreadManager();

public:
    //���������߳�
    int fireSendThreads();

    //���������߳�
    int fireUpdateThreads();

    //������ѯ�߳�
    int fireQueryThreads();

    //��ȡ�����߳�
    CThreadBase* getSendThread();

    //��ȡ�����߳�
    CThreadBase* getUpdateThread();

    //��ȡ��ѯ�߳�
    CThreadBase* getQueryThreads();

private:
    //�����߳���
    unsigned int send_thread_count;

    //�����߳���ѭ����
    unsigned int send_robin_count;

    //�����߳��̳߳�
    std::vector<CThreadBase*> send_thread_pool;

    //�����̻߳�߳���
    unsigned int send_active_count;

    //���»�ִ�߳��߳���
    unsigned int update_thread_count;

    //���»�ִ�߳���ѭ����
    unsigned int update_robin_count;

    //���»�ִ�̳߳�
    std::vector<CThreadBase*> update_thread_pool;

    //���»�ִ�̻߳�߳���
    unsigned int update_active_count;

    //��ѯ�߳�
    CThreadBase* query_thread;
};


#endif






