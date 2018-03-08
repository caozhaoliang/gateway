/* ===========================================================================
*   Description: 更新线程
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include <sys/time.h>
#include <map>
#include <stdio.h>
#include <json/json.h>
#include "simple_log.h"
#include "sms_platform_svr.h"
#include "sms_platform_update_thread.h"


using namespace std;

unsigned long long push_count = 0;
unsigned long long pop_count = 0;


//构造函数
CSmsPlatformUpdateThread::CSmsPlatformUpdateThread(const int thread_stack_size) :
CThreadBase(thread_stack_size)
{

}

//析构函数
CSmsPlatformUpdateThread::~CSmsPlatformUpdateThread()
{

}

//初始化线程
int CSmsPlatformUpdateThread::init()
{
	string db_host = CSmsPlatformSvr::GetConfig()->database_infor.db_host;
	unsigned short db_port = CSmsPlatformSvr::GetConfig()->database_infor.db_port;
	string db_name = CSmsPlatformSvr::GetConfig()->database_infor.db_name;
	string db_user = CSmsPlatformSvr::GetConfig()->database_infor.db_user;
	string db_pwd = CSmsPlatformSvr::GetConfig()->database_infor.db_pass; 

	db_manager_ = new CDataSvrFreeTDS();
	if (db_manager_ == NULL) {
        ERROR_LOG("db_manager_ was NULL new CDataSvrFreeTDS() error.\n");
		return -1;
	}
    // modify by czl for catch exception at 20180203 begin
    try{
    	db_manager_->init(db_host, db_port, db_name, db_user, db_pwd);
    	if (!db_manager_->connect()) {
            ERROR_LOG("db_manager_ connect() failed.\n");
            delete db_manager_;
    		return -1;
    	}
    }catch(std::exception& ex){
        ERROR_LOG("at init catch exception:%s\n",ex.what());
    }
    // modify by czl for catch exception at 20180203 end
    return 0;
}

//投放任务
int CSmsPlatformUpdateThread::pushTask(const STSmsTask * task)
{
    int result = 0;

    if((result = pthread_mutex_lock(&queue_lock)) != 0)
    {
        ERROR_LOG("call pthread_mutex_lock() fail, " \
            "errno: %d, error info: %s", \
            result, STRERROR(result));
    }

    task_queue.push((STSmsTask *)task);

    if((result = pthread_mutex_unlock(&queue_lock)) != 0)
    {
        ERROR_LOG("call pthread_mutex_unlock() fail, " \
            "errno: %d, error info: %s", \
            result, STRERROR(result));
    }

    if((result = pthread_cond_signal(&queue_cond)) != 0)
    {
        ERROR_LOG("pthread_cond_signal() fail, " \
            "errno: %d, error info: %s", \
            result, STRERROR(result));

    }

    return result;
}

//启动线程
int CSmsPlatformUpdateThread::start()
{
    int result = 0;

    if(0 != (result = init()))
    {
        return result;
    }

    if(0 != (result = CThreadBase::initPthreadLock(&queue_lock)))
    {
        return result;
    }

    if(0 != (result = CThreadBase::initPthreadLock(&cond_lock)))
    {
        return result;
    }

    pthread_attr_t thread_attr;
    if(0 != (result = CThreadBase::initPthreadAttr(&thread_attr, thread_stack_size)))
    {
        return result;
    }

    if (0 != (result = pthread_cond_init(&queue_cond, NULL)))
    {
        ERROR_LOG("pthread_cond_init() fail, " \
            "errno: %d, error info: %s\n", \
            result, STRERROR(result));
        return result;
    }

    if (0 != (result = pthread_create(&thread_id, &thread_attr, CThreadBase::runThread, (void *)this)))
    {
        ERROR_LOG("call pthread_create() fail, " \
            "errno: %d, error info: %s", \
            result, STRERROR(result));
        //return result;
    }

    pthread_attr_destroy(&thread_attr);

    return result;
}


//获取任务
STSmsTask* CSmsPlatformUpdateThread::getTask()
{
	STSmsTask* task = NULL;
    int result = 0;

    if ((result = pthread_mutex_lock(&queue_lock)) != 0)
    {
        ERROR_LOG("call pthread_mutex_lock() fail, " \
            "errno: %d, error info: %s", \
            result, STRERROR(result));
        return task;
    }

    if(!task_queue.empty()) 
    {
        task = task_queue.front();
        task_queue.pop();
    }

    if((result = pthread_mutex_unlock(&queue_lock)) != 0)
    {
        ERROR_LOG("call pthread_mutex_unlock() fail, " \
            "errno: %d, error info: %s", \
            result, STRERROR(result));
        //已取到任务不能返回false
    }

    return task;
}

//主处理流程
int CSmsPlatformUpdateThread::process()
{
    INFO_LOG("Work thread begin process task ....\n");

    int result = 0;

	STSmsTask*  task = NULL;

    runing_flag = true;
    pthread_mutex_lock(&cond_lock);
    while(runing_flag)
    {
        //等待任务
        if((result = pthread_cond_wait(&queue_cond, &cond_lock)) != 0)
        {
            ERROR_LOG("call pthread_cond_wait() fail, " \
                "errno: %d, error info: %s", \
                result, STRERROR(result));
        }

        //处理任务
        //task = getTask(); 
        while(NULL != (task = getTask()))
        {
            //业务处理
            result = processTask(task);
            if(-1 == result)
            {
                //TODO: 程序运行异常或等其他错误...
                ERROR_LOG("at update database processTask return a error..\n");
            }

            delete task;
        }
    }
    pthread_mutex_unlock(&cond_lock);

    return 0;
}


//处理任务
int CSmsPlatformUpdateThread::processTask(const STSmsTask * task)
{
	int nret;

	if (task->task_type == SMS_SEND_TASK) {
		nret = NotifySendedStatus(task);
	}
	else if(task->task_type == SMS_RECEIPT_TASK){
		nret = NotifyReceiptStatus(task);
	}
	else if (task->task_type == SMS_UPLINK_TASK) {
		nret = SaveUplinkMsg(task);
	}
    return nret;
}


int CSmsPlatformUpdateThread::NotifySendedStatus(const STSmsTask * task)
{
	int ret = 0;
	try {
		char sqlstr_buf[128];
		memset(sqlstr_buf, 0, sizeof(sqlstr_buf));
		sprintf(sqlstr_buf, "USP_SMG_UpdateMessageStatus %s, %d, '%s'",
			task->snd_id.c_str(), task->snd_status, task->msg_id.c_str());

		INFO_LOG("sqlstr.c_str():%s\n", sqlstr_buf);
		ret = db_manager_->execSQL(sqlstr_buf);
        db_manager_->cancel();
	}
	catch (std::exception &ex) {
		db_manager_->disconnect();
		db_manager_->connect();
        ERROR_LOG("db_manager_ execSQL was error ex.what():%s\n",ex.what() );
		return -1;
	}
	return ret;
}

int CSmsPlatformUpdateThread::NotifyReceiptStatus(const STSmsTask * task)
{
	int ret = 0;
	/*DELIVRD：短信发送成功，手机并能正常展示
	MBBLACK：黑名单（回复过退订或者退订投诉过的号码）
	REPEATD：相同号码相同内容发送频率受限
	REJECTD：被运营商拦截，涉及非法关键字，内容不合法，比如双签名
	待发：一般是网络异常或者是通道商通道堵塞导致（现象持续 20 分钟以上）
	EXPIRED：超时，运营商迟迟没有检测到有效的手机端信号，保持在网关的时间过长*/
	try {
		std::string status_type;
		if (task->report_status == 0) {
			status_type = "DELIVRD";
		}else if(task->report_status > 0){
            status_type = "ZX:"+ StringUtility::Int2Str(task->report_status);
        }

		char sqlstr_buf[128];
		memset(sqlstr_buf, 0, sizeof(sqlstr_buf));
		sprintf(sqlstr_buf, "USP_SMG_UpdateMessageReportStatus %d, %s, '%s'",
			task->oper_id, task->msg_id.c_str(), status_type.c_str());

		INFO_LOG("tsqlstr_buf:%s\n", sqlstr_buf);
		ret = db_manager_->execSQL(sqlstr_buf);
        db_manager_->cancel();
	}
	catch (std::exception &ex) {
		db_manager_->disconnect();
		db_manager_->connect();
        ERROR_LOG("NotifyReceiptStatus catch a exception,ex.what():%s\n",ex.what() );
		return -1;
	}
	
	return ret;
}

int CSmsPlatformUpdateThread::SaveUplinkMsg(const STSmsTask *task)
{	
	int ret = 0;
	try {
		char sqlstr_buf[128];
		memset(sqlstr_buf, 0, sizeof(sqlstr_buf));
		sprintf(sqlstr_buf, "USP_SMG_SaveRecvMessage '%s', '%s', '%s', '%s', %d, '%s', '%s'",
			task->msg_id.c_str(), task->mobile_from.c_str(), "", task->content.c_str(), task->oper_id, "", "");

		INFO_LOG("sqlstr_buf:%s\n", sqlstr_buf);
		ret = db_manager_->execSQL(sqlstr_buf);
        db_manager_->cancel();
	}
	catch (std::exception &ex) {
		db_manager_->disconnect();
		db_manager_->connect();
        ERROR_LOG("SaveUplinkMsg catch a exception ex.what():%s\n",ex.what() );
		return -1;
	}
	return ret;
}