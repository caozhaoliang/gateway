/* ===========================================================================
*   Description: ²éÑ¯Ïß³Ì
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include <sys/time.h>
#include <unistd.h>
#include <map>
#include <stdio.h>
#include <json/json.h>
#include "simple_log.h"
#include "sms_platform_thread_manager.h"
#include "sms_platform_svr.h"
#include "sms_platform_send_thread.h"
#include "sms_platform_query_thread.h"
#include "comm_def.h"

using namespace std;



//¹¹Ôìº¯Êý
CSmsPlatformQueryThread::CSmsPlatformQueryThread(CSmsPlatformThreadManager* thread_mgr, const int thread_stack_size) :
    CThreadBase(thread_stack_size),
    thread_manager(thread_mgr),
	db_manager_(NULL)
{
	
}

//Îö¹¹º¯Êý
CSmsPlatformQueryThread::~CSmsPlatformQueryThread()
{
	if (db_manager_ != NULL) {
		delete db_manager_;
		db_manager_ = NULL;
	}
}

//³õÊ¼»¯Ïß³Ì
int CSmsPlatformQueryThread::init()
{
	char sBuf[512];
	memset(sBuf, 0x00, sizeof(sBuf));

	string db_host = CSmsPlatformSvr::GetConfig()->database_infor.db_host;
	unsigned short db_port = CSmsPlatformSvr::GetConfig()->database_infor.db_port;
	string db_name = CSmsPlatformSvr::GetConfig()->database_infor.db_name;
	string db_user = CSmsPlatformSvr::GetConfig()->database_infor.db_user;
	string db_pwd = CSmsPlatformSvr::GetConfig()->database_infor.db_pass;

	db_manager_ = new CDataSvrFreeTDS();
	if (db_manager_ == NULL) {
		ERROR_LOG("db_manager_ is NULL...");
		return -1;
	}
	// modify by czl for catch exception at 20180203 begin
	try{
		db_manager_->init(db_host, db_port, db_name, db_user, db_pwd);
		if (!db_manager_->connect()) {
			ERROR_LOG("db_manager_ connect error..");
			return -1;
		}
		
	}catch(std::exception& ex){
		ERROR_LOG("at init catch exception:%s\n",ex.what());
	}
	// modify by czl for catch exception at 20180203 end 
    return 0;
}


//Æô¶¯Ïß³Ì
int CSmsPlatformQueryThread::start()
{
    int result = 0;
    if(0 != (result = init()))
    {
        return result;
    }

    pthread_attr_t thread_attr;
    if(0 != (result = CThreadBase::initPthreadAttr(&thread_attr, thread_stack_size)))
    {
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


//Add by czl for splite smsContent at 20180223 begin
int CSmsPlatformQueryThread::spliteByWord(string s, vector<string>& v_str)
{
	unsigned int c;
	int temp = 0;
	char buf[32] ={0};
	for(int i =0; i < s.length(); ++i) 
	{
		temp =i;
		c=(unsigned char)s[i];
		if( c>0 && c <= 127)
		{
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%c",c);
			v_str.push_back(string(buf));
			continue;
		} else if( (c & 0xE0)== 0xC0 ) {
			i+=1;	
		}else if( (c & 0xF0)== 0xE0 ) {
			i+=2;	
		} else if((c & 0xF8) == 0xF0) {
			i+=3;	
		}
		string strtemp = s.substr(temp,i-temp+1);
		v_str.push_back(strtemp);
	}
	
	return 0;	
}
int CSmsPlatformQueryThread::spliteByLen(string s,int len,vector<string>& v_str)
{
	if( len <= 0) {
		v_str.push_back(s);
		return 1;
	}
	vector<string> v_temp;
	spliteByWord(s,v_temp);
	int vSize = v_temp.size();
	int count = vSize%len > 0 ? vSize/len + 1: vSize/len;

	string sTemp;
	int i =0, j = 0;
	for(vector<string>::iterator it=v_temp.begin();it!=v_temp.end();++it) {
		sTemp+=*it;
		if(++i >= len) {
			if(count > 1){
				v_str.push_back(sTemp + "<"+StringUtility::Int2Str(j+1)+"/"+StringUtility::Int2Str(count)+">");	
			}else{
				v_str.push_back(sTemp);
			}
			sTemp = "";
			i = 0;
			j++;	
		}
	}
	if( !sTemp.empty() && count > 1) {
		v_str.push_back(sTemp+ "<"+StringUtility::Int2Str(count)+"/"+StringUtility::Int2Str(count)+">");
		++j;
	}else if( !sTemp.empty() ) {
		v_str.push_back(sTemp);
		++j;
	}
	return j;	
}

int CSmsPlatformQueryThread::groupSendPushTask(std::vector<STSmsInfor> sms_list,
												string m_Signature, int contenLen,
												int count)
{
	CSmsPlatformSendThread* pThread = NULL;
	int iRet = 0;
	vector<string> v_item;
	spliteByLen(sms_list[0].sms_content,contenLen,v_item);
	for(vector<string>::iterator it=v_item.begin(); it!=v_item.end(); ++it) {
		pThread = (CSmsPlatformSendThread*)thread_manager->getSendThread();
		if (NULL == pThread) {
			ERROR_LOG("Get send thread failed.\n");
			return -1;
		}
		NEW(pThread->ptask, STSmsTask());
		iRet = buildTaskformList(pThread->ptask,sms_list,*it+m_Signature,count);
		if(0 != iRet ) {
			ERROR_LOG("buildTaskformList was error..");
			return -1;
		}		
		pThread->pushTask(pThread->ptask);
	}
	return 0;
}

int CSmsPlatformQueryThread::singleSendPushTask(std::vector<STSmsInfor> sms_list,
												string m_Signature, int contenLen,
												int count)
{
	CSmsPlatformSendThread* pThread = NULL;
	int iRet = 0;
	vector<string> v_item;
	for(int i= 0; i < count; ++i ) {
		spliteByLen(sms_list[i].sms_content,contenLen,v_item);
		for(vector<string>::iterator it=v_item.begin();it!=v_item.end(); ++it) {
			pThread = (CSmsPlatformSendThread*)thread_manager->getSendThread();
			if (NULL == pThread)
			{
				ERROR_LOG("Get send thread failed.\n");
				return -1;
			}
			NEW(pThread->ptask, STSmsTask());
			if( !FillLoadMessageData(*it+m_Signature,sms_list[i],pThread->ptask) )
			{
				ERROR_LOG("FillLoadMessageData failed.\n");
			}
			pThread->ptask->mp_snd_mobile.insert(make_pair(StringUtility::Int2Str(sms_list[i].snd_id),
											string(sms_list[i].user_mblphone)));
			pThread->pushTask(pThread->ptask);
		}
		v_item.clear();
	}
	return 0;
}

int CSmsPlatformQueryThread::buildTaskformList(STSmsTask* task, vector<STSmsInfor> sms_list, string signature,int iNum) 
{
	time_t t_now;
	time(&t_now);
	task->attach = StringUtility::Int2Str((int)t_now);
	task->snd_id = sms_list[0].snd_id;
	task->content = signature; //È¡ÅäÖÃÎÄ¼þ
	task->oper_id = sms_list[0].sms_operator;
	for (int i = 0; i<iNum; ++i) {
		if ((StringUtility::Trim(sms_list[i].user_mblphone).length() > 0 ) && (sms_list[i].snd_id != 0)) {
			task->mp_snd_mobile.insert(make_pair(StringUtility::Int2Str(sms_list[i].snd_id), string(sms_list[i].user_mblphone)));
			task->snd_id += StringUtility::Int2Str(sms_list[i].snd_id) + ",";
			task->mobile_to += string(sms_list[i].user_mblphone) + ",";
		} else {
			//TODO: user_mblphone or snd_id was error when it get from database.
			ERROR_LOG("user_mblphone or snd_id was error when it get from database.\n");
		}
	}
	task->snd_id = StringUtility::TrimRight(task->snd_id, ",");
	task->mobile_to = StringUtility::TrimRight(task->mobile_to, ",");  // 去除最后的逗号(,)
	if( !task->mobile_to.empty() && task->mobile_to.length() > 0 ) {
		return 0;
	}
	ERROR_LOG("task->mobile_to is empty. must be empty mobile at database.\n");
	return -1;
}

//Add by czl for splite smsContent at 20180223 end 
//Ö÷´¦ÀíÁ÷³Ì
int CSmsPlatformQueryThread::process()
{
    INFO_LOG("Query thread begin process task ....\n");
    //add by czl for add config info at 20180130 begin
    int sms_content_lmt_length = CSmsPlatformSvr::GetConfig()->smslength;
    int msgformat = 0 ;
    string m_Signature = CSmsPlatformSvr::GetConfig()->signature;
    //add by czl for add config info at 20180130 end
	while (true)
	{
		std::vector<STSmsInfor>  sms_list;
		int iRet = getSmsList(sms_list);
		if(iRet != 0 ) {
			ERROR_LOG("getSmsList return a error value...");
			sleep(CSmsPlatformSvr::GetConfig()->iIntervalLoadDB);
			continue;
		}

		//modify by czl for groupsend at 20180223 begin
		int iNum = sms_list.size();
		if( CSmsPlatformSvr::GetConfig()->groupsend && iNum > 0 ){
			iRet = groupSendPushTask(sms_list,m_Signature, sms_content_lmt_length,iNum);
			if( iRet != 0 ) {
				ERROR_LOG("groupSendPushTask function error.");
				return -1;
			}
		} else if(iNum > 0) {		  //单发 
			iRet = singleSendPushTask(sms_list,m_Signature,sms_content_lmt_length,iNum);
			if(iRet != 0 ) {
				ERROR_LOG("singleSendPushTask function error.");
				return -1;
			}
		}
		//modify by czl for for groupsend at 20180223 end
		sleep(CSmsPlatformSvr::GetConfig()->iIntervalLoadDB);
	}

    return 0;
}


//»ñÈ¡¶ÌÐÅÁÐ±íÐÅÏ¢
int CSmsPlatformQueryThread::getSmsList(std::vector<STSmsInfor> & sms_list)
{
	try
	{
		int ret = 0;
		string sqlstr;
		ret = GetLoadMessageSql(sqlstr);
		if (!ret) {
			ERROR_LOG("GetLoadMessageSql return false.");
			return -1;
		}
		ret = db_manager_->execSQL(sqlstr.c_str());

		STSmsInfor smsinfo;
		while ((ret = db_manager_->getResult()) != 0) {
			if (ret == 1) {
				smsinfo.reset();
				db_manager_->bindColumn(1, 0, &smsinfo.snd_id);
				db_manager_->bindColumn(2, 0, &smsinfo.cp_id);
				db_manager_->bindColumn(3, 0, smsinfo.sms_spport);
				db_manager_->bindColumn(4, 0, smsinfo.user_mblphone);
				db_manager_->bindColumn(5, 0, smsinfo.sms_serviceid);
				db_manager_->bindColumn(6, 0, smsinfo.sms_feetype);
				db_manager_->bindColumn(7, 0, smsinfo.sms_feecode);
				db_manager_->bindColumn(8, 0, &smsinfo.sms_msgformat);
				db_manager_->bindColumn(9, 0, smsinfo.sms_content);
				db_manager_->bindColumn(10, 0, &smsinfo.sms_operator);
				db_manager_->bindColumn(11, 0, smsinfo.sms_linkid);
				db_manager_->bindColumn(12, 0, &smsinfo.sms_report);
				db_manager_->bindColumn(13, 0, smsinfo.sms_timer);
				db_manager_->bindColumn(14, 0, &smsinfo.sms_momtflag);
				db_manager_->bindColumn(15, 0, smsinfo.sms_intime);
				db_manager_->bindColumn(16, 0, &smsinfo.sms_clientid);

				STSmsTask smstask;
				while (db_manager_->moveNextRow()) {
					/*smstask.reset();
					if (!FillLoadMessageData(smsinfo, smstask)) {

					}*/
					sms_list.push_back(smsinfo);

				}
			}
		}
		db_manager_->cancel();
	}
	catch (const std::exception& ex)
	{
		db_manager_->disconnect();
		db_manager_->connect();
		ERROR_LOG("getSmsList catch a exception ex.what():%s\n", ex.what());
		return -1;
	}
	
    return 0;
}

//
bool CSmsPlatformQueryThread::GetLoadMessageSql(string &sqlstr) 
{
	bool ret = true;
	string max_load_str = CSmsPlatformSvr::GetConfig()->max_load_str;	//Ã¿´Î´ÓÊý¾Ý¿âÖÐ»ñÈ¡¶ÌÐÅµÄ×î´óÌõÊý£¬ ´ÓÅäÖÃÎÄ¼þÈ¡
	string oper_str = CSmsPlatformSvr::GetConfig()->oper_str;	//ÔËÓªÉÌid£¬¿ÉÄÜÓÐ¶à¸ö£¬ÒÔ¶ººÅ¸ô¿ª£¬´ÓÅäÖÃÎÄ¼þÈ¡
	sqlstr = "USP_SMG_LoadOperSendMessage " + max_load_str + ", '" + oper_str + "'";
	INFO_LOG("CSmsPlatformQueryThread::GetLoadMessageSql sqlstr is %s\n", sqlstr.c_str());
	return ret;
}


bool CSmsPlatformQueryThread::FillLoadMessageData(string smsContent, STSmsInfor &smsinfo, STSmsTask* smstask)
{
	bool ret = true;

	char sndid_buf[16];
	memset(sndid_buf, 0, sizeof(sndid_buf));
	sprintf(sndid_buf, "%d", smsinfo.snd_id);
	smstask->snd_id = sndid_buf;
	smstask->mobile_from = smsinfo.sms_spport;
	smstask->mobile_to = smsinfo.user_mblphone;
	smstask->content = smsContent; //È¡ÅäÖÃÎÄ¼þ
	smstask->oper_id = smsinfo.sms_operator;

	return ret;
}



