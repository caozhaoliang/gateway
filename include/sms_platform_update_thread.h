/* ===========================================================================
*   Description: 更新线程
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _SMS_PLATFORM_UPDATE_THREAD_H_
#define _SMS_PLATFORM_UPDATE_THREAD_H_

#include <queue>
#include <string>
#include "thread_base.h"
#include "inet_comm.h"
//#include "dbmanager.h"
#include "datasvr_freetds.h"
#include "sms_platform_task.h"


using namespace std;


class CSmsPlatformUpdateThread : public CThreadBase
{
public:
    //构造函数
    CSmsPlatformUpdateThread(const int thread_stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //析构函数
    virtual ~CSmsPlatformUpdateThread();

public:
	STSmsTask * ptask;
	//enum ETaskType
	//{
	//	//心跳任务, 直接应答
	//	SMS_HEART_TASK = 0,

	//	//工作任务, 需要做任务具体处理
	//	SMS_SEND_TASK = 1,

	//	SMS_REPORT_TASK = 2
 //   };

 //   //数据任务
 //   struct STUpdateTask
 //   {
 //       ETaskType      task_type;
	//	int     	   sms_status;
	//	std::string	   sms_id;
	//	std::string    msg_id;


 //       STUpdateTask()
 //       {
 //           reset();
 //       }

 //       STUpdateTask(const ETaskType &tasktype, const std::string & smsid, const std::string & msgid, const int status) :
 //       task_type(tasktype),
 //           sms_id(smsid),
	//		msg_id(msgid),
	//		sms_status(status)
 //       {
 //       }

 //       void reset()
 //       {
 //           task_type = SMS_HEART_TASK;
	//		sms_status = -1;
 //           sms_id = "";
	//		msg_id = "";
 //       }
 //   };


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

	//记录发送状态
	int NotifySendedStatus(const STSmsTask * task);

	//记录回执
	int NotifyReceiptStatus(const STSmsTask * task);

	//上行短信
	int SaveUplinkMsg(const STSmsTask *task);

private:
    //任务队列
    std::queue<STSmsTask*> task_queue;

    //队列互斥量
    pthread_mutex_t queue_lock;

    //条件互斥量
    pthread_mutex_t cond_lock;

    //队列条件变量
    pthread_cond_t queue_cond;

    //CDbManager dbManager;
	CDataSvrFreeTDS* db_manager_;
};


#endif

