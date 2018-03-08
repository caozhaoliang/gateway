/* ===========================================================================
*   Description: 工作线程
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
    //构造函数
    CSmsPlatformSendThread(CSmsPlatformThreadManager* thread_mgr, const int thread_stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //析构函数
    virtual ~CSmsPlatformSendThread();

public:
	STSmsTask * ptask;

public:
    //初始化线程
    virtual int init();

    //启动线程
    virtual int start();

    //投放任务
    virtual int pushTask(const STSmsTask * task);

protected:
    //主处理流程
    virtual int process();

    //获取任务
    virtual STSmsTask* getTask();

    //处理任务
    virtual int processTask(const STSmsTask * task);

	//创建HTTP请求消息
	int createHttpReqMsg(const STSmsTask * task, char* http_msg);

	//HTTP应答处理
	int httpRspProcess(const STSmsTask * task, const std::string & rsp_msg);

    //更新短信状态
    int updateSmsStatus(vector<STSmsTask*>& v_task);

    int updateSmsStatusWithFail(const STSmsTask* task);

    int updateSmsStatusWithErrCode(const STSmsTask* task, int retcode );

protected:
	Json::Value buildBody_batch(const STSmsTask* task);

	string getSvrFormatTime();

	CSmsPlatformThreadManager* thread_manager;

private:
    //任务队列
    std::queue<STSmsTask*> task_queue;

    //队列互斥量
    pthread_mutex_t queue_lock;

    //条件互斥量
    pthread_mutex_t cond_lock;

    //队列条件变量
    pthread_cond_t queue_cond;

	//HTTP解析器
	CHttpParserClient  http_parser;

};


#endif

