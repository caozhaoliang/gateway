/* ===========================================================================
*   Description: ·¢ËÍÏß³Ì
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include <sys/time.h>
#include <map>
#include <stdio.h>
#include <json/json.h>
#include "comm_def.h"
#include "simple_log.h"
#include "sms_platform_svr.h"
#include "sms_platform_send_thread.h"
#include "md5_packing.h"
#include "sms_platform_update_thread.h"


using namespace std;



//¹¹Ôìº¯Êý
CSmsPlatformSendThread::CSmsPlatformSendThread(CSmsPlatformThreadManager* thread_mgr, int thread_stack_size) :
    CThreadBase(thread_stack_size),
	thread_manager(thread_mgr)
{
	ptask = NULL;
}

//Îö¹¹º¯Êý
CSmsPlatformSendThread::~CSmsPlatformSendThread()
{
	if (ptask != NULL) {
		delete ptask;
		ptask = NULL;
	}
}

//³õÊ¼»¯Ïß³Ì
int CSmsPlatformSendThread::init()
{

    return 0;
}

//Í¶·ÅÈÎÎñ
int CSmsPlatformSendThread::pushTask(const STSmsTask * task)
{
    int result = 0;

    //struct timeval tv_begin, tv_end;
    //gettimeofday(&tv_begin, NULL);

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

    //gettimeofday(&tv_end, NULL);

    //INFO_LOG("beign time:%ld . %ld\n", tv_begin.tv_sec, tv_begin.tv_usec);
    //INFO_LOG("end   time:%ld . %ld\n", tv_end.tv_sec,   tv_end.tv_usec);

    return result;
}

//Æô¶¯Ïß³Ì
int CSmsPlatformSendThread::start()
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


//»ñÈ¡ÈÎÎñ
STSmsTask* CSmsPlatformSendThread::getTask()
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
		//bRet = true;
	}

    if((result = pthread_mutex_unlock(&queue_lock)) != 0)
	{
		ERROR_LOG("call pthread_mutex_unlock() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
        //ÒÑÈ¡µ½ÈÎÎñ²»ÄÜ·µ»Øfalse
	}

    return task;
}

//Ö÷´¦ÀíÁ÷³Ì
int CSmsPlatformSendThread::process()
{
    INFO_LOG("Work thread begin process task ....\n");

    int result = 0;
    
	STSmsTask*  task = NULL;

    runing_flag = true;
    pthread_mutex_lock(&cond_lock);
    while(runing_flag)
    {
        //µÈ´ýÈÎÎñ
        if((result = pthread_cond_wait(&queue_cond, &cond_lock)) != 0)
        {
            ERROR_LOG("call pthread_cond_wait() fail, " \
	            "errno: %d, error info: %s", \
	            result, STRERROR(result));
        }
      
        //´¦ÀíÈÎÎñ
        //task = getTask(); 
        while(NULL != (task = getTask()))
        {
            //ÒµÎñ´¦Àí
            result = processTask(task);
            if( 0 != result)
            {
                // task 发送失败,将任务重新扔到队列之中？？ 或者直接更新DB状态... 
            	ERROR_LOG("send_tcp_data or catch other error.\n");
            }
            
            delete task;
        }
    }
    pthread_mutex_unlock(&cond_lock);

    return 0;
}


//Add by czl for test begin
static string getSign(string account, string agent_id, time_t nonce, string key)
{
	string output;
	char buf[32] = { 0 };
	sprintf(buf, "%ld", nonce);
	string clearText = "account=" + account + "&agent_id=" + agent_id + "&nonce=" + string(buf) +
		"&key=" + key;
	MD5::Encode32((unsigned char*)const_cast<char*>(clearText.c_str()), clearText.size(), output);
	return output;
}

Json::Value CSmsPlatformSendThread::buildBody_batch(const STSmsTask* task)
{
	Json::Value jsonRet;
	string account = CSmsPlatformSvr::GetConfig()->zxst_account;
	string agent_id = CSmsPlatformSvr::GetConfig()->zxst_agentId;
	string key = CSmsPlatformSvr::GetConfig()->zxst_key;				//5XhQFtXxi5J8sgiy

	time_t t_now;
	time(&t_now);
	string sign = getSign(account, agent_id, t_now, key);
	jsonRet["nonce"] = (int)t_now;
	jsonRet["sign"] = sign;
	jsonRet["attach"] = task->attach;
	jsonRet["content"] = task->content;
	jsonRet["mobiles"] = task->mobile_to;
	jsonRet["account"] = account;
	jsonRet["agent_id"] = atoi(agent_id.c_str());

	return jsonRet;
}

string CSmsPlatformSendThread::getSvrFormatTime()
{
	time_t t;
	t = time(&t);
	struct tm* ptm_now;
	char sTime[64] = { 0 };
	struct tm tmNow;
	ptm_now = localtime_r(&t, &tmNow);
	size_t length = strftime(sTime, sizeof(sTime), "%a %b %d %H:%M:%S %Z %Y", ptm_now);
	return string(sTime);
}

int CSmsPlatformSendThread::updateSmsStatusWithErrCode(const STSmsTask* task, int retcode )
{
	// updateSmsStatusWithFail(task);
	map<string,string>::const_iterator it = task->mp_snd_mobile.begin();

	for(; it!=task->mp_snd_mobile.end(); ++it) {				
		CSmsPlatformUpdateThread* pThread = (CSmsPlatformUpdateThread*)thread_manager->getUpdateThread();
		if (NULL == pThread)
		{
			ERROR_LOG("Get update thread failed.\n");
			return -1;
		}
		
		NEW(pThread->ptask, STSmsTask());
		std::string operid = CSmsPlatformSvr::GetConfig()->oper_str;
		pThread->ptask->task_type = SMS_RECEIPT_TASK;
		pThread->ptask->report_status = retcode ;
		pThread->ptask->msg_id = it->second + task->attach;
		
		pThread->ptask->oper_id = atoi(operid.c_str());

		pThread->pushTask(pThread->ptask);
		
	}
	return 0;

}

//´´½¨HTTPÇëÇóÏûÏ¢
int CSmsPlatformSendThread::createHttpReqMsg(const STSmsTask * task, char* http_msg)
{
	// Add by czl for test..
	Json::Value jsonBody = buildBody_batch(task);
	//modify by czl for running test at 20180130 begin
	char http_uri[16] = { 0 };
	if (CSmsPlatformSvr::GetConfig()->groupsend)
	{
		sprintf(http_uri, "/sms_batch");
	}
	else {
		sprintf(http_uri, "/sms_send");
	}
	string http_method = CSmsPlatformSvr::GetConfig()->http_method;
	
	Json::StyledWriter writer;
	string http_body = writer.write(jsonBody);
	int http_req_len = sprintf(http_msg, "%s %s HTTP/1.1\r\n" \
		"Content-Type: application/json;charset=UTF-8\r\n" \
		"Accept: */*\r\n" \
		"Host:0.0.0.0:80\r\n" \
		"Content-Length:%d\r\n" \
		"Data: %s\r\n\r\n%s\n",
		http_method.c_str(), http_uri,
		(int)strlen(http_body.c_str()), getSvrFormatTime().c_str(), http_body.c_str()
	);
	printf("%s\n", http_msg);
	return http_req_len;
}

//´¦ÀíÈÎÎñ
int CSmsPlatformSendThread::processTask(const STSmsTask * task)
{
	char sHttpData[MAX_SEND_BUFF];
	memset(sHttpData, 0x00, sizeof(sHttpData));

	const unsigned int len = createHttpReqMsg(task, sHttpData);

	INFO_LOG("CSmsPlatformSendThread::processTask sHttpData is %s\n", sHttpData);

    int sms_fd = -1;
	bool bRet = inet::create_tcp_client(CSmsPlatformSvr::GetConfig()->sms_ip.c_str(), CSmsPlatformSvr::GetConfig()->sms_port, sms_fd);
	if(!bRet){
		ERROR_LOG("Conn SMS server failed, ip:%s, port:%u.\n", 
			CSmsPlatformSvr::GetConfig()->sms_ip.c_str(), CSmsPlatformSvr::GetConfig()->sms_port);
		return -1;
	}
	//Add by czl for Update TBL_SMS_SNDCACHE before send at 20180205 begin

	updateSmsStatusWithFail(task);
	//Add by czl for Update TBL_SMS_SNDCACHE before send at 20180205 end
	int iRetry = CSmsPlatformSvr::GetConfig()->sendRetry;
	unsigned int send_len = inet::send_tcp_data(sms_fd, sHttpData, len);
	if(send_len != len){
		ERROR_LOG("Send SMS msg failed, ip:%s, port:%u. sHttpData:%s\n", 
			CSmsPlatformSvr::GetConfig()->sms_ip.c_str(), CSmsPlatformSvr::GetConfig()->sms_port, sHttpData);
		close(sms_fd);
		return -1;
	}
	
	int iRecvLen = 0;
	char sRecvBuf[MAX_RECV_BUFF]; //后续调整
	int times = 3000;	//后续调整
	http_parser.Reset();
	do{
		memset(sRecvBuf, 0x00, sizeof(sRecvBuf));
		iRecvLen = recv(sms_fd, sRecvBuf, sizeof(sRecvBuf), 0);
		//Add by czl for test begin
		if (iRecvLen == 0) {
			ERROR_LOG("socket closed by peer, errno:%d,desc:%s,client_ip:%s\n",
				errno, strerror(errno), CSmsPlatformSvr::GetConfig()->sms_ip.c_str());
			close(sms_fd);
			return -1;
		}
		else if (iRecvLen < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				ERROR_LOG("socket recv error, errno:%d,desc:%s,client_ip:%s\n",
					errno, strerror(errno), CSmsPlatformSvr::GetConfig()->sms_ip.c_str());
				close(sms_fd);
				return -1;
			}
			usleep(1000);
			continue;
		}
		INFO_LOG("sRecvBuf:%s,iRecvLen:%d\n", sRecvBuf, iRecvLen);
		//Add by czl for test end
		int iRet = http_parser.ParseHttp(sRecvBuf, iRecvLen);
		if(I_RET_CODE_INCOMPLETE == iRet)
		{
			//数据源包异常
			usleep(1000);
			continue;
			
		}
		else if(I_RET_CODE_SUCCESS == iRet)
		{
			iRet = httpRspProcess(task, http_parser.GetHttpBody());
			if (0 == iRet) {
				break;
			}else if(iRet > 0 ) {
				updateSmsStatusWithErrCode(task, iRet );
			}
			close(sms_fd);
			return 0;
		}
		else
		{
			//TODO: html text with not a http format such as 404 and so on.
			ERROR_LOG("recv http format error, sms_ip=%s:%d; %s\n",
				CSmsPlatformSvr::GetConfig()->sms_ip.c_str(), CSmsPlatformSvr::GetConfig()->sms_port, sRecvBuf);
			close(sms_fd);
			return -1;
		}

		//usleep(10000);
		//--times;
	}while(--times > 0);

	close(sms_fd);

    return 0;
}

//¸üÐÂ¶ÌÐÅ×´Ì¬
int CSmsPlatformSendThread::updateSmsStatus(vector<STSmsTask*>& v_task)
{
	for(vector<STSmsTask*>::iterator it=v_task.begin(); it!=v_task.end(); ++it) {
		STSmsTask* task = *it;
		if (task == NULL) {
			ERROR_LOG("task is NULL ...\n");
			return -1;
		}
		CSmsPlatformUpdateThread* pThread = (CSmsPlatformUpdateThread*)thread_manager->getUpdateThread();
		if (NULL == pThread)
		{
			ERROR_LOG("Get update thread failed.\n");
			return -1;
		}
		
		//×é×°»ØÖ´ÈÎÎñ´«¸ø¸üÐÂÏß³Ì£¬°Ñ»ØÖ´¸üÐÂµ½Êý¾Ý¿âÖÐ
		NEW(pThread->ptask, STSmsTask());
		pThread->ptask->task_type = SMS_SEND_TASK;
		pThread->ptask->snd_id = task->snd_id;
		pThread->ptask->msg_id = task->msg_id;
		pThread->ptask->snd_status = task->snd_status;

		pThread->pushTask(pThread->ptask);
		delete task;
	}

	return 0;
}
int CSmsPlatformSendThread::updateSmsStatusWithFail(const STSmsTask* task) 
{
	map<string,string>::const_iterator it = task->mp_snd_mobile.begin();

	for(; it!=task->mp_snd_mobile.end(); ++it) {
				
		CSmsPlatformUpdateThread* pThread = (CSmsPlatformUpdateThread*)thread_manager->getUpdateThread();
		if (NULL == pThread)
		{
			ERROR_LOG("Get update thread failed.\n");
			return -1;
		}
		
		NEW(pThread->ptask, STSmsTask());
		pThread->ptask->task_type = SMS_SEND_TASK;
		pThread->ptask->snd_id = it->first ;
		pThread->ptask->msg_id = it->second + task->attach;
		pThread->ptask->snd_status = 0;

		pThread->pushTask(pThread->ptask);
		
	}
	return 0;
}

//HTTPÓ¦´ð´¦Àí
int CSmsPlatformSendThread::httpRspProcess(const STSmsTask * task, const std::string & rsp_msg)
{
	Json::Reader reader;
	Json::Value root;

	try {
		if (!reader.parse(rsp_msg.c_str(), root))
		{
			ERROR_LOG("Parse json text error: %s\n", rsp_msg.c_str());
			return -1;
		}
		//modify by czl for finsh update db function at 20180131 begin 
		vector<STSmsTask*> v_SendTask;
		int retcode = root["code"].asInt();
		//短信发送失败
		if (retcode != 0) {
			//cause system or net task failed.. and next to update db status
			ERROR_LOG("server return error and update to db status.\n");
			return retcode;			
		}
		//mgsid组装
		// if (!root["data"]["item"].isNull()) {
					
		// 	for (unsigned int i = 0; i < root["data"]["item"].size(); ++i) {
		// 		STSmsTask * tasktmp = new STSmsTask(task);
		// 		tasktmp->snd_status = 0;
		// 		tasktmp->mobile_to = root["data"]["item"][i]["mobile"].asString();
		// 		map<string, string>::const_iterator it = task->mp_snd_mobile.begin();
				
		// 		// INFO_LOG("the task->mp_snd_mobile.size(). is %d\n", task->mp_snd_mobile.size());
		// 		for (; it != task->mp_snd_mobile.end(); ++it) {
		// 			if (it->second == tasktmp->mobile_to) {
		// 				tasktmp->snd_id = it->first;
		// 				break;
		// 			}
		// 		}

		// 		tasktmp->msg_id = tasktmp->mobile_to + task->attach;
		// 		INFO_LOG("tasktmp->mobile_to:%s\n",tasktmp->mobile_to.c_str());
		// 		v_SendTask.push_back(tasktmp);
		// 	}
		// }
		
		// int nret = updateSmsStatus(v_SendTask);
		//modify by czl for finsh update db function at20180131 end
		// return nret;
	}
	catch (exception &ex) {
		ERROR_LOG("analytical json object was error,ex.what() is:%s\n",ex.what() );
		return -1;
	}

	return 0;
}










