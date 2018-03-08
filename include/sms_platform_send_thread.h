/* ===========================================================================
*   Description: �����߳�
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _SMS_PLATFORM_SEND_THREAD_H_
#define _SMS_PLATFORM_SEND_THREAD_H_

#include <queue>
#include <string>
#include "http_parser.h"
#include "http_parser_client.h"
#include "thread_base.h"
#include "inet_comm.h"
#include "sms_platform_task.h"
#include <json/json.h>
#include "sms_platform_thread_manager.h"
#include <vector>


using namespace std;

#define MAX_SEND_BUFF               20 * 1024 * 12
#define MAX_RECV_BUFF               80 * 1024 * 12 


class CSmsPlatformSendThread : public CThreadBase
{
public:
    //���캯��
    CSmsPlatformSendThread(CSmsPlatformThreadManager* thread_mgr, const int thread_stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //��������
    virtual ~CSmsPlatformSendThread();

public:
	STSmsTask * ptask;

public:
    //��ʼ���߳�
    virtual int init();

    //�����߳�
    virtual int start();

    //Ͷ������
    virtual int pushTask(const STSmsTask * task);

protected:
    //����������
    virtual int process();

    //��ȡ����
    virtual STSmsTask* getTask();

    //��������
    virtual int processTask(const STSmsTask * task);

	//����HTTP������Ϣ
	int createHttpReqMsg(const STSmsTask * task, char* http_msg);

	//HTTPӦ����
	int httpRspProcess(const STSmsTask * task, const std::string & rsp_msg);

    //���¶���״̬
    int updateSmsStatus(vector<STSmsTask*>& v_task);

    int updateSmsStatusWithFail(const STSmsTask* task);

    int updateSmsStatusWithErrCode(const STSmsTask* task, int retcode );

protected:
	Json::Value buildBody_batch(const STSmsTask* task);

	string getSvrFormatTime();

	CSmsPlatformThreadManager* thread_manager;

private:
    //�������
    std::queue<STSmsTask*> task_queue;

    //���л�����
    pthread_mutex_t queue_lock;

    //����������
    pthread_mutex_t cond_lock;

    //������������
    pthread_cond_t queue_cond;

	//HTTP������
	CHttpParserClient  http_parser;

};


#endif

